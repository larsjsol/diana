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
#include <q3listbox.h>
#include <qtToggleButton.h>
#include <qtUtility.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <diSpectrumManager.h>
#include <qtSpectrumModelDialog.h>

#define HEIGHTLISTBOX 100

/***************************************************************************/

SpectrumModelDialog::SpectrumModelDialog( QWidget* parent,SpectrumManager * vm )
  : QDialog(parent),spectrumm(vm)
{
#ifdef DEBUGPRINT 
  cout<<"SpectrumModelDialog::SpectrumModelDialog called"<<endl;
#endif

  //caption to appear on top of dialog
  setCaption( tr("Diana Wavespectrum - models") );
   
  // text constants
  ASFIELD = tr("As field").latin1();
  OBS     = tr("Observations").latin1();

  // send translated menunames to manager
  map<miString,miString> textconst;
  textconst["ASFIELD"]  = ASFIELD;
  textconst["OBS"]      = OBS;
  vm->setMenuConst(textconst);
  
//********** create the various QT widgets to appear in dialog ***********

  //**** the three buttons "auto", "tid", "fil" *************
  vector<miString> model;
  model.push_back(tr("Model").latin1());
  model.push_back(tr("File").latin1());

  //if a model is selected- should be as model- else default model(hirlam)

  //********** the list of files/models to choose from **************

  modelfileList = new Q3ListBox( this);
  modelfileList->setMinimumHeight(HEIGHTLISTBOX);
  modelfileList->setSelectionMode( Q3ListBox::Multi );
  modelfileList->setEnabled(true);  
  updateModelfileList();

  modelfileBut = new Q3ButtonGroup( this );
  modelButton = new ToggleButton(modelfileBut, tr("Model").latin1());
  fileButton = new ToggleButton(modelfileBut, tr("File").latin1());
  Q3HBoxLayout* modelfileLayout = new Q3HBoxLayout(modelfileBut);
  modelfileLayout->addWidget(modelButton);
  modelfileLayout->addWidget(fileButton);
  modelfileBut->setExclusive( true );
  modelfileBut->setButton(0);
  //modelfileClicked is called when auto,tid,fil buttons clicked 
  connect( modelfileBut, SIGNAL( clicked(int) ), 
	   SLOT( modelfileClicked(int) ) );

  //push button to show help
  QPushButton * modelhelp = NormalPushButton( tr("Help"), this );
  connect(  modelhelp, SIGNAL(clicked()), SLOT( helpClicked()));

  //push button to delete
  QPushButton * deleteAll = NormalPushButton( tr("Delete All"), this );
  connect( deleteAll, SIGNAL(clicked()),SLOT(deleteAllClicked()));

  //push button to refresh
  QPushButton * refresh = NormalPushButton( tr("Refresh"), this );
  connect(  refresh, SIGNAL(clicked()), SLOT( refreshClicked()));

  //push button to hide dialog
  QPushButton * modelhide = NormalPushButton( tr("Hide"), this );
  connect( modelhide, SIGNAL(clicked()), SIGNAL(ModelHide()));

   //push button to apply the selected command and then hide dialog
  QPushButton  * modelapplyhide = NormalPushButton( tr("Apply+Hide"), this );
  connect( modelapplyhide, SIGNAL(clicked()), SLOT(applyhideClicked()));

  //push button to apply the selected command
  QPushButton * modelapply = NormalPushButton( tr("Apply"), this );
  connect( modelapply, SIGNAL(clicked()), SLOT(applyClicked()));

  // ************ place all the widgets in layouts ****************

  //place buttons "oppdater", "hjelp" etc. in horizontal layout
  Q3HBoxLayout* hlayout1 = new Q3HBoxLayout( 5 );
  hlayout1->addWidget( modelhelp );
  hlayout1->addWidget( deleteAll );
  hlayout1->addWidget( refresh );

  //place buttons "utf�r", "help" etc. in horizontal layout
  Q3HBoxLayout* hlayout2 = new Q3HBoxLayout( 5 );
  hlayout2->addWidget( modelhide );
  hlayout2->addWidget( modelapplyhide );
  hlayout2->addWidget( modelapply );


  //create a vertical layout to put all widgets and layouts in
  Q3VBoxLayout * vlayout = new Q3VBoxLayout( this, 5, 5 );                     
  vlayout->addWidget( modelfileBut );
  vlayout->addWidget( modelfileList );
  vlayout->addLayout( hlayout1 );
  vlayout->addLayout( hlayout2 );

  //redo and freeze the layout (this makes it impossible for the
  //user to change the size of the dialogbox
//vlayout->activate();
//vlayout->freeze();

}

/*********************************************/

void SpectrumModelDialog::modelfileClicked(int tt){
  //this slot is called when modelfile button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumModelDialog::modelfileClicked()\n";
#endif

  //update the index to current item in time file button
  m_modelfileButIndex = tt;

  updateModelfileList();
}
  

/*********************************************/

void SpectrumModelDialog::refreshClicked(){
  //this slot is called when refresh button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumModelDialog::refreshClicked()\n";
#endif
  updateModelfileList();

}

/*********************************************/

void SpectrumModelDialog::deleteAllClicked(){
  //this slot is called when delete button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumModelDialog::deleteAllClicked()\n";
#endif
  modelfileList->clearSelection();
}

/*********************************************/

void SpectrumModelDialog::helpClicked(){
  //this slot is called when help button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumModelDialog::helpClicked()\n";
#endif
  emit showdoc("ug_spectrum.html");
}


/*********************************************/

void SpectrumModelDialog::applyClicked(){
  //this slot is called when apply button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumModelDialog::applyClicked(int tt)\n";
#endif
  setModel();
  emit ModelApply();

}

/*********************************************/

void SpectrumModelDialog::applyhideClicked(){
  //this slot is called when applyhide button pressed
#ifdef DEBUGPRINT
  cerr <<"SpectrumModelDialog::applyhideClicked(int tt)\n";
#endif
  setModel();
  emit ModelHide();
  emit ModelApply();

}


/*********************************************/
void SpectrumModelDialog::setSelection(){
#ifdef DEBUGPRINT
  cerr<< "SpectrumModelDialog::setSelection()" << endl;
#endif
  if (m_modelfileButIndex==0){
    vector <miString> models = spectrumm->getSelectedModels();
    int n = models.size();
    for (int i = 0;i<n;i++){
      miString model = models[i];
      int m = modelfileList->count();
      for (int j = 0;j<m;j++){
	miString listModel =  modelfileList->text(j).latin1();
	if (model==listModel) modelfileList->setSelected(j,true);
      }
    }
  }
}

/*********************************************/
void SpectrumModelDialog::setModel(){
#ifdef DEBUGPRINT
  cerr<< "SpectrumModelDialog::setModel()" << endl;
#endif

  bool showObs=false;
  bool asField=false;

  if (m_modelfileButIndex==0){

    vector <miString> models;
    int n = modelfileList->count();
    for (int i = 0; i<n;i++){
      if(modelfileList->isSelected(i)){
	miString model = modelfileList->text(i).latin1();
	if(model==OBS){
	  showObs=true;
	} else if (model==ASFIELD){
	  asField = true;
	} else models.push_back(model);
      }
    }
    spectrumm->setSelectedModels(models,showObs,asField);

  } else if (m_modelfileButIndex==1){

    vector <miString> files;
    int n = modelfileList->count();
    for (int i = 0; i<n;i++){
      if(modelfileList->isSelected(i)){
	miString file = modelfileList->text(i).latin1(); 
	files.push_back(file);
      }
    }
    spectrumm->setSelectedFiles(files,showObs,asField);
  }
  
}
/*********************************************/

void SpectrumModelDialog::updateModelfileList(){
#ifdef DEBUGPRINT
  cerr << "SpectrumModelDialog::updateModelfileList()\n";
#endif

  //want to keep the selected models/files
  int n= modelfileList->count();
  set<miString> current;
  for (int i=0; i<n; i++)
    if (modelfileList->isSelected(i))
      current.insert(miString(modelfileList->text(i).latin1()));

  //clear box with list of files 
  modelfileList->clear();

  if (m_modelfileButIndex==0){
    //make a string list with models to insert into modelfileList
    vector <miString> modelnames =spectrumm->getModelNames();
    int nr_models = modelnames.size();
  //modelfileList->insertItem(OBS);
    modelfileList->insertItem(ASFIELD);
    for (int i=0; i<nr_models; i++)
      modelfileList->insertItem(modelnames[i].c_str());

    //insert into modelfilelist
  } else if (m_modelfileButIndex==1){
    //make a string list with files to insert into modelfileList
    vector <miString> modelfiles =spectrumm->getModelFiles();
    int nr_files = modelfiles.size();
    for (int i=0; i<nr_files; i++)
      modelfileList->insertItem(modelfiles[i].c_str());
  }

  set<miString>::iterator pend= current.end();
  n= modelfileList->count();
  for (int i=0; i<n; i++)
    if (current.find(miString(modelfileList->text(i).latin1()))!=pend)
      modelfileList->setSelected(i,true);
  
}


bool SpectrumModelDialog::close(bool alsoDelete){
#ifdef DEBUGPRINT
  cerr <<"SpectrumModel was closed!" << endl;
#endif
  emit ModelHide();
  return true;
}
