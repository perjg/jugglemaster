// 	$Id$	 

/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
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

#include "prefs.h"

void  JMPreferences::loadPreferences() {}
void  JMPreferences::savePreferences() {}
int   JMPreferences::getIntDefault(int key)   { return 0; }
float JMPreferences::getFloatDefault(int key) { return 0.0F; }

void JMPreferences::cleanup(void) {
  JMPref* pref = prefListHead;
  JMPref* temp;

  while (pref != NULL) {
    // save position
    temp = pref;
    pref = pref->next;

    // delete current
    delete temp;
  }

  // delete MRU list
  for (int i = 0; i < MRULen; i++) {
    delete MRUList[i];
  }
}

void JMPreferences::addPref(JMPref* pref) {
  // First element?
  if (prefListHead == NULL && prefListTail == NULL)
    prefListHead = prefListTail = pref;
  else
    prefListHead = pref;
}

struct JMPreferences::JMPref* JMPreferences::findPref(int key) {
  JMPref* pref = prefListHead;

  while (pref != NULL) {
    if (pref->key == key)
      return pref;
    pref = pref->next;
  }

  return NULL;
}

void JMPreferences::setPref(int key, int value, bool transient) {
  JMPref* pref = new JMPref(key, value, transient, prefListHead);
  addPref(pref);
}

void JMPreferences::setPref(int key, float value, bool transient) {
  JMPref* pref = new JMPref(key, value, transient, prefListHead);
  addPref(pref);
}

void JMPreferences::setPref(int key, void* value, int len, bool transient) {
  JMPref* pref = new JMPref(key, value, len, transient, prefListHead);
  addPref(pref);
}

int JMPreferences::getIntPref(int key) {
  JMPref* pref = findPref(key);

  if (pref)
    return pref->value.intval;
  else
    return getIntDefault(key);
}

float JMPreferences::getFloatPref(int key) {
  JMPref* pref = findPref(key);

  if (pref)
    return pref->value.floatval;
  else
    return getFloatDefault(key);
}

void* JMPreferences::getPtrPref(int key, int& len) {
  JMPref* pref = findPref(key);

  if (pref) {
    len = pref->ptrLen;
    return pref->value.ptrval;
  }
  else {
    len = 0;
    return NULL;
  }
}

void JMPreferences::addToMRU(char* site) {
  if (MRULen < 10)
    MRULen++;
  
  // Move all in list one notch down
  for (int i = MRULen-1; i > 0; i--) {
    MRUList[i] = MRUList[i-1];
  }

  MRUList[0] = new char[strlen(site)+1];
  strcpy(MRUList[0], site);
}

char* JMPreferences::getMRUAt(int pos) {
  if (pos > MRULen-1 || pos < 0 || pos > MRUSIZE)
    return NULL;
  else
    return MRUList[pos];
}

char** JMPreferences::getMRUList(void) {
  return (char**)MRUList;
}

/*
#include <stdio.h>
#define LASTCAT 0
#define LASTPATT 1
#define GRAVITY 2
#define FOO 3
#define BAR 4

int main(void) {
  char* foo = "hello there, i'm a string";
  int len;

  JMPreferences* prefs = new JMPreferences();

  prefs->setPref(LASTCAT, 7);
  prefs->setPref(LASTPATT, 9);
  prefs->setPref(GRAVITY, 9.81F);
  prefs->setPref(FOO, foo, strlen(foo)+1);

  printf("lastcat = %d\n", prefs->getIntPref(LASTCAT));
  printf("lastpatt = %d\n", prefs->getIntPref(LASTPATT));
  printf("gravity = %f\n", prefs->getFloatPref(GRAVITY));
  printf("bar = %d\n", prefs->getIntPref(BAR));
  printf("foo = '%s'\n", (char*)prefs->getPtrPref(FOO, len));
  printf("lastpatt = %d\n", prefs->getIntPref(LASTPATT));
  
  delete prefs;
}
*/

