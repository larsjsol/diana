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
#ifndef _obsdialog_h
#define _obsdialog_h

#include <qdialog.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QLabel>
#include <Q3VBoxLayout>
#include <diController.h>
#include <miString.h>

class QComboBox;
class Q3VBoxLayout;
class Q3HBoxLayout;
class ObsWidget;
class QPushButton;
class QLabel;
class ToggleButton;
class AdvancedButton;
class Q3ListBox;
class QColor;
class QLCDNumber;
class QSlider;
class QCheckBox;
class QRadioButton;
class Q3VButtonGroup;
class QLineEdit;
class QStackedWidget;

/**

  \brief Observation dialogue
   
   Dialogue for selection of plot styles, data types, parameters etc.

*/
class ObsDialog: public QDialog
{
  Q_OBJECT
public:   
    
  ObsDialog( QWidget* parent, Controller* llctrl );
  ///return command strings
  vector<miString> getOKString();
  ///insert command strings
  void putOKString(const vector<miString>& vstr);
  ///return short name of current commonad
  miString getShortname();
  ///check command strings, and return legal command strings
  void requestQuickUpdate(vector<miString>& oldstr,
                          vector<miString>& newstr);
  ///change plottype
  bool setPlottype(const miString& name, bool on);

  vector<miString> writeLog();
  void readLog(const vector<miString>& vstr,
               const miString& thisVersion, const miString& logVersion);
///called when the dialog is closed by the window manager
  bool close(bool alsoDelete);

public slots:
  void archiveMode( bool on );

private slots:
  void getTimes();
  void plotSelected( int index , bool sendTimes=true);
  void applyhideClicked();
  void helpClicked();
  void multiplotClicked(bool);
  void extensionToggled(bool);
  void criteriaOn();
  void rightButtonClicked(miString);
  void criteriaListSelected(int);
  void criteriaSelected(int);
  void signSlot(int);
  void sliderSlot(int);
  void stepSlot(int);
  void changeCriteriaString();
  void deleteSlot();
  void deleteAllSlot();
  void saveSlot();

signals:
  void ObsApply();
  void ObsHide();
  void showdoc(const miString);
  void emitTimes( const miString&,const vector<miTime>& );
  void setCriteria( miString, bool );

private:
  void makeExtension();
  void markButton(miString& );
  miString makeCriteriaString();
  bool newCriteriaString();
  void updateExtension();
  void numberList( QComboBox* cBox, float number );
  int findPlotnr(const miString&);

 //ATTRIBUTES
  int nr_plot;
  vector<miString> m_name;
  vector<miString> savelog;
  int m_selected;
  QColor* colour;
  QComboBox* plotbox;
  QStackedWidget* stackedWidget;
  Q3VBoxLayout* vlayout;
  Q3HBoxLayout* criterialayout;
  Q3HBoxLayout* helplayout;
  Q3HBoxLayout* applylayout;
  Q3HBoxLayout* toplayout;
  Q3HBoxLayout* obsWidgetlayout;
  ObsWidget** obsWidget;
  miString parameterSelected;
  QLabel * label;
  bool multiplot;
  ToggleButton* multiplotButton;
  QPushButton* obsapply;
  QPushButton* obshide;
  QPushButton* obsapplyhide;
  QPushButton* obsrefresh;
  QPushButton* obshelp;

  //Extension
  QWidget* extension;
  //  QLabel* parameterLabel;
  QComboBox* criteriaBox;
  Q3ListBox* criteriaListbox;
  miString parameter;
  QLCDNumber* limitLcd;
  QSlider* limitSlider;
  QComboBox* stepComboBox;
  QComboBox* signBox;
  Q3VButtonGroup* radiogroup;
  QRadioButton* plotButton;
  QRadioButton* colourButton;
  QRadioButton* totalColourButton;
  QRadioButton* markerButton;
  QComboBox* colourBox;
  QComboBox* markerBox;
  QLineEdit* lineedit;
  vector<Colour::ColourInfo> cInfo;
  vector<miString> markerName;
  Controller* m_ctrl;
  bool freeze;
};





#endif
