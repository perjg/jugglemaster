/****************************************************************************
** Form interface generated from reading ui file 'siteswapdlgbase.ui'
**
** Created: Mon Oct 13 18:19:57 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SITESWAPDLGBASE_H
#define SITESWAPDLGBASE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class SiteSwapDlgBase : public QDialog
{
    Q_OBJECT

public:
    SiteSwapDlgBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SiteSwapDlgBase();

    QLabel* TextLabel2;
    QLineEdit* siteSwap;
    QPushButton* ok;

protected:
    QVBoxLayout* SiteSwapDlgBaseLayout;

protected slots:
    virtual void languageChange();

};

#endif // SITESWAPDLGBASE_H
