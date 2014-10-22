/*
  Diana - A Free Meteorological Visualisation Tool

  $Id$

  Copyright (C) 2014 met.no

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

#include "GL/gl.h"
#include "diDrawingManager.h"
#include "EditItems/drawingtext.h"
#include "EditItems/drawingstylemanager.h"
#include "diFontManager.h"
#include "diPlotModule.h"

namespace DrawingItem_Text {

Text::Text()
{
  margin_ = 4;
  spacing_ = 0.5;
}

Text::~Text()
{
}

void Text::draw()
{
  QStringList lines_ = text();
  if (points_.isEmpty() || lines_.isEmpty())
    return;

  DrawingStyleManager *styleManager = DrawingStyleManager::instance();

  QRectF bbox = drawingRect();
  QList<QPointF> points;
  points << bbox.bottomLeft() << bbox.bottomRight() << bbox.topRight() << bbox.topLeft();

  // Use the fill colour defined in the style to fill the text area.
  styleManager->beginFill(this);
  styleManager->fillLoop(this, points);
  styleManager->endFill(this);

  // Draw the outline using the border colour and line pattern defined in
  // the style.
  styleManager->beginLine(this);
  styleManager->drawLines(this, points);
  styleManager->endLine(this);

  styleManager->beginText(this, poptions);
  styleManager->setFont(this, poptions);
  float scale = PlotModule::instance()->getStaticPlot()->getPhysWidth() / PlotModule::instance()->getStaticPlot()->getPlotSize().width();

  float x = points.at(0).x() + margin_;
  float y = points.at(0).y() - margin_;

  foreach (QString text, lines_) {
    QSizeF size = getStringSize(text);
    glPushMatrix();
    glTranslatef(x, y - size.height(), 0);
    glScalef(scale, scale, 1.0);
    PlotModule::instance()->getStaticPlot()->getFontPack()->drawStr(text.toStdString().c_str(), 0, 0, 0);
    glPopMatrix();
    y -= size.height() * (1.0 + spacing_);
  }

  styleManager->endText(this);
}

QSizeF Text::getStringSize(const QString &text, int index) const
{
  if (index == -1)
    index = text.size();

  DrawingStyleManager *styleManager = DrawingStyleManager::instance();
  styleManager->setFont(this, poptions);

  float width, height;
  if (!PlotModule::instance()->getStaticPlot()->getFontPack()->getStringSize(text.left(index).toStdString().c_str(), width, height))
    width = height = 0;

  QSizeF size(width, height);

  if (height == 0) {
    PlotModule::instance()->getStaticPlot()->getFontPack()->getStringSize("X", width, height);
    size.setHeight(qMax(height, poptions.fontsize));
  }

  float scale = PlotModule::instance()->getStaticPlot()->getPhysWidth() / PlotModule::instance()->getStaticPlot()->getPlotSize().width();
  return scale * size;
}

DrawingItemBase::Category Text::category() const
{
  return DrawingItemBase::Text;
}

QStringList Text::text() const
{
  return ConstDrawing(this)->property("text").toStringList();
}

QRectF Text::drawingRect() const
{
  QRectF bbox = boundingRect();
  if (property("alignment", Qt::AlignCenter) == Qt::AlignCenter)
    bbox.translate(-bbox.width()/2, bbox.height()/2);

  return bbox;
}

void Text::updateRect()
{
  float x = points_.at(0).x();
  float y = points_.at(0).y();
  qreal width = 0;
  QStringList lines_ = text();

  for (int i = 0; i < lines_.size(); ++i) {
    QString text = lines_.at(i);
    QSizeF size = getStringSize(text);
    width = qMax(width, size.width());
    size.setHeight(qMax(size.height(), qreal(poptions.fontsize)));
    y -= size.height();
    if (i < lines_.size() - 1)
      y -= size.height() * spacing_;
  }

  points_[1] = QPointF(x + width + 2 * margin_, y - 2 * margin_);
}

void Text::setText(const QStringList &lines)
{
  setProperty("text", lines);
  updateRect();
}

QDomNode Text::toKML(const QHash<QString, QString> &extraExtData) const
{
  QHash<QString, QString> extra;
  QStringList lines = text();
  extra["text"] = lines.join("\n");
  extra["margin"] = QString::number(margin_);
  extra["spacing"] = QString::number(spacing_);
  return DrawingItemBase::toKML(extra.unite(extraExtData));
}

void Text::fromKML(const QHash<QString, QString> &extraExtData)
{
  DrawingManager::instance()->setFromLatLonPoints(*this, getLatLonPoints());
  setText(extraExtData.value("met:text", "").split("\n"));
  margin_ = extraExtData.value("met:margin", "4").toInt();
  spacing_ = extraExtData.value("met:spacing", "0.5").toFloat();
}

} // namespace
