/*
 * JMQt - Qt JuggleMaster
 * (C) Greg Gilbert 2004
 *
 * JuggleMaster is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */ 

#include <qpainter.h>
#include "jugglewidget.h"

    JuggleWidget::JuggleWidget(QWidget * parent, char * name, WFlags fl) :
QWidget(parent, name, fl | WRepaintNoErase)
{
    m_buf=0;

    // Initialize jmlib
    m_jmlib = new JMLib();

    // jmlib->setErrorCallback(ErrorCallBack);
    m_jmlib->setWindowSize((JML_INT32)width(),(JML_INT32)height());
    m_jmlib->setPatternDefault();
    m_jmlib->setStyleDefault();
    m_jmlib->startJuggle();

    m_timer = new QTimer(this);
    m_timer->start(25);
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( juggle() ) ); 
    setBackgroundMode(NoBackground);

    m_buf= new QPixmap(width(),height());
    updateBuffer();
}

JuggleWidget::~JuggleWidget()
{
    if (m_buf)
        delete m_buf;
}

void  JuggleWidget::juggle(void)
{
    m_jmlib->doJuggle();
    updateBuffer();
    repaint(FALSE);
}

void JuggleWidget::paintEvent ( QPaintEvent * e) 
{
    bitBlt(this, 0, 0, m_buf);
}

void JuggleWidget::resizeEvent ( QResizeEvent * e  )
{
    QWidget::resizeEvent(e);

    
    if (m_buf) {
     //   delete m_buf;
     //   m_buf= new QPixmap(e->size().width(), e->size().height());
     m_buf->resize(e->size().width(), e->size().height());
    }

    updateBuffer();
    m_jmlib->setWindowSize((JML_INT32)e->size().width(),(JML_INT32)e->size().height());

}

void JuggleWidget::updateBuffer(void)
{
    int i;


    m_buf->fill();
    QPainter p(m_buf);

    arm* ap = &(m_jmlib->ap);
    ball* rhand = &(m_jmlib->rhand);
    ball* lhand = &(m_jmlib->lhand);
    hand* handp = &(m_jmlib->handpoly);

    p.setBackgroundColor(white);

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


    p.setBrush(QBrush(red));

    for(int i=m_jmlib->balln-1;i>=0;i--) {
        int diam = (11*m_jmlib->dpm/DW)*2;
        p.drawEllipse(m_jmlib->b[i].gx, m_jmlib->b[i].gy, diam, diam);
    }
    p.flush();
    p.end();

}


void JuggleWidget::setSiteSwap(QString pattern, float hr, float dr)
{

    if (hr == -1) 
        hr = HR_DEF;
    if (dr == -1) 
        dr = DR_DEF;

    m_jmlib->stopJuggle();
    m_jmlib->setPattern("Something",(char * )(pattern.latin1()),hr, dr);
    m_jmlib->setStyleDefault();
    m_jmlib->startJuggle();

}
void JuggleWidget::setStyle(QString style)
{
    m_jmlib->setStyle((char * )style.latin1());
}

void JuggleWidget::setMirror(bool mirror)
{
    m_jmlib->setMirror(mirror);
}
