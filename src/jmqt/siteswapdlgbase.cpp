/****************************************************************************
** Form implementation generated from reading ui file 'siteswapdlgbase.ui'
**
** Created: Mon Oct 13 17:52:33 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "siteswapdlgbase.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a SiteSwapDlgBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SiteSwapDlgBase::SiteSwapDlgBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "SiteSwapDlgBase" );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 30, 10, 161, 21 ) );

    ok = new QPushButton( this, "ok" );
    ok->setGeometry( QRect( 150, 90, 125, 36 ) );

    siteSwap = new QLineEdit( this, "siteSwap" );
    siteSwap->setGeometry( QRect( 10, 50, 271, 31 ) );
    languageChange();
}

/*
 *  Destroys the object and frees any allocated resources
 */
SiteSwapDlgBase::~SiteSwapDlgBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SiteSwapDlgBase::languageChange()
{
    setCaption( tr( "Choose SiteSwap" ) );
    TextLabel2->setText( tr( "Please Enter SiteSwap Here" ) );
    ok->setText( tr( "Ok" ) );
}

