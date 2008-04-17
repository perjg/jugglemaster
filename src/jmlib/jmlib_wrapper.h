/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */

#ifndef JMLIB_WRAPPER_H
#define JMLIB_WRAPPER_H

#include "jmlib_if.h"
#include "jmlib_types.h"

class JMLibWrapper : public AbstractJMLib {
private:
 JuggleMaster* jm;
 JuggleSaver* js;
 JMLib* active;

 JMLibWrapper();
public:
 void initialize();
 void shutdown();

 virtual engine_t getType();

 virtual void setErrorCallback(ERROR_CALLBACK* _cb);
 virtual void setErrorCallback(void *aUData, void (*aCallback)(void*, JML_CHAR *));
 virtual void error(JML_CHAR* msg);

 virtual JML_BOOL setPattern(JML_CHAR* name, JML_CHAR* site,
JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF);
 virtual JML_BOOL setPattern(JML_CHAR* site) { return setPattern(site, site); }
 virtual JML_BOOL setStyle(JML_CHAR* name, JML_UINT8 length,
JML_INT8* data, JML_INT32 offset = 0);
 virtual JML_BOOL setStyle(JML_CHAR* name);
 virtual JML_CHAR **getStyles(void);
 virtual JML_INT32 numStyles();
 virtual void setPatternDefault(void);
 virtual void setStyleDefault(void);

 virtual void setHR(JML_FLOAT HR);
 virtual JML_FLOAT getHR();
 virtual void setDR(JML_FLOAT DR);
 virtual JML_FLOAT getDR();

 virtual JML_INT32 numBalls(void);

 virtual void startJuggle(void);
 virtual void stopJuggle(void);
 virtual void togglePause(void);
 virtual void setPause(JML_BOOL pauseOn = true);
 virtual JML_INT32  getStatus(void);

 virtual JML_INT32 doJuggle(void);

 virtual JML_BOOL setWindowSize(JML_INT32 width, JML_INT32 height);
 virtual void setMirror(JML_BOOL mir = true);

 virtual JML_CHAR* getSite(void);
 virtual JML_CHAR* getPattName(void);
 virtual JML_CHAR* getStyle(void);

 virtual JML_INT32 getImageWidth();
 virtual JML_INT32 getImageHeight();

 virtual void speedUp(void);
 virtual void speedDown(void);
 virtual void speedReset(void);
 virtual void setSpeed(float s);
 virtual float speed(void);

 virtual JML_CHAR  getSiteposStart(void);
 virtual JML_CHAR  getSiteposStop(void);
 virtual JML_INT32 getSiteposLen(void);
 virtual JML_INT32 getBallRadius(void);
};

#endif