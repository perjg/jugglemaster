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

#include "jugglemaster.h"

#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include "siteswapdlg.h"
#include "siteswapadvdlg.h"
#include "styledlg.h"

JuggleMaster::JuggleMaster(QWidget * parent, char * name, WFlags fl) :
    QMainWindow(parent, name, fl)
{

    QMenuBar * menu = new QMenuBar( this );

    QPopupMenu *file = new QPopupMenu( this );
    menu->insertItem( tr( "File" ), file );

    QAction * n = new QAction( tr( "Change SiteSwap ( Simple )" ), QString::null, 0, this, 0 );
    connect( n, SIGNAL( activated() ), this, SLOT( changeSiteSwapSimple() ) ); 
    n->addTo( file );

    n = new QAction( tr( "Change SiteSwap ( Advanced )" ), QString::null, 0, this, 0 );
    connect( n, SIGNAL( activated() ), this, SLOT( changeSiteSwapAdvanced() ) ); 
    n->addTo( file );

    n = new QAction( tr( "Change Style" ), QString::null, 0, this, 0 );
    connect( n, SIGNAL( activated() ), this, SLOT( changeStyle() ) ); 
    n->addTo( file );
    
    n = new QAction( tr( "Exit" ), QString::null, 0, this, 0 );
    connect( n, SIGNAL( activated() ), this, SLOT( close() ) ); 
    n->addTo( file );
    
    QPopupMenu *options = new QPopupMenu( this );
    menu->insertItem( tr( "Options" ), options );

    mirrorAction = new QAction( tr( "Mirror" ), QString::null,0,this,0 );
    mirrorAction->setToggleAction(true);
    mirrorAction->setOn(false);
    connect( mirrorAction, SIGNAL( activated() ), this, SLOT( mirror() ) ); 
    mirrorAction->addTo( options );
    
    m_jw = new JuggleWidget(this);
    setCentralWidget(m_jw);
}

JuggleMaster::~JuggleMaster()
{
}

void JuggleMaster::changeSiteSwapAdvanced(void)
{

    SiteSwapAdvDlg * dlg = new SiteSwapAdvDlg;
    connect( dlg->ok, SIGNAL( clicked() ), dlg, SLOT( accept() ) ); 
    if (dlg->exec()) {
        float hr =  dlg->hr->value()/100.0F;
        float dr =  dlg->dr->value()/100.0F;
        m_jw->setSiteSwap(dlg->siteSwap->text(),hr,dr);
    }

}

void JuggleMaster::changeSiteSwapSimple(void)
{

    SiteSwapDlg * dlg = new SiteSwapDlg;
    connect( dlg->ok, SIGNAL( clicked() ), dlg, SLOT( accept() ) ); 
    if (dlg->exec()) {
        m_jw->setSiteSwap(dlg->siteSwap->text());
    }

}

void JuggleMaster::changeStyle(void)
{

    StyleDlg * dlg = new StyleDlg;
    connect( dlg->ok, SIGNAL( clicked() ), dlg, SLOT( accept() ) ); 
    if (dlg->exec()) {
        m_jw->setStyle(dlg->style->currentText());
    }

}

void JuggleMaster::mirror(void)
{
    m_jw->setMirror(mirrorAction->isOn());
    mirrorAction->setOn(mirrorAction->isOn());

}
