/*
  Diana - A Free Meteorological Visualisation Tool

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
#ifndef VCROSSSETUPDIALOG_H
#define VCROSSSETUPDIALOG_H

#include <QDialog>
#ifdef USE_VCROSS_V2
#include "vcross_v2/VcrossQtManager.h"
#else
#include "vcross_v1/diVcross1Manager.h"
#endif

#include <vector>

class VcrossOptions;
class VcrossSetupUI;
class QGridLayout;

/**
   \brief Dialogue to select Vertical Crossection background/diagram options
*/
class VcrossSetupDialog: public QDialog
{
  Q_OBJECT

public:
  VcrossSetupDialog(QWidget* parent, VcrossManager * vm);
  void start();

protected:
  void closeEvent(QCloseEvent*);

private:
  VcrossManager  * vcrossm;

  void initOptions();

  void setup(VcrossOptions * vcopt);
  void printSetup();
  void applySetup();

  QGridLayout* glayout;

  bool isInitialized;

  VcrossSetupUI* mSetupTEXTPLOT;
  VcrossSetupUI* mSetupFRAME;
  VcrossSetupUI* mSetupPOSNAMES;
  VcrossSetupUI* mSetupLEVELNUMBERS;
  VcrossSetupUI* mSetupUPPERLEVEL;
  VcrossSetupUI* mSetupLOWERLEVEL;
  VcrossSetupUI* mSetupOTHERLEVELS;
  VcrossSetupUI* mSetupSURFACE;
  VcrossSetupUI* mSetupDISTANCE;
  VcrossSetupUI* mSetupGEOPOS;
  VcrossSetupUI* mSetupVERTGRID;
  VcrossSetupUI* mSetupMARKERLINES;
  VcrossSetupUI* mSetupVERTICALMARKER;
  VcrossSetupUI* mSetupEXTRAPOLP;
  VcrossSetupUI* mSetupBOTTOMEXT;
  VcrossSetupUI* mSetupTHINARROWS;
  VcrossSetupUI* mSetupVERTICALTYPE;
  VcrossSetupUI* mSetupVHSCALE;
  VcrossSetupUI* mSetupSTDVERAREA;
  VcrossSetupUI* mSetupSTDHORAREA;
  VcrossSetupUI* mSetupBACKCOLOUR;
  VcrossSetupUI* mSetupONMAPDRAW;
  VcrossSetupUI* mSetupHITMAPDRAW;

private Q_SLOTS:
  void standardClicked();
  void helpClicked();
  void applyClicked();
  void applyhideClicked();

Q_SIGNALS:
  void SetupHide();
  void SetupApply();
  void showsource(const std::string&, const std::string& = "");
};

#endif