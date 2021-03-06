/*
  Diana - A Free Meteorological Visualisation Tool

  $Id$

  Copyright (C) 2006 met.no

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "diVprofPlot.h"

#include "diColour.h"
#include "diGlUtilities.h"
#include "diUtilities.h"

#include <diField/diMetConstants.h>

#include <cmath>
#include <iomanip>
#include <sstream>

#define MILOGGER_CATEGORY "diana.VprofPlot"
#include <miLogger/miLogging.h>

using namespace std;
using namespace miutil;

//#define DEBUGPRINT 1

VprofPlot::VprofPlot()
  : windInKnots(true)
{
  METLIBS_LOG_SCOPE();
}

VprofPlot::~VprofPlot() {
  METLIBS_LOG_SCOPE();
}

bool VprofPlot::idxForValue(float& v, int& i) const
{
  if (isnan(v) or v <= -1e35 or v >= 1e35)
    return false;
  v /= idptab;
  i = int(v); // FIXME this will probably not work correctly if v < 0
  return (i>=0 and i+1<mptab);
}

float VprofPlot::tabForValue(const float* tab, float x) const
{
  int i = 0;
  if (not idxForValue(x, i))
    return 0;
  return tab[i]+(tab[i+1]-tab[i])*(x-i);
}

bool VprofPlot::plot(DiGLPainter* gl, VprofOptions *vpopt, int nplot)
{
  METLIBS_LOG_SCOPE(LOGVAL(nplot));

  if (text.posName.empty())
    return false;

  METLIBS_LOG_DEBUG("start plotting " << text.posName);

  const float dx1deg= dx1degree;

  // VprofPlot::maxLevels is set in VprofData::getData (VprofData is a friend)
  float *xx= new float[maxLevels];
  float *xz= new float[maxLevels];
  float *yy= new float[maxLevels];

  const int nstyles = std::min(vpopt->dataColour.size(),
      std::min(vpopt->dataLinewidth.size(), vpopt->windLinewidth.size()));
  const int istyle = nplot % nstyles;

  const Colour c(vpopt->dataColour[istyle]);
  const float dataWidth= vpopt->dataLinewidth[istyle];
  const float windWidth= vpopt->windLinewidth[istyle];
  gl->setLineStyle(c, dataWidth);

  // levels for T (always)
  if (ptt.size()>0) {
    const size_t nlevel = std::min(maxLevels, ptt.size());
    for (unsigned int k=0; k<nlevel; k++) {
      yy[k] = tabForValue(yptab, ptt[k]);
      xz[k] = tabForValue(xztab, ptt[k]);
    }
  }

  // T
  if (vpopt->ptttt and tt.size() >= ptt.size()) {
    const size_t nlevel = std::min(maxLevels, tt.size());
    for (unsigned int k=0; k<nlevel; k++)
      xx[k]= xz[k]+dx1deg*tt[k];
    diutil::xyclip(nlevel,xx,yy,&xysize[1][0], gl);
  }

  // levels for Td (if not same as T levels)
  if (vpopt->ptdtd && ptd.size()>0) {
    const size_t nlevel = std::min(maxLevels, ptd.size());
    for (unsigned int k=0; k<nlevel; k++) {
      yy[k] = tabForValue(yptab, ptd[k]);
      xz[k] = tabForValue(xztab, ptd[k]);
    }
  }

  // Td
  if (vpopt->ptdtd && td.size()>0) {
    gl->Enable(DiGLPainter::gl_LINE_STIPPLE);
    gl->LineStipple(1,0xFFC0);
    const size_t nlevel = std::min(maxLevels, td.size());
    for (unsigned int k=0; k<nlevel; k++)
      xx[k]= xz[k]+dx1deg*td[k];
    diutil::xyclip(nlevel,xx,yy,&xysize[1][0], gl);
    gl->Disable(DiGLPainter::gl_LINE_STIPPLE);
  }

  // levels for wind and significant levels (if not same as T levels)
  if ((vpopt->pwind || vpopt->pslwind) && puv.size()>0) {
    const size_t nlevel = std::min(maxLevels, puv.size());
    for (unsigned int k=0; k<nlevel; k++)
      yy[k] = tabForValue(yptab, puv[k]);
  }

  // wind (u(e/w) and v(n/s)in unit knots)
  if (vpopt->pwind && uu.size()>0) {
    float xw= xysize[5][1] - xysize[5][0];
    float x0= xysize[5][0] + xw*0.5;
    if (vpopt->windseparate)
      x0+=xw*float(nplot);
    float ylim1= xysize[1][2];
    float ylim2= xysize[1][3];
    float flagl = xw * 0.5 * 0.85;

    gl->setLineStyle(c, windWidth);
    const float windScale = windInKnots ? 1 : (3600.0/1852.0);
    const size_t nlevel = std::min(maxLevels, std::min(uu.size(), vv.size()));
    for (unsigned int k=0; k<nlevel; k++) {
      if (yy[k]>=ylim1 && yy[k]<=ylim2)
        gl->drawWindArrow(uu[k] * windScale, vv[k] * windScale,
            x0, yy[k], flagl, false);
    }
  }

  if (vpopt->pslwind && dd.size()>0) {
    // significant wind levels, wind as numbers (temp and prog)
    // (other levels also if space)
    float dchy= chylab*1.3;
    float ylim1= xysize[4][2]+dchy*0.5;
    float ylim2= xysize[4][3]-dchy*0.5;
    int k1= -1;
    int k2= -1;
    const size_t nlevel = maxLevels;
    for (unsigned int k=0; k<nlevel; k++) {
      if (yy[k]>ylim1 && yy[k]<ylim2) {
        if (k1==-1) k1= k;
        k2= k;
      }
    }

    if (k1>=0) {
      float *used= new float[nlevel];
      size_t nused = 0;
      const float x= xysize[4][0]+(xysize[4][1]-xysize[4][0])*nplot+chxlab*0.5;
      setFontsize(gl, chylab);

      for (int sig=3; sig>=0; sig--) {
        for (int k=k1; k<=k2; k++) {
          if (sigwind[k]==sig) {
            ylim1= yy[k]-dchy;
            ylim2= yy[k]+dchy;
            size_t i= 0;
            while (i<nused && (used[i]<ylim1 || used[i]>ylim2)) i++;
            if (i==nused) {
              used[nused++]= yy[k];
              int idd= (dd[k]+5)/10;
              int iff= ff[k];
              if (idd==0 && iff>0) idd=36;
              ostringstream ostr;
              ostr << setw(2) << setfill('0') << idd << "-"
              << setw(3) << setfill('0') << iff;
              std::string str= ostr.str();
              const float y= yy[k]-chylab*0.5;
              gl->drawText(str, x, y, 0.0);
            }
          }
        }
      }
      delete[] used;
    }
  }

  // levels for vertical wind, omega (if not same as T levels)
  if (vpopt->pvwind && pom.size()>0) {
    const size_t nlevel = std::min(maxLevels, pom.size());
    for (unsigned int k=0; k<nlevel; k++)
      yy[k] = tabForValue(yptab, pom[k]);
  }

  float xylimit[4] = { 0., 0., xysize[1][2], xysize[1][3] };

  // vertical wind, omega
  if (vpopt->pvwind && om.size()>0) {
    float dx= xysize[6][1] - xysize[6][0];
    float x0= xysize[6][0] + dx*0.5;
    float scale= -dx/vpopt->rvwind;
    const size_t nlevel = std::min(maxLevels, om.size());
    for (unsigned int k=0; k<nlevel; k++)
      xx[k]= x0 + scale*om[k];
    xylimit[0]= xysize[6][0];
    xylimit[1]= xysize[6][1];
    diutil::xyclip(nlevel,xx,yy,xylimit, gl);
  }

  if (!prognostic && (vpopt->prelhum || vpopt->pducting)) {
    const size_t nlevel = std::min(maxLevels, pcom.size());
    for (unsigned int k=0; k<nlevel; k++)
      yy[k] = tabForValue(yptab, pcom[k]);
  }

  // levels for relative humidity (same as T levels)
  if (vpopt->prelhum && ptt.size()>0 && (ptt.size() == ptd.size())) {
    const size_t nlevel = std::min(maxLevels, ptt.size());
    for (unsigned int k=0; k<nlevel; k++)
      yy[k] = tabForValue(yptab, ptt[k]);
  }

  // relative humidity
  if (vpopt->prelhum) {
    float dx= xysize[7][1] - xysize[7][0];
    float x0= xysize[7][0];
    float scale= dx/100.;
    if (rhum.size()==0) {
      if (prognostic)
        relhum(tt,td);
      else
        relhum(tcom,tdcom);
    }
    if (rhum.size() == ptt.size()) {
      const size_t nlevel = std::min(maxLevels, rhum.size());
      for (unsigned int k=0; k<nlevel; k++)
        xx[k]= x0 + scale*rhum[k];
      xylimit[0]= xysize[7][0];
      xylimit[1]= xysize[7][1];
      diutil::xyclip(nlevel,xx,yy,xylimit, gl);
    }
  }

  // levels for ducting (same as T levels)
  if (vpopt->pducting && ptt.size()>0 && (ptt.size() == ptd.size())) {
    const size_t nlevel = std::min(maxLevels, ptt.size());
    for (unsigned int k=0; k<nlevel; k++)
      yy[k] = tabForValue(yptab, ptt[k]);
  }

  // ducting
  if (vpopt->pducting) {
    float rd= vpopt->ductingMax - vpopt->ductingMin;
    float dx= xysize[8][1] - xysize[8][0];
    float x0= xysize[8][0] + dx * (0.0 - vpopt->ductingMin)/rd;
    float scale= dx/rd;
    if (duct.size()==0) {
      if (prognostic)
        ducting(ptt,tt,td);
      else
        ducting(pcom,tcom,tdcom);
    }
    if (duct.size()==ptt.size()) {
      const size_t nlevel = std::min(maxLevels, duct.size());
      for (unsigned int k=0; k<nlevel; k++)
        xx[k]= x0 + scale*duct[k];
      xylimit[0]= xysize[8][0];
      xylimit[1]= xysize[8][1];
      diutil::xyclip(nlevel,xx,yy,xylimit, gl);
    }
  }

  if (vpopt->pkindex && !text.kindexFound) {
    if (prognostic)
      kindex(ptt,tt,td);
    else
      kindex(pcom,tcom,tdcom);
  }

  // text
  text.index= nplot;
  text.colour= c;
  vptext.push_back(text);

  delete[] xx;
  delete[] xz;
  delete[] yy;

  return true;
}


void VprofPlot::relhum(const vector<float>& tt, const vector<float>& td)
{
  METLIBS_LOG_SCOPE();
  using namespace MetNo::Constants;

  int nlev= tt.size();
  if (tt.size() != td.size()) {
    rhum.clear();
    return;
  }
  rhum.resize(nlev);

  for (int k=0; k<nlev; k++) {
    const ewt_calculator ewt(tt[k]), ewt2(td[k]);
    float rhx = 0;
    if (ewt.defined() and ewt2.defined()) {
      const float et = ewt.value();
      const float etd = ewt2.value();
      rhx = 100.*etd/et;
    }
    if (rhx < 0)
        rhx = 0;
    if (rhx > 100)
        rhx = 100;
    rhum[k] = rhx;
  }
}


void VprofPlot::ducting(const vector<float>& pp,
    const vector<float>& tt,
    const vector<float>& td) {
  METLIBS_LOG_DEBUG("++ VprofPlot::ducting(...)");

  // p,t,td -> ducting index
  //
  // t and td in degrees celsius (and at the same levels)
  //
  // ducting = 77.6*(p/t)+373000.*(q*p)/(eps*t*t)
  //
  // q*p/eps = rh*qsat*p/eps = rh*(eps*e(t)/p)*p/eps
  //         = rh*e(t) = (e(td)/e(t))*e(t) = e(td)
  //
  //    =>  ducting = 77.6*(p/t)+373000.*e(td)/(t*t)
  //
  // output: duct(k) = (ducting(k+1)-ducting(k))/(dz*0.001) + 157.

  const float g= 9.8;
  const float r= 287.;
  const float cp= 1004.;
  const float p0= 1000.;
  const float t0= 273.15;
  const float ginv= 1./g;
  const float rcp = r/cp;
  const float p0inv= 1./p0;

  int nlev= pp.size();
  if (nlev<2 || td.size() != pp.size() || tt.size() != pp.size()) {
    duct.clear();
    return;
  }

  duct.resize(nlev);
  int   k;
  float tk,pi1,pi2,th1,th2,dz;

  for (k=0; k<nlev; k++) {
    const MetNo::Constants::ewt_calculator ewt(td[k]);
    if (ewt.defined()) {
      const float etd = ewt.value();
      tk= tt[k] + t0;
      ////duct[k]= 77.6*(pp[k]/tk) + 373000.*etd/(tk*tk);
      duct[k]= 77.6*(pp[k]/tk) + 373256.*etd/(tk*tk);
    } else {
      duct[k]= 0;
    }
  }

  pi2= cp*powf(pp[0]*p0inv,rcp);
  th2= cp*(tt[0]+t0)/pi2;

  for (k=1; k<nlev; k++) {
    pi1= pi2;
    th1= th2;
    pi2= cp*powf(pp[k]*p0inv,rcp);
    th2= cp*(tt[k]+t0)/pi2;
    dz= (th1+th2)*0.5*(pi1-pi2)*ginv;
    ////duct[k-1]= (duct[k]-duct[k-1])/(dz*0.001);
    duct[k-1]= (duct[k]-duct[k-1])/(dz*0.001) + 157.;
  }

  if (pp[0]>pp[nlev-1]) {
    duct[nlev-1]= duct[nlev-2];
  } else {
    for (k=nlev-1; k>0; k--)
      duct[k]= duct[k-1];
    duct[0]= duct[1];
  }
}


void VprofPlot::kindex(const vector<float>& pp,
    const vector<float>& tt, const vector<float>& td)
{
  METLIBS_LOG_SCOPE();

  // K-index = (t+td)850 - (t-td)700 - (t)500

  text.kindexFound= false;

  const int nlev= pp.size();
  if (nlev<2 || td.size() != pp.size() || tt.size() != pp.size())
    return;

  const bool pIncreasing= (pp[0]<pp[nlev-1]);
  if (pIncreasing) {
    if (pp[0]>500. || pp[nlev-1]<850.) return;
  } else {
    if (pp[0]<850. || pp[nlev-1]>500.) return;
  }

  const int NP = 3;
  const float pfind[NP]= { 850., 700., 500. };
  float tfind[NP], tdfind[NP];

  for (int n=0; n<NP; n++) {
    int k= 1;
    if (pIncreasing)
      while (k<nlev && pfind[n]>pp[k])
        k++;
    else
      while (k<nlev && pfind[n]<pp[k])
        k++;
    if (k==nlev)
      k--;

    // linear interpolation in exner function
    const float pi1 = tabForValue(pitab, pp[k-1]);
    const float pi2 = tabForValue(pitab, pp[k]);
    const float pi = tabForValue(pitab, pfind[n]);

    tfind[n]=  tt[k-1] + (tt[k]-tt[k-1])*(pi-pi1)/(pi2-pi1);
    tdfind[n]= td[k-1] + (td[k]-td[k-1])*(pi-pi1)/(pi2-pi1);
  }

  // K-index = (t+td)850 - (t-td)700 - (t)500

  text.kindexValue= (tfind[0]+tdfind[0]) - (tfind[1]-tdfind[1]) - tfind[2];
  text.kindexFound= true;
}
