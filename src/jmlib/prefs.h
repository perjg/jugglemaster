// 	$Id$	 

/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Persson 2000-2002, Gary Briggs 2003
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

#ifndef PREFS__HDR_
#define PREFS__HDR_

#define MRUSIZE   10
#define VAL_INT    0
#define VAL_FLOAT  1
#define VAL_PTR    2

#include <string.h>

class JMPreferences {
 protected:
  struct JMPref {
    int key;
    union {
      int intval;
      float floatval;
      void* ptrval;
    } value;
    int valType;
    int ptrLen;
    bool transient;
    JMPref* next;

    JMPref(int key, int val, bool transient, JMPref* next) {
      this->key = key;
      value.intval = val;
      valType = VAL_INT;
      ptrLen = 0;
      this->transient = transient;
      this->next = next;
    }

    JMPref(int key, float val, bool transient, JMPref* next) {
      this->key = key;
      value.floatval = val;
      valType = VAL_FLOAT;
      ptrLen = 0;
      this->transient = transient;
      this->next = next;
    }

    JMPref(int key, void* val, int len, bool transient, JMPref* next) {
      this->key = key;
      value.ptrval = new char[len];
      memcpy(value.ptrval, val, (size_t)len);
      valType = VAL_PTR;
      ptrLen = len;
      this->transient = transient;
      this->next = next;
    }

    ~JMPref() {
      if (valType == VAL_PTR)
        delete[] (char*)value.ptrval;
    }
  };

  JMPref* prefListHead;
  JMPref* prefListTail;
  char* MRUList[MRUSIZE];
  int MRULen;

  void cleanup(void);
  void addPref(JMPref* pref);
  JMPref* findPref(int key);
 public:
  JMPreferences() { prefListHead = NULL; prefListTail = NULL; MRULen = 0;}
  virtual ~JMPreferences() { cleanup(); }
  
  virtual void loadPreferences();
  virtual void savePreferences();
  virtual int  getIntDefault(int key);
  virtual float getFloatDefault(int key);

  // Simple (key, value) prefs
  void setPref(int key, int value, bool transient = false);
  void setPref(int key, float value, bool transient = false);
  void setPref(int key, void* value, int len, bool transient = false);
  int   getIntPref(int key);
  float getFloatPref(int key);
  void* getPtrPref(int key, int& len);

  // MRU list
  void addToMRU(char* site);
  char* getMRUAt(int pos);
  char** getMRUList(void);
  int getMRULen(void) { return MRULen; }
};

#endif
