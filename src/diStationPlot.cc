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

#include <diStationPlot.h>
#include <math.h>
#include <diFontManager.h>

using namespace::miutil;

//  static members
miString StationPlot::ddString[16];

StationPlot::StationPlot(const vector<float> & lons, const vector<float> & lats) :
  Plot()
{
  init();
  unsigned int n = lons.size();
  if (n > lats.size())
    n = lats.size();
  for (unsigned int i = 0; i < n; i++) {
    addStation(lons[i], lats[i]);
  }
  defineCoordinates();
}

StationPlot::StationPlot(const vector<miString> & names,
    const vector<float> & lons, const vector<float> & lats) :
  Plot()
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::StationPlot(miString,float,float)"<< endl;
#endif
  init();
  unsigned int n = names.size();
  if (n > lons.size())
    n = lons.size();
  if (n > lats.size())
    n = lats.size();
  for (unsigned int i = 0; i < n; i++) {
    addStation(lons[i], lats[i], names[i]);
  }
  defineCoordinates();
}

StationPlot::StationPlot(const vector<miString> & names,
    const vector<float> & lons, const vector<float> & lats, const vector<
        miString> images) :
  Plot()
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::StationPlot(miString,float,float)"<< endl;
#endif
  init();
  unsigned int n = names.size();
  if (n > lons.size())
    n = lons.size();
  if (n > lats.size())
    n = lats.size();
  for (unsigned int i = 0; i < n; i++) {
    addStation(lons[i], lats[i], names[i], images[i]);
  }
  useImage = true;
  defineCoordinates();
}

StationPlot::StationPlot(const vector <Station*> &stations)
{
  init();
  for (unsigned int i = 0; i < stations.size(); ++i)
    addStation(stations[i]);

  useImage = false;
  defineCoordinates();
}

StationPlot::StationPlot(const miString& commondesc, const miString& common,
    const miString& description, int from, const vector<miString>& data)
{
  //   cerr <<"commondesc:"<<commondesc<<endl;
  //   cerr <<"common:"<<common<<endl;
  //   cerr <<"description:"<<description<<endl;
  //   for(int i=0;i<data.size();i++)
  //     cerr <<"data:"<<data[i]<<endl;
  init();

  // Lisbeth: Er dette greit? hilsen Audun
  //if(data.size()==0) return;

  id = from;
  vector<miString> vcommondesc = commondesc.split(":");
  vector<miString> vcommon = common.split(":");
  vector<miString> vdesc = description.split(":");

  if (vcommondesc.size() != vcommon.size()) {
    cerr << "commondesc:" << commondesc << " and common:" << common
        << " do not match" << endl;
    return;
  }

  int num = vcommondesc.size();
  map<miString, int> commonmap;
  for (int i = 0; i < num; i++)
    commonmap[vcommondesc[i]] = i;
  if (commonmap.count("dataset"))
    name = plotname = vcommon[commonmap["dataset"]];
  if (commonmap.count("showname")) {
    useStationNameNormal = (vcommon[commonmap["showname"]] == "true");
    useStationNameSelected = (vcommon[commonmap["showname"]] == "true"
        || vcommon[commonmap["showname"]] == "selected");
  }
  if (commonmap.count("showtext"))
    showText = (vcommon[commonmap["showtext"]] == "true");
  //obsolete
  if (commonmap.count("normal"))
    useStationNameNormal = (vcommon[commonmap["normal"]] == "true");
  //obsolete
  if (commonmap.count("selected"))
    useStationNameSelected = (vcommon[commonmap["selected"]] == "true");
  if (commonmap.count("image")) {
    imageNormal = imageSelected = vcommon[commonmap["image"]];
    useImage = true;
  }
  if (commonmap.count("icon"))
    iconName = vcommon[commonmap["icon"]];
  if (commonmap.count("annotation"))
    annotation = vcommon[commonmap["annotation"]];
  if (commonmap.count("priority"))
    priority = atoi(vcommon[commonmap["priority"]].cStr());

  //decode data
  int ndata = data.size();
  map<miString, int> datamap;
  unsigned int ndesc = vdesc.size();
  for (unsigned int i = 0; i < ndesc; i++)
    datamap[vdesc[i]] = i;
  if (!datamap.count("name") || !datamap.count("lat") || !datamap.count("lon")) {
    cerr << "diStationPlot:" << endl;
    cerr << " positions must contain name:lat:lon" << endl;
    return;
  }
  miString stationname;
  float lat, lon;
  int alpha = 255;
  for (int i = 0; i < ndata; i++) {
    vector<miString> token = data[i].split(":");
    if (token.size() != ndesc)
      continue;
    stationname = token[datamap["name"]];
    lat = atof(token[datamap["lat"]].c_str());
    lon = atof(token[datamap["lon"]].c_str());
    if (datamap.count("alpha"))
      alpha = atoi(token[datamap["alpha"]].cStr());
    if (datamap.count("image")) {
      miString image = token[datamap["image"]];
      addStation(lon, lat, stationname, image, alpha);
      useImage = true;
    } else {
      addStation(lon, lat, stationname);
    }
  }

  defineCoordinates();

  hide();

}

void StationPlot::init()
{
  // Add a default plot area.
  StationArea area(-90.0, 90.0, -180.0, 180.0);
  stationAreas.push_back(area);

  //coordinates to be plotted
  pi = acosf(-1.0);
  show();
  useImage = true;
  useStationNameNormal = false;
  useStationNameSelected = false;
  showText = true;
  textColour = Colour("white");
  textSize = 16;
  textStyle = "bold";
  name = "vprof";
  id = -1;
  priority = 1;
  index = -1;
  editIndex = -1;
}

StationPlot::~StationPlot()
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::~StationPlot" << endl;
#endif
  int n = stations.size();
  for (int i = 0; i < n; i++) {
    delete stations[i];
  }
  stations.clear();
  stationAreas.clear();
}

// void StationPlot::addStation(const miString newname){
//   //at the moment, this should only be called from constructor, since
//   //define coordinates must be called to actually plot stations
//   Station * newStation = new Station;
//   newStation->name = newname;
//   newStation->lon = 0;
//   newStation->lat = 0;
//   newStation->isVisible=true;
//   newStation->isSelected=false;
//   newStation->edit=false;
//   stations.push_back(newStation);
// }


void StationPlot::addStation(const float lon, const float lat,
    const miString newname, const miString newimage, int alpha, float scale)
{
  //at the moment, this should only be called from constructor, since
  //define coordinates must be called to actually plot stations
  Station * newStation = new Station;
  if (newname.exists()) {
    newStation->name = newname;
  } else {
    miCoordinates coordinates(lon, lat);
    newStation->name = coordinates.str();
  }
  newStation->lon = lon;
  newStation->lat = lat;
  newStation->alpha = alpha;
  newStation->scale = scale;
  newStation->isSelected = false;
  if (newimage == "HIDE") {
    newStation->isVisible = false;
  } else {
    newStation->isVisible = true;
    newStation->image = newimage;
  }
  newStation->edit = false;

  addStation(newStation);
}

void StationPlot::addStation(Station* station)
{
  stations.push_back(station);

  // Add the station to the area tree.
  stationAreas[0].addStation(station);
}

bool StationPlot::plot()
{
  /* Plot stations at positions xplot,yplot if stations[i]->isVisible
   different plotting options
   if stations[i]->image not empty, plot image
   else if useImage==true, plot imageNormal or imageSelected
   else plot red crosses, yellow circle around selected station
   if useStationNameNormal/useStationNameSelected==true plot name
   */
#ifdef DEBUGPRINT
  cerr <<"StationPlot::plot "<< name << endl;
#endif
  if (!enabled || !visible)
    return false;

  //Circle
  GLfloat xc, yc;
  GLfloat radius = 0.3;
  circle = glGenLists(1);
  glNewList(circle, GL_COMPILE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_POLYGON);
  for (int i = 0; i < 100; i++) {
    xc = radius * cos(i * 2 * pi / 100.0);
    yc = radius * sin(i * 2 * pi / 100.0);
    glVertex2f(xc, yc);
  }
  glEnd();
  glEndList();

  vector<int> selected; //index of selected stations

  //first loop, only unselected stations
  int n = stations.size();
  for (int i = 0; i < n; i++) {
    if (!stations[i]->isVisible)
      continue;
    if (stations[i]->isSelected) {
      selected.push_back(i);
      continue;
    }
    plotStation(i);
  }

  //second loop, only selected stations
  int m = selected.size();
  for (int i = 0; i < m; i++) {
    plotStation(selected[i]);
  }

  return true;
}

void StationPlot::plotStation(int i)
{
#ifdef DEBUGPRINT
  cerr <<"StationPlot::plotStation "<< i <<endl;
#endif

  float h = 0, w = 0; //height for displaying text
  float x = xplot[i];
  float y = yplot[i];
  bool plotted = true;

  if (useImage) {
    //use either stations[i]->image or imageNormal/imageSelected
    //cerr << "useImage=true" << endl;
    if (!stations[i]->image.empty() && stations[i]->image2.empty()) {
      if (stations[i]->image == "wind") {
        if (stations[i]->isSelected)
          h = 40 * fullrect.height() / (pheight > 0 ? pheight * 1.0 : 1.0);
        else
          h = 30 * fullrect.height() / (pheight > 0 ? pheight * 1.0 : 1.0);
        w = 30 * fullrect.width() / (pwidth > 0 ? pwidth * 1.0 : 1.0);
        //  	if(stations[i]->edit)
        //  	  glPlot(Station::redCircle,x,y,h,w);
        plotWind(i, x, y);
      } else {
        h = ig.height(stations[i]->image);
        w = ig.width(stations[i]->image);
        // 	if(stations[i]->edit)
        // 	  glPlot(Station::redCircle,x,y,h,w);
        if (!ig.plotImage(stations[i]->image, x, y, true, stations[i]->scale,
            stations[i]->alpha))
          plotted = false;
      }
      if (stations[i]->isSelected && stations[i]->image != "wind")
        glColor3ub(255, 0, 0); //red
        glPlot(Station::noStatus, x, y, w, h);
    } else if (!stations[i]->image.empty() && !stations[i]->image2.empty()) {
      float h1 = ig.height(stations[i]->image);
      float h2 = ig.height(stations[i]->image2);
      if (h1 > h2)
        h = h1;
      else
        h = h2;
      float w1 = ig.width(stations[i]->image);
      float w2 = ig.width(stations[i]->image2);
      if (w1 > w2)
        w = 2 * w1;
      else
        w = 2 * w2;
      glColor3ub(128, 128, 128); //grey
      glPlot(Station::noStatus, x, y, w, h);
      if (!ig.plotImage(stations[i]->image, x - w1 / 2, y, true, stations[i]->scale,
          stations[i]->alpha))
        plotted = false;
      if (!ig.plotImage(stations[i]->image2, x + w2 / 2, y, true, stations[i]->scale,
          stations[i]->alpha))
        plotted = false;
      if (stations[i]->isSelected)
        glColor3ub(255, 0, 0); //red
        glPlot(Station::noStatus, x, y, w, h);
    } else if (!stations[i]->isSelected && !imageNormal.empty()) {
      //otherwise plot images for selected/normal stations
      if (!ig.plotImage(imageNormal, x, y, true, stations[i]->scale, stations[i]->alpha))
        plotted = false;
      h = ig.height(imageNormal);
    } else if (stations[i]->isSelected && !imageSelected.empty()) {
      if (!ig.plotImage(imageSelected, x, y, true, stations[i]->scale, stations[i]->alpha))
        plotted = false;
      h = ig.height(imageSelected);
    } else {
      //if no image plot crosses and circles for selected/normal stations
      //cerr << "useImage=false" << endl;
      glPlot(Station::failed, x, y, w, h, stations[i]->isSelected);
    }

    //if something went wrong,
    //plot crosses and circles for selected/normal stations
    if (!plotted) {
      glPlot(Station::failed, x, y, w, h, stations[i]->isSelected);
      plotted = true;
    }
  } else if (stations[i]->status != Station::noStatus) {
    glPlot(stations[i]->status, x, y, w, h, stations[i]->isSelected);
  }

  if (useStationNameNormal && !stations[i]->isSelected) {
    float cw, ch;
    glColor3ub(0, 0, 0); //black
    fp->set("BITMAPFONT", "normal", 10);
    fp->getStringSize(stations[i]->name.c_str(), cw, ch);
    fp->drawStr(stations[i]->name.c_str(), x - cw / 2, y + h / 2, 0.0);
  } else if (useStationNameSelected && stations[i]->isSelected) {
    float cw, ch;
    fp->set("BITMAPFONT", "normal", 10);
    fp->getStringSize(stations[i]->name.c_str(), cw, ch);
    glColor3ub(255, 255, 255); //white
    glPlot(Station::noStatus, x, y + h / 2 + ch * 0.1, cw * 0.6, ch * 1.1);
    glColor3ub(0, 0, 0); //black
    fp->drawStr(stations[i]->name.c_str(), x - cw / 2, y + h / 2 + ch * 0.35,
        0.0);
  }

  if (showText || stations[i]->isSelected) {
    int nt = stations[i]->vsText.size();
    for (int it = 0; it < nt; it++) {
      float cw, ch;
      glColor4ubv(textColour.RGBA());
      miString text = stations[i]->vsText[it].text;
      fp->set("BITMAPFONT", textStyle, textSize);
      fp->getStringSize(text.c_str(), cw, ch);
      if (stations[i]->vsText[it].hAlign == align_center)
        fp->drawStr(text.c_str(), x - cw / 2, y - ch / 4, 0.0);
      else if (stations[i]->vsText[it].hAlign == align_top)
        fp->drawStr(text.c_str(), x - cw / 2, y + h / 2, 0.0);
      else if (stations[i]->vsText[it].hAlign == align_bottom) {
        if (stations[i]->isSelected)
          glColor3ub(255, 255, 255); //white
          glPlot(Station::noStatus, x, y - h / 1.9 - ch * 1.0, cw * 0.5 + 0.2 * w,
              ch);
        glColor4ubv(textColour.RGBA());
        fp->drawStr(text.c_str(), x - cw / 2, y - h / 1.9 - ch * 0.7, 0.0);
      }
    }
  }
}

void StationPlot::hide()
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::hide" << endl;
#endif

  visible = false;
  unselect();

}

void StationPlot::show()
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::show" << endl;
#endif

  visible = true;
  unselect();
  changeProjection();

}

void StationPlot::unselect()
{

  int n = stations.size();
  for (int i = 0; i < n; i++)
    stations[i]->isSelected = false;
  index = -1;

}

bool StationPlot::isVisible()
{
  return visible;
}

void StationPlot::defineCoordinates()
{
  //should be called from constructor and when new stations have
  //been added
#ifdef DEBUGPRINT
  cerr << "StationPlot::defineCoordinates()" << endl;
#endif
  // correct spec. when making Projection for long/lat coordinates
  //positions from lat/lon will be converted in changeprojection
  int npos = stations.size();
  xplot.clear();
  yplot.clear();
  for (int i = 0; i < npos; i++) {
    xplot.push_back(stations[i]->lon);
    yplot.push_back(stations[i]->lat);
  }
  changeProjection();
}

bool StationPlot::changeProjection()
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::changeProjection" << endl;
  cerr << "Change projection to: "<< area << endl;
#endif

  int npos = xplot.size();
  if (npos == 0) {
    return false;
  }

  float *xpos = new float[npos];
  float *ypos = new float[npos];
  for (int i = 0; i < npos; i++) {
    xpos[i] = stations[i]->lon;
    ypos[i] = stations[i]->lat;
  }

  if (area.P().convertFromGeographic(npos, xpos, ypos) != 0 ) {
    cerr << "changeProjection: getPoints error" << endl;
    delete[] xpos;
    delete[] ypos;
    return false;
  }
  for (int i = 0; i < npos; i++) {
    xplot[i] = xpos[i];
    yplot[i] = ypos[i];
  }

  float *u = new float[npos];
  float *v = new float[npos];

  for (int i = 0; i < npos; i++) {
    u[i] = 0;
    v[i] = 10;
  }
//TODO: crashes, but why? Only do this if "wind"-image exists
//  gc.getVectors(oldarea, area, npos, xpos, ypos, u, v);

  for (int i = 0; i < npos; i++) {
    if (stations[i]->image == "wind") {
      int angle = (int) (atan2f(u[i], v[i]) * 180 / pi);
      int dd = stations[i]->north + angle;
      if (dd < 1)
        dd += 360;
      if (dd > 360)
        dd -= 360;
      stations[i]->north = dd;;
      dd = stations[i]->dd + angle;
      if (dd < 1)
        dd += 360;
      if (dd > 360)
        dd -= 360;
      stations[i]->dd = dd;
    }
  }

  delete[] xpos;
  delete[] ypos;
  delete[] u;
  delete[] v;

  return true;
}

vector<Station*> StationPlot::getStations() const
{
  return stations;
}

Station* StationPlot::stationAt(int x, int y)
{
  vector<Station*> found = stationsAt(x, y);

  if (found.size() > 0) {
    float xpos = x * fullrect.width() / pwidth + fullrect.x1;
    float ypos = y * fullrect.height() / pheight + fullrect.y1;

    float min_r = 10.0f * fullrect.width() / pwidth;
    min_r = powf(min_r, 2);
    int min_i = 0;

    // Find the closest station to the point within a given radius.
    for (unsigned int i = 0; i < found.size(); ++i) {
      float sx = found[i]->lon, sy = found[i]->lat;
      if (area.P().convertFromGeographic(1, &sx, &sy) == 0) {
        float r = powf(xpos - sx, 2) + powf(ypos - sy, 2);
        if (r < min_r) {
          min_r = r;
          min_i = i;
        }
      }
    }

    return found[min_i];
  }

  return 0;
}

vector<Station*> StationPlot::stationsAt(int x, int y)
{
  float xpos = x * fullrect.width() / pwidth + fullrect.x1;
  float ypos = y * fullrect.height() / pheight + fullrect.y1;

  float min_r = 10.0f * fullrect.width() / pwidth;
  min_r = powf(min_r, 2);

  vector<Station*> within;

  float gx = xpos, gy = ypos;
  if (!area.P().convertToGeographic(1, &gx, &gy)) {
    vector<Station*> found = stationAreas[0].findStations(gy, gx);

    for (unsigned int i = 0; i < found.size(); ++i) {
      if (found[i]->isVisible) {
        float sx = found[i]->lon, sy = found[i]->lat;
        if (area.P().convertFromGeographic(1, &sx, &sy) == 0) {
          float r = powf(xpos - sx, 2) + powf(ypos - sy, 2);
          if (r < min_r)
            within.push_back(found[i]);
        }
      }
    }
  }

  return within;
}

vector<miString> StationPlot::findStation(int x, int y, bool add)
{
  vector<miString> stationstring;

  if (!visible || !enabled)
    return stationstring;

  Station* found = stationAt(x, y);

  if (found && found != stations[index]) {
    add = found->isSelected || add;
    setSelectedStation(found->name, add);

    for (unsigned int i = 0; i < xplot.size(); i++) {
      if (stations[i]->isSelected)
        stationstring.push_back(stations[i]->name);
    }
  }

  return stationstring;
}

float StationPlot::getImageScale(int i)
{
  return stations[i]->scale;
}

vector<Station*> StationPlot::getSelectedStations() const
{
  vector<Station*> selected;
  for (unsigned int i = 0; i < stations.size(); ++i) {
    if (stations[i]->isSelected)
      selected.push_back(stations[i]);
  }
  return stations;
}

int StationPlot::setSelectedStation(miString station, bool add)
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::setSelectedStation" << station << endl;
#endif

  int n = stations.size();
  for (int i = 0; i < n; i++)
    if (stations[i]->name == station) {
      return setSelectedStation(i);
    }
  return -1;
}

/**
 Selects the station specified by its index \a i. If \a add is true then
 the station will be added to the existing selection; otherwise, the
 existing selection will be cleared before the station is added to it.

 Returns the index of the station if it was added to the selection, or -1
 if it could not be added.
 */
int StationPlot::setSelectedStation(int i, bool add)
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::setSelectedStation: " << i << endl;
#endif
  int n = stations.size();

  //remove old selections
  if (!add) {
    for (int j = 0; j < n; j++) {
      stations[j]->isSelected = false;
    }
  }

  //select
  if (i < n && i > -1) {
    stations[i]->isSelected = true;
    index = i;

    //edit
    if (stations[i]->edit)
      editIndex = i;

    return i;
  }
  return -1;
}

void StationPlot::getStationPlotAnnotation(miString &str, Colour &col)
{
  if (visible) {
    str = annotation;
    Colour c("red");
    col = c;
  } else {
    str.erase();
  }
}

void StationPlot::setStationPlotAnnotation(miString &str)
{
  annotation = str;
}

void StationPlot::setName(miString nm)
{
  name = nm;
  if (!plotname.exists())
    plotname = name;
}

void StationPlot::setImage(miString im1)
{
  imageNormal = im1;
  imageSelected = im1;
  useImage = true;
}

void StationPlot::setImage(miString im1, miString im2)
{
  imageNormal = im1;
  imageSelected = im2;
  useImage = true;
}

void StationPlot::setImageScale(float new_scale)
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::setImageScale "
  << endl;
#endif
  int n = stations.size();
  for (int i = 0; i < n; i++) {
    stations[i]->scale = new_scale;
  }
}

void StationPlot::clearText()
{
#ifdef DEBUGPRINT
  cerr << "StationPlot::clearText "
  << endl;
#endif
  int n = stations.size();
  for (int i = 0; i < n; i++) {
    stations[i]->vsText.clear();
  }
}

void StationPlot::setUseStationName(bool normal, bool selected)
{
  useStationNameNormal = normal;
  useStationNameSelected = selected;
}

void StationPlot::setEditStations(const vector<miString>& st)
{
  //  cerr <<"setEditStations:"<<st.size()<<endl;

  int m = st.size();

  int n = stations.size();
  for (int i = 0; i < n; i++) {
    int j = 0;
    while (j < m && st[j] != stations[i]->name)
      j++;
    if (j < m) {
      stations[i]->edit = true;
    } else {
      stations[i]->edit = false;
    }
  }

  editIndex = 0;

  if (!m) { //no edit stations
    editIndex = -1;
    return;
  }

  //   if(editIndex >= n || editIndex<0 || !stations[editIndex]->edit){
  //     int i=0;
  //     while(i<n && !stations[i]->edit) i++;
  //     setSelectedStation(i);
  //   }

}

bool StationPlot::getEditStation(int step, miString& nname, int& iid, vector<
    miString>& sstations, bool& updateArea)
{
  //  cerr <<"getEditStations:"<<step<< "  editIndex:"<<editIndex<<endl;

  if (editIndex < 0)
    return false;

  bool add = (nname == "add");
  nname = name;
  iid = id;

  int n = stations.size();

  if (step == 0) {
    for (int i = 0; i < n; i++)
      if (stations[i]->isSelected && stations[i]->edit) {
        sstations.push_back(stations[i]->name);
      }
    return stations.size();
  }

  int i;
  if (step > 0) {
    i = editIndex + 1;
    while (i < n && (!stations[i]->edit))
      i++;
    if (i == n) {
      i = 0;
      while (i < n && (!stations[i]->edit))
        i++;
    }
  } else {
    i = editIndex - 1;
    while (i > -1 && !stations[i]->edit)
      i--;
    if (i == -1) {
      i = n - 1;
      while (i > -1 && !stations[i]->edit)
        i--;
    }
  }

  if (i < n && i > -1) {
    editIndex = i;
    setSelectedStation(i, add);

    //if stations[i] isn't on the map, pan the map
    Rectangle r = area.R();
    if (!r.isinside(xplot[i], yplot[i])) {
      r.putinside(xplot[i], yplot[i]);
      area.setR(r);
      updateArea = true;; //area need update
    }

    for (int i = 0; i < n; i++)
      if (stations[i]->isSelected && stations[i]->edit) {
        sstations.push_back(stations[i]->name);
      }
    return true;
  }

  return false;
}

bool StationPlot::stationCommand(const miString& command,
    vector<miString>& data, const miString& misc)
{
  //   cerr <<"Command:"<<command<<endl;

  if (command == "changeImageandText") {
    vector<miString> description = misc.split(":");
    int ndesc = description.size();
    map<miString, int> datamap;
    for (int i = 0; i < ndesc; i++)
      datamap[description[i]] = i;
    if (!datamap.count("name")) {
      cerr << " StationPlot::stationCommand: missing name of station" << endl;
      return false;
    }
    bool chImage, chImage2, chText, defAlign, ch_dd, ch_ff, ch_colour, ch_alpha;
    ;
    chImage = chImage2 = chText = defAlign = ch_dd = ch_ff = ch_colour
        = ch_alpha = false;
    if (datamap.count("image"))
      chImage = true;
    if (datamap.count("image2"))
      chImage2 = true;
    if (datamap.count("text"))
      chText = true;
    if (datamap.count("alignment"))
      defAlign = true;
    if (datamap.count("dd"))
      ch_dd = true;
    if (datamap.count("ff"))
      ch_ff = true;
    if (datamap.count("colour"))
      ch_colour = true;
    if (datamap.count("alpha"))
      ch_alpha = true;

    //decode data
    int n = data.size();
    miString name, image, image2, text, alignment;
    int dd=0, ff=0, alpha=0;
    Colour colour;
    for (int i = 0; i < n; i++) {
      //       cerr <<"StationPlot::stationCommand:data:"<<data[i]<<endl;
      vector<miString> token = data[i].split(":");
      if (token.size() < description.size()) {
        cerr << "StationPlot::stationCommand: Description:" << misc
            << " and data:" << data[i] << " do not match" << endl;
        continue;
      }
      name = token[datamap["name"]];
      if (chImage)
        image = token[datamap["image"]];
      if (chImage2)
        image2 = token[datamap["image2"]];
      if (chText)
        text = token[datamap["text"]];
      if (defAlign)
        alignment = token[datamap["alignment"]];
      if (ch_dd)
        dd = atoi(token[datamap["dd"]].cStr());
      if (ch_ff)
        ff = atoi(token[datamap["ff"]].cStr());
      if (ch_colour)
        colour = Colour(token[datamap["colour"]]);
      if (ch_alpha)
        alpha = atoi(token[datamap["alpha"]].cStr());

      if (image == "_")
        image = "";
      if (image2 == "_")
        image2 = "";

      //find station
      int m = stations.size();
      for (int i = 0; i < m; i++) {
        if (stations[i]->name == name) {
          stations[i]->vsText.clear(); //clear text
          if (chImage) {
            if (image == "HIDE") {
              stations[i]->isVisible = false;
            } else {
              stations[i]->isVisible = true;
              stations[i]->image = image;
            }
          }
          stations[i]->image2 = image2;

          if (ch_alpha)
            stations[i]->alpha = alpha;

          if (chText) {
            stationText stext;
            stext.hAlign = align_center; //default
            if (text != "_") { //underscore means no text
              stext.text = text;
              if (alignment == "center")
                stext.hAlign = align_center;
              else if (alignment == "top")
                stext.hAlign = align_top;
              else if (alignment == "bottom")
                stext.hAlign = align_bottom;
              stations[i]->vsText.push_back(stext);
            }
          }

          if (ch_dd && ch_ff) {
            //init ddString
            if (!ddString[0].exists()) {
              ddString[0] = "N";
              ddString[1] = "NN�";
              ddString[2] = "N�";
              ddString[3] = "�N�";
              ddString[4] = "�";
              ddString[5] = "�S�";
              ddString[6] = "S�";
              ddString[7] = "SS�";
              ddString[8] = "S";
              ddString[9] = "SSV";
              ddString[10] = "SV";
              ddString[11] = "VSV";
              ddString[12] = "V";
              ddString[13] = "VNV";
              ddString[14] = "NV";
              ddString[15] = "NNV";
            }
            stations[i]->image = "wind";
            stations[i]->isVisible = true;
            //change projection from geo to current
            int num = 1;
            float* xx = new float[num];
            float* yy = new float[num];
            float* u = new float[num];
            float* v = new float[num];
            // 	    xx[0] = stations[i]->lon;
            // 	    yy[0] = stations[i]->lat;
            xx[0] = xplot[i];
            yy[0] = yplot[i];
            u[0] = 0;
            v[0] = 10;
            gc.geov2xy(area, num, xx, yy, u, v);
            int angle = (int) (atan2f(u[0], v[0]) * 180 / pi);
            dd += angle;
            if (dd < 1)
              dd += 360;
            if (dd > 360)
              dd -= 360;
            stations[i]->dd = dd;
            stations[i]->ff = ff;
            stations[i]->north = angle;
            delete[] xx;
            delete[] yy;
            delete[] v;
            delete[] u;
            if (ch_colour)
              stations[i]->colour = colour;
          }
          break;
        }
      }
    }

    return true;

  } else if (command == "setEditStations") {
    setEditStations(data);
    return true;

  } else if (command == "annotation" && data.size() > 0) {
    setStationPlotAnnotation(data[0]);
    return true;
  }

  else if (command == "setSelectedStation" && data.size() > 0) {
    setSelectedStation(data[0]);
    return true;
  }

  else if (command == "showPositionName" && data.size() > 0) {
    //    vector<miString> token = data[0].split(":"); //new syntax
    vector<miString> token = data[0].split(":");
    //Obsolete
    if (token.size() < 2)
      token = data[0].split(",");
    //
    if (token.size() < 2)
      return false;
    bool normal = false, selected = false;
    if (token[0] == "true")
      normal = true;
    if (token[1] == "true")
      selected = true;
    if (token.size() == 3) {
      if (token[2] == "true")
        useImage = true;
      else
        useImage = false;
    }
    setUseStationName(normal, selected);

    return true;
  }

  else if (command == "showPositionText" && data.size() > 0) {
    //    cerr <<"StationPlot showPositionText:"<< misc<<endl;

    vector<miString> description = misc.split(":");
    int ndesc = description.size();
    map<miString, int> datamap;
    for (int i = 0; i < ndesc; i++)
      datamap[description[i]] = i;
    if (!datamap.count("showtext")) {
      cerr << " StationPlot::stationCommand: description must contain showtext"
          << endl;
      return false;
    }
    vector<miString> token = data[0].split(":");
    if (token.size() < description.size()) {
      cerr << "StationPlot::stationCommand: Description:" << misc
          << " and data:" << data[0] << " do not match" << endl;
      return false;
    }
    showText = (token[datamap["showtext"]] == "true"
        || token[datamap["showtext"]] == "normal");
    if (datamap.count("colour"))
      textColour = Colour(token[datamap["colour"]]);
    if (datamap.count("size"))
      textSize = atoi(token[datamap["size"]].cStr());
    if (datamap.count("style"))
      textStyle = token[datamap["style"]].cStr();

    return true;
  }

  return false;
}

bool StationPlot::stationCommand(const miString& command)
{

  if (command == "show") {
    show();
    return true;
  }

  else if (command == "hide") {
    hide();
    return true;
  }

  else if (command == "unselect") {
    unselect();
    return true;
  }

  return false;
}

miString StationPlot::stationRequest(const miString& command)
{
  ostringstream ost;

  if (command == "selected") {
    int n = stations.size();
    for (int i = 0; i < n; i++)
      if (stations[i]->isSelected)
        ost << ":" << stations[i]->name.cStr();
    ost << ":" << name.cStr() << ":" << id;
  }

  return ost.str();
}

void StationPlot::glPlot(Station::Status tp, float x, float y, float w, float h, bool selected)
{
  //called from StationPlot::plotStation: Add GL things to plot here.
  float linewidth, scale, r;
  GLfloat xc, yc;
  GLfloat radius;
  switch (tp) {
  case Station::unknown:
    linewidth = 2;
    scale = fullrect.width() / pwidth * 1.5;
    r = linewidth * scale;
    //plot grey transparent square
    glColor4ub(100, 100, 100, 50);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glVertex2f(x - r, y - r);
    glVertex2f(x - r, y + r);
    glVertex2f(x + r, y + r);
    glVertex2f(x + r, y - r);
    glEnd();
    glDisable(GL_BLEND);
    break;
  case Station::failed:
    linewidth = 4;
    glLineWidth(linewidth);
    scale = fullrect.width() / pwidth * 1.5;
    r = linewidth * scale;
    h = 1.5 * r;
    //plot crosses
    glColor3ub(255, 0, 0); //red
    glBegin(GL_LINES);
    glVertex2f(x - r, y - r);
    glVertex2f(x + r, y + r);
    glVertex2f(x + r, y - r);
    glVertex2f(x - r, y + r);
    glEnd();
    break;
  case Station::underRepair:
    linewidth = 4;
    glLineWidth(linewidth);
    scale = fullrect.width() / pwidth * 1.5;
    r = linewidth * scale;
    h = 1.5 * r;
    //plot crosses
    glColor3ub(255, 255, 0); //yellow
    glBegin(GL_LINES);
    glVertex2f(x - r, y - r);
    glVertex2f(x + r, y + r);
    glVertex2f(x + r, y - r);
    glVertex2f(x - r, y + r);
    glEnd();
    break;
  case Station::working:
    linewidth = 4;
    glLineWidth(linewidth);
    scale = fullrect.width() / pwidth * 1.5;
    r = linewidth * scale;
    h = 1.5 * r;
    //plot crosses
    glColor3ub(0, 255, 0); //green
    glBegin(GL_LINES);
    glVertex2f(x - r, y - r);
    glVertex2f(x + r, y + r);
    glVertex2f(x + r, y - r);
    glVertex2f(x - r, y + r);
    glEnd();
    break;
  case Station::noStatus:
  default:
    ;
  }

  // Plot a yellow circle if the station is selected.
  if (selected) {
    linewidth = 4;
    glLineWidth(linewidth);
    scale = fullrect.width() / pwidth * 1.5;
    r = linewidth * scale;
    radius = 1.5 * r;
    glColor3ub(255, 255, 0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
      xc = radius * cos(i * 2 * pi / 100.0);
      yc = radius * sin(i * 2 * pi / 100.0);
      glVertex2f(x + xc, y + yc);
    }
    glEnd();
  }
}

void StationPlot::plotWind(int ii, float x, float y, bool classic, float scale)
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Colour colour = stations[ii]->colour;
  colour.set(Colour::alpha, stations[ii]->alpha);

  int dd = stations[ii]->dd;
  int ff = stations[ii]->ff;
  float radius = scale * 14 * fullrect.width() / (pwidth > 0 ? pwidth * 1.0
      : 1.0);

  glPushMatrix();
  glTranslatef(x, y, 0.0);
  glScalef(radius, radius, 0.0);

  if (stations[ii]->isSelected) {
    //compass
    GLfloat xc, yc;
    int linewidth = 1;
    glLineWidth(linewidth);
    glColor3ub(0, 0, 0);
    //Circle
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
      xc = cos(i * 2 * pi / 100.0);
      yc = sin(i * 2 * pi / 100.0);
      glVertex2f(xc, yc);
    }
    glEnd();

    glPushMatrix();
    glRotatef(-1 * stations[ii]->north, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(0.1, 1);
    glVertex2f(0.1, 1.5);
    glVertex2f(-0.1, 1.0);
    glVertex2f(-0.1, 1.5);

    glVertex2f(0.0, 1.5);
    glVertex2f(0.4, 1.1);
    glVertex2f(0.0, 1.5);
    glVertex2f(-0.4, 1.1);

    glVertex2f(0.05, -1);
    glVertex2f(0.05, -1.3);
    glVertex2f(-0.05, -1);
    glVertex2f(-0.05, -1.3);
    glEnd();

    for (int i = 0; i < 7; i++) {
      glRotatef(45.0, 0.0, 0.0, 1.0);
      glBegin(GL_LINES);
      glVertex2f(0, 1.0);
      glVertex2f(0, 1.3);
      glEnd();
    }
    glPopMatrix();

  }

  if (ff < 1) {
    glRotatef(45, 0.0, 0.0, 1.0);
  } else {
    glRotatef(-1 * (dd + 180), 0.0, 0.0, 1.0);
  }

  glColor3ub(0, 0, 0); //black
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  // ff in m/s
  if (ff < 1) {
    glBegin(GL_POLYGON);
    glVertex2f(-0.1, 0.5);
    glVertex2f(-0.1, -0.5);
    glVertex2f(0.1, -0.5);
    glVertex2f(0.1, 0.5);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-0.5, 0.1);
    glVertex2f(-0.5, -0.1);
    glVertex2f(0.5, -0.1);
    glVertex2f(0.5, 0.1);
    glEnd();

  } else {
    if (classic) {
      // main body
      glColor4ubv(colour.RGBA());
      glBegin(GL_POLYGON);
      glVertex2f(-0.1, 0.6);
      glVertex2f(-0.1, -1.0);
      glVertex2f(0.1, -1.0);
      glVertex2f(0.1, 0.6);
      glEnd();
      // arrowhead
      glBegin(GL_POLYGON);
      glVertex2f(0.5, 0.6);
      glVertex2f(0.0, 1.0);
      glVertex2f(-0.5, 0.6);
      glEnd();

      // 22-40 knots
      if (ff >= 11) {
        glBegin(GL_POLYGON);
        glVertex2f(-0.1, -1.0);
        glVertex2f(-0.7, -1.4);
        glVertex2f(-0.7, -1.2);
        glVertex2f(-0.1, -0.8);
        glEnd();
      }
      // 41-63 knots
      if (ff >= 21) {
        glBegin(GL_POLYGON);
        glVertex2f(-0.1, -0.6);
        glVertex2f(-0.7, -1.0);
        glVertex2f(-0.7, -0.8);
        glVertex2f(-0.1, -0.4);
        glEnd();
      }

    } else {
      glShadeModel(GL_SMOOTH);
      glColor4ubv(colour.RGBA());
      // arrowhead
      glBegin(GL_POLYGON);
      glVertex2f(1.0, 0.5);
      glVertex2f(0.0, 1.0);
      glVertex2f(-1.0, 0.5);
      glEnd();
      // main body
      glBegin(GL_POLYGON);
      glVertex2f(-0.5, 0.5);
      glVertex2f(0.5, 0.5);
      glVertex2f(0.5, -0.3);
      glVertex2f(-0.5, -0.3);
      glEnd();

      glBegin(GL_POLYGON);
      glVertex2f(0.5, -0.3);
      glColor4f(colour.fR(), colour.fG(), colour.fB(), 0.3);
      glVertex2f(0.5, -1.0);
      glVertex2f(-0.5, -1.0);
      glColor4ubv(colour.RGBA());
      glVertex2f(-0.5, -0.3);
      glEnd();

      glShadeModel(GL_FLAT);
      glColor4f(1.0, 1.0, 1.0, 1.0);
    }
  }

  glPopMatrix();

  if (ff > 0 && !classic) {
    fp->set("BITMAPFONT", "normal", 10);
    float sW, sH;
    ostringstream ost;
    ost << ff;
    fp->getStringSize(ost.str().c_str(), sW, sH);
    float sx = x - 0.45 * sW;
    float sy = y - 0.35 * sH;
    glColor4f(1.0, 1.0, 1.0, 1.0); //white
    fp->drawStr(ost.str().c_str(), sx, sy);
  }

  if (stations[ii]->isSelected) {
    //wind direction
    fp->set("BITMAPFONT", "normal", 10);
    float sW, sH;
    dd = (dd - stations[ii]->north);
    float df = dd;
    df += 11.25;
    df /= 22.5;
    dd = (int) df;
    if (dd < 0)
      dd += 16;
    if (dd > 15)
      dd -= 16;
    fp->getStringSize(ddString[dd].c_str(), sW, sH);
    float sx = x - 0.45 * sW;
    float sy = y - 2.35 * sH;
    glColor3ub(255, 255, 255); //white
    glPlot(Station::noStatus, x, y - 2.5 * sH, sW * 0.6, sH * 1.1);
    glColor4f(0.0, 0.0, 0.0, 1.0); //black
    fp->drawStr(ddString[dd].c_str(), sx, sy);
  }

  glDisable(GL_BLEND);
}

StationArea::StationArea(float minLat, float maxLat, float minLon, float maxLon) :
  minLat(minLat), maxLat(maxLat), minLon(minLon), maxLon(maxLon)
{
}

vector<Station*> StationArea::findStations(float lat, float lon) const
{
  for (unsigned int i = 0; i < areas.size(); ++i) {
    if (lat >= areas[i].minLat && lat < areas[i].maxLat && lon >= areas[i].minLon && lon < areas[i].maxLon)
      return areas[i].findStations(lat, lon);
  }
  return stations;
}

Station* StationArea::findStation(float lat, float lon) const
{
  vector<Station*> found = findStations(lat, lon);
  if (found.size() > 0)
    return *(found.begin());
  else
    return 0;
}

void StationArea::addStation(Station* station)
{
  if (areas.size() == 0) {
    stations.push_back(station);

    // ### TODO: Handle the case where there are more than 10 stations at the same location.

    if (stations.size() > 10) {
      // If there are more than 10 stations in the area, split up the area and
      // move each of the stations into the appropriate subarea.
      StationArea topLeft((minLat + maxLat)/2, maxLat, minLon, (minLon + maxLon)/2);
      areas.push_back(topLeft);

      StationArea topRight((minLat + maxLat)/2, maxLat, (minLon + maxLon)/2, maxLon);
      areas.push_back(topRight);

      StationArea bottomLeft(minLat, (minLat + maxLat)/2, minLon, (minLon + maxLon)/2);
      areas.push_back(bottomLeft);

      StationArea bottomRight(minLat, (minLat + maxLat)/2, (minLon + maxLon)/2, maxLon);
      areas.push_back(bottomRight);

      // Move all the stations into the subareas.
      for (unsigned int i = 0; i < stations.size(); ++i) {
        for (unsigned int j = 0; j < areas.size(); ++j) {
          // If the station fits into the subarea, add it to it and ignore the other subareas.
          if (stations[i]->lat >= areas[j].minLat && stations[i]->lat < areas[j].maxLat && stations[i]->lon >= areas[j].minLon && stations[i]->lon < areas[j].maxLon) {
            areas[j].stations.push_back(stations[i]);
            break;
          }
        }
      }

      // Clear the vector of stations in this area.
      stations.clear();
    }
  } else {
    // Find the appropriate subarea to store the station in.
    for (unsigned int i = 0 ; i < areas.size(); ++i) {
      if (station->lat >= areas[i].minLat && station->lat < areas[i].maxLat && station->lon >= areas[i].minLon && station->lon < areas[i].maxLon)
        areas[i].addStation(station);
    }
  }
}
