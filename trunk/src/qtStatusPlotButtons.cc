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

#include <qtStatusPlotButtons.h>

#include <QToolTip>
#include <QLabel>
#include <QHBoxLayout>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QImage>
#include <QPixmap>
#include <QAction>
#include <QMenu>

#include <iostream>
#include <qtImageGallery.h>
#include <question.xpm>

static bool oktoemit;


PlotButton::PlotButton(QWidget * parent,
    PlotElement& pe)
: QToolButton(parent)
{
  setMinimumWidth(30);

  setPlotElement(pe);

  connect(this,SIGNAL(toggled(bool)),SLOT(togg(bool)));

  origcolor_= palette().color(backgroundRole());

}

void PlotButton::setPlotElement(const PlotElement& pe)
{
  QtImageGallery ig;
  miutil::miString str= pe.str;
  if (pe.icon.exists()){
    if (plotelement_.icon!=pe.icon ){
      QImage image;
      if(ig.Image(pe.icon,image)){
        setIcon(QPixmap::fromImage(image));
      } else {
        setIcon(QIcon(question_xpm));
      }
    }
  } else {
    setIcon(QIcon(question_xpm));
  }

  plotelement_= pe;

  tipstr_= QString(str.cStr());
  setToolTip(tipstr_);
  setCheckable(true);
  setChecked(plotelement_.enabled);
}

void PlotButton::togg(bool b)
{
  plotelement_.enabled= b;
  if (oktoemit)
    emit enabled(plotelement_);
}

void PlotButton::highlight(bool b)
{

  QPalette pal(palette());
  QPalette::ColorRole role(backgroundRole());
  if (b){
    QColor color(origcolor_.dark(200));
    pal.setColor(role, color);
  } else {
    QColor color(origcolor_);
    pal.setColor(role, color);
  }
  setPalette(pal);
}


StatusPlotButtons::StatusPlotButtons(QWidget* parent)
: QWidget(parent), numbuttons(0), activebutton(0) {

  PlotElement pe;
  pe.enabled= true;
  oktoemit= false;

  QHBoxLayout* hl= new QHBoxLayout(this); // parent,margin
  hl->setMargin(1);
  for (int i=0; i<MAXBUTTONS; i++){
    buttons[i] = new PlotButton(this, pe);
    connect(buttons[i], SIGNAL(enabled(PlotElement)),
        this, SLOT(enabled(PlotElement)));
    hl->addWidget(buttons[i]);
  }

  showtip= new QMenu(this);

  //Action
  plotButtonsAction = new QAction( this );
  plotButtonsAction->setShortcutContext(Qt::ApplicationShortcut);
  plotButtonsAction->setShortcut(Qt::Key_End);
  connect( plotButtonsAction, SIGNAL( triggered() ),SLOT(setfocus()));
  addAction( plotButtonsAction );


}

void StatusPlotButtons::calcTipPos()
{
  int fx= x()+width();
  int bx= x()+(numbuttons>0 ? buttons[numbuttons-1]->x() +
      buttons[numbuttons-1]->width() : 0);

  int tx= (fx < bx ? fx : bx) + 10;
  int ty= y() + 5;

  QPoint localtip_pos= QPoint(tx,ty);
  tip_pos = mapToGlobal(localtip_pos);
}

void StatusPlotButtons::setfocus()
{

  grabKeyboard();
  activebutton= 0;

  showActiveButton(true);
}


void StatusPlotButtons::showText(const QString& s)
{
  calcTipPos();
  showtip->popup(tip_pos);

  showtip->clear();
}

void StatusPlotButtons::showActiveButton(bool b)
{
  if (activebutton>=0 && activebutton<numbuttons){
    buttons[activebutton]->highlight(b);
    
    int x= buttons[activebutton]->x();
    int y= buttons[activebutton]->y();
    //    sv->ensureVisible(x,y);

    if (b) showText(buttons[activebutton]->tipText());
  }
}

void StatusPlotButtons::releasefocus()
{
  showActiveButton(false);
  activebutton= 0;
  showActiveButton(false);
  releaseKeyboard();
  showtip->hide();
}

void StatusPlotButtons::keyPressEvent ( QKeyEvent * e )
{
  // no modifiers recognized here
  if (e->modifiers() != Qt::NoButton){
    releasefocus();
    e->ignore();
    return;
  }

  if (e->key() == Qt::Key_Left){
    if (activebutton > 0){
      showActiveButton(false);
      activebutton--;
      showActiveButton(true);
    }
  } else if (e->key() == Qt::Key_Right){
    if (activebutton < numbuttons-1){
      showActiveButton(false);
      activebutton++;
      showActiveButton(true);
    }
  } else if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down){
    if (activebutton>=0 && activebutton < numbuttons){
      buttons[activebutton]->toggle();
    }
  } else if (e->key() == Qt::Key_End){
    releasefocus();
  } else {
    releasefocus();
    e->ignore();
  }
}

void StatusPlotButtons::focusInEvent ( QFocusEvent * )
{
}

void StatusPlotButtons::focusOutEvent ( QFocusEvent * )
{
}

void StatusPlotButtons::reset()
{
  oktoemit= true;
  for (int i=0; i<numbuttons; i++){
    if (!buttons[i]->isChecked()){
      buttons[i]->toggle();
      buttons[i]->setDown(true);
    }
  }
  oktoemit= false;
}


void StatusPlotButtons::setPlotElements(const vector<PlotElement>& vpe)
{
  oktoemit= false;

  int n= vpe.size();
  if (n>MAXBUTTONS) n= MAXBUTTONS;

  // first clean up
  for (int i=n; i<MAXBUTTONS; i++){
    buttons[i]->hide();
  }

  // add buttons
  for (int i=0; i<n; i++){
    buttons[i]->setPlotElement(vpe[i]);
    buttons[i]->show();
  }
  numbuttons= n;
  oktoemit= true;

}

void StatusPlotButtons::enabled(PlotElement pe)
{
  emit toggleElement(pe);
}
