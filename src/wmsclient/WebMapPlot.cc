/*
  Diana - A Free Meteorological Visualisation Tool

  Copyright (C) 2015 MET Norway

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no

  This file is part of Diana

  Diana is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Diana is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Diana; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "WebMapPlot.h"

#include "WebMapPainting.h"
#include "WebMapService.h"
#include "WebMapUtilities.h"
#include "diGLPainter.h"

#include <QImage>

#define MILOGGER_CATEGORY "diana.WebMapPlot"
#include <miLogger/miLogging.h>

static const std::string EMPTY_STRING;
static const std::vector<std::string> EMPTY_STRING_V;

WebMapPlot::WebMapPlot(WebMapService* service, const std::string& layer)
  : mService(service)
  , mLayerId(layer)
  , mLayer(0)
  , mTimeIndex(-1)
  , mTimeSelected(-1)
  , mTimeTolerance(3600)
  , mTimeOffset(0)
  , mColourTransform(new diutil::SimpleColourTransform)
  , mRequest(0)
{
  if (mService) {
    serviceRefreshFinished();
    connect(mService, SIGNAL(refreshStarting()), this, SLOT(serviceRefreshStarting()));
    connect(mService, SIGNAL(refreshFinished()), this, SLOT(serviceRefreshFinished()));
  }
}

WebMapPlot::~WebMapPlot()
{
  dropRequest();
  delete mColourTransform;
}

std::string WebMapPlot::title() const
{
  if (mLayer)
    return mLayer->title();
  else
    return std::string();
}

std::string WebMapPlot::attribution() const
{
  if (mLayer)
    return mLayer->attribution();
  else
    return std::string();
}

void WebMapPlot::dropRequest()
{
  if (!mRequest)
    return;
  if (!mRequestCompleted) {
    disconnect(mRequest, SIGNAL(completed()), this, SIGNAL(update()));
    mRequest->abort();
    mRequest->deleteLater();
  } else {
    delete mRequest;
  }
  mRequest = 0;
}

void WebMapPlot::plot(DiGLPainter* gl, PlotOrder porder)
{
  METLIBS_LOG_SCOPE();
  if (!isEnabled() || porder != LINES)
    return;
  if (!mLayer)
    return;

  if (!mRequest) {
    METLIBS_LOG_DEBUG("about to request tiles...");
    const double mapw = getStaticPlot()->getPlotSize().width(),
        m_per_unit = diutil::metersPerUnit(getStaticPlot()->getMapArea().P()),
        phys_w = getStaticPlot()->getPhysWidth();
    const double viewScale = mapw * m_per_unit / phys_w
        / diutil::WMTS_M_PER_PIXEL;
    METLIBS_LOG_DEBUG("map.w=" << mapw << " m/unit=" << m_per_unit
        << " phys.w=" << phys_w << " scale=" << viewScale);
    mRequestCompleted = false;
    mRequest = mService->createRequest(mLayer->identifier(),
        getStaticPlot()->getPlotSize(), getStaticPlot()->getMapArea().P(), viewScale);
    if (!mRequest) {
      METLIBS_LOG_DEBUG("no request object");
      return;
    }
    for (std::map<std::string, std::string>::const_iterator it = mDimensionValues.begin(); it != mDimensionValues.end(); ++it)
      mRequest->setDimensionValue(it->first, it->second);
    if (mTimeIndex >= 0 && !mFixedTime.empty())
      mRequest->setDimensionValue(mLayer->dimension(mTimeIndex).identifier(), mFixedTime);
    connect(mRequest, SIGNAL(completed()), this, SLOT(requestCompleted()));
    mRequest->submit();
  } else if (mRequestCompleted) {
    METLIBS_LOG_DEBUG("about to plot tiles...");
    const Projection& tp = mRequest->tileProjection();
    METLIBS_LOG_DEBUG(LOGVAL(tp.getProjDefinitionExpanded()));
    for (size_t i=0; i<mRequest->countTiles(); ++i) {
      const Rectangle& tr = mRequest->tileRect(i);
      const QImage& ti = mRequest->tileImage(i);
      if (!ti.isNull()) {
        METLIBS_LOG_DEBUG("tile " << i << " is valid, rectangle=" << tr);
        // draw tile using plotFillCell

        const size_t N = (ti.width()+1)*(ti.height()+1);
        float* vx = new float[N];
        float* vy = new float[N];
        const double dx = tr.width() / ti.width(), dy = tr.height() / ti.height();
        size_t ixy = 0;
        for (int iy=0; iy<=ti.height(); ++iy) {
          for (int ix=0; ix<=ti.width(); ++ix) {
            vx[ixy] = tr.x1 + ix*dx;
            vy[ixy] = tr.y2 - iy*dy;
            ixy += 1;
          }
        }
        getStaticPlot()->ProjToMap(tp, N, vx, vy);
        diutil::QImageData imagepixels(&ti, vx, vy);
        imagepixels.setColourTransform(mColourTransform);
        diutil::drawFillCell(gl, imagepixels);
        delete[] vx;
        delete[] vy;

      } else {
        METLIBS_LOG_DEBUG("tile " << i << " is null, drawing rectangle " << tr);
        // draw rect
        float corner_x[4] = { tr.x1, tr.x2, tr.x2, tr.x1 };
        float corner_y[4] = { tr.y1, tr.y1, tr.y2, tr.y2 };
        getStaticPlot()->ProjToMap(tp, 4, corner_x, corner_y);

        const float red = 0, green = 1, blue = 0, alpha = 0;
        gl->Color4f(red, green, blue, alpha);
        gl->LineWidth(2.0);
        gl->Begin(DiGLPainter::gl_LINE_LOOP);
        for (int i=0; i<4; ++i)
          gl->Vertex2f(corner_x[i], corner_y[i]);
        gl->End();
      }
    }
    const QImage li = mRequest->legendImage();
    if (!li.isNull()) {
      METLIBS_LOG_DEBUG("about to plot legend...");
      // draw legend using plotFillCell

      const int w = li.width(), h = li.height();
      const size_t N = (w+1)*(h+1);
      float* vx = new float[N];
      float* vy = new float[N];
      size_t ixy = 0;
      for (int iy=0; iy<=h; ++iy) {
        for (int ix=0; ix<=w; ++ix) {
          const float px = getStaticPlot()->getPhysWidth() - w - 10 + ix,
              py = 10 + h - iy;
          getStaticPlot()->PhysToMap(px, py, vx[ixy], vy[ixy]);
          ixy += 1;
        }
      }
      diutil::QImageData imagepixels(&li, vx, vy);
      imagepixels.setColourTransform(mColourTransform);
      diutil::drawFillCell(gl, imagepixels);
      delete[] vx;
      delete[] vy;
    }
  } else {
    METLIBS_LOG_DEBUG("waiting for tiles...");
  }
}

void WebMapPlot::setStyleAlpha(float offset, float scale)
{
  mColourTransform->setStyleAlpha(offset, scale);
}

void WebMapPlot::setStyleGrey(bool makeGrey)
{
  mColourTransform->setStyleGrey(makeGrey);
}

void WebMapPlot::requestCompleted()
{
  METLIBS_LOG_SCOPE();
  mRequestCompleted = true;
  Q_EMIT update();
}

void WebMapPlot::changeProjection()
{
  if (mOldArea != getStaticPlot()->getMapArea()) {
    mOldArea = getStaticPlot()->getMapArea();
    dropRequest();
  }
}

size_t WebMapPlot::countDimensions() const
{
  if (!mLayer)
    return 0;
  else
    return mLayer->countDimensions();
}

const std::string& WebMapPlot::dimensionTitle(size_t idx) const
{
  if (!mLayer || idx >= mLayer->countDimensions())
    return EMPTY_STRING;
  return mLayer->dimension(idx).title();
}

const std::vector<std::string>& WebMapPlot::dimensionValues(size_t idx) const
{
  if (!mLayer || idx >= mLayer->countDimensions())
    return EMPTY_STRING_V;
  return mLayer->dimension(idx).values();
}

void WebMapPlot::setDimensionValue(const std::string& dimId, const std::string& dimValue)
{
  METLIBS_LOG_SCOPE(LOGVAL(dimId) << LOGVAL(dimValue));
  if (!dimValue.empty())
    mDimensionValues[dimId] = dimValue;
  else
    mDimensionValues.erase(dimId);
}

void WebMapPlot::setTimeValue(const miutil::miTime& time)
{
  METLIBS_LOG_SCOPE(LOGVAL(time) << LOGVAL(mTimeIndex));
  if (!mLayer || mTimeIndex < 0)
    return;

  int bestIndex = -1, bestDifference = 0;

  miutil::miTime actualTime = time;
  actualTime.addSec(mTimeOffset);
  const WebMapDimension& timeDim = mLayer->dimension(mTimeIndex);
  for (size_t i=0; i<timeDim.count(); ++i) {
    const miutil::miTime dimTime
        = diutil::to_miTime(diutil::parseWmsIso8601(timeDim.value(i)));
    if (dimTime.undef()) {
      METLIBS_LOG_DEBUG("undef time");
      continue;
    }
    const int diff = abs(miutil::miTime::secDiff(dimTime, actualTime));
    if (diff < mTimeTolerance && (bestIndex < 0 || diff < bestDifference)) {
      bestIndex = i;
      bestDifference = diff;
      METLIBS_LOG_DEBUG(LOGVAL(bestDifference));
    }
  }
  if (bestIndex == mTimeSelected)
    return;
  mTimeSelected = bestIndex;
  const std::string& timeId = timeDim.identifier();
  if (bestIndex >= 0) {
    METLIBS_LOG_DEBUG(LOGVAL(bestIndex) << "='" << timeDim.value(bestIndex) << "'");
    setDimensionValue(timeId, timeDim.value(bestIndex));
  } else {
    setDimensionValue(timeId, EMPTY_STRING);
  }
  dropRequest();
}

void WebMapPlot::setFixedTime(const std::string& fixedTime)
{
  mFixedTime = fixedTime;
}

void WebMapPlot::serviceRefreshStarting()
{
  METLIBS_LOG_SCOPE();
  dropRequest();
  mLayer = 0;
}

void WebMapPlot::serviceRefreshFinished()
{
  METLIBS_LOG_SCOPE(LOGVAL(mLayerId));
  mLayer = mService->findLayerByIdentifier(mLayerId);
  mTimeIndex = -1;
  mTimeSelected = -1;

  if (!mLayer) {
    METLIBS_LOG_INFO("layer '" << mLayerId << "' not found");
  } else {
    // search new time index
    for (size_t i=0; i<mLayer->countDimensions(); ++i) {
      if (mLayer->dimension(i).isTime()) {
        mTimeIndex = i;
        break;
      }
    }
    // search plot time
    setTimeValue(getStaticPlot()->getTime());
  }
  Q_EMIT update();
}
