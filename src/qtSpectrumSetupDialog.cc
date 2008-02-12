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
#include <qapplication.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <q3vbox.h>
#include <qtUtility.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <diSpectrumManager.h>
#include <qtSpectrumSetup.h>
#include <qtSpectrumSetupDialog.h>
#include <diSpectrumOptions.h>


SpectrumSetupDialog::SpectrumSetupDialog( QWidget* parent, SpectrumManager* vm )
  : QDialog(parent), spectrumm(vm)
{
#ifdef DEBUGPRINT
  cout<<"SpectrumSetupDialog::SpectrumSetupDialog called"<<endl;
#endif

  //caption to appear on top of dialog
  setCaption( tr("Diana Wavespectrum - settings") );

  // text constants
 TEXTPLOT         =  tr("Text").latin1();
 FIXEDTEXT        =  tr("Fixed text").latin1();
 FRAME            =  tr("Frame").latin1();
 SPECTRUMLINES    =  tr("Spectrum lines").latin1();
 SPECTRUMCOLOUR   =  tr("Spectrum coloured").latin1();
 ENERGYLINE       =  tr("Graph line").latin1();
 ENERGYCOLOUR     =  tr("Graph coloured").latin1();
 PLOTWIND         =  tr("Wind").latin1();
 PLOTPEAKDIREC    =  tr("Max direction").latin1();
 FREQUENCYMAX     =  tr("Max frequency").latin1();
 BACKCOLOUR       =  tr("Background colour").latin1();

  //******** create the various QT widgets to appear in dialog *****

  spSetups.clear();
  initOptions( this );


  //******** standard buttons **************************************

  // push button to show help
      QPushButton * setuphelp = NormalPushButton( tr("Help"), this );
  connect(  setuphelp, SIGNAL(clicked()), SLOT( helpClicked()));

  // push button to set to default
  QPushButton * standard = NormalPushButton( tr("Default"), this );
  connect(  standard, SIGNAL(clicked()), SLOT( standardClicked()));

  // push button to hide dialog
  QPushButton * setuphide = NormalPushButton( tr("Hide"), this );
  connect( setuphide, SIGNAL(clicked()), SIGNAL(SetupHide()));

  // push button to apply the selected setup and then hide dialog
  QPushButton * setupapplyhide = NormalPushButton( tr("Apply+Hide"), this );
  connect( setupapplyhide, SIGNAL(clicked()), SLOT(applyhideClicked()));

  // push button to apply the selected setup
  QPushButton * setupapply = NormalPushButton( tr("Apply"), this );
  connect(setupapply, SIGNAL(clicked()), SLOT( applyClicked()) );

  // *********** place all the widgets in layouts ****************

  //place buttons "oppdater", "hjelp" etc. in horizontal layout
  Q3HBoxLayout* hlayout1 = new Q3HBoxLayout( 5 );
  hlayout1->addWidget( setuphelp );
  hlayout1->addWidget( standard );

  //place buttons "utf�r", "help" etc. in horizontal layout
  Q3HBoxLayout* hlayout2 = new Q3HBoxLayout( 5 );
  hlayout2->addWidget( setuphide );
  hlayout2->addWidget( setupapplyhide );
  hlayout2->addWidget( setupapply );

  //now create a vertical layout to put all the other layouts in
  Q3VBoxLayout * vlayout = new Q3VBoxLayout( this, 10, 10 );
  vlayout->addLayout( glayout );
  vlayout->addLayout( hlayout1 );
  vlayout->addLayout( hlayout2 );

  isInitialized=false;

#ifdef DEBUGPRINT
  cout<<"SpectrumSetupDialog::SpectrumSetupDialog finished"<<endl;
#endif
}


void SpectrumSetupDialog::initOptions(QWidget* parent)
{
#ifdef DEBUGPRINT
  cerr <<"SpectrumSetupDialog::initOptions" << endl;
#endif

  //make a grid with 4 rows, columms for labels and
  // for the checkboxes/comboboxes/spinboxes
  int numrows= 14;
//glayout = new QGridLayout(numrows,4); // linewidth not used, yet...
  glayout = new Q3GridLayout(numrows,3);
  glayout->setMargin( 5 );
  glayout->setSpacing( 2 );

  int nrow=0;

  QLabel* label1= new QLabel(tr("On/off"),parent);
  QLabel* label2= new QLabel(tr("Colour"),parent);
  QLabel* label3= new QLabel(tr("Line thickness"),parent);
//QLabel* label4= new QLabel(tr("Line type"),parent);
  glayout->addWidget(label1,nrow,0);
  glayout->addWidget(label2,nrow,1);
  glayout->addWidget(label3,nrow,2);
//glayout->addWidget(label4,nrow,3);
  nrow++;

  int n,opts;

  opts= (SpectrumSetup::useOnOff | SpectrumSetup::useColour);
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,TEXTPLOT,glayout,nrow++,opts,true));
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,FIXEDTEXT,glayout,nrow++,opts,true));
  opts= (SpectrumSetup::useOnOff | SpectrumSetup::useColour |
	 SpectrumSetup::useLineWidth);
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,FRAME,glayout,nrow++,opts,true));
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,SPECTRUMLINES,glayout,nrow++,opts,true));
  opts= (SpectrumSetup::useOnOff);
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,SPECTRUMCOLOUR,glayout,nrow++,opts,true));
  opts= (SpectrumSetup::useOnOff | SpectrumSetup::useColour |
	 SpectrumSetup::useLineWidth);
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,ENERGYLINE,glayout,nrow++,opts,true));
  opts= (SpectrumSetup::useOnOff | SpectrumSetup::useColour);
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,ENERGYCOLOUR,glayout,nrow++,opts,true));
  opts= (SpectrumSetup::useOnOff | SpectrumSetup::useColour |
	 SpectrumSetup::useLineWidth);
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,PLOTWIND,glayout,nrow++,opts,true));
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,PLOTPEAKDIREC,glayout,nrow++,opts,true));

  nrow++;
  opts= SpectrumSetup::useText;
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,FREQUENCYMAX,glayout,nrow++,opts,true));
  vector<miString> vfreq;
  vfreq.push_back(miString(0.50));
  vfreq.push_back(miString(0.45));
  vfreq.push_back(miString(0.40));
  vfreq.push_back(miString(0.35));
  vfreq.push_back(miString(0.30));
  vfreq.push_back(miString(0.25));
  vfreq.push_back(miString(0.20));
  vfreq.push_back(miString(0.15));
  vfreq.push_back(miString(0.10));
  n= spSetups.size()-1;
  spSetups[n]->defineText(vfreq,4);

  nrow++;
  opts= SpectrumSetup::useColour;
  spSetups.push_back(new SpectrumSetup(parent,spectrumm,BACKCOLOUR,glayout,nrow++,opts,true));

  if (nrow!=numrows) {
    cerr<<"=================================================="<<endl;
    cerr<<"===== SpectrumSetupDialog: glayout numrows= "<<numrows<<endl;
    cerr<<"=====                                 nrow= "<<nrow<<endl;
    cerr<<"=================================================="<<endl;
  }
}


void SpectrumSetupDialog::standardClicked()
{
  //this slot is called when standard button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumSetupDialog::standardClicked()" << endl;
#endif
  SpectrumOptions * spopt= new SpectrumOptions; // diana defaults
  setup(spopt);
  delete spopt;
  //emit SetupApply();
}


void SpectrumSetupDialog::start()
{
  if (!isInitialized){
    // pointer to logged options (the first time)
    SpectrumOptions * spopt= spectrumm->getOptions();
    setup(spopt);
    isInitialized=true;
  }
}


void SpectrumSetupDialog::setup(SpectrumOptions *spopt)
{
#ifdef DEBUGPRINT
  cerr <<"SpectrumSetupDialog::setup()" << endl;
#endif

  int n= spSetups.size();

  for (int i=0; i<n; i++) {

    if (spSetups[i]->name== TEXTPLOT) {
      spSetups[i]->setOn    (spopt->pText);
      spSetups[i]->setColour(spopt->textColour);

    } else if (spSetups[i]->name== FIXEDTEXT) {
      spSetups[i]->setOn    (spopt->pFixedText);
      spSetups[i]->setColour(spopt->fixedTextColour);

    } else if (spSetups[i]->name== FRAME) {
      spSetups[i]->setOn       (spopt->pFrame);
      spSetups[i]->setColour   (spopt->frameColour);
      spSetups[i]->setLinewidth(spopt->frameLinewidth);

    } else if (spSetups[i]->name== SPECTRUMLINES) {
      spSetups[i]->setOn       (spopt->pSpectrumLines);
      spSetups[i]->setColour   (spopt->spectrumLineColour);
      spSetups[i]->setLinewidth(spopt->spectrumLinewidth);

    } else if (spSetups[i]->name== SPECTRUMCOLOUR) {
      spSetups[i]->setOn(spopt->pSpectrumColoured);

    } else if (spSetups[i]->name== ENERGYLINE) {
      spSetups[i]->setOn       (spopt->pEnergyLine);
      spSetups[i]->setColour   (spopt->energyLineColour);
      spSetups[i]->setLinewidth(spopt->energyLinewidth);

    } else if (spSetups[i]->name== ENERGYCOLOUR) {
      spSetups[i]->setOn       (spopt->pEnergyColoured);
      spSetups[i]->setColour   (spopt->energyFillColour);

    } else if (spSetups[i]->name== PLOTWIND) {
      spSetups[i]->setOn       (spopt->pWind);
      spSetups[i]->setColour   (spopt->windColour);
      spSetups[i]->setLinewidth(spopt->windLinewidth);

    } else if (spSetups[i]->name== PLOTPEAKDIREC) {
      spSetups[i]->setOn       (spopt->pPeakDirection);
      spSetups[i]->setColour   (spopt->peakDirectionColour);
      spSetups[i]->setLinewidth(spopt->peakDirectionLinewidth);

    } else if (spSetups[i]->name== FREQUENCYMAX) {
      spSetups[i]->setText(miString(spopt->freqMax));

    } else if (spSetups[i]->name== BACKCOLOUR) {
      spSetups[i]->setColour(spopt->backgroundColour);

    } else {
      cerr<<"SpectrumSetupDialog::setup ERROR : "
	  <<spSetups[i]->name<<endl;
    }

  }
}


void SpectrumSetupDialog::applySetup()
{
#ifdef DEBUGPRINT
  cerr <<"SpectrumSetupDialog::applySetup()" << endl;
#endif
  SpectrumOptions * spopt= spectrumm->getOptions();

  int n= spSetups.size();

  for (int i=0; i<n; i++) {

    if (spSetups[i]->name== TEXTPLOT) {
      spopt->pText=      spSetups[i]->isOn();
      spopt->textColour= spSetups[i]->getColour().name;

    } else if (spSetups[i]->name== FIXEDTEXT) {
      spopt->pFixedText=      spSetups[i]->isOn();
      spopt->fixedTextColour= spSetups[i]->getColour().name;

    } else if (spSetups[i]->name== FRAME) {
      spopt->pFrame=         spSetups[i]->isOn();
      spopt->frameColour=    spSetups[i]->getColour().name;
      spopt->frameLinewidth= spSetups[i]->getLinewidth();

    } else if (spSetups[i]->name== SPECTRUMLINES) {
      spopt->pSpectrumLines=     spSetups[i]->isOn();
      spopt->spectrumLineColour= spSetups[i]->getColour().name;
      spopt->spectrumLinewidth=  spSetups[i]->getLinewidth();

    } else if (spSetups[i]->name== SPECTRUMCOLOUR) {
      spopt->pSpectrumColoured= spSetups[i]->isOn();

    } else if (spSetups[i]->name== ENERGYLINE) {
      spopt->pEnergyLine=      spSetups[i]->isOn();
      spopt->energyLineColour= spSetups[i]->getColour().name;
      spopt->energyLinewidth=  spSetups[i]->getLinewidth();

    } else if (spSetups[i]->name== ENERGYCOLOUR) {
      spopt->pEnergyColoured=  spSetups[i]->isOn();
      spopt->energyFillColour= spSetups[i]->getColour().name;

    } else if (spSetups[i]->name== PLOTWIND) {
      spopt->pWind=         spSetups[i]->isOn();
      spopt->windColour=    spSetups[i]->getColour().name;
      spopt->windLinewidth= spSetups[i]->getLinewidth();

    } else if (spSetups[i]->name== PLOTPEAKDIREC) {
      spopt->pPeakDirection=         spSetups[i]->isOn();
      spopt->peakDirectionColour=    spSetups[i]->getColour().name;
      spopt->peakDirectionLinewidth= spSetups[i]->getLinewidth();

    } else if (spSetups[i]->name== FREQUENCYMAX) {
      miString str= spSetups[i]->getText();
      spopt->freqMax= atof(str.cStr());

    } else if (spSetups[i]->name== BACKCOLOUR) {
      spopt->backgroundColour= spSetups[i]->getColour().name;

    } else {
      cerr<<"SpectrumSetupDialog::applySetup ERROR : "
	  <<spSetups[i]->name<<endl;
    }

  }
}


void SpectrumSetupDialog::helpClicked()
{
  //this slot is called when help button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumSetupDialog::helpClicked()" << endl;
#endif
  emit showdoc("ug_spectrum.html");
}


void SpectrumSetupDialog::applyClicked()
{
  //this slot is called when applyhide button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumSetupDialog::applyClicked()" << endl;
#endif
  applySetup();
  emit SetupApply();
}


void SpectrumSetupDialog::applyhideClicked()
{
  //this slot is called when applyhide button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumSetupDialog::applyhideClicked()" << endl;
#endif
  applySetup();
  emit SetupHide();
  emit SetupApply();
}


bool SpectrumSetupDialog::close(bool alsoDelete)
{
  emit SetupHide();
  return true;
}
