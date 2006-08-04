/******************************************************************
  JMQt - JuggleMaster for Qt
  Copyright (C) catstar 2005
  Copyright (C) Greg Gilbert 2004

JuggleMaster is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even  the implied  warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You can see the GNU General Public License on the COPYING file 
in this source archive.
******************************************************************/

#include <stdio.h>
#include <qmessagebox.h>
#include "callback.h"

void messageCallback(char* msg) {
  QString str(msg);
  QMessageBox::information(0, "message", str);
#ifndef NO_DEBUG
  printf("MessageCallback:%s\n",msg);
#endif//NO_DEBUG
}

void errorCallback(char* msg) {
  QString str(msg);
  QMessageBox::information(0, "error", str);
#ifndef NO_DEBUG
  printf("ErrorCallback:%s\n",msg);
#endif//NO_DEBUG
}



