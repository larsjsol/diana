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
#ifndef diTimeFilter_h
#define diTimeFilter_h

#include <puTools/miTime.h>
#include <puTools/miClock.h>
#include <puTools/miString.h>


using namespace std;

/**

  \brief get time info from file name

*/
class TimeFilter {
private:

  std::string::size_type yyyy,yy,mm,dd,HH,MM,M,XX;
  bool dat;
  bool OK;
  bool noSlash;
  vector<bool> legalPos;
  bool advanced;

  void memberCopy(const TimeFilter& rhs);
  void replaceKey(miutil::miString& str);
  std::string::size_type findPos(const miutil::miString& filter, const miutil::miString& s);
  bool getClock(miutil::miString name, miutil::miClock &);

public:

  TimeFilter();

  ///remember time info, return filename whith time info replaced by *
  bool initFilter(miutil::miString& filename);
  ///returns true if initFilter found timeinfo in filename
  bool ok(void) {return OK;}
  ///find time from filename
  bool getTime(miutil::miString name, miutil::miTime & t);


};

#endif

