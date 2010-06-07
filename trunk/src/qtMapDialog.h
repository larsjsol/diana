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
#ifndef _mapdialog_h
#define _mapdialog_h

#include <QDialog>

#include <diController.h>
#include <vector>
#include <puTools/miString.h>

using namespace std;

class QComboBox;
class QListWidget;
class QListWidgetItem;
class QLabel;
class ToggleButton;
class QCheckBox;

/**
 \brief Map selection dialogue

 Dialogue for selections of maps, predefined projections/areas and various map-settings

 */

class MapDialog: public QDialog {
Q_OBJECT
public:

  MapDialog(QWidget* parent, const MapDialogInfo& mdi);
  MapDialog(QWidget* parent, Controller* llctrl);

  /// the plot info strings
  vector<miutil::miString> getOKString();
  /// set the dialogue elements from a plot info string
  void putOKString(const vector<miutil::miString>& vstr);
  /// creates a short name for the current settings (used in quick menues)
  miutil::miString getShortname();
  /// returns all settings in logfile format
  vector<miutil::miString> writeLog();
  /// set the dialogue elements from logfile settings
  void readLog(const vector<miutil::miString>& vstr, const miutil::miString& thisVersion,
      const miutil::miString& logVersion);
  /// choose the favorite settings
  void useFavorite();

protected:
  void closeEvent(QCloseEvent*);

private:

  void ConstructorCernel(const MapDialogInfo mdi);

private slots:
  //  void areaboxSelected( QListWidgetItem* item );
  void mapboxChanged();
  void selectedMapboxClicked(QListWidgetItem* item);
  void mapdeleteClicked();
  void mapalldeleteClicked();

  void lon_checkboxActivated(bool);
  void lon_linecboxActivated(int index);
  void lon_linetypeboxActivated(int index);
  void lon_colorcboxActivated(int index);
  void lon_densitycboxActivated(int index);
  void lon_zordercboxActivated(int index);
  void lon_showValueActivated(bool);

  void lat_checkboxActivated(bool);
  void lat_linecboxActivated(int index);
  void lat_linetypeboxActivated(int index);
  void lat_colorcboxActivated(int index);
  void lat_densitycboxActivated(int index);
  void lat_zordercboxActivated(int index);
  void lat_showValueActivated(bool);

  void backcolorcboxActivated(int index);

  void showframe_checkboxActivated(bool);
  void ff_linecboxActivated(int index);
  void ff_linetypeboxActivated(int index);
  void ff_colorcboxActivated(int index);
  void ff_zordercboxActivated(int index);

  void cont_checkboxActivated(bool);
  void cont_linecboxActivated(int index);
  void cont_linetypeboxActivated(int index);
  void cont_colorcboxActivated(int index);
  void cont_zordercboxActivated(int index);

  void land_checkboxActivated(bool);
  void land_colorcboxActivated(int index);
  void land_zordercboxActivated(int index);

  void helpClicked();
  void applyhideClicked();
  void saveFavoriteClicked();
  void useFavoriteClicked();

  signals:
  void MapApply();
  void MapHide();
  void showsource(const miutil::miString, const miutil::miString = "");

private:
  MapDialogInfo m_MapDI; // all maps and areas
  vector<miutil::miString> favorite; // favorite options
  int numMaps; // number of maps
  vector<int> selectedmaps; // maps selected
  int activemap; // active selected map
  vector<int> logmaps; // selected maps ready for logging

  vector<Colour::ColourInfo> cInfo; // all defined colours
  vector<miutil::miString> linetypes; // all defined linetypes
  vector<miutil::miString> zorders; // all defined zorders
  vector<miutil::miString> densities; // latlon densities (degrees)
  vector<miutil::miString> positions; // all defined positions
  map<miutil::miString,int> positions_map; // all defined positions
  Controller* m_ctrl;

  // areas
  QLabel* arealabel;
  QListWidget* areabox;

  // latlon options
  QLabel* lon_label;
  QLabel* lon_linelabel;
  QLabel* lon_linetypelabel;
  QLabel* lon_colorlabel;
  QLabel* lon_zorderlabel;
  QLabel* lon_densitylabel;
  QCheckBox* showlon;
  QComboBox* lon_linecbox;
  QComboBox* lon_linetypebox;
  QComboBox* lon_colorcbox;
  QComboBox* lon_zorder;
  QComboBox* lon_density;
  QCheckBox* lon_showvalue;
  QComboBox* lon_valuepos;
  bool lonb;
  miutil::miString lonc;
  miutil::miString lonlw;
  miutil::miString lonlt;
  int lonz;
  float lond;
  bool lonshowvalue;

  QLabel* lat_label;
  QLabel* lat_linelabel;
  QLabel* lat_linetypelabel;
  QLabel* lat_colorlabel;
  QLabel* lat_zorderlabel;
  QLabel* lat_densitylabel;
  QCheckBox* showlat;
  QComboBox* lat_linecbox;
  QComboBox* lat_linetypebox;
  QComboBox* lat_colorcbox;
  QComboBox* lat_zorder;
  QComboBox* lat_density;
  QCheckBox* lat_showvalue;
  QComboBox* lat_valuepos;
  bool latb;
  miutil::miString latc;
  miutil::miString latlw;
  miutil::miString latlt;
  int latz;
  float latd;
  bool latshowvalue;

  // backgroundcolour
  QLabel* backcolorlabel;
  QComboBox* backcolorcbox;

  // frame
  QCheckBox* showframe;
  bool frameb;
  QLabel* framelabel;
  QLabel* ff_linelabel;
  QLabel* ff_linetypelabel;
  QLabel* ff_colorlabel;
  QLabel* ff_zorderlabel;
  QComboBox* ff_linecbox;
  QComboBox* ff_linetypebox;
  QComboBox* ff_colorcbox;
  QComboBox* ff_zorder;
  miutil::miString framec;
  miutil::miString framelw;
  miutil::miString framelt;
  int framez;

  // maps and selected maps
  QLabel* maplabel;
  QListWidget* mapbox;
  QLabel* selectedMaplabel;
  QListWidget* selectedMapbox;
  QPushButton* mapdelete;
  QPushButton* mapalldelete;

  // contourlines options
  QLabel* cont_label;
  QLabel* cont_linelabel;
  QLabel* cont_linetypelabel;
  QLabel* cont_colorlabel;
  QLabel* cont_zorderlabel;
  QCheckBox* contours;
  QComboBox* cont_linecbox;
  QComboBox* cont_linetypebox;
  QComboBox* cont_colorcbox;
  QComboBox* cont_zorder;

  // filled land options
  QLabel* land_label;
  QLabel* land_colorlabel;
  QLabel* land_zorderlabel;
  QCheckBox* filledland;
  QComboBox* land_colorcbox;
  QComboBox* land_zorder;

  // buttons
  QPushButton* savefavorite;
  QPushButton* usefavorite;
  QPushButton* mapapply;
  QPushButton* mapapplyhide;
  QPushButton* maphide;
  QPushButton* maphelp;

  QColor* pixcolor;

};

#endif