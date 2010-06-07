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
#ifndef _qtTimeSlider_h
#define _qtTimeSlider_h

#include <qslider.h>
#include <qpalette.h>
#include <puTools/miTime.h>
#include <puTools/miString.h>
#include <map>
#include <vector>

using namespace std; 

/**

  \brief Main plot time 

  The time slider class holds times from all data sources selected,
  but the slider itself only shows times from one data source at the time 

*/
class TimeSlider : public QSlider {
  Q_OBJECT

public:
  TimeSlider(QWidget*);
  
  TimeSlider(Qt::Orientation, QWidget*);

  void clear();
  void setLastTimeStep();
  ///Current time
  miutil::miTime Value();
  /// current index
  int current() { return value(); };
  /// get start time
  miutil::miTime getStartTime() { return start; };
  ///Number of times currently in the slider
  int numTimes() const {return times.size();}
  ///Next/previous time
  bool nextTime(const int dir, miutil::miTime& time, bool restricted =false);
  void setLoop(const bool b);
  vector<miutil::miTime> getTimes(){return times;}
  void startAnimation(){startani= true;}
  ///Remove times from data type
  void deleteType(const miutil::miString& type);
  
  void set(const miutil::miTime&);

 public slots:
  void setMinMax(const miutil::miTime& t1, const miutil::miTime& t2);
  void clearMinMax();
  ///add new times for datatype
  void insert(const miutil::miString& datatype, const vector<miutil::miTime>&,bool =true);
 /// force new value
  void setTime(const miutil::miTime&);
 /// force new value if datatype match
  void setTime( const miutil::miString& datatype, const miutil::miTime&);
  /// time-interval changed
  void setInterval(int);
  ///use times from datatype(field, sat, obs ..)
  void useData(miutil::miString datatype);

  signals:
  /// emits smallest timeinterval (in hours)
  void minInterval(int);
  /// emits minimum timesteps
  void timeSteps(int,int);
  /// enable/disable spinbox
  void enableSpin(bool);
  void sliderSet();
  /// emits times
  void newTimes(vector<miutil::miTime>&);

private:
  map<miutil::miString,vector<miutil::miTime> > tlist; // times
  map<miutil::miString,bool>  usetlist; // false if only one spesific time is set
  vector<miutil::miTime> orig_times; // the actual timepoints (all)
  vector<miutil::miTime> times; // the actual timepoints (min-max)
  miutil::miTime prevtime; // previous selected time
  float interval;  // timeinterval in hours
  bool loop;       // time-loop in use
  miutil::miTime start;    // restrict nextTime()  
  miutil::miTime stop;     // -------
  bool useminmax;  // use restricted timeinterval
  QPalette pal;    // keep original slider-palette
  bool startani;   // animation just started
  miutil::miString  dataType; //dataType has priority   
  miutil::miString  dataTypeUsed; //dataType in use

  void init();
  void setFirstTime(const miutil::miTime&);
  void updateList();
  bool setSliderValue(const int v);
};

#endif