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
#ifndef diVprofPlot_h
#define diVprofPlot_h

#include <diVprofTables.h>
#include <diVprofOptions.h>

#include <puTools/miString.h>
#include <puTools/miTime.h>
#include <vector>
#include <GL/gl.h>

using namespace std;

/**
   \brief Plots observed or prognostic Vertical Profiles (soundings)

   Data preparation and plotting. A few computations can be done.
   An object holds data for one sounding.
*/
class VprofPlot : public VprofTables
{

  friend class VprofData;
#ifdef METNOOBS
  friend class VprofTemp;
  friend class VprofPilot;
#endif
#ifdef BUFROBS
  friend class ObsBufr;
#endif
#ifdef ROADOBS
  friend class VprofRTemp;
#endif

public:
  VprofPlot();
  ~VprofPlot();
  bool plot(VprofOptions *vpopt, int nplot);
  void setName(const miutil::miString& name) { text.posName=name; }

private:

  VprofText text;
  bool   prognostic;
  bool   windInKnots;
  int    maxLevels;

  vector<float> ptt, tt;
  vector<float> ptd, td;
  vector<float> puv, uu, vv;
  vector<float> pom, om;
  vector<int>   dd, ff, sigwind;

  vector<float> pcom, tcom, tdcom; // common T and Td levels
  vector<float> rhum, duct;

  void  relhum(const vector<float>& tt,
	       const vector<float>& td);
  void ducting(const vector<float>& pp,
	       const vector<float>& tt,
	       const vector<float>& td);
  void  kindex(const vector<float>& pp,
	       const vector<float>& tt,
	       const vector<float>& td);

};

#endif