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

#include <QApplication>
#include <QSlider>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLayout>
#include <QLabel>
#include <QLCDNumber>
#include <qmessagebox.h>
#include <QToolTip>
#include <QButtonGroup>

#include "qtSatDialog.h"
#include "qtSatDialogAdvanced.h"
#include "qtToggleButton.h"
#include "qtUtility.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QVBoxLayout>

#include <puTools/miString.h>
#include <stdio.h>
#include <iostream>
#include <puTools/miTime.h>

#include "up12x12.xpm"
#include "down12x12.xpm"


#define HEIGHTLISTBOX 45


//static

miutil::miTime SatDialog::ztime = miutil::miTime(1970,1,1,0,0,0);

/*********************************************/
SatDialog::SatDialog( QWidget* parent, Controller* llctrl )
  : QDialog(parent), m_ctrl(llctrl)
{

  setWindowTitle(tr("Satellite and radar"));

  dialogInfo = llctrl->initSatDialog();

  //put satellite names (NOAA,Meteosat,radar) in namebox
  vector<miutil::miString> name;
  int nImage=dialogInfo.image.size();
  for( int i=0; i< nImage; i++ )
    name.push_back(dialogInfo.image[i].name);
  namebox = ComboBox( this, name, true, 0);
  connect( namebox, SIGNAL( activated(int) ),
	   SLOT( nameActivated(int) )  );


  //****

  //fileListWidget contains filetypes for each satellite
  //NOAA Europa N-Europa etc.
  //METEOSAT Visuell, IR
  //insert filetypes for default sat - m_image[0]-NOAA
  fileListWidget = new QListWidget( this );
  fileListWidget->setMinimumHeight(HEIGHTLISTBOX);

  updateFileListWidget(0);
  connect( fileListWidget, SIGNAL( itemClicked( QListWidgetItem * ) ),
	   SLOT( fileListWidgetClicked( QListWidgetItem * ) )  );

  autoButton = new ToggleButton(this, tr("Auto").toStdString());
  timeButton = new ToggleButton(this, tr("Time").toStdString());
  fileButton = new ToggleButton(this, tr("File").toStdString());
  timefileBut = new QButtonGroup( this );
  timefileBut->addButton(autoButton,0);
  timefileBut->addButton(timeButton,1);
  timefileBut->addButton(fileButton,2);
  QHBoxLayout* timefileLayout = new QHBoxLayout();
  timefileLayout->addWidget(autoButton);
  timefileLayout->addWidget(timeButton);
  timefileLayout->addWidget(fileButton);
  timefileBut->setExclusive( true );
  autoButton->setChecked(true);
  //timefileClicked is called when auto,tid,fil buttons clicked
  connect( timefileBut, SIGNAL( buttonClicked(int) ),
	   SLOT( timefileClicked(int) ) );

  //list of times or files will be filled when "tid","fil" clicked
  // (in timefileClicked)

  timefileList = new QListWidget( this);
  timefileList->setMinimumHeight(HEIGHTLISTBOX);

  //timefileListSlot called when an item (time of file) highlighted in
  // timefileList
  connect( timefileList, SIGNAL(itemClicked(QListWidgetItem *) ),
	   SLOT( timefileListSlot( QListWidgetItem *) ) );


  //channelbox filled with available channels
  QLabel *channellabel= TitleLabel( tr("Channels"), this);
  channelbox = new QListWidget( this);
  channelbox->setMinimumHeight(HEIGHTLISTBOX);

  //channelboxSlot called when an item highlighted in channelbox
  connect( channelbox, SIGNAL( itemClicked( QListWidgetItem * ) ),
	   SLOT( channelboxSlot( QListWidgetItem * ) ) );

  //****

  //pictures contains one or more selected pictures !
      QLabel *picturesLabel = TitleLabel(tr("Selected pictures"), this);
  pictures = new QListWidget( this );
  pictures->setMinimumHeight(HEIGHTLISTBOX);

  connect( pictures, SIGNAL( itemClicked( QListWidgetItem * ) ),
	   SLOT( picturesSlot( QListWidgetItem * ) ) );


  //**  all the other QT buttons


  // upPicture
  QPixmap upPicture = QPixmap(up12x12_xpm);
  upPictureButton = PixmapButton( upPicture, this, 14, 12 );
  upPictureButton->setEnabled( false );

  connect( upPictureButton, SIGNAL(clicked()), SLOT(upPicture()));

  // downPicture
  QPixmap downPicture = QPixmap(down12x12_xpm);
  downPictureButton = PixmapButton( downPicture, this, 14, 12 );
  downPictureButton->setEnabled( false );

  connect( downPictureButton, SIGNAL(clicked()), SLOT(downPicture()));

  Delete = NormalPushButton( tr("Delete"), this );
  connect( Delete, SIGNAL(clicked()),
	   SLOT(DeleteClicked()));

  DeleteAll = NormalPushButton( tr("Delete All"), this );
  connect( DeleteAll, SIGNAL(clicked()),
	   SLOT(DeleteAllClicked()));

  multiPicture = new ToggleButton(  this, tr("Add picture").toStdString() );
  multiPicture->setToolTip(tr("Add new picture if any of above settings change"));

  mosaic = new ToggleButton( this, tr("Mosaic").toStdString() );
  connect( mosaic, SIGNAL( toggled(bool)), SLOT( mosaicToggled( bool) ));
  mosaic->setChecked(false);
  mosaic->setEnabled(false);

  //SLIDER FOR MAX TIME DIFFERENCE
  m_scalediff= dialogInfo.timediff.scale;
  QLabel *diffLabel = new QLabel( tr("Time diff"), this);
  int difflength=dialogInfo.timediff.maxValue/40+3;
  diffLcdnum= LCDNumber( difflength, this);
  diffSlider= new QSlider(Qt::Horizontal, this );
  diffSlider->setMinimum(dialogInfo.timediff.minValue);
  diffSlider->setMaximum(dialogInfo.timediff.maxValue);
  diffSlider->setValue(dialogInfo.timediff.value);
  QHBoxLayout* difflayout = new QHBoxLayout();
  difflayout->addWidget( diffLabel,0,0 );
  difflayout->addWidget( diffLcdnum, 0,0 );
  difflayout->addWidget( diffSlider,0,0  );
  connect( diffSlider, SIGNAL( valueChanged(int)), SLOT( doubleDisplayDiff(int) ));


  QPushButton* sathelp = NormalPushButton( tr("Help"), this );
  refresh = NormalPushButton(tr("Refresh"), this);

  miutil::miString more_str[2] = { tr("<<Less").toStdString(), tr("More>>").toStdString() };
  advanced= new ToggleButton( this, more_str );
  advanced->setChecked(false);

  QPushButton* sathide = NormalPushButton( tr("Hide"), this );
  QPushButton* satapplyhide = NormalPushButton( tr("Apply+hide"), this );
  QPushButton* satapply = NormalPushButton( tr("Apply"), this);
  satapply->setDefault( true );

  connect(  sathelp, SIGNAL(clicked()), SLOT( helpClicked()));
  connect( refresh, SIGNAL( clicked() ), SLOT( Refresh() ));
  connect( advanced, SIGNAL( toggled(bool)),SLOT( advancedtoggled( bool ) ));
  connect( sathide, SIGNAL(clicked()), SLOT(hideClicked()));
  connect( satapplyhide, SIGNAL(clicked()), SLOT(applyhideClicked()));
  connect( satapply, SIGNAL(clicked()), SIGNAL( SatApply()) );

  QHBoxLayout* hlayout2 = new QHBoxLayout();
  hlayout2->addWidget(upPictureButton);
  hlayout2->addWidget( Delete );
  hlayout2->addWidget( DeleteAll );

  QHBoxLayout* hlayout1 = new QHBoxLayout();
  hlayout1->addWidget(downPictureButton);
  hlayout1->addWidget( multiPicture );
  hlayout1->addWidget( mosaic );

  QHBoxLayout* hlayout4 = new QHBoxLayout();
  hlayout4->addWidget( sathelp );
  hlayout4->addWidget( refresh  );
  hlayout4->addWidget( advanced );

  QHBoxLayout* hlayout3 = new QHBoxLayout();
  hlayout3->addWidget( sathide );
  hlayout3->addWidget( satapplyhide );
  hlayout3->addWidget( satapply );

  QVBoxLayout* vlayout = new QVBoxLayout( this);
  vlayout->setMargin(10);
  vlayout->addWidget( namebox );
  vlayout->addWidget( fileListWidget );
  vlayout->addLayout( timefileLayout );
  vlayout->addWidget( timefileList );
  vlayout->addWidget( channellabel );
  vlayout->addWidget( channelbox );
  vlayout->addWidget( picturesLabel );
  vlayout->addWidget( pictures );
  vlayout->addLayout( hlayout2 );
  vlayout->addLayout( hlayout1 );
  vlayout->addLayout( difflayout );
  vlayout->addLayout( hlayout4 );
  vlayout->addLayout( hlayout3 );
  vlayout->activate();
  //       vlayout->freeze();


  // INNITIALISATION AND DEFAULT
  doubleDisplayDiff(dialogInfo.timediff.value);

  this->hide();
  setOrientation(Qt::Horizontal);
  sda = new SatDialogAdvanced( this,  dialogInfo);
  setExtension(sda);
  advancedtoggled(false);
  connect(sda,SIGNAL(getSatColours()),SLOT(updateColours()));
  connect(sda,SIGNAL(SatChanged()),SLOT(advancedChanged()));

  //****

  times.clear();

#ifdef dSatDlg
  cerr<<"SatDialog::ConstructorCernel returned"<<endl;
#endif

  return;
}


/*********************************************/
void SatDialog::nameActivated(int in)
{
#ifdef dSatDlg
  cerr<<"SatDialog::nameActivated called"<<endl;
#endif

  //insert in fileListWidget the list of files for selected satellite
  //but no file is selected, takes to much time
  updateFileListWidget(in);

  //clear timefileList and channelbox
  timefileList->clear();
  channelbox->clear();

#ifdef dSatDlg
  cerr<<"SatDialog::nameActivated returned"<<endl;
#endif

  return;
}

/*********************************************/
void SatDialog::fileListWidgetClicked(QListWidgetItem * item)
{
#ifdef dSatDlg
  cerr<<"SatDialog::fileListWidgetClicked called"<<endl;
#endif

  QApplication::setOverrideCursor(Qt::WaitCursor);

  int index = timefileBut->checkedId();

  //restore options if possible
  miutil::miString name = namebox->currentText().toStdString();
  miutil::miString area = item->text().toStdString();
  if (satoptions[name][area].exists()) {
    vector<miutil::miString> tokens = satoptions[name][area].split(" ");
    state okVar = decodeString(tokens);
    putOptions(okVar);
    bool restore = multiPicture->isChecked();
    multiPicture->setChecked(false);
    timefileBut->button(index)->setChecked(true);
    timefileClicked(index);
    multiPicture->setChecked(restore);
  } else { //no options saved
    timefileBut->button(index)->setChecked(true);
    timefileClicked(index);
  }

  QApplication::restoreOverrideCursor();

#ifdef dSatDlg
  cerr<<"fileListWidgetClicked returned"<<endl;
#endif
  return;
}

/*********************************************/
void SatDialog::timefileClicked(int tt)
{

  /* DESCRIPTION: This function is called when timefileBut (auto/tid/fil)is
   selected, and is returned without doing anything if the new value
   selected is equal to the old one
   */
#ifdef dSatDlg
  cerr<<" SatDialog::timefileClicked called:"<<tt<<endl;
#endif

  if (fileListWidget->currentRow() == -1)
    return;

  //update list of files
  updateTimefileList();

  if (tt == 0) {
    // AUTO clicked

    m_ctrl->setSatAuto(true, namebox->currentText().toStdString(),
        fileListWidget->currentItem()->text().toStdString());

    //    timefileList->setEnabled( false );
    updateChannelBox(true);

  } else {
    // "time"/"file" clicked

    m_ctrl->setSatAuto(false, namebox->currentText().toStdString(),
        fileListWidget->currentItem()->text().toStdString());

    //    timefileList->setEnabled ( true );

    timefileListSlot(timefileList->currentItem());
  }

#ifdef dSatDlg
  cerr<<" SatDialog::timefileClicked returned"<<endl;
#endif
  return;
}

/*********************************************/
void SatDialog::timefileListSlot(QListWidgetItem *)
{
  /* DESCRIPTION: This function is called when the signal highlighted() is
   sent from the list of time/file and a new list item is highlighted
   */
#ifdef dSatDlg
  cerr<<"SatDialog::timefileListSlot called"<<endl;
#endif

  int index = timefileList->currentRow();

  if (index < 0 && timefileList->count() > 0)
    index = 0;

  if (index < 0 || int(files.size()) <= index)
    return;

  m_time = files[index].time;
  vector<miutil::miTime> tt;
  tt.push_back(m_time);emit
  emitTimes("sat", tt, false);

  //  m_timefileListIndex = index;

  updateChannelBox(true);

#ifdef dSatDlg
  cerr<<"SatDialog::timefileListSlot returned"<<endl;
#endif
  return;
}

/*********************************************/
void SatDialog::channelboxSlot(QListWidgetItem * item)
{
  /* DESCRIPTION: This function is called when a channel is selected
   -add new state to m_state (if picture already exists with this)
   satellite, just replace, else add.
   update pictures box accordingly
   */

#ifdef dSatDlg
  cerr<<"channelboxSlot called"<<endl;
#endif

  //currently selected channel
  //  if(channelbox->currentRow() == -1) return;
  m_channelstr = item->text().toStdString();

  int newIndex = addSelectedPicture();

  // AF: avoid coredump, and delete a previous selection in same "group"
  if (newIndex < 0) {
    cerr << "   newIndex " << newIndex << endl;
    if (newIndex == -1)
      DeleteClicked();
    return;
  }

  updatePictures(newIndex, false);

#ifdef dSatDlg
  cerr<<"channelboxSlot returned"<<endl;
#endif
  return;
}


/*********************************************/
int SatDialog::addSelectedPicture()
{
  /* DESCRIPTION: this function adds a new picture to
   the list of pictures to plot, in the vector m_state,
   with information from the dialogbox...
   If a picture already exists for this satellite, replace,
   else add a new one.
   return index for this picture...
   */
#ifdef dSatDlg
  cerr<<"SatDialog::addSelectedPicture" << endl;
#endif

  if (!files.size())
    return -2;

  miutil::miString fstring;
  miutil::miTime ltime;
  if (timeButton->isChecked() || fileButton->isChecked()) {
    //"time"/"file" clicked, find filename
    int current = timefileList->currentRow();
    if (current > -1 && current < int(files.size())) {
      fstring = files[current].name;
      ltime = files[current].time;
    }
  }

  //define a new state !
  state lstate;
  lstate.iname = namebox->currentIndex();
  lstate.iarea = fileListWidget->currentRow();
  lstate.iautotimefile = timefileBut->checkedId();
  lstate.ifiletime = timefileList->currentRow();
  lstate.ichannel = channelbox->currentRow();
  lstate.name = namebox->currentText().toStdString();
  lstate.area = fileListWidget->currentItem()->text().toStdString();
  lstate.filetime = ltime;
  lstate.channel = channelbox->currentItem()->text().toStdString();
  lstate.filename = fstring;
  lstate.mosaic = false;
  lstate.totalminutes = 60;

  int newIndex = -1;

  //check if we tried to select an existing picture
  //   for( int i=0; i<m_state.size(); i++ ){
  //      if(pictureString(lstate,true)== pictureString(m_state[i],true)){
  //        //set already existing picture as selected
  //        newIndex=i;
  //        break;
  //      }
  //   }


  if (!multiPicture->isChecked()) {
    int i = pictures->currentRow();
    if (i > -1 && i < int(m_state.size())) {
      //replace existing picture(same sat). advanced options saved
      // get info about picture we are replacing
      vector<SatFileInfo> f = m_ctrl->getSatFiles(m_state[i].name,
          m_state[i].area, false);
      lstate.mosaic = m_state[i].mosaic;
      lstate.totalminutes = m_state[i].totalminutes;
      if ((f.size() && f[0].palette && !files[0].palette) || (f.size()
          && !f[0].palette && files[0].palette)) {
        //special case changing from palette to rgb or vice versa
        sda->setStandard();
        lstate.advanced = sda->getOKString();
      } else if (files[0].palette && lstate.area != m_state[i].area) {
        //clear colour selection in advanced
        lstate.advanced = m_state[i].advanced;
        sda->putOKString(lstate.advanced);
        vector<Colour> c;
        sda->setColours(c);
        lstate.advanced = sda->getOKString();
      } else {
        lstate.advanced = m_state[i].advanced;
      }
      m_state[i] = lstate;
      newIndex = i;
    }

  }

  lstate.OKString = makeOKString(lstate);
  if (newIndex == -1) {
    //new picture (new sat)
    sda->setStandard();
    lstate.advanced = sda->getOKString();
    m_state.push_back(lstate);
    newIndex = m_state.size() - 1;
  }

#ifdef dSatDlg
  cerr<<"returning from addSelectedPicture" << endl;
#endif

  return newIndex;

}

/*********************************************/
miutil::miString SatDialog::pictureString(state i_state, bool timefile)
{
  /* make a string of the picture in m_state[i]*/
#ifdef dSatDlg
  cerr<<"SatDialog::picturesString" << endl;
#endif
  miutil::miString str = i_state.name;
  if (i_state.mosaic)
    str += " MOSAIKK ";
  str += " " + i_state.area + " " + i_state.channel + " ";
  if (timefile && !i_state.filename.empty())
    str += i_state.filetime.isoTime();
  return str;
}

/*********************************************/
void SatDialog::picturesSlot(QListWidgetItem * item)
{
#ifdef dSatDlg
  cerr<<"SatDialog::picturesSlot"<< endl;
  cerr <<"m_state.size:"<<m_state.size()<<endl;
#endif

  miutil::miString str, advanced;
  vector<Colour> colours;
  int index = pictures->currentRow();

  if (index > -1) {
    vector<miutil::miString> vstr;
    vstr.push_back(m_state[index].OKString);
    namebox->setCurrentIndex(m_state[index].iname);
    nameActivated(m_state[index].iname); // update fileListWidget
    fileListWidget->setCurrentItem(fileListWidget->item(m_state[index].iarea));
    if (m_state[index].iautotimefile == 0) {
      autoButton->setChecked(true);
      m_ctrl->setSatAuto(true, namebox->currentText().toStdString(),
          fileListWidget->currentItem()->text().toStdString());
    } else {
      if (m_state[index].iautotimefile == 1)
        timeButton->setChecked(true);
      else if (m_state[index].iautotimefile == 2)
        fileButton->setChecked(true);
      m_ctrl->setSatAuto(false, namebox->currentText().toStdString(),
          fileListWidget->currentItem()->text().toStdString());
      updateTimefileList();
      timefileList->setCurrentRow(m_state[index].ifiletime);
      vector<miutil::miTime> tt;
      tt.push_back(files[m_state[index].ifiletime].time);
      emit emitTimes("sat", tt, false);
    }
    updateChannelBox(false);
    channelbox->setCurrentRow(m_state[index].ichannel);
    m_channelstr = m_state[index].channel;
    updateColours();
    str = pictureString(m_state[index], false);
    sda->setPictures(str);
    advanced = m_state[index].advanced;
    sda->putOKString(advanced);
    sda->greyOptions();
    int number = int(m_state[index].totalminutes / m_scalediff);
    diffSlider->setValue(number);
    bool mon = m_state[index].mosaic;
    mosaic->setChecked(mon);
    mosaic->setEnabled(true);
  } else {
    sda->setPictures("");
    //colours is empty
    sda->setColours(colours);
  }

  //check if up and down buttons should be enabled
  if (m_state.size() > 1) {
    if (index == 0) {
      //downbutton
      downPictureButton->setEnabled(true);
      upPictureButton->setEnabled(false);
    } else if (index == int(m_state.size()) - 1) {
      //upbutton
      upPictureButton->setEnabled(true);
      downPictureButton->setEnabled(false);
    } else {
      downPictureButton->setEnabled(true);
      upPictureButton->setEnabled(true);
    }
  } else {
    downPictureButton->setEnabled(false);
    upPictureButton->setEnabled(false);
  }

}


/*********************************************/
void SatDialog::RefreshList()
{
#ifdef dSatDlg
  cerr<<"SatDialog::RefreshList() called" <<endl;
#endif
  // update the dialog and timeslider
  int old_picturesIndex = pictures->currentRow();
  updateTimefileList();
  pictures->setCurrentRow(old_picturesIndex);

  //check if m_state - "Valgte bilder" contains pictures no longer existing
  //give a qmessagebox warning
  for (unsigned int i = 0; i < m_state.size(); i++) {
    bool found = false;
    if (m_state[i].filename.empty()) {
      found = true;
    } else {
      vector<SatFileInfo> f = m_ctrl->getSatFiles(m_state[i].name,
          m_state[i].area, false);
      int n = f.size();
      for (int j = 0; j < n; j++)
        if (m_state[i].filename == f[j].name) {
          found = true;
          break;
        }
      if (!found) {
        QString tmp1 = "<nobr>" + tr("Chosen picture") + " <i>";
        QString tmp2 = pictureString(m_state[i], true).c_str();
        QString tmp3 = " </i></nobr> <br>" + tr("No longer available on disk");
        QString messagestring = tmp1 + tmp2 + tmp3;
        QMessageBox::information(this, tr("Satellite dialog - info"),
            messagestring);
      }
    }
  }

  emitSatTimes();

#ifdef dSatDlg
  cerr << "Returning from refreshList" << endl;
#endif

  return;
}

/*********************************************/

void SatDialog::Refresh()
{
#ifdef dSatDlg
  cerr<<"SatDialog::Refresh() called; Filene blir hentet nytt fra disken"
  <<endl;
#endif
  QApplication::setOverrideCursor(Qt::WaitCursor);

  for (unsigned int i = 0; i < m_state.size(); i++) {
    //    if(m_state[i].filename.empty()){
    //auto option for this state
    m_ctrl->SatRefresh(m_state[i].name, m_state[i].area);
    //    }
  }

  RefreshList();

  QApplication::restoreOverrideCursor();
  return;
}

/*********************************************/

void SatDialog::mosaicToggled(bool on)
{

  int index = pictures->currentRow();
  if (index > -1 && int(m_state.size()) > index)
    m_state[index].mosaic = on;
  updatePictures(index, false);

}

/*********************************************/

void SatDialog::advancedChanged()
{

  int index = pictures->currentRow();
  miutil::miString advancedstring = sda->getOKString();
  if (index > -1)
    m_state[index].advanced = advancedstring;
}

/**********************************************/

void SatDialog::upPicture()
{

  int index = pictures->currentRow();
  state lstate = m_state[index];
  m_state[index] = m_state[index - 1];
  m_state[index - 1] = lstate;
  updatePictures(index - 1, true);

}

/**********************************************/

void SatDialog::downPicture()
{

  int index = pictures->currentRow();
  state lstate = m_state[index];
  m_state[index] = m_state[index + 1];
  m_state[index + 1] = lstate;
  updatePictures(index + 1, true);

}

/*********************************************/
void SatDialog::doubleDisplayDiff(int number)
{
  /* This function is called when diffSlider sends a signald valueChanged(int)
   and changes the numerical value in the lcd display diffLcdnum */
  int totalminutes = int(number * m_scalediff);
  int index = pictures->currentRow();
  if (index > -1 && int(m_state.size()) > index)
    m_state[index].totalminutes = totalminutes;
  int hours = totalminutes / 60;
  int minutes = totalminutes - hours * 60;
  ostringstream ostr;
  ostr << hours << ":" << setw(2) << setfill('0') << minutes;
  miutil::miString str = ostr.str();
  diffLcdnum->display(str.c_str());
}

/*********************************************/
void SatDialog::DeleteAllClicked()
{
#ifdef dSatDlg
  cerr<<"SatDialog::DeleteAllClicked called"<<endl;
#endif

  fileListWidget->clearSelection();
  timefileList->clear();
  channelbox->clear();

  m_state.clear();
  //  m_picturesIndex = -1;
  pictures->clear();
  downPictureButton->setEnabled(false);
  upPictureButton->setEnabled(false);
  mosaic->setChecked(false);
  mosaic->setEnabled(false);
  sda->setPictures("");
  sda->setOff();
  sda->greyOptions();

  downPictureButton->setEnabled(false);
  upPictureButton->setEnabled(false);

  //Emit empty time list
  times.clear();emit
  emitTimes("sat", times, false);

#ifdef dSatDlg
  cerr<<"SatDialog::DeleteAllClicked returned"<<endl;
#endif
  return;
}

/*********************************************/
void SatDialog::DeleteClicked()
{
#ifdef dSatDlg
  cerr<<"SatDialog::DeleteClicked called"<<endl;
#endif

  if (m_state.size() == 1) {
    // check needed when empty list was found
    DeleteAllClicked();
  } else {
    if (pictures->currentRow() > -1) {
      //remove from m_state and picture listbox
      vector<state>::iterator p;
      p = m_state.begin() + pictures->currentRow();
      m_state.erase(p);
      pictures->takeItem(pictures->currentRow());
      //      if (m_picturesIndex > m_state.size()-1) m_picturesIndex--;
      //      updatePictures(pictures->currentRow());
      picturesSlot(pictures->currentItem());
      emitSatTimes();
    }
  }

}
/*********************************************/
vector<miutil::miString> SatDialog::getOKString()
{
#ifdef dSatDlg
  cerr<<"SatDialog::getOKString() called"<<endl;
#endif

  vector<miutil::miString> vstr;

  if (pictures->count()) {

    for (unsigned int i = 0; i < m_state.size(); i++) {
      miutil::miString str = makeOKString(m_state[i]);
      satoptions[m_state[i].name][m_state[i].area] = str;
      vstr.push_back(str);
    }

  }

  return vstr;
}

/********************************************/

miutil::miString SatDialog::makeOKString(state & okVar)
{
  /* This function is called by getOKString,
   makes the part of OK string corresponding to state okVar  */
#ifdef dSatDlg
  cerr << "SatDialog::makeOKString" << endl;
#endif

  miutil::miString str = "SAT ";
  str += okVar.name;
  str += " ";
  str += okVar.area;
  str += " ";
  str += okVar.channel;
  if (!okVar.filename.empty()) {
    str += " file=";
    str += okVar.filename;
  }
  ostringstream ostr;
  ostr << " timediff=" << okVar.totalminutes;
  str += ostr.str();

  if (okVar.mosaic)
    str += " mosaic=1";
  else
    str += " mosaic=0";

  str += okVar.advanced;

  str += " font=BITMAPFONT face=normal";

  str += okVar.external;

  //should only be cleared if something has changed at this picture
  okVar.external.clear();
  return str;
}

/*********************************************
 ***********quickMenu functions***************
 **********************************************/

void SatDialog::putOKString(const vector<miutil::miString>& vstr)
{
  /* Called from MainWindow to put vstr values into dialog  */
#ifdef dSatDlg
  cerr << "SatDialog::putOKstring" << endl;
#endif

  //update dialog
  DeleteAllClicked();
  int npi = vstr.size();

  if (npi == 0)
    return;

  bool restore = multiPicture->isChecked();
  multiPicture->setChecked(true);

  m_state.clear();
  // loop through all PlotInfo's
  for (int ip = 0; ip < npi; ip++) {
    //decode string
    vector<miutil::miString> tokens = vstr[ip].split('"', '"');
    state okVar = decodeString(tokens);

    if (okVar.name.empty() || okVar.area.empty() || okVar.channel.empty())
      break;
    bool found = false;
    int ns = namebox->count();
    for (int j = 0; j < ns; j++) {
      QString qstr = namebox->itemText(j);
      if (qstr.isNull())
        continue;
      miutil::miString listname = qstr.toStdString();
      if (okVar.name == listname) {
        namebox->setCurrentIndex(j);
        nameActivated(j);
        found = true;
      }
    }
    if (!found)
      continue;

    found = false;
    int ng = fileListWidget->count();
    for (int j = 0; j < ng; j++) {
      QString qstr = fileListWidget->item(j)->text();
      if (qstr.isNull())
        continue;
      miutil::miString listname = qstr.toStdString();
      if (okVar.area == listname) {
        fileListWidget->setCurrentRow(j);
        found = true;
      }
    }
    if (!found)
      continue;

    putOptions(okVar);
  }

  multiPicture->setChecked(restore);
}

void SatDialog::putOptions(const state okVar)
{

  if (!okVar.filetime.undef() && okVar.filetime != ztime) {
    timefileBut->button(1)->setChecked(true);
    updateTimefileList();
    int nt = files.size();
    for (int j = 0; j < nt; j++) {
      if (okVar.filetime == files[j].time) {
        timefileList->setCurrentRow(j);
      }
    }
  } else if (!okVar.filename.empty()) {
    timefileBut->button(2)->setChecked(true);;
    updateTimefileList();
    int nf = files.size();
    for (int j = 0; j < nf; j++) {
      if (okVar.filename == files[j].name) {
        timefileList->setCurrentRow(j);
      }
    }
  } else {
    timefileBut->button(0)->setChecked(true); //auto
    updateTimefileList();
  }

  updateChannelBox(false);
  bool found = false;
  int nc = channelbox->count();
  for (int j = 0; j < nc; j++) {
    QString qstr = channelbox->item(j)->text();
    if (qstr.isNull())
      continue;
    miutil::miString listchannel = qstr.toStdString();
    if (okVar.channel == listchannel) {
      unsigned int np = m_state.size();
      channelbox->setCurrentRow(j);
      channelboxSlot(channelbox->currentItem());
      // check if new picture added..
      found = (m_state.size() > np);
      found = true;
      break;
    }
  }

  if (!found)
    return;

  if (okVar.totalminutes >= 0) {
    int number = int(okVar.totalminutes / m_scalediff);
    diffSlider->setValue(number);
  }
  mosaic->setChecked(okVar.mosaic);

  //advanced dialog
  int index = pictures->currentRow();
  if (index < 0 || index >= int(m_state.size()))
    return;
  m_state[index].external = sda->putOKString(okVar.advanced);
  advancedChanged();
  m_state[index].advanced = sda->getOKString();
}
/*********************************************/

SatDialog::state SatDialog::decodeString(const vector<miutil::miString> & tokens)
{
  /* This function is called by putOKstring.
   It decodes tokens, and puts plot variables into struct state */

#ifdef dObjectDlg
  cerr << "ObjectDialog::decodeString" << endl;
#endif

  state okVar;
  okVar.name = "";

  int n = tokens.size();
  if (n < 4)
    return okVar;

  okVar.name = tokens[1];
  okVar.area = tokens[2];
  okVar.channel = tokens[3];
  okVar.totalminutes = -1;
  okVar.mosaic = false;

  //loop
  miutil::miString token;
  for (int i = 4; i < n; i++) {
    token = tokens[i].downcase();
    miutil::miString key, value;
    vector<miutil::miString> stokens = tokens[i].split('=');
    if (stokens.size() == 2) {
      key = stokens[0].downcase();
      value = stokens[1];
    }
    if (key == "time") {
      okVar.filetime = timeFromString(value);
    } else if (key == "file") {
      okVar.filename = value;
    } else if (key == "timediff") {
      okVar.totalminutes = atoi(value.c_str());
    } else if (key == "mosaic") {
      okVar.mosaic = (atoi(value.c_str()) != 0);
      //ignore keys "font" and "face"
    } else if (key != "font" && key != "face") {
      //add to advanced string
      okVar.advanced += " " + token;
    }
  }

  return okVar;

}

/*********************************************/

miutil::miString SatDialog::getShortname()
{
  miutil::miString name;

  if (pictures->count()) {
    for (unsigned int i = 0; i < m_state.size(); i++)
      name += pictureString(m_state[i], false);
  }
  if (name.exists())
    name = "<font color=\"#990000\">" + name + "</font>";
  return name;
}

/********************************************/

/*********************************************
 ****  end of quickMenu functions***************
 **********************************************/

void SatDialog::applyhideClicked()
{
  emit SatHide();
  emit SatApply();
}

/********************************************/
void SatDialog::helpClicked()
{
  emit showsource("ug_satdialogue.html");
}

/********************************************/
void SatDialog::advancedtoggled(bool on)
{
  showExtension(on);
  return;
}

/********************************************/
void SatDialog::hideClicked()
{
  emit SatHide();
}

/**********************************************/

void SatDialog::updateFileListWidget(int in)
{
  // cerr <<"updateFileListWidget:"<<in<<endl;
  if ( in < int(dialogInfo.image.size())) {

    fileListWidget->clear();
    //insert in fileListWidget the list of files.. Europa,N-Europa etc...
    int nfile = dialogInfo.image[in].file.size();
    for (int k = 0; k < nfile; k++) {
      fileListWidget->addItem(dialogInfo.image[in].file[k].name.cStr());
    }

  }
}

/**********************************************/

void SatDialog::updateTimefileList()
{
#ifdef dSatDlg
  cerr<<"satDialog::updateTimefileList called" <<endl;
#endif

  //if no file group selected, nothing to do
  if (fileListWidget->currentRow() == -1)
    return;

  //clear box with list of files
  timefileList->clear();

  //get new list of sat files
  QApplication::setOverrideCursor(Qt::WaitCursor);
  files = m_ctrl->getSatFiles(namebox->currentText().toStdString(),
      fileListWidget->currentItem()->text().toStdString(), true);
  QApplication::restoreOverrideCursor();

  if (autoButton->isChecked())
    return;

  channelbox->clear();

  int nr_file = files.size();
  if (nr_file == 0)
    return;

  //insert times into timefileList
  if (timeButton->isChecked()) {

    for (int i = 0; i < nr_file; i++) {
      timefileList->addItem(QString(files[i].time.isoTime().cStr()));
    }

  } else if (fileButton->isChecked()) {

    for (int i = 0; i < nr_file; i++) {
      timefileList->addItem(QString(files[i].name.c_str()));
    }

  }

  if (timeButton->isChecked() || fileButton->isChecked()) {
    //set current item in timefileList to same as before, or first item
    int timeDefIndex = -1;
    for (int i = 0; i < nr_file; i++) {
      if (m_time == files[i].time) {
        timeDefIndex = i;
      }
    }

    //    m_timefileListIndex=timeDefIndex;
    if (timeDefIndex > -1)
      timefileList->setCurrentRow(timeDefIndex);
    else {
      m_time = miutil::miTime();
      timefileList->setCurrentRow(0);
    }
  }

  return;
}


/**********************************************/
void SatDialog::updateChannelBox(bool select)
{
  /* DESCRIPTION: This function updates the list of channels in
   ChannelBox.
   If auto is pressed the list is taken from SatDialogInfo (setupfile)
   If "time" or "file" is pressed the list is taken from file header
   Selected channel is set to the same as before, if possible
   */

#ifdef dSatDlg
  cerr<<"satDialog::updateChannelBox called" <<endl;
#endif

  channelbox->clear();

  vector<miutil::miString> vstr;
  int index;
  if (autoButton->isChecked())
    index = -1;
  else
    index = timefileList->currentRow();

  vstr = m_ctrl->getSatChannels(namebox->currentText().toStdString(),
      fileListWidget->currentItem()->text().toStdString(), index);

  int nr_channel = vstr.size();
  if (nr_channel <= 0)
    return;

  for (int i = 0; i < nr_channel; i++) {
    vstr[i].trim();
    channelbox->addItem(QString(vstr[i].c_str()));
  }

  //insert string list in channelbox
  channelbox->setEnabled(true);

  if (!select)
    return;
  //HK ??? comment out this part which remembers selected channels

  m_channelstr.trim();

  //selct same channel as last time, if possible ...
  for (int i = 0; i < nr_channel; i++) {
    if (m_channelstr == vstr[i]) {
      channelbox->setCurrentRow(i);
      channelboxSlot(channelbox->currentItem());
      return;
    }
  }

  // ... or first channel
  m_channelstr = vstr[0];
  channelbox->setCurrentRow(0);
  channelboxSlot(channelbox->currentItem());

  //  cerr <<"returning from updateChannelBox()"<<endl;

  return;
}



/**********************************************/
void SatDialog::updatePictures(int index, bool updateAbove)
{
#ifdef dSatDlg
  cerr<<"SatDialog::updatePictures called:" <<index<<endl;
#endif
  /* DESCRIPTION: This function updates the list of selected pictures
   send new list of times to timeslider
   */
  pictures->clear();

  for (unsigned int i = 0; i < m_state.size(); i++) {
    //insert item into picturebox
    miutil::miString str = pictureString(m_state[i], true);
    pictures->addItem(str.c_str());
  }

  if (index > -1 && index < int(m_state.size())) {
    pictures->setCurrentRow(index);
    if (updateAbove)
      picturesSlot(pictures->currentItem());
    updateColours();
    miutil::miString str = pictureString(m_state[index], false);
    sda->setPictures(str);
    miutil::miString advanced = m_state[index].advanced;
    sda->putOKString(advanced);
    sda->greyOptions();
    int number = int(m_state[index].totalminutes / m_scalediff);
    diffSlider->setValue(number);
    bool mon = m_state[index].mosaic;
    mosaic->setChecked(mon);
    mosaic->setEnabled(true);
  }

  //check if up and down buttons should be enabled
  if (m_state.size() > 1) {
    if (index == 0) {
      //downbutton
      downPictureButton->setEnabled(true);
      upPictureButton->setEnabled(false);
    } else if (index == int(m_state.size()) - 1) {
      //upbutton
      upPictureButton->setEnabled(true);
      downPictureButton->setEnabled(false);
    } else {
      downPictureButton->setEnabled(true);
      upPictureButton->setEnabled(true);
    }
  } else {
    downPictureButton->setEnabled(false);
    upPictureButton->setEnabled(false);
  }

  emitSatTimes();
  //  cerr <<"returning from updatePictures"<<endl;
}

/**********************************************/
void SatDialog::updateColours()
{
#ifdef dSatDlg
  cerr<<"SatDialog::updateColours called" <<endl;
#endif

  int index = pictures->currentRow();
  if (index > -1) {
    state lstate = m_state[index];
    vector<Colour> colours = m_ctrl->getSatColours(lstate.name, lstate.area);
    sda->setColours(colours);
  }
}

/*********************************************/
void SatDialog::emitSatTimes(bool update)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  //    cerr <<"emitSatTimes"<<endl;
  times.clear();
  set<miutil::miTime> timeset;

  for (unsigned int i = 0; i < m_state.size(); i++) {
    if (m_state[i].filename.empty() || update) {
      //auto option for this state
      //get times to send to timeslider
      vector<SatFileInfo> f = m_ctrl->getSatFiles(m_state[i].name,
          m_state[i].area, update);
      for (unsigned int i = 0; i < f.size(); i++)
        timeset.insert(f[i].time);
    } else
      timeset.insert(m_state[i].filetime);
  }

  if (timeset.size() > 0) {
    set<miutil::miTime>::iterator p = timeset.begin();
    for (; p != timeset.end(); p++)
      times.push_back(*p);
  }

  bool useTimes = (times.size() > m_state.size());
  emit
  emitTimes("sat", times, useTimes);
  QApplication::restoreOverrideCursor();
}

/*********************************************/

miutil::miString SatDialog::stringFromTime(const miutil::miTime& t)
{

  ostringstream ostr;
  ostr << setw(4) << setfill('0') << t.year() << setw(2) << setfill('0')
      << t.month() << setw(2) << setfill('0') << t.day() << setw(2) << setfill(
      '0') << t.hour() << setw(2) << setfill('0') << t.min();

  return ostr.str();
}

miutil::miTime SatDialog::timeFromString(const miutil::miString &timeString)
{
  //get time from a string with yyyymmddhhmm
  int year = atoi(timeString.substr(0, 4).c_str());
  int mon = atoi(timeString.substr(4, 2).c_str());
  int day = atoi(timeString.substr(6, 2).c_str());
  int hour = atoi(timeString.substr(8, 2).c_str());
  int min = 0;
  if (timeString.length() >= 12)
    min = atoi(timeString.substr(10, 2).c_str());
  if (year < 0 || mon < 0 || day < 0 || hour < 0 || min < 0)
    return ztime;
  return miutil::miTime(year, mon, day, hour, min, 0);
}

vector<miutil::miString> SatDialog::writeLog()
{
  vector<miutil::miString> vstr;
  map<miutil::miString, map<miutil::miString, miutil::miString> >::iterator p = satoptions.begin();
  map<miutil::miString, map<miutil::miString, miutil::miString> >::iterator pend = satoptions.end();

  while (p != pend) {
    map<miutil::miString, miutil::miString>::iterator q = p->second.begin();
    map<miutil::miString, miutil::miString>::iterator qend = p->second.end();
    while (q != qend) {
      vstr.push_back(satoptions[p->first][q->first]);
      q++;
    }
    p++;
  }

  return vstr;
}

void SatDialog::readLog(const vector<miutil::miString>& vstr,
    const miutil::miString& thisVersion, const miutil::miString& logVersion)
{
  int n = vstr.size();
  for (int i = 0; i < n; i++) {
    vector<miutil::miString> tokens = vstr[i].split(" ");
    if (tokens.size() < 4)
      continue;
    satoptions[tokens[1]][tokens[2]] = vstr[i];
  }

}

void SatDialog::closeEvent(QCloseEvent* e)
{
  emit SatHide();
}


























