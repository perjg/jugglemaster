/*
 * AAJM - AA JuggleMaster
 * (C) Gary Briggs 2003
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

#ifndef AAJM_H
#define AAJM_H

#include <aalib.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <getopt.h>
#include <malloc.h>
#include "../jmlib/jmlib.h"
#include "./aajm_common.h"
#include "./aa_drawline.h"
#include "./aa_drawcircle.h"


aa_context *context;
aa_renderparams *params;
JMLib* jmlib;

struct loadavg {
	float one, five, fifteen;
};

/* speed is in microseconds-between-frames, because I'm that lazy */
#define DEFSPEED 18000
#define DEFLOAD (float)0.2


void errorCB(char* msg);
void draw_juggler(int show_loadavg);
void loadaverage(struct loadavg *load);
int startlistening(int port);
void stoplistening(int fd);
void resizehandler(aa_context *resized_context);
void main_loop(int max_iterations, int delay,
			int loadavg_flag, int normal_load, int socket_fd);
#endif
