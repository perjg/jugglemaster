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

#ifndef JUGGLEWIDGET_H
#define JUGGLEWIDGET_H

#include <jmlib.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qtimer.h>

class JuggleWidget : public QWidget
{
    Q_OBJECT
    public:
        JuggleWidget( QWidget * parent=0, char * name="", WFlags fl=0);
        ~JuggleWidget();
        void setSiteSwap(QString pattern, float hr = -1, float dr=-1);
        void setStyle(QString pattern);
        void setMirror(bool);

    public slots:
        void juggle(void);

    protected:
        void paintEvent ( QPaintEvent * );
        void resizeEvent ( QResizeEvent * );
        void updateBuffer(void);

    private:

        QPixmap * m_buf;
        QTimer * m_timer;
        JMLib * m_jmlib;

};

#endif
