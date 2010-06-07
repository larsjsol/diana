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

#include <diLocationPlot.h>
#include <set>
#include <math.h>

using namespace::miutil;

// constructor
LocationPlot::LocationPlot()
  : Plot()
{
#ifdef DEBUGPRINT
  cerr << "LocationPlot constructor"<< endl;
#endif
  visible= false;
  locdata.locationType= location_unknown;
  numPos= 0;
  px= 0;
  py= 0;
}


// destructor
LocationPlot::~LocationPlot()
{
#ifdef DEBUGPRINT
  cerr << "LocationPlot destructor"<< endl;
#endif
  cleanup();
}


void LocationPlot::cleanup()
{
#ifdef DEBUGPRINT
  cerr << "LocationPlot::cleanup"<< endl;
#endif
  if (px) delete[] px;
  if (py) delete[] py;
  px= py= 0;
  numPos= 0;
  locinfo.clear();
  visible= false;
  selectedName.clear();

  locdata.name.clear();
  locdata.locationType= location_unknown;
  locdata.elements.clear();
  locdata.annotation.clear();
}


bool LocationPlot::setData(const LocationData& locationdata)
{
#ifdef DEBUGPRINT
  cerr << "LocationPlot::setData"<< endl;
#endif
  cleanup();

  // check if sensible input data
  bool ok= true;

  int nelem= locationdata.elements.size();
  if (nelem==0)
  {
	cerr<<"LocationPlot::setData nelem==0!"<<endl;
    return false;
  }

  set<miString> nameset;

  for (int i=0; i<nelem; i++) {
    if (locationdata.elements[i].name.empty())
	{
      cerr<<"LocationPlot::setData " << i << " locationdata.elements[i].name.empty()!"<<endl;
      return false;
	}
    else if (nameset.find(locationdata.elements[i].name)==nameset.end())
      nameset.insert(locationdata.elements[i].name);
    else
	{
	  cerr<<"LocationPlot::setData duplicate name: " << i << " locationdata.elements[i].name!"<< locationdata.elements[i].name << endl;
      return false;
	}
    if (locationdata.elements[i].xpos.size()<2)
	{
		cerr<<"LocationPlot::setData " << i << " locationdata.elements[i].xpos.size()<2!" << locationdata.elements[i].xpos.size() << endl;
		return false;
	}
    if (locationdata.elements[i].xpos.size()!=locationdata.elements[i].ypos.size())
	{
		cerr<<"LocationPlot::setData " << i << " locationdata.elements[i].xpos.size()!=locationdata.elements[i].ypos.size()!" << locationdata.elements[i].xpos.size() << "," << locationdata.elements[i].ypos.size() << endl;
		return false;
	}
  }

  locdata= locationdata;

  numPos= 0;
  locinfo.resize(nelem);
  for (int i=0; i<nelem; i++) {
    locinfo[i].beginpos= numPos;
    numPos+=locdata.elements[i].xpos.size();
    locinfo[i].endpos= numPos;
  }

  px= new float[numPos];
  py= new float[numPos];
  Projection p;
  Rectangle r;
  posArea= Area(p,r);  // impossible area spec

  visible= true;

  // ADC - name appearing on StatusPlotButtons
  plotname= locdata.annotation;

  return true;
}


void LocationPlot::updateOptions(const LocationData& locationdata)
{
#ifdef DEBUGPRINT
  cerr << "LocationPlot::updateOptions"<< endl;
#endif

  // change colour etc. (not positions, name,...)
  locdata.colour=            locationdata.colour;
  locdata.linetype=          locationdata.linetype;
  locdata.linewidth=         locationdata.linewidth;
  locdata.colourSelected=    locationdata.colourSelected;
  locdata.linetypeSelected=  locationdata.linetypeSelected;
  locdata.linewidthSelected= locationdata.linewidthSelected;
}


bool LocationPlot::changeProjection()
{
#ifdef DEBUGPRINT
  cerr << "LocationPlot::changeProjection"<< endl;
#endif

  if (numPos<1 || posArea.P()==area.P()) return false;

  int nlines= locdata.elements.size();
  int np1, np= 0;

  for (int l=0; l<nlines; l++) {
    np1= locdata.elements[l].xpos.size();
    for (int i=0; i<np1; i++) {
      px[np+i]= locdata.elements[l].xpos[i];
      py[np+i]= locdata.elements[l].ypos[i];
    }
    np+=np1;
  }

  if (!gc.geo2xy(area,numPos,px,py)) {
     cerr << "LocationPlot::changeProjection getPoints error" << endl;
     return false;
  }

  posArea= area;

  float xmin,xmax,ymin,ymax,dmax,dx,dy;
  int n1,n2, numLines= locinfo.size();

  for (int l=0; l<numLines; l++) {
    n1= locinfo[l].beginpos;
    n2= locinfo[l].endpos;
    xmin= xmax= px[n1];
    ymin= ymax= py[n1];
    dmax= 0.0f;
    for (int n=n1+1; n<n2; n++) {
      dx= px[n-1]-px[n];
      dy= py[n-1]-py[n];
      if (dmax<dx*dx+dy*dy) dmax=dx*dx+dy*dy;
      if      (xmin>px[n]) xmin= px[n];
      else if (xmax<px[n]) xmax= px[n];
      if      (ymin>py[n]) ymin= py[n];
      else if (ymax<py[n]) ymax= py[n];
    }
    locinfo[l].xmin= xmin;
    locinfo[l].xmax= xmax;
    locinfo[l].ymin= ymin;
    locinfo[l].ymax= ymax;
    locinfo[l].dmax= sqrtf(dmax);
  }

  return true;
}


miString LocationPlot::find(int x, int y)
{
#ifdef DEBUGPRINT
  cerr << "LocationPlot::find"<< endl;
#endif
  const float maxdist= 10.0f;

  miString name;
  float xpos= x*fullrect.width() /pwidth  + fullrect.x1;
  float ypos= y*fullrect.height()/pheight + fullrect.y1;

  float dmax= maxdist*fullrect.width()/pwidth;
  float dmin2= dmax*dmax;
  int   lmin= -1;
  float dx,dy,sx,sy,sdiv;
  int n1,n2,ndiv, numLines= locinfo.size();

  for (int l=0; l<numLines; l++) {
    if (xpos>locinfo[l].xmin-dmax && xpos<locinfo[l].xmax+dmax &&
        ypos>locinfo[l].ymin-dmax && ypos<locinfo[l].ymax+dmax) {
      n1= locinfo[l].beginpos;
      n2= locinfo[l].endpos;
      ndiv= int(locinfo[l].dmax/dmax) + 1;
      if (ndiv<2) {
	for (int n=n1; n<n2; n++) {
	  dx= px[n]-xpos;
	  dy= py[n]-ypos;
	  if (dmin2>dx*dx+dy*dy) {
	    dmin2= dx*dx+dy*dy;
	    lmin= l;
	  }
	}
      } else {
	sdiv= 1.0f/float(ndiv);
	ndiv++;
	for (int n=n1+1; n<n2; n++) {
          sx= (px[n]-px[n-1])*sdiv;
          sy= (py[n]-py[n-1])*sdiv;
	  for (int j=0; j<ndiv; j++) {
	    dx= px[n-1]+sx*float(j)-xpos;
	    dy= py[n-1]+sy*float(j)-ypos;
	    if (dmin2>dx*dx+dy*dy) {
	      dmin2= dx*dx+dy*dy;
	      lmin= l;
	    }
	  }
	}
      }
    }
  }

  if (lmin>=0 && selectedName!=locdata.elements[lmin].name)
    name= selectedName= locdata.elements[lmin].name;

  return name;
}


bool LocationPlot::plot()
{
#ifdef DEBUGPRINT
  cerr << "LocationPlot::plot " << locdata.name << endl;
#endif
  if (!enabled) return false;

  if (numPos<1) return false;

  if (posArea.P()!=area.P()) {
    if (!changeProjection()) return false;
  }

  int n1,n2, numLines= locinfo.size();

  Colour   c1= Colour(locdata.colour);
  Colour   c2= Colour(locdata.colourSelected);
  float    w1= locdata.linewidth;
  float    w2= locdata.linewidthSelected;
  Linetype l1= Linetype(locdata.linetype);
  Linetype l2= Linetype(locdata.linetypeSelected);

  if (c1==backgroundColour) c1= backContrastColour;
  if (c2==backgroundColour) c2= backContrastColour;

  glColor3ubv(c1.RGB());
  glLineWidth(w1);
  if (l1.stipple) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(l1.factor,l1.bmap);
  }

  int lselected= -1;

  for (int l=0; l<numLines; l++) {
    if (locdata.elements[l].name!=selectedName) {
      n1= locinfo[l].beginpos;
      n2= locinfo[l].endpos;
      glBegin(GL_LINE_STRIP);
      for (int n=n1; n<n2; n++)
        glVertex2f(px[n],py[n]);
      glEnd();
    } else {
      lselected= l;
    }
  }

  UpdateOutput();
  glDisable(GL_LINE_STIPPLE);

  if (lselected>=0) {
    glColor3ubv(c2.RGB());
    glLineWidth(w2);
    if (l2.stipple) {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(l2.factor,l2.bmap);
    }
    n1= locinfo[lselected].beginpos;
    n2= locinfo[lselected].endpos;
    glBegin(GL_LINE_STRIP);
    for (int n=n1; n<n2; n++)
      glVertex2f(px[n],py[n]);
    glEnd();
    UpdateOutput();
    glDisable(GL_LINE_STIPPLE);
  }

  return true;
}


void LocationPlot::getAnnotation(miString &str, Colour &col)
{
  if (visible) {
    str = locdata.annotation;
    col = Colour(locdata.colour);
  } else {
    str.clear();
  }
}