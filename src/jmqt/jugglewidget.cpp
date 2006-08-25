/******************************************************************
  JMQt - JuggleMaster for Qt
  Copyright (C) catstar 2005
  Copyright (C) Greg Gilbert 2004

JuggleMaster is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even  the implied  warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You can see the GNU General Public License on the COPYING file 
in this source archive.
******************************************************************/

#include <qpainter.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include "callback.h"
#include "jugglewidget.h"

JuggleWidget::JuggleWidget(UserPreference * p, JMLib* pJMLib, QWidget * parent, char * name, WFlags fl) :
QWidget(parent, name, fl | WRepaintNoErase)
{
  pUP = p;
  m_buf=0;
  // Initialize jmlib
  m_jmlib = pJMLib;
  m_jmlib->setWindowSize((JML_INT32)width(),(JML_INT32)height());
  m_jmlib->setPatternDefault();
  m_jmlib->setStyleDefault();
  m_jmlib->startJuggle();

  setBackgroundMode(NoBackground);

  m_buf= new QPixmap(width(),height());
  updateBuffer();
}

JuggleWidget::~JuggleWidget() {
  if (m_buf) {
    delete m_buf;
  }
}

void  JuggleWidget::juggle(void) {
  m_jmlib->doJuggle();
  updateBuffer();
  repaint(FALSE);
}

void JuggleWidget::paintEvent ( QPaintEvent * e) {
  bitBlt(this, 0, 0, m_buf);
}

void JuggleWidget::resizeEvent ( QResizeEvent * e  ) {
  QWidget::resizeEvent(e);
  if (m_buf) {
    m_buf->resize(e->size().width(), e->size().height());
  }
  updateBuffer();
  m_jmlib->setWindowSize(
    (JML_INT32)e->size().width(),
    (JML_INT32)(e->size().height() - ZAURUS_SL_QTOPIATASKBAR_HEIGHT)
    );
}

void JuggleWidget::updateBuffer(void) {
  int i;

  m_buf->fill(pUP->getBGColor());
  QPainter p(m_buf);

  arm* ap = &(m_jmlib->ap);
  ball* rhand = &(m_jmlib->rhand);
  ball* lhand = &(m_jmlib->lhand);
  hand* handp = &(m_jmlib->handpoly);

  p.setBackgroundColor(pUP->getBGColor());
  p.setPen(pUP->getLineColor());

  // Draw the head
  p.drawEllipse (ap->hx - ap->hr, ap->hy-ap->hr, ap->hr*2,ap->hr*2 );

  p.setBrush(QBrush(black));

  // draw juggler
  for( i=0;i<5;i++){
    p.drawLine(ap->rx[i], ap->ry[i], ap->rx[i+1], ap->ry[i+1]);
    p.drawLine(ap->lx[i], ap->ly[i], ap->lx[i+1], ap->ly[i+1]);
  }

  // hands
  for (int i=0; i <= 8; i++) {
    p.drawLine(rhand->gx + handp->rx[i], rhand->gy + handp->ry[i],
               rhand->gx + handp->rx[i+1], rhand->gy + handp->ry[i+1]);
    p.drawLine(lhand->gx + handp->lx[i], lhand->gy + handp->ly[i],
               lhand->gx + handp->lx[i+1], lhand->gy + handp->ly[i+1]);
  }
  p.drawLine(rhand->gx + handp->rx[9], rhand->gy + handp->ry[9],
             rhand->gx + handp->rx[0], rhand->gy + handp->ry[0]);
  p.drawLine(lhand->gx + handp->lx[9], lhand->gy + handp->ly[9],
             lhand->gx + handp->lx[0], lhand->gy + handp->ly[0]);

  // draw balls
  p.setPen(pUP->getBallLineColor());

  int diam = (11*m_jmlib->dpm/DW)*2;
  if(pUP->isUseColorList()) {
    for(int i=0;i<=m_jmlib->numBalls()-1;i++) {
      p.setBrush(pUP->getBallColor(i));
      p.drawEllipse(m_jmlib->b[i].gx, m_jmlib->b[i].gy, diam, diam);
    }
  } else {
    p.setBrush(pUP->getBallColorSame());
    for(int i=0;i<=m_jmlib->numBalls()-1;i++) {
      p.drawEllipse(m_jmlib->b[i].gx, m_jmlib->b[i].gy, diam, diam);
    }
  }

  // draw siteswap and parameters
  QString str, strSite;
  QTextOStream(&strSite) << "<" << m_jmlib->getSite() << ">";
  QTextOStream tos(&str);
  tos << "\"" << m_jmlib->getPattName() << "\" (" << m_jmlib->getStyle() << ")";
//  tos << "[HR=" << m_jmlib->getHR()*100 << "%]";
//  tos << "[DR=" << m_jmlib->getDR()*100 << "%]";

  p.setBrush(pUP->getTextColor());

  int py = height() - 5;
  p.drawText(5, py, str);
  p.drawText(5, py - TEXT_ROW_MARGIN,   strSite);

  // buffer flush
  p.flush();
  p.end();
}
