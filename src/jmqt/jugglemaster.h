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

#ifndef JUGGLEMASTER_H
#define JUGGLEMASTER_H

#include <qmainwindow.h>
#include <qaction.h>
#include "jugglewidget.h"
class JuggleMaster  : public QMainWindow
{
    Q_OBJECT
    public:
        JuggleMaster( QWidget * parent=0, char * name="", WFlags fl=0);
        ~JuggleMaster();

    public slots:
        void changeSiteSwapSimple(void);
        void changeSiteSwapAdvanced(void);
        void changeStyle(void);
        void mirror(void);
    private:
        JuggleWidget * m_jw;
        QAction * mirrorAction;
};


#endif
