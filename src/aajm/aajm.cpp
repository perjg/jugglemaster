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

#include "./aajm.h"

void errorCB(char* msg) {
	aa_printf(context, 1, 1, AA_NORMAL, "%s", msg);
	aa_flush(context);
	sleep(2);
}

void draw_juggler(void) {
	int color;
	int i;

	arm* ap = &(jmlib->ap);
	ball* rhand = &(jmlib->rhand);
	ball* lhand = &(jmlib->lhand);
	hand* handp = &(jmlib->handpoly);

	color = 64;
	// draw head
	aa_drawcircle(context,ap->hx, ap->hy,
			ap->hr, color, -1);

	aa_drawline(context, 0, 0, AAWIDTH(context)/2, AAHEIGHT(context)/2, 1);
	// draw juggler
	for (i=0;i<5;i++) {
		aa_drawline(context, ap->rx[i], ap->ry[i],
			ap->rx[i+1], ap->ry[i+1], color);
		aa_drawline(context, ap->lx[i], ap->ly[i],
			ap->lx[i+1], ap->ly[i+1], color);
	}

	// hands
	for (i=0; i <= 8; i++) {
		aa_drawline(context, rhand->gx + handp->rx[i],
			rhand->gy + handp->ry[i],
			rhand->gx + handp->rx[i+1],
			rhand->gy + handp->ry[i+1], color);
		aa_drawline(context, lhand->gx + handp->lx[i],
			lhand->gy + handp->ly[i],
			lhand->gx + handp->lx[i+1],
			lhand->gy + handp->ly[i+1], color);
	}
	aa_drawline(context, rhand->gx + handp->rx[9],
		rhand->gy + handp->ry[9],
		rhand->gx + handp->rx[0],
		rhand->gy + handp->ry[0], color);
	aa_drawline(context, lhand->gx + handp->lx[9],
		lhand->gy + handp->ly[9],
		lhand->gx + handp->lx[0],
		lhand->gy + handp->ly[0], color);

	color = 255;
	// draw balls
	int diam = (11*jmlib->dpm/DW);
	for(i=jmlib->balln-1;i>=0;i--) {
		aa_drawcircle(context, jmlib->b[i].gx + diam,
				jmlib->b[i].gy + diam,
				diam, color, color);
	}


	aa_render(context, params, 0, 0,
		AAWIDTH(context), AAHEIGHT(context));

	aa_printf(context, 0, 0, AA_SPECIAL,
		"Site: %s    Style: %s    Balls: %i",
		jmlib->getSite(), jmlib->getStyle(), jmlib->balln);

	if(jmlib->getStatus() == ST_PAUSE) {
		aa_puts(context, 0, 1, AA_SPECIAL, "Paused");
	}
	aa_flush(context);

	memset(context->imagebuffer,0,AAWIDTH(context)*AAHEIGHT(context));
}

void resizehandler(aa_context *resized_context) {
	jmlib->setWindowSize(AAWIDTH(resized_context),
			AAHEIGHT(resized_context));
	context = resized_context;
}

#define DEFSPEED 18000
void main_loop(int max_iterations, int delay) {
	struct timeval starttime, endtime;
	long sleeptime;
	long speed = DEFSPEED; /* microseconds between frames */
	int loop_forever = 0;
	char c;
	int i;
	char newsite[JML_MAX_SITELEN];
	char newstyle[2];
	int newstyle_index;
	int numstyles = sizeof(possible_styles)/sizeof(possible_styles[0]);

	if(delay > 0) {
		speed = delay * 1000;
	}

	if(max_iterations <= 0) {
		loop_forever = 1;
	}
	while (1) {
		gettimeofday(&starttime,NULL);
		jmlib->doJuggle();
		draw_juggler();
		c=aa_getkey(context,0);
		if(c=='s' || c=='S') {
			/* Change SiteSwap */
			memset(newsite,0,JML_MAX_SITELEN);
			strncpy(newsite, jmlib->getSite(), JML_MAX_SITELEN);
			aa_puts(context, 1, 4, AA_SPECIAL,
				"Enter New SiteSwap...");
			aa_edit(context, 1, 5, 20,
				newsite, JML_MAX_SITELEN);
			if(newsite[0]!=0) {
				jmlib->setPattern("Something",newsite,
					HR_DEF, DR_DEF);
				jmlib->setStyleDefault();
				jmlib->startJuggle();
			}
		} else if(c=='q' || c=='Q' || c==27) {
			/* Quit */
			/* 27 == Escape
			Don't complain. This was a hack before it started */
			return;
		} else if(c==' ' || c=='p' || c=='P') {
			/* Toggle Pause */
			jmlib->togglePause();
		} else if(c=='t' || c=='T') {
			/* Change Style */
			aa_puts(context, 3, 4, AA_SPECIAL,
				"Choose New Style...");
			for (i=0;i<numstyles;i++) {
				aa_printf(context, 3, 5+i, AA_SPECIAL,
					"%i: %s",i+1,possible_styles[i]);
			}
			aa_flush(context);
			memset(newstyle,0,JML_MAX_SITELEN);
			aa_edit(context, 3, 5+i, 2, newstyle, 2);
			if(newstyle[0]!=0) {
				newstyle_index=atoi(newstyle)-1;
				if(newstyle_index>=0
				  && newstyle_index<numstyles) {
					jmlib->setStyle(possible_styles[newstyle_index]);
				}
			}
		} else if(c=='h' || c=='H') {
			/* Help */
			aa_puts(context, 3, 4, AA_SPECIAL,
				"Key Help");
			aa_puts(context, 3, 6, AA_SPECIAL,
				"h - This screen");
			aa_puts(context, 3, 7, AA_SPECIAL,
				"s - Change Siteswap");
			aa_puts(context, 3, 8, AA_SPECIAL,
				"t - Change Style");
			aa_puts(context, 3, 9, AA_SPECIAL,
				"q - Quit");
			aa_puts(context, 3, 10, AA_SPECIAL,
				"space - Pause");
			aa_puts(context, 3, 11, AA_SPECIAL,
				"+, -, enter - Speed up, down, reset");
			aa_puts(context, 3, 13, AA_SPECIAL,
				"Press any key to remove this menu");
			aa_flush(context);
			aa_getkey(context, 1);
		} else if(c=='+' || c=='=' || c=='k') {
			/* Speed Up */
			speed -= 1500;
			if(speed < 0) { speed = 0; }
		} else if(c=='-' || c=='j') {
			/* Speed Down */
			speed += 1500;
		} else if(c==13) {
			/* Speed Reset */
			/* 13 == Enter */
			speed = DEFSPEED;
		}
		gettimeofday(&endtime,NULL);
		endtime.tv_sec -= starttime.tv_sec;
		endtime.tv_usec -= starttime.tv_usec;
		while (endtime.tv_usec < 0) {
			endtime.tv_sec --;
			endtime.tv_usec += 1000000;
		}
		sleeptime = speed - (endtime.tv_usec + endtime.tv_sec*1000000);
		if(sleeptime > 0) {
			usleep(sleeptime);
		} else {
			usleep(1);
		}
		if(!loop_forever && max_iterations-- <= 0) {
			break;
		}
	}
}

int main(int argc, char **argv) {
	char options[] = "ajhd:m:t:s:";
	int help_flag = 0;
	int aahelp_flag = 0;
	int justoutput_flag = 0;
	int max_iterations = 0;
	int delay = 0;
	static struct option long_options[] =
        {
		{"help", no_argument, &help_flag, 1},
		{"aahelp", no_argument, &aahelp_flag, 1},
		{"justoutput", no_argument, &justoutput_flag, 1},
		{"maxiterations", required_argument, 0, 'm'},
		{"delay", required_argument, 0, 'd'},
		{"siteswap", required_argument, 0, 's'},
		{"style", required_argument, 0, 't'}
	};

	char optch;
	int option_index = 0;

	jmlib = new JMLib(errorCB);
	jmlib->setPatternDefault();
	jmlib->setStyleDefault();
	jmlib->startJuggle();

	aa_parseoptions(NULL, NULL, &argc, argv);

	while( (optch = getopt_long(argc,argv,options,
			long_options,&option_index)) != -1)
		switch(optch) {
			case 's':
				jmlib->setPattern("Something",optarg,
					HR_DEF, DR_DEF);
				break;
			case 't':
				jmlib->setStyle(optarg);
				break;
			case 'h':
				help_flag=1;
				break;
			case 'a':
				aahelp_flag=1;
				break;
			case 'j':
				justoutput_flag=1;
				break;
			case 'm':
				max_iterations = atoi(optarg);
				break;
			case 'd':
				delay = atoi(optarg);
				break;
		}

	if(aahelp_flag || help_flag) {
		printf("AAJM, An ASCII Art Juggling program\n");
		printf("Usage: %s [OPTIONS]\n",argv[0]);
	}
	if(help_flag) {
		printf("Jugglemaster Options:\n");
		printf("  -s, --siteswap=XX          show siteswap XX\n");
		printf("  -t, --style=XX             use style XX\n");
		printf("  -d, --delay=XX             delay XX ms between frames\n");
		printf("  -m, --maxiterations=XX     do at most XX iterations\n");
		printf("  -j, --justoutput           only output [don't init kb or mouse]\n");
		printf("  -h, --help                 get help\n");
		printf("  -a, --aahelp               get help on AA options\n\n");
	}
	if(aahelp_flag) {
		printf("AALib Options:\n%s\n\n",aa_help);
	}
	if(aahelp_flag || help_flag) {
		return 0;
	}


	context = aa_autoinit(&aa_defparams);
	if (context == NULL) {
		printf("Failed to initialize aalib\n");
		exit(1);
	}
	if(!justoutput_flag) {
		aa_autoinitkbd(context, 0);
		aa_hidecursor(context);
	}
	params = aa_getrenderparams();
	jmlib->setWindowSize(AAWIDTH(context),AAHEIGHT(context));
	jmlib->startJuggle();

	aa_resizehandler(context, resizehandler);

	main_loop(max_iterations,delay);

	aa_close(context);

	delete jmlib;
	return 1;
}

