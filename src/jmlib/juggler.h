// 	$Id$	 

/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Persson 2000-2002
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 */ 

#ifndef JUGGLER__HDR
#define JUGGLER__HDR

class Juggler {
 public:
  struct arm ap;
  struct ball rhand, lhand;
  struct ball balls[BMAX]; // make dynamic
}

#endif
