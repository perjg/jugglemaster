/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2002, Gary Briggs 2003
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

/*
#include "jmlib_wrapper.h"
//#include "jmlib.h"

JMLibWrapper::JMLibWrapper() {
 jm = JMLib::create_JuggleMaster();
 js = JMLib::create_JuggleSaver();

 active = jm;
}

void JMLibWrapper::initialize() {
 jm->initialize();
 js->initialize();
}

void JMLibWrapper::shutdown() {
 jm->shutdown();
 js->shutdown();
}

engine_t JMLibWrapper::getType() {
 return active->getType();
}

void JMLibWrapper::setErrorCallback(ERROR_CALLBACK* _cb) {
 jm->setErrorCallback(_cb);
 js->setErrorCallback(_cb);
}

void JMLibWrapper::setErrorCallback(void *aUData, void
(*aCallback)(void *, JML_CHAR *)) {
 //fixme: implement
}

void JMLibWrapper::error(JML_CHAR* msg) {
active->error(msg);
}

JML_BOOL JMLibWrapper::setPattern(JML_CHAR* name, JML_CHAR* site,
JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF) {
 // fixme:
 // - scan site:
 //   Vanilla siteswap: use currently active engine -or- JuggleMaster
if "prefer JM engine" is selected
 //   Sync/Multi siteswap: JuggleMaster only
 //   JuggleSaver pattern: JuggleSaver only
 //   Siteswap with number of balls higher than JuggleSaver max:
JuggleMaster only
 //
 //
 // also consider:
 //  - Vanilla siteswap to JuggleSaver should add b, c or r after name
 //    (how about vanilla siteswaps that contain b, c or r?)
 //  - JuggleMaster patterns that use the JuggleSaver rendering
engine, should have some way
 //    of signalling what kind of objects to use (ball, ring, club)
 //  -
 //
}

// fixme: only JuggleMaster cares about styles, in JuggleSaver, style
is embedded into the pattern
// see if calling like this has any negative implication (e.g. when switching)
// perhaps all calls here should always call both setStyle (same for
setPattern when compatible)
JML_BOOL JMLibWrapper::setStyle(JML_CHAR* name, JML_UINT8 length,
JML_INT8* data, JML_INT32 offset = 0) {
 active->setStyle(name, length, data, offset);
}

//fixme: Built-in styles should probably be supported in JuggleSaver as well
// e.g. add a default styling for Normal, Reverse, Mills Mess etc.
JML_BOOL JMLibWrapper::setStyle(JML_CHAR* name) {
 active->setStyle(name);
}

//fixme: This is probably used inside UI code, so returning NULL might
have implications
JML_CHAR** JMLibWrapper::getStyles(void) {
 return active->getStyles();
}

JML_INT32 JMLibWrapper::numStyles() {
 return active->numStyles();
}

void JMLibWrapper::setPatternDefault(void) {
 active->setPatternDefault();
}

void JMLibWrapper::setStyleDefault(void) {
 active->setStyleDefault();
}

void JMLibWrapper::setHR(JML_FLOAT HR) {
 active->setHR(HR);
}

JML_FLOAT JMLibWrapper::getHR() {
 return active->getHR();
}

void JMLibWrapper::setDR(JML_FLOAT DR) {
 active->setDR(DR);
}

JML_FLOAT JMLibWrapper::getDR() {
 return active->getDR();
}

JML_INT32 JMLibWrapper::numBalls(void) {
 return active->numBalls();
}

void JMLibWrapper::startJuggle(void) {}
void JMLibWrapper::stopJuggle(void) {}

void JMLibWrapper::togglePause(void) {
 jm->togglePause();
 js->togglePause();
}

void JMLibWrapper::setPause(JML_BOOL pauseOn = true) {
 jm->setPause(pauseOn);
 js->setPause(pauseOn);
}

JML_INT32 JMLibWrapper::getStatus(void) {
 return active->getStatus();
}

JML_INT32 JMLibWrapper::doJuggle(void) {
 active->doJuggle();
}

JML_BOOL JMLibWrapper::setWindowSize(JML_INT32 width, JML_INT32 height) {
 jm->setWindowSize(width, height);
 js->setWindowSize(width, height);
}

void JMLibWrapper::setMirror(JML_BOOL mir = true) {
 jm->setMirror(mir);
 js->setMirror(mir);
}

JML_CHAR* JMLibWrapper::getSite(void) {
 return active->getSite();
}

JML_CHAR* JMLibWrapper::getPattName(void) {
 return active->getPattName();
}

JML_CHAR* JMLibWrapper::getStyle(void) {
 return active->getStyle();
}

JML_INT32 JMLibWrapper::getImageWidth() {
 return active->getImageWidth();
}

JML_INT32 JMLibWrapper::getImageHeight() {
 return active->getImageHeight();
}

void JMLibWrapper::speedUp(void) {}
void JMLibWrapper::speedDown(void) {}
void JMLibWrapper::speedReset(void) {}
void JMLibWrapper::setSpeed(float s) {}
float JMLibWrapper::speed(void) {}

JML_CHAR  JMLibWrapper::getSiteposStart(void) {}
JML_CHAR  JMLibWrapper::getSiteposStop(void) {}
JML_INT32 JMLibWrapper::getSiteposLen(void) {}
JML_INT32 JMLibWrapper::getBallRadius(void) {}
*/

