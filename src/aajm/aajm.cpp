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

	color = 255;
	// draw head
	aa_drawcircle(context,ap->hx, ap->hy,
			ap->hr, color);

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

	// draw balls
	for(i=jmlib->balln-1;i>=0;i--) {
		int diam = (11*jmlib->dpm/DW);
		aa_drawcircle(context, jmlib->b[i].gx + diam,
				jmlib->b[i].gy + diam,
				diam, color);
	}

	aa_render(context, params, 0, 0,
		AAWIDTH(context), AAHEIGHT(context));
	aa_flush(context);
	memset(context->imagebuffer,0,AAWIDTH(context)*AAHEIGHT(context));
}

void resizehandler(aa_context *resized_context) {
	jmlib->setWindowSize(AAWIDTH(resized_context),
			AAHEIGHT(resized_context));
	context = resized_context;
}

void main_loop(void) {
	while (1) {
		jmlib->doJuggle();
		draw_juggler();
		usleep(20);
	}
}

int main(int argc, char **argv) {
	char options[] = "s:";
	int help_flag = 0;
	static struct option long_options[] =
        {
		{"help", no_argument, &help_flag, 1},
		{"siteswap", required_argument, 0, 's'},
	};

	char optch;
	int option_index = 0;

	jmlib = new JMLib(errorCB);
	jmlib->setPatternDefault();
	jmlib->setStyleDefault();
	jmlib->startJuggle();

	aa_parseoptions(NULL, NULL, &argc, argv);

	context = aa_autoinit(&aa_defparams);
	if (context == NULL) {
		printf("Failed to initialize aalib\n");
		exit(1);
	}
	aa_autoinitkbd(context, 0);
	params = aa_getrenderparams();
	aa_hidecursor(context);
	jmlib->setWindowSize(AAWIDTH(context),AAHEIGHT(context));

	aa_resizehandler(context, resizehandler);

	while( (optch = getopt_long(argc,argv,options,
			long_options,&option_index)) != -1)
		switch(optch) {
			case 's':
				jmlib->setPattern("Something",optarg,
					HR_DEF, DR_DEF);
				jmlib->startJuggle();
				break;
		}

	if(help_flag) {
		printf("AAJM, An ASCII Art Juggling program\n");
		printf("Usage: %s [OPTIONS]\n",argv[0]);
		printf("  -s, --siteswap=XX  show siteswap XX\n");
		return 0;
	}


	main_loop();

	aa_close(context);
	delete jmlib;
	return 1;
}

