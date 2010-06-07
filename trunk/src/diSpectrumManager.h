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
#ifndef SPECTRUMMANAGER_H
#define SPECTRUMMANAGER_H

#include <puTools/miString.h>
#include <puTools/miTime.h>
#include <vector>
#include <map>
#include <set>
#include <diCommonTypes.h>
#include <diSetupParser.h>
#include <diPrintOptions.h>

using namespace std;

class SpectrumOptions;
class SpectrumFile;
class SpectrumPlot;


/**
   \brief Managing Wave Spectrum data sources and plotting

*/
class SpectrumManager{

private:

  enum obsType {
    obsAAA,
    obsBBB
  };

  struct ObsFile {
    miutil::miString filename;
    obsType  obstype;
    miutil::miTime   time;
    long     modificationTime;
  };

  struct StationPos {
    float latitude;
    float longitude;
    miutil::miString obs;
  };

  // map<model,filename>
  map<miutil::miString,miutil::miString> filenames;

  // for use in dialog (unique lists in setup order)
  vector<miutil::miString> dialogModelNames;
  vector<miutil::miString> dialogFileNames;

  // temp file paths  ("/dir/xxxx??.dat*")
  vector<miutil::miString> obsAaaPaths;
  // pilot file paths ("/dir/xxxx??.dat*")
  vector<miutil::miString> obsBbbPaths;

  SetupParser sp;

  SpectrumOptions *spopt;
  vector<SpectrumFile*> spfile;
  bool showObs;
  bool asField;

  vector<miutil::miString> nameList;
  vector<float> latitudeList;
  vector<float> longitudeList;
  vector<miutil::miString> obsList;
  vector<miutil::miTime>   timeList;

  vector<ObsFile> obsfiles;
  vector<miutil::miTime> obsTime;
  bool onlyObs;

  vector<miutil::miString> fieldModels;
  vector<miutil::miString> selectedModels;
  vector<miutil::miString> selectedFiles;
  set<miutil::miString> usemodels;

  int plotw, ploth;

  miutil::miString plotStation;
  miutil::miString lastStation;
  miutil::miTime   plotTime;
  miutil::miTime   ztime;

  bool dataChange;
  vector<SpectrumPlot*> spectrumplots;

  bool hardcopy;
  printOptions printoptions;
  bool hardcopystarted;

  map<miutil::miString,miutil::miString> menuConst;

  void parseSetup();
  miutil::miString getDefaultModel();
  void updateObsFileList();
  bool initSpectrumFile(miutil::miString file,miutil::miString model);
  void initStations();
  void initTimes();
  void checkObsTime(int hour=-1);
  void preparePlot();

public:
  // constructor
  SpectrumManager();
  // destructor
  ~SpectrumManager();

  SpectrumOptions* getOptions() { return spopt; }
  void setPlotWindow(int w, int h);

  //routines from controller
  vector<miutil::miString> getLineThickness();

  void setModel();
  void setStation(const miutil::miString& station);
  void setTime(const miutil::miTime& time);
  miutil::miString setStation(int step);
  miutil::miTime setTime(int step);

  const miutil::miTime getTime(){return plotTime;}
  const miutil::miString getStation() { return plotStation; }
  const miutil::miString getLastStation() { return lastStation; }
  const vector<miutil::miString>& getStationList() { return nameList; }
  const vector<float>& getLatitudes() { return latitudeList; }
  const vector<float>& getLongitudes() { return longitudeList; }
  const vector<miutil::miTime>& getTimeList() { return timeList; }

  vector<miutil::miString> getModelNames();
  vector<miutil::miString> getModelFiles();
  void setFieldModels(const vector<miutil::miString>& fieldmodels);
  void setSelectedModels(const vector<miutil::miString>& models,
			 bool obs, bool field);
  void setSelectedFiles(const vector<miutil::miString>& files,
			bool obs, bool field);

  vector<miutil::miString> getSelectedModels();

  bool plot();
  void startHardcopy(const printOptions& po);
  void endHardcopy();
  bool onlyObsState() { return onlyObs; }
  void mainWindowTimeChanged(const miutil::miTime& time);
  void updateObs();
  miutil::miString getAnnotationString();

  void setMenuConst(map<miutil::miString,miutil::miString> mc)
  { menuConst = mc;}

  vector<miutil::miString> writeLog();
  void readLog(const vector<miutil::miString>& vstr,
	       const miutil::miString& thisVersion, const miutil::miString& logVersion);

};


#endif