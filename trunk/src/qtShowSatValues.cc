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

#include <qtShowSatValues.h>

#include <QLabel>
#include <QComboBox>
#include <QToolTip>
#include <QHBoxLayout>
#include <QFrame>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

ShowSatValues::ShowSatValues(QWidget* parent)
  : QWidget(parent) {

  // Create horisontal lay1out manager
  QHBoxLayout* thlayout = new QHBoxLayout( this);
  thlayout->setMargin(1);
  thlayout->setSpacing(5);

  channelbox = new QComboBox(this);
  channelbox->setMinimumWidth(channelbox->sizeHint().width());
  connect(channelbox,SIGNAL(activated(int)),SLOT(channelChanged(int)));
  thlayout->addWidget(channelbox);

  chlabel= new QLabel("XXXXXXXXXXXX",this);
  chlabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  chlabel->setMinimumSize(chlabel->sizeHint());
  chlabel->setText("    ");
  thlayout->addWidget(chlabel,0);

}

void ShowSatValues::channelChanged(int index)
{
  if( index < int(tooltip.size()) && index > -1 ){
    miutil::miString tip = tooltip[index].replace('|',' ');
    channelbox->setToolTip( QString(tip.cStr() ));
  }
}

void ShowSatValues::SetChannels(const vector<miutil::miString>& channel)
{
  int nch = channel.size();

  //  cerr << " ShowSatValues::SetChannels:"<<channel.size()<<endl;
  //try to remember currentItem
  int index = -1;
  if(tooltip.size( )> 0 && channelbox->count()>0  ){
    miutil::miString currentText = tooltip[channelbox->currentIndex()];
    int i = 0;
    while(i<nch && channel[i]!=currentText) i++;
    if(i<nch) index=i;
  }

  //refresh channelbox
  channelbox->clear();
  for(int i=0;i<nch;i++){
    //    cerr <<"channel:"<<i<<"  "<<channel[i]<<endl;
    vector<miutil::miString> token = channel[i].split("|");
    if(token.size()==2){
      channelbox->addItem(token[1].cStr());
      //if no currentItem, use channel"4"
      if(index == -1 && token[1].contains("4")) index = i;
    }
  }
  tooltip = channel;

  //reset currentItem
  if(index>-1)
    channelbox->setCurrentIndex(index);

  //update tooltip
  if(nch > 0)
    channelChanged(channelbox->currentIndex());
  else
    channelbox->setToolTip( QString());

}

void ShowSatValues::ShowValues(const vector<SatValues> &satval)
{
  if(!channelbox->count()) return;
  ostringstream svalue;
  int n = satval.size();
  int i=0;
//   cerr <<"ShowValues:"<<n<<endl;
//   cerr <<"channelbox->currentItem():"<<channelbox->currentItem()<<endl;
  while(i<n && satval[i].channel != tooltip[channelbox->currentIndex()]) i++;

  //no value
  if(i==n){
     chlabel->setText("");
     return;
  }

   if (satval[i].channel.contains("Infrared")
       || satval[i].channel.contains("IR_CAL")
       || satval[i].channel.contains("TEMP"))
     svalue << setprecision(1) << setiosflags(ios::fixed)<< satval[i].value <<"�C";
   else
     svalue << setprecision(2) << setiosflags(ios::fixed)<< satval[i].value;
   //check values
//    cerr <<"satval[i].value:"<<satval[i].value<<endl;
   if (satval[i].value < -999)
     chlabel->setText(satval[i].text.c_str());
   else
     chlabel->setText(svalue.str().c_str());

}














