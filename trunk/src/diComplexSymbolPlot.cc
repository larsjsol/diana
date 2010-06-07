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

#include <diComplexSymbolPlot.h>
#include <diFontManager.h>
#include <diWeatherSymbol.h>

using namespace::miutil;

//Define symbols from font
#define SIG1SYMBOL 248
#define SIG2SYMBOL 249
#define SIG3SYMBOL 250
#define SIG4SYMBOL 251
#define RIGHTARROW 247
#define LEFTARROW  246
#define HIGHSYMBOL 243
#define LOWSYMBOL  242
#define THUNDERSYMBOL  119
#define CROSS  252
#define NEW_CROSS  255
#define MOUNTAINWAVESYMBOL 130
#define VULCANOSYMBOL 226
#define FOGSYMBOL 62
#define SNOWSYMBOL 254
#define WIDESPREADBRSYMBOL 136
#define MOUNTAINOBSCURATIONSYMBOL 106
#define HAILSYMBOL 238
#define SNOWSHOWERSYMBOL 114
#define SHOWERSYMBOL 109
#define FZRASYMBOL 93

//#define DEBUGPRINT
//static variables
// text used in new complex symbols
vector <miString> ComplexSymbolPlot::currentSymbolStrings; //symbolstrings
vector <miString> ComplexSymbolPlot::currentXStrings; //xtext
set <miString> ComplexSymbolPlot::clist; //texts used in combobox
float ComplexSymbolPlot::textShrink=1.5;

ComplexSymbolPlot::ComplexSymbolPlot() : Plot(){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::ComplexSymbolPlot()"<< endl;
#endif
  xvisible=false;
  nstringsvisible=0;
  symbolStrings=currentSymbolStrings;
  xstrings=currentXStrings;
  whiteBox=true;
}


ComplexSymbolPlot::ComplexSymbolPlot(int drawIndex) : Plot(){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::ComplexSymbolPlot(int drawIndex)"<< endl;
#endif
  xvisible=false;
  nstringsvisible=0;
  symbolStrings=currentSymbolStrings;
  xstrings=currentXStrings;
  //rightarrow and thunder - no white box
//  if (drawIndex==1018 || drawIndex==1021)
//    whiteBox=false;
//  else
//    whiteBox=true;
  whiteBox = isComplexText(drawIndex);
}


void ComplexSymbolPlot::initStrings(int drawIndex){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::initStrings()"<< endl;
#endif
  switch (drawIndex){
  case 1000:
    if (symbolStrings.size()==0)
      symbolStrings.push_back("");
    break;
  case 1007:
    if (xstrings.size()==0)
      xstrings.push_back("");
    if (xstrings.size()==1)
      xstrings.push_back(xstrings[0]);
    break;
  case 900:
    if (xstrings.size()==0)
      xstrings.push_back("");
    break;
  case 2000:
    if (symbolStrings.size()==0)
      symbolStrings.push_back("");
    if (symbolStrings.size()==1)
      symbolStrings.push_back(symbolStrings[0]);
    break;
  case 3000:
    if (symbolStrings.size()==0)
      symbolStrings.push_back("");
    break;
  case 3001:
    if (symbolStrings.size()==0)
      symbolStrings.push_back("");
    break;
  case 1033:
    if (symbolStrings.size()==0)
      currentSymbolStrings.push_back("");
    break;
  }
}


void ComplexSymbolPlot::initCurrentStrings(int drawIndex){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::initCurrentStrings()"<< endl;
#endif
  currentSymbolStrings.clear();
  currentXStrings.clear();
  switch (drawIndex){
  case 1000:
    currentSymbolStrings.push_back("");
    break;
  case 1007:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    break;
  case 1008:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    break;
  case 1009:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    break;
  case 1010:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    break;
  case 1011:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    break;
  case 1012:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    break;
  case 1013:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    break;
  case 1014:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    currentSymbolStrings.push_back("");
    break;
  case 1015:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    currentSymbolStrings.push_back("");
    break;
  case 1016:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    currentSymbolStrings.push_back("");
    currentSymbolStrings.push_back("");
    break;
  case 1018:
    //currentSymbolStrings.push_back("");
    break;
  case 1019:
    //currentSymbolStrings.push_back("");
    break;
  case 1020:
    //currentSymbolStrings.push_back("9xx");
    break;
  case 1021:
    //currentSymbolStrings.push_back("1xxx");
    break;
  case 1022:
    //currentSymbolStrings.push_back("1xxx");
    break;
  case 1023:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    currentSymbolStrings.push_back("BKN/OVC STF");
    break;
  case 1024:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    currentSymbolStrings.push_back("BKN/OVC STF");
    break;
  case 1027:
    currentSymbolStrings.push_back("");
    break;
  case 1028:
    currentSymbolStrings.push_back("");
    break;
  case 1029:
    currentSymbolStrings.push_back("");
    break;
  case 1033:
    currentSymbolStrings.push_back("");
    break;
  case 1034:
    currentSymbolStrings.push_back("");
    currentSymbolStrings.push_back("");
    break;
  case 1036:
    currentSymbolStrings.push_back("0�:x");
    currentSymbolStrings.push_back("");
    break;
 case 1040:
    currentXStrings.push_back("");
    currentXStrings.push_back("");
    currentSymbolStrings.push_back("");
    currentSymbolStrings.push_back("");
    break;
  case 900:
    currentXStrings.push_back("");
    currentSymbolStrings.push_back("");
    break;
  case 2000:
    currentSymbolStrings.push_back("");
    currentSymbolStrings.push_back("");
    break;
  case 3000:
    currentSymbolStrings.push_back("");
    break;
  case 3001:
    currentSymbolStrings.push_back("");
    break;
  }
}


void ComplexSymbolPlot::draw(int drawIndex, float x,float y,int size,float rot){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::draw()"<< endl;
  cerr << "drawIndex = " << drawIndex << endl;
  cerr << " float x = " << x << endl;
  cerr << " float y = " << y << endl;
#endif
  symbolSizeToPlot=size;
  xvisible=false;
  nstringsvisible=0;
  glPushMatrix();
  glTranslatef(x, y, 0.0);
  glRotatef(rot,0.0,0.0,1.0);
  //scale linewidth to symbolsize
  float linewidth=symbolSizeToPlot/50+1;
  glLineWidth(linewidth);
  switch (drawIndex){
  case 1000:
    symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
    drawSigText(0,0,whiteBox);
    break;
  case 1001:
    drawSig1(0,0,SIG1SYMBOL);
    break;
  case 1002:
    drawSig1(0,0,SIG2SYMBOL);
    break;
  case 1003:
    drawSig1(0,0,SIG3SYMBOL);
    break;
  case 1004:
    drawSig1(0,0,SIG4SYMBOL);
    break;
  case 1005:
    drawSig5(0,0);
    break;
  case 1006:
    drawSig6(0,0);
    break;
  case 1007:
    symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
    drawSig7(0,0);
    break;
  case 1008:
    drawSig8(0,0);
    break;
  case 1009:
    drawSig9(0,0);
    break;
  case 1010:
    drawSig10(0,0);
    break;
  case 1011:
    drawSig11(0,0);
    break;
  case 1012:
    drawSig12(0,0);
    break;
  case 1013:
    drawSig13(0,0);
    break;
  case 1014:
    drawSig14(0,0);
    break;
  case 1015:
    drawSig15(0,0);
    break;
  case 1016:
    drawSig16(0,0);
    break;
  case 1018:
    symbolSizeToPlot=int (symbolSizeToPlot);
    drawSig1(0,0,RIGHTARROW);
    break;
  case 1019:
    symbolSizeToPlot=int (symbolSizeToPlot);
    drawSig1(0,0,LOWSYMBOL);
    break;
  case 1020:
    symbolSizeToPlot=int (symbolSizeToPlot);
    drawSig1(0,0,HIGHSYMBOL);
    break;
  case 1021:
    drawSig1(0,0,THUNDERSYMBOL);
    break;
  case 1022:
    drawSig1(0,0,CROSS);
    break;
  case 1023:
    drawSig14(0,0);
    break;
  case 1024:
    drawSig15(0,0);
    break;
  case 1025:
    drawSig1(0,0,MOUNTAINWAVESYMBOL);
    break;
  case 1026:
    drawSig1(0,0,VULCANOSYMBOL);
    break;
  case 1027:
    drawSig27(0,0);
    break;
  case 1028:
    drawSig28(0,0);
    break;
  case 1029:
    drawSig29(0,0);
    break;
  case 1030:
    drawSig30(0,0);
    break;
  case 1031:
    drawSig31(0,0);
    break;
  case 1032:
    drawSig32(0,0);
    break;
  case 1033:
    drawSig33(0,0);
    break;
  case 1034:
    drawSig34(0,0);
    break;
  case 1035:
    drawSig1(0,0,NEW_CROSS);
    break;
  case 1036:
    drawSig36(0,0);
    break;
  case 1037:
    drawSig1(0,0,WIDESPREADBRSYMBOL);
    break;
  case 1038:
    drawSig1(0,0,MOUNTAINOBSCURATIONSYMBOL);
    break;
  case 1039:
    drawSig1(0,0,HAILSYMBOL);
    break;
  case 1040:
    drawSig40(0,0);
    break;
  case 1041:
    drawSig1(0,0,FOGSYMBOL);
    break;
  case 1042:
    drawSig1(0,0,SNOWSYMBOL);
    break;
  case 1043:
    symbolSizeToPlot=int (symbolSizeToPlot);
    drawSig1(0,0,SNOWSHOWERSYMBOL);
    break;
  case 1044:
    symbolSizeToPlot=int (symbolSizeToPlot);
    drawSig1(0,0,SHOWERSYMBOL);
    break;
  case 1045:
    drawSig1(0,0,FZRASYMBOL);
    break;
  case 900:
    symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
    drawColoredSigText(0,0);
    break;
  case 2000:
    symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
    drawDoubleSigText(0,0);
    break;
  case 3000:
    symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
    drawSigEditText(0,0,whiteBox);
    break;
  case 3001:
    symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
    drawSigTextBox();
    break;
  default:
    cerr << "ComplexSymbolPlot::drawComplexSymbols - Index " <<
      drawIndex << " not defined " << endl;
    return;
  }
  glPopMatrix();
}



void ComplexSymbolPlot::drawSymbol(int index,float x,float y){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawSymbol()"<< endl;
#endif
  float cw,ch;
  fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
  fp->getCharSize(index,cw,ch);
  fp->drawChar(index,x-cw/2,y-ch/2,0.0);
}


void ComplexSymbolPlot::drawSigString(float x,float y, bool whitebox){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawSigString()"<< endl;
#endif
  if (whitebox){
    drawBox(1999,x,y);
  }
  float cw,ch;
  getComplexSize(1999,cw,ch);
  fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
  fp->drawStr(sigString.c_str(),x-0.45*cw,y-0.4*ch,0.0);
}

void ComplexSymbolPlot::drawSigEditString(float& x,float& y, bool whitebox){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawSigEditString()"<< endl;
  cerr << "x = " <<x << "         y = "<< y << endl;
#endif
  float cw,ch;
  getComplexSize(1999,cw,ch);
  fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
  fp->drawStr(sigString.c_str(),x+0.45 ,y-0.4,0.0);
#ifdef DEBUGPRINT
  cerr << "** sigString  = **"<<sigString.c_str() << endl;
#endif
}

void ComplexSymbolPlot::drawSigText(float x,float y, bool whitebox){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawSigText()"<< endl;
#endif
  initStrings(1000);
  if (symbolStrings.size()>0)
    sigString=symbolStrings[0];
  drawSigString(x,y,whitebox);
  nstringsvisible=1;
}
// Drawing texbox on right low corner of the frame
void ComplexSymbolPlot::drawSigTextBox(){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawSigTextBox()"<< endl;
 // cerr << "x = " <<x << "         y = "<< y << endl;
#endif
#ifdef DEBUGPRINT
  cerr << "** drawing filled area**"<< endl;
#endif
   float x1, x2, y1, y2;
   x1 = 111.651;
   x2 = 131.999;
   y1 = 66.0913;
   y2 = 72.0713;
//----------------------------------
  // draw filled area
  Colour fc = poptions.fillcolour;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(0,0,0,0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x1, y1);
    glVertex2f(x1, y2);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glEnd();
    glDisable(GL_BLEND); // end of drawing filled area

    glIndexi(poptions.bordercolour.Index());
    glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x1, y2);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glEnd();
//----------------------------
/*  float x, y;
  x = 0.0; y = 0.0;
  bool whitebox = true;
  initStrings(3000);
  if (symbolStrings.size()>0)
    for (int j= 0 ; j < symbolStrings.size(); j++)
    {
       sigString=symbolStrings[j];
#ifdef DEBUGPRINT
  cerr << "sigString = " << sigString.c_str() << endl;
#endif
       drawSigEditString(x,y,whitebox);
       y -= 0.8;
    }*/
  nstringsvisible=1;
}

void ComplexSymbolPlot::drawColoredSigText(float x,float y, bool whitebox){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawColoredSigTextt()"<< endl;
  cerr << "x = " <<x << "         y = "<< y << endl;
#endif
  //GLfloat currentColor[4];
  //glGetFloatv(GL_CURRENT_COLOR,currentColor);
  //glColor4ub(objectColour.R(),objectColour.G(),objectColour.B(),objectColour.A());

  float sw,sh;
 // symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  initStrings(1000);
  if (symbolStrings.size()>0)
    sigString=symbolStrings[0];
  getComplexSize(1999,sw,sh);
  drawSigText(x-sw/2,y,whiteBox);
  symbolSizeToPlot= int(symbolSizeToPlot/textShrink);
  initStrings(900);
  getComplexSize(900,sw,sh);
  drawSigNumber(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
  nstringsvisible=1;

}


// Plotting multiline text
void ComplexSymbolPlot::drawSigEditText(float x,float y, bool whitebox){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawSigEditText()"<< endl;
  cerr << "x = " <<x << "         y = "<< y << endl;
#endif
/*#ifdef DEBUGPRINT
  cerr << "** drawing filled area**"<< endl;
#endif
   float x1, x2, y1, y2;
   x1 = 111.651;
   x2 = 131.999;
   y1 = 66.0913;
   y2 = 72.0713;
//----------------------------------
  // draw filled area
  Colour fc = poptions.fillcolour;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(0,0,0,0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x1, y1);
    glVertex2f(x1, y2);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glEnd();
    glDisable(GL_BLEND); // end of drawing filled area

    glIndexi(poptions.bordercolour.Index());
    glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x1, y2);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glEnd();*/
//----------------------------
  float cw,ch;
  initStrings(3000);
  if (symbolStrings.size()>0)
    for (int j= 0 ; j < symbolStrings.size(); j++)
    {
       sigString=symbolStrings[j];
#ifdef DEBUGPRINT
  cerr << "sigString = " << sigString.c_str() << endl;
#endif
       getComplexSize(1999,cw,ch);
       drawSigEditString(x,y,whitebox);
       y -= 0.9*ch;
    }
  nstringsvisible=1;
}

void ComplexSymbolPlot::drawDoubleSigText(float x,float y, bool whitebox){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawDoubleSigText()"<< endl;
#endif
  float cw1,ch1;
  float cw2,ch2;
  initStrings(2000);
  if (symbolStrings.size()>0)
    sigString=symbolStrings[0];
  getComplexSize(1999,cw1,ch1);
  drawSigString(x,y+ch1/2,whitebox);
  if (symbolStrings.size()>1)
    sigString=symbolStrings[1];
  getComplexSize(1999,cw2,ch2);
  drawSigString(x,y-ch2/2,whitebox);
  nstringsvisible=2;
}

void ComplexSymbolPlot::drawDoubleSigTextAndSymbol(int symbol, float x,float y){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::idrawDoubleSigTextAndSymbol()"<< endl;
#endif
  float cw1,ch1;
  float cw2,ch2;
  float sw,sh;
  initStrings(2000);
  if (symbolStrings.size()>0)
    sigString=symbolStrings[0];
  getComplexSize(symbol,sw,sh);
  getComplexSize(1999,cw1,ch1);
  drawSymbol(symbol,x-cw1/2,y+ch1/2);
  drawSigString(x+sw/2,y+ch1/2);
  if (symbolStrings.size()>1)
    sigString=symbolStrings[1];
  getComplexSize(1999,cw2,ch2);
  drawSigString(x,y-ch2/2);
  nstringsvisible=2;
}

void ComplexSymbolPlot::drawSigNumber(float x,float y){
  if (xstrings.size()<1)
    initStrings(900);
  if(whiteBox){
    drawBox(900,x,y);
  }
  fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
  float cw1,ch1;
  //float cw2,ch2;
  fp->getStringSize(xstrings[0].c_str(),cw1,ch1);
  //fp->getStringSize(xstrings[1].c_str(),cw2,ch2);
  fp->drawStr(xstrings[0].c_str(),x-cw1/2,y-1.1*ch1,0.0);
  //fp->drawStr(xstrings[1].c_str(),x-cw2/2,y-1.1*ch2,0.0);
  float sw,sh;
  getComplexSize(900,sw,sh);
  //xvisible=true;
}



void ComplexSymbolPlot::drawSig1(float x,float y, int metSymbol){
  if (whiteBox) {
    drawBox(metSymbol,0,0);
  }
  drawSymbol(metSymbol,x,y);
}

void ComplexSymbolPlot::drawSig5(float x,float y){
 if (whiteBox) {
   drawBox(1005,x,y);
 }
  float cw,ch;
  getComplexSize(1001,cw,ch);
  drawSymbol(SIG1SYMBOL,x-0.4*cw,y+0.3*ch);
  getComplexSize(1002,cw,ch);
  drawSymbol(SIG2SYMBOL,x+0.4*cw,y-0.5*ch);

  getComplexSize(1005,cw,ch);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x-0.3*cw,y-0.3*ch);
  glVertex2f(x+0.3*cw,y+0.3*ch);
  glEnd();
}


void ComplexSymbolPlot::drawSig6(float x,float y){
  if (whiteBox) {
    drawBox(1006,x,y);
  }
  float cw,ch;
  getComplexSize(1003,cw,ch);
  drawSymbol(SIG3SYMBOL,x-0.3*cw,y+1.05*ch);
  getComplexSize(1004,cw,ch);
  drawSymbol(SIG4SYMBOL,x+0.45,y-0.5*ch);
  getComplexSize(1006,cw,ch);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x-0.35*cw,y-0.35*ch);
  glVertex2f(x+0.35*cw,y+0.35*ch);
  glEnd();
}


void ComplexSymbolPlot::drawSig7(float x,float y){
  if (xstrings.size()<2)
    initStrings(1007);
  if(whiteBox){
    drawBox(1007,x,y);
  }
  fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
  float cw1,ch1;
  float cw2,ch2;
  fp->getStringSize(xstrings[0].c_str(),cw1,ch1);
  fp->getStringSize(xstrings[1].c_str(),cw2,ch2);
  fp->drawStr(xstrings[0].c_str(),x-cw1/2,y+0.1*ch1,0.0);
  fp->drawStr(xstrings[1].c_str(),x-cw2/2,y-1.1*ch2,0.0);
  float sw,sh;
  getComplexSize(1007,sw,sh);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x-sw/2,y);
  glVertex2f(x+sw/2,y);
  glEnd();
  xvisible=true;
}


void ComplexSymbolPlot::drawSig8(float x,float y){
  float sw,sh;
  getComplexSize(1001,sw,sh);
  drawSig1(x-sw/2,y,SIG1SYMBOL);
  initStrings(1007);
  symbolSizeToPlot= int(symbolSizeToPlot/textShrink);
  getComplexSize(1007,sw,sh);
  drawSig7(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}


void ComplexSymbolPlot::drawSig9(float x,float y){
  float sw,sh;
  getComplexSize(1002,sw,sh);
  drawSig1(x-sw/2,y,SIG2SYMBOL);
  initStrings(1007);
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  getComplexSize(1007,sw,sh);
  drawSig7(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}


void ComplexSymbolPlot::drawSig10(float x,float y){
  float sw,sh;
  getComplexSize(1005,sw,sh);
  drawSig5(x-sw/2,y);
  initStrings(1007);
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  getComplexSize(1007,sw,sh);
  drawSig7(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}

void ComplexSymbolPlot::drawSig11(float x,float y){
  float sw,sh;
  getComplexSize(1003,sw,sh);
  drawSig1(x-sw/2,y,SIG3SYMBOL);
  initStrings(1007);
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  getComplexSize(1007,sw,sh);
  drawSig7(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}

void ComplexSymbolPlot::drawSig12(float x,float y){
  float sw,sh;
  getComplexSize(1004,sw,sh);
  drawSig1(x-sw/2,y,SIG4SYMBOL);
  initStrings(1007);
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  getComplexSize(1007,sw,sh);
  drawSig7(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}
void ComplexSymbolPlot::drawSig13(float x,float y){
  float sw,sh;
  getComplexSize(1006,sw,sh);
  drawSig6(x-sw/2,y);
  initStrings(1007);
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  getComplexSize(1007,sw,sh);
  drawSig7(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}

void ComplexSymbolPlot::drawSig14(float x,float y){
  float sw,sh;
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  initStrings(1000);
  getComplexSize(1000,sw,sh);
  drawSigText(x-sw/2,y,whiteBox);
  initStrings(1007);
  getComplexSize(1007,sw,sh);
  drawSig7(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}


void ComplexSymbolPlot::drawSig15(float x,float y){
  float sw,sh;
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  initStrings(1000);
  getComplexSize(1000,sw,sh);
  drawSigText(x,y+sh/2,whiteBox);
  initStrings(1007);
  getComplexSize(1007,sw,sh);
  drawSig7(x,y-sh/2);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}

void ComplexSymbolPlot::drawSig40(float x,float y){
  float sw,sh;
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  initStrings(2000);
  getComplexSize(2000,sw,sh);
  drawDoubleSigText(x,y+sh/1.75,whiteBox);
  initStrings(1007);
  getComplexSize(1007,sw,sh);
  drawSig7(x,y-sh/1.75);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}

void ComplexSymbolPlot::drawSig16(float x,float y){
  //draw two texts
  float sw,sh;
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  initStrings(2000);
  getComplexSize(2000,sw,sh);
  drawDoubleSigText(x-sw/2,y,whiteBox);
  initStrings(1007);
  getComplexSize(1007,sw,sh);
  drawSig7(x+sw/2,y);
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}

void ComplexSymbolPlot::drawSig17(float x,float y){
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  if (symbolStrings.size()>0)
    sigString=symbolStrings[0];
  if(whiteBox) {
    drawBox(1000,x,y);
  }
  drawSigString(x,y);
  nstringsvisible=1;
  float sw,sh;
  getComplexSize(1000,sw,sh);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x-0.5*sw,y-0.5*sh);
  glVertex2f(x-0.5*sw,y+0.5*sh);
  glVertex2f(x+0.5*sw,y+0.5*sh);
  glVertex2f(x+0.5*sw,y-0.5*sh);
  glEnd();
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}



void ComplexSymbolPlot::drawSig22(float x,float y){
  drawBox(1022,x,y);
  drawSymbol(CROSS,x,y);
}

//Sea temp, blue circle
void ComplexSymbolPlot::drawSig27(float x,float y){
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  if (symbolStrings.size()>0)
    sigString=symbolStrings[0];
  drawCircle(1000,x,y,true);
  drawSigString(x,y,false);
  nstringsvisible=1;
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}

//Mean SFC wind, red diamond
void ComplexSymbolPlot::drawSig28(float x,float y){
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  if (symbolStrings.size()>0)
    sigString=symbolStrings[0];
  drawDiamond(1000,x,y);
  drawSigString(x,y,false);
  nstringsvisible=1;
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);
}

// Sea state, black flag
void ComplexSymbolPlot::drawSig29(float x,float y){
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink/2);
  if (symbolStrings.size()>0)
    sigString=symbolStrings[0];
  drawFlag(1000,x,y,true); //fill;
  drawFlag(1000,x,y,false); //border
  drawSigString(x,y,false);
  nstringsvisible=1;
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink*2);
}

// Freezing fog
void ComplexSymbolPlot::drawSig30(float x,float y){
  if(whiteBox) {
    drawBox(1030,x,y);
  }
  drawSymbol(FOGSYMBOL,x,y);
  GLfloat currentColor[4];
  glGetFloatv(GL_CURRENT_COLOR,currentColor);
  glColor4ub(borderColour.R(),borderColour.G(),borderColour.B(),borderColour.A());
  float cw,ch;
  fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot/2);
  fp->getStringSize("V",cw,ch);
  fp->drawStr("V",x-cw/2,y-ch/2,0.0);
  //  drawSymbol(135,x,y);
  glColor4fv(currentColor);

}

//Nuclear
void ComplexSymbolPlot::drawSig31(float x,float y){

  drawNuclear(x,y);

}

//precipitation, green lines
void ComplexSymbolPlot::drawSig32(float x,float y){

  drawPrecipitation(x,y);

}

//Visibility, black rectangular box
void ComplexSymbolPlot::drawSig33(float x,float y){

  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  sigString=symbolStrings[0];
  //  if (whiteBox) drawBox(1000,x,y,true);
  drawBox(1000,x,y,false);

  poptions.fontface="italic";
  drawSigString(x,y);
  nstringsvisible=1;


}

//Vulcano box,
void ComplexSymbolPlot::drawSig34(float x,float y){
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  if (whiteBox) drawBox(1034,x,y,true);
  drawBox(1034,x,y,false);
  drawDoubleSigTextAndSymbol(VULCANOSYMBOL,x,y);

}

//Freezing level (new)
void ComplexSymbolPlot::drawSig36(float x,float y){
  symbolSizeToPlot=int(symbolSizeToPlot/textShrink);
  drawCircle(1036,x,y,false);
  if (symbolStrings.size()==1  ||
      (symbolStrings.size()==2 && symbolStrings[1]=="")){
    sigString=symbolStrings[0];
    drawSigString(x,y,false);
    nstringsvisible=1;
  } else if (symbolStrings.size()==2){
    drawDoubleSigText(x,y,false);
  }
  symbolSizeToPlot=int(symbolSizeToPlot*textShrink);

}


void ComplexSymbolPlot::drawBox(int index,float x, float y,bool fill){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::drawBox"<< endl;
  cerr << "*float x = "<< x << endl;
  cerr << "*float y = "<< y << endl;
  cerr << "*float fill = "<< fill << endl;
#endif

  GLfloat currentColor[4];
  glGetFloatv(GL_CURRENT_COLOR,currentColor);

  float sw,sh;
  getComplexSize(index,sw,sh);
#ifdef DEBUGPRINT
  cerr << "float sw = "<< sw << endl;
  cerr << "float sh = "<< sh << endl;
#endif

  if( fill ){
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(1.0,1.0,1.0,1.0);
    glBegin(GL_POLYGON);
  } else {
    glColor4ub(borderColour.R(),borderColour.G(),borderColour.B(),borderColour.A());
    glBegin(GL_LINE_LOOP);
  }

    glVertex2f(x-0.5*sw,y-0.5*sh);
    glVertex2f(x-0.5*sw,y+0.5*sh);
    glVertex2f(x+0.6*sw,y+0.5*sh);
    glVertex2f(x+0.6*sw,y-0.5*sh);
  glEnd();

  glColor4fv(currentColor);

}

void ComplexSymbolPlot::drawFlag(int index,float x, float y, bool fill){
  float PI=3.14;

  GLfloat currentColor[4];
  glGetFloatv(GL_CURRENT_COLOR,currentColor);

  float sw,sh;
  miString s = "10";
  fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
  fp->getStringSize(s.c_str(),sw,sh);
  sw=1.1*sw; sh=1.2*sh;
  sh = sh*1.5;
  sw = sw*1.5;

  GLfloat radius = sw/2;

  GLfloat y2 = y+0.5*sh;
  GLfloat y3 = y-0.5*sh;
  GLfloat x1;
  GLfloat y1;
  GLfloat offset=sh/3;

  glLineWidth(1);
  if(fill) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glColor4f(1.0,1.0,1.0,1.0);
  } else {
    glBegin(GL_LINE_LOOP);
    glColor4ub(borderColour.R(),borderColour.G(),borderColour.B(),borderColour.A());
  }

  glVertex2f(x-sw,y-0.5*sh+offset);
  glVertex2f(x-sw,y+0.5*sh+offset);
  for(int i=0;i<11;++i){
    x1 = x -radius*cos(i*PI/10.0)-sw/2;
    y1 = y2 - 0.5*radius*sin(i*PI/10.0);
    glVertex2f(x1,y1+offset);
    offset-=sh/30;
  }
  offset+=sh/30;

  for(int i=0;i<10;++i){
      x1 = x + sw/2 - radius*cos(i*PI/10.0);
      y1 = y2+0.5*radius*sin(i*PI/10.0);
      glVertex2f(x1,y1+offset);
      offset-=sh/30;
  }

  glVertex2f(x+sw,y+0.5*sh+offset);
  glVertex2f(x+sw,y-0.5*sh+offset);
  //  offset+=sh/50;

  for(int i=0;i<11;++i){
      x1 = x + sw/2 +radius*cos(i*PI/10.0);
      y1 = y3+0.5*radius*sin(i*PI/10.0);
      glVertex2f(x1,y1+offset);
      offset+=sh/30;
  }
  offset-=sh/30;

  for(int i=0;i<10;++i){
    x1 = x +radius*cos(i*PI/10.0)-sw/2;
    y1 = y3 - 0.5*radius*sin(i*PI/10.0);
    glVertex2f(x1,y1+offset);


 offset+=sh/30;
  }

  glEnd();

  glColor4fv(currentColor);
}

void ComplexSymbolPlot::drawCircle(int index,
				   float x,
				   float y,
				   bool circle){

  GLfloat currentColor[4];
  glGetFloatv(GL_CURRENT_COLOR,currentColor);

  float sw,sh;
  if(circle) {
    miString s = "10";
    fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
    fp->getStringSize(s.c_str(),sw,sh);
    sw=1.1*sw; sh=1.2*sh;
  } else {
    getComplexSize(index,sw,sh);
  }

  //  if(symbolStrings.size()==2&& symbolStrings[1]!="") sh*=2;
  GLfloat xc,yc;
  GLfloat radius;
  if( circle) {
    radius = sw/1.5;
    sw = 0.0;
  } else {
    radius = sh/1.5;
    sw = sw/1.8;
  }
  float PI=3.14;

  //draw white background
  if (whiteBox) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(1.0,1.0,1.0,1.0);
  glBegin(GL_POLYGON);
    for(int i=0;i<50;i++){
      xc = sw/2+radius*sin(i*2*PI/100.0);
      yc = radius*cos(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }
    for(int i=50;i<100;i++){
      xc = radius*sin(i*2*PI/100.0)-sw/2;
      yc = radius*cos(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }
    glEnd();
  }

  //draw circle
  glLineWidth(2);
  glBegin(GL_LINE_LOOP);
  glColor4ub(borderColour.R(),borderColour.G(),borderColour.B(),borderColour.A());
  glBegin(GL_POLYGON);
    for(int i=0;i<50;i++){
      yc = sw/2+radius*sin(i*2*PI/100.0);
      xc = radius*cos(i*2*PI/100.0);
      glVertex2f(yc,xc);
    }
    for(int i=50;i<100;i++){
      yc = radius*sin(i*2*PI/100.0)-sw/2;
      xc = radius*cos(i*2*PI/100.0);
      glVertex2f(yc,xc);
    }
  glEnd();

  //reset colour
  glColor4fv(currentColor);
}

void ComplexSymbolPlot::drawDiamond(int index,float x, float y){

  //save curent colour
  GLfloat currentColor[4];
  glGetFloatv(GL_CURRENT_COLOR,currentColor);

  float sw,sh;
  miString s = "10";
  fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
  fp->getStringSize(s.c_str(),sw,sh);
  sw=1.1*sw; sh=1.2*sh;

  //draw white background
  if (whiteBox) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(1.0,1.0,1.0,1.0);
    glBegin(GL_POLYGON);
      glVertex2f(0,y-sh);
      glVertex2f(x-sw,0);
      glVertex2f(0,y+sh);
      glVertex2f(x+sw,0);
    glEnd();
   }

  //draw diamond
  glLineWidth(2);
  glBegin(GL_LINE_LOOP);
  glColor4ub(borderColour.R(),borderColour.G(),borderColour.B(),borderColour.A());
  glBegin(GL_POLYGON);
    glVertex2f(0,y-sh);
    glVertex2f(x-sw,0);
    glVertex2f(0,y+sh);
    glVertex2f(x+sw,0);
  glEnd();

  //reset colour
  glColor4fv(currentColor);
}

void ComplexSymbolPlot::drawNuclear(float x, float y){

  GLfloat currentColor[4];
  glGetFloatv(GL_CURRENT_COLOR,currentColor);

  float PI=3.14;
  float sw,sh;
  getComplexSize(1031,sw,sh);
  GLfloat xc,yc;
  GLfloat radius = sh/1.5;

  //  cerr <<"x:"<<x<<"  y:"<<y<<endl;
  glColor4ub(borderColour.R(),borderColour.G(),borderColour.B(),borderColour.A());
  glBegin(GL_LINE_LOOP);
    for(int i=0;i<100;i++){
      xc = radius*cos(i*2*PI/100.0);
      yc = radius*sin(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }
    glEnd();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_POLYGON);
    glVertex2f(x,y);
    for(int i=0;i<100/6;i++){
      xc = radius*cos(i*2*PI/100.0);
      yc = radius*sin(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }
    glVertex2f(x,y);
    for(int i=200/6;i<300/6;i++){
      xc = radius*cos(i*2*PI/100.0);
      yc = radius*sin(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }
    glVertex2f(x,y);
    for(int i=400/6;i<500/6;i++){
      xc = radius*cos(i*2*PI/100.0);
      yc = radius*sin(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }
    glVertex2f(x,y);
    glEnd();

    glColor4f(1.0,1.0,1.0,1.0);
  glBegin(GL_POLYGON);
    glVertex2f(x,y);
    for(int i=100/6;i<200/6;i++){
      xc = radius*cos(i*2*PI/100.0);
      yc = radius*sin(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }
    glVertex2f(x,y);
    for(int i=300/6;i<400/6;i++){
      xc = radius*cos(i*2*PI/100.0);
      yc = radius*sin(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }
    glVertex2f(x,y);
    for(int i=500/6;i<600/6;i++){
      xc = radius*cos(i*2*PI/100.0);
      yc = radius*sin(i*2*PI/100.0);
      glVertex2f(xc,yc);
    }

    glEnd();


  glColor4fv(currentColor);
}

void ComplexSymbolPlot::drawPrecipitation(float x, float y){

  GLfloat currentColor[4];
  glGetFloatv(GL_CURRENT_COLOR,currentColor);

  float sw,sh;
  getComplexSize(1026,sw,sh);

  glRotatef(-45,0.0,0.0,1.0);

  if (whiteBox) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(1.0,1.0,1.0,1.0);
    glBegin(GL_POLYGON);
      glVertex2f(-sw,-sh);
      glVertex2f(-sw,sh);
      glVertex2f(sw,sh);
      glVertex2f(sw,-sh);
    glEnd();
  }

  glColor4fv(currentColor);
  glLineWidth(3);
  glBegin(GL_LINE_STRIP);
    glVertex2f(sw/4,sh/2-sh/4);
    glVertex2f(sw/4,-sh/2-sh/4);
  glEnd();
  glBegin(GL_LINE_STRIP);
    glVertex2f(sw/12,sh/2);
    glVertex2f(sw/12,-sh/2);
  glEnd();
  glBegin(GL_LINE_STRIP);
      glVertex2f(-sw/12,sh/2-sh/4);
      glVertex2f(-sw/12,-sh/2-sh/4);
    glEnd();
  glBegin(GL_LINE_STRIP);
      glVertex2f(-sw/4,sh/2);
      glVertex2f(-sw/4,-sh/2);
    glEnd();

  glRotatef(45,0.0,0.0,1.0);

  glColor4fv(currentColor);
}

void ComplexSymbolPlot::getComplexSize(int index, float& sw, float & sh){
  float cw,ch;
  float cw1,ch1;
  float cw2,ch2;
  diffx=0;
  diffy=0;
  if (index < 1000){
    fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
    fp->getCharSize(index,cw,ch);
    sw=ch; sh=ch;
  }
  else{
    switch (index){
    case 1999:
      //sigstring
      fp->set("BITMAPFONT",poptions.fontface,symbolSizeToPlot);
      fp->getStringSize(sigString.c_str(),cw,ch);
      sw=1.1*cw; sh=1.2*ch;
      break;
    case 3000:
      //sigeditstring
      fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
      fp->getStringSize(sigString.c_str(),cw,ch);
      sw=1.1*cw; sh=1.2*ch;
      break;
    case 900:
      fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
      if (xstrings.size()>0)
	fp->getStringSize(xstrings[0].c_str(),cw1,ch1);
      //if (xstrings.size()>1)
	//fp->getStringSize(xstrings[1].c_str(),cw2,ch2);
      //if (cw1>cw2)
	sw=cw1;
      //else
	//sw=cw2;
      //if (ch1>ch2)
	sh=2.0*ch1;
      //else
	//sh=2.0*ch2;
      break;
    case 1000:
      if (symbolStrings.size()>0){
	fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
	fp->getStringSize(symbolStrings[0].c_str(),cw,ch);
	sw=1.1*cw; sh=1.2*ch;
      }
      break;
    case 1001:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(SIG2SYMBOL,cw,ch);
      sw=1.4*cw; sh=1.8*ch;
      break;
    case 1002:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(SIG2SYMBOL,cw,ch);
      sw=1.4*cw; sh=1.8*ch;
      break;
    case 1003:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(SIG3SYMBOL,cw,ch);
      sw=1.2*cw; sh=ch;
      break;
    case 1004:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(SIG3SYMBOL,cw,ch);
      sw=1.2*cw; sh=1.4*ch;
      break;
    case 1005:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      getComplexSize(1001,cw,ch);
      sw=1.8*cw; sh=1.5*ch;
      break;
    case 1006:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      getComplexSize(1003,cw,ch);
      sw=1.8*cw; sh=2.8*ch;
      break;
    case 1007:
      fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
      if (xstrings.size()>0)
	fp->getStringSize(xstrings[0].c_str(),cw1,ch1);
      if (xstrings.size()>1)
	fp->getStringSize(xstrings[1].c_str(),cw2,ch2);
      if (cw1>cw2)
	sw=cw1;
      else
	sw=cw2;
      if (ch1>ch2)
	sh=2.0*ch1;
      else
	sh=2.0*ch2;
      break;
    case 1008:
      getComplexSize(1001,cw1,ch1);
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1007,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sw=cw1+cw2;
      diffx=(cw2-cw1)*0.5;
      if (ch1>ch2)
	sh=ch1;
      else
	sh=ch2;
      break;
    case 1009:
      getComplexSize(1002,cw1,ch1);
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1007,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sw=cw1+cw2;
      diffx=(cw2-cw1)*0.5;
      if (ch1>ch2)
	sh=ch1;
      else
	sh=ch2;
      break;
    case 1010:
      getComplexSize(1005,cw1,ch1);
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1007,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sw=cw1+cw2;
      diffx=(cw2-cw1)*0.5;
      if (ch1>ch2)
	sh=ch1;
      else
	sh=ch2;
      break;
    case 1011:
      getComplexSize(1003,cw1,ch1);
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1007,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sw=cw1+cw2;
      if (ch1>ch2)
	sh=ch1;
      else
	sh=ch2;
      break;
    case 1012:
      getComplexSize(1004,cw1,ch1);
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1007,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sw=cw1+cw2;
      diffx=(cw2-cw1)*0.5;
      if (ch1>ch2)
	sh=ch1;
      else
	sh=ch2;
      break;
    case 1013:
      getComplexSize(1006,cw1,ch1);
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1007,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sw=cw1+cw2;
      diffx=(cw2-cw1)*0.5;
      if (ch1>ch2)
	sh=ch1;
      else
	sh=ch2;
      break;
    case 1014:
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1000,cw1,ch1);
      getComplexSize(1007,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sw=cw1+cw2;
      diffx=(cw2-cw1)*0.5;
      if (ch1>ch2)
	sh=ch1;
      else
	sh=ch2;
      break;
    case 1015:
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1007,cw1,ch1);
      getComplexSize(1000,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sh=ch1+ch2;
      diffy=(ch2-ch1)*0.5;
      if (cw1>cw2)
	sw=cw1;
      else
	sw=cw2;
      break;
    case 1016:
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(2000,cw1,ch1);
      getComplexSize(1007,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sw=cw1+cw2;
      diffx=(cw2-cw1)*0.5;
      if (ch1>ch2)
	sh=ch1;
      else
	sh=ch2;
      break;
    case 1018:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(RIGHTARROW,cw,ch);
      sw=cw; sh=ch;
      break;
    case 1019:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(LOWSYMBOL,cw,ch);
      sw=cw; sh=ch;
      break;
    case 1020:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(HIGHSYMBOL,cw,ch);
      sw=cw; sh=ch;
      break;
    case 1021:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(THUNDERSYMBOL,cw,ch);
      sw=cw; sh=ch;
      break;
    case 1022:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(CROSS,cw,ch);
      sw=cw; sh=0.5*ch;
      break;
    case 1025:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(MOUNTAINWAVESYMBOL,cw,ch);
      sw=cw; sh=ch;
      break;
    case 1026:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(VULCANOSYMBOL,cw,ch);
      sw=cw; sh=ch;
      break;
    case 1027:
      getComplexSize(1000,sw,sh);
      break;
    case 1028:
      getComplexSize(1000,sw,sh);
      break;
    case 1029:
      getComplexSize(1000,sw,sh);
      break;
    case 1030:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(FOGSYMBOL,cw,ch);
      sw=cw; sh=ch;
      break;
    case 1031:
      getComplexSize(1030,sw,sh);
      break;
    case 1032:
      getComplexSize(1030,sw,sh);
      break;
    case 1033:
      getComplexSize(1000,sw,sh);
      break;
    case 1034:
      if (symbolStrings.size()>1){
	getComplexSize(1026,cw2,ch2);
	fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
	fp->getStringSize(symbolStrings[0].c_str(),cw1,ch1);
	cw1 += (cw2*1);
	fp->getStringSize(symbolStrings[1].c_str(),cw2,ch2);
	if (cw2>cw1)  cw1=cw2;
	ch1 += ch2 ;
	sw = cw1 * 1.3;
	sh = ch1 * 1.3;
      }
      break;
    case 1035:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(NEW_CROSS,cw,ch);
      sw=cw; sh=0.5*ch;
      break;
    case 1036:
      fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
      if (symbolStrings.size()>0)
	fp->getStringSize(symbolStrings[0].c_str(),sw,sh);{
	if (symbolStrings.size()>1 && symbolStrings[1].exists()){
	  fp->getStringSize(symbolStrings[1].c_str(),cw2,ch2);
	  if (cw2>sh)  sw=cw2;
	  sh += ch2 ;
	}
	sw *= 1.7;
	sh *= 1.1;
      }
      break;
    case 1037:
      fp->set("METSYMBOLFONT",poptions.fontface,symbolSizeToPlot);
      fp->getCharSize(WIDESPREADBRSYMBOL,cw,ch);
      sh =ch;
      sw=cw;
      sw=cw; sh=ch;
      break;
    case 1040:
      symbolSizeToPlot=int (symbolSizeToPlot/textShrink);
      getComplexSize(1007,cw1,ch1);
      getComplexSize(2000,cw2,ch2);
      symbolSizeToPlot=int (symbolSizeToPlot*textShrink);
      sh=ch1+ch2;
      diffy=(ch2-ch1)*0.5;
      if (cw1>cw2)
	sw=cw1;
      else
	sw=cw2;
      break;
    case 2000:
      if (symbolStrings.size()>1){
	float cw1,ch1;
	float cw2,ch2;
	fp->set(poptions.fontname,poptions.fontface,symbolSizeToPlot);
	fp->getStringSize(symbolStrings[0].c_str(),cw1,ch1);
	fp->getStringSize(symbolStrings[1].c_str(),cw2,ch2);
	if (cw1>cw2)
	  sw=cw1;
	else
	  sw=cw2;
	if (ch1>ch2)
	  sh=1.8*ch1;
	else
	  sh=1.8*ch2;
      }
      break;
    }
  }
}


void ComplexSymbolPlot::getComplexBoundingBox(int index, float& sw, float & sh, float & x, float & y){
  getComplexSize(index,sw,sh);
  x=x+diffx;
  y=y+diffy;
}

void ComplexSymbolPlot::hideBox(){
  whiteBox=!whiteBox;
}


int ComplexSymbolPlot::hasWhiteBox(){
  return (whiteBox)?1:0;
}


void ComplexSymbolPlot::setWhiteBox(int on){
   (on==1)?whiteBox=true:whiteBox=false;
}



bool ComplexSymbolPlot::isTextEdit(int drawIndex){
  //cerr << "complexSymbolPlot::isTextEdit " << drawIndex << endl;
  switch (drawIndex){
  case 3000:
    return true;
  case 3001:
    return true;
  default:
    return false;
  }
}

bool ComplexSymbolPlot::isComplexTextColor(int drawIndex){
  cerr << "*************complexSymbolPlot::isComplexTextColor " << drawIndex << endl;
  switch (drawIndex){
  case 900:
    return true;
  default:
    return false;
  }
}


bool ComplexSymbolPlot::isComplexText(int drawIndex){
  //cerr << "complexSymbolPlot::isComplexText " << drawIndex << endl;
  switch (drawIndex){
  case 1000:
    return true;
  case 1001:
    return false;
  case 1002:
    return false;
  case 1003:
    return false;
  case 1004:
    return false;
  case 1005:
    return false;
  case 1006:
    return false;
  case 1007:
    return true;
  case 1008:
    return true;
  case 1009:
    return true;
  case 1010:
    return true;
  case 1011:
    return true;
  case 1012:
    return true;
  case 1013:
    return true;
  case 1014:
    return true;
  case 1015:
    return true;
  case 1016:
    return true;
  case 1018:
    return false;
  case 1019:
    return false;
  case 1020:
    return false;
  case 1021:
    return false;
  case 1022:
    return false;
  case 1023:
    return true;
  case 1024:
    return true;
  case 1027:
    return true;
  case 1028:
    return true;
  case 1029:
    return true;
  case 1030:
    return false;
  case 1031:
    return false;
  case 1032:
    return false;
  case 1033:
    return true;
  case 1034:
    return true;
  case 1035:
    return false;
  case 1036:
    return true;
  case 1037:
    return false;
  case 1038:
    return false;
  case 1039:
    return false;
  case 1040:
    return true;
  case 1041:
    return false;
  case 1042:
    return false;
  case 1043:
    return false;
  case 1044:
    return false;
  case 1045:
    return false;
  case 2000:
    return true;
  default:
    return false;
  }
}



void ComplexSymbolPlot::getCurrentComplexText(vector <miString> & symbolText,
					  vector <miString> & xText){
  symbolText=currentSymbolStrings;
  xText=currentXStrings;
}

void ComplexSymbolPlot::setCurrentComplexText(const vector <miString> &
symbolText, const vector <miString> & xText){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::setCurrentComplexText" << endl;
#endif
  currentSymbolStrings=symbolText;
  //insert into list of texts
  for (unsigned int i =0;i<symbolText.size();i++){
#ifdef DEBUGPRINT
  cerr << "******* symbolText["<< i << "]"<<symbolText[i] << endl;
#endif
    clist.insert(symbolText[i]);
  }
  currentXStrings=xText;
}


void ComplexSymbolPlot::getComplexText(vector <miString> & symbolText, vector <miString> & xText){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::getComplexText" << endl;
#endif
  if (xvisible)
    xText=xstrings;
  symbolText.clear();
  for (unsigned int i = 0;i<nstringsvisible && i<symbolStrings.size();i++)
    symbolText.push_back(symbolStrings[i]);
}

void ComplexSymbolPlot::changeComplexText(const vector <miString> & symbolText, const vector <miString> & xText){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::changeComplexText" << endl;
#endif
  symbolStrings=symbolText;
  //insert into list of texts
  for (unsigned int i =0;i<symbolText.size();i++)
    clist.insert(symbolText[i]);
  xstrings=xText;
}



void ComplexSymbolPlot::readComplexText(miString complexString){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::readComplexText" << complexString << endl;
#endif
  miString key,value;
  vector <miString> tokens = complexString.split('/');
  for (unsigned int i = 0; i<tokens.size();i++){
    vector <miString> stokens = tokens[i].split(':');
    if (stokens.size()==2){
      key = stokens[0].downcase();
      value = stokens[1];
      vector <miString> texts = value.split(',');
      if (key=="symbolstrings"){
	symbolStrings=texts;
	for (unsigned int i=0;i<symbolStrings.size();i++)
	  WeatherSymbol::replaceText(symbolStrings[i],false);
      }
      else if (key=="xstrings"){
	xstrings=texts;
	for (unsigned int i=0;i<xstrings.size();i++)
	  WeatherSymbol::replaceText(xstrings[i],false);
      }
    }
  }
}


miString ComplexSymbolPlot::writeComplexText(){
#ifdef DEBUGPRINT
  cerr << "ComplexSymbolPlot::writeComplexText-"  << endl;
#endif
  miString ret;
  int ns=symbolStrings.size();
  int nx=xstrings.size();
  if (ns || nx){
    ret ="ComplexText=";
   if (ns){
      ret+="symbolstrings:";
      for (int i=0;i<ns;i++){
	miString tempString=symbolStrings[i];
	WeatherSymbol::replaceText(tempString,true);
	ret+=tempString;
	if (i<ns-1)
	  ret+=",";
      }
    }
    if (ns&&nx)ret+="/";
    if (nx){
      ret+="xstrings:";
      for (int i=0;i<nx;i++){
	miString tempString=xstrings[i];
	WeatherSymbol::replaceText(tempString,true);
	ret+=tempString;
	if (i<nx-1)
	  ret+=",";
      }
    }
    ret+=";\n";
  }
  return ret;
}


void ComplexSymbolPlot::initComplexList(){
  clist.insert("BKN/OVC STF");
  clist.insert("RA");
  clist.insert("RADZ");
  clist.insert("SHRA");
  clist.insert("SN");
  clist.insert("SHSN");
  clist.insert("LOC FG");
  clist.insert("BR");
  clist.insert("NSC");
  clist.insert("RASN");
  clist.insert("SHSNGS");
  clist.insert("SHRASN");
  clist.insert("SHRA");
  clist.insert("BR/FG");
  clist.insert("DZ");
  clist.insert("FZRA");
  clist.insert("SFC VIS");
  clist.insert("LT 5 KM");
  clist.insert("ISOL");
  clist.insert("EMBD CB");
  clist.insert("SCT/BKN");
  clist.insert("CB/TCU");
}



set <miString> ComplexSymbolPlot::getComplexList(){
  return clist;
}





