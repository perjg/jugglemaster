#ifndef AAJM_H
#define AAJM_H

#include <aalib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <malloc.h>
#include "../jmlib/jmlib.h"

aa_context *context;
aa_renderparams *params;
JMLib* jmlib;

/* Now, if only this were part of aalib... */
extern "C" {
#include "./aa_drawline.h"
#include "./aa_drawcircle.h"
}

#define AAWIDTH(context) aa_imgwidth(context)
#define AAHEIGHT(context) (aa_imgheight(context)-4)

void errorCB(char* msg);
void draw_juggler(void);
void resizehandler(aa_context *resized_context);
void main_loop(void);

#endif
