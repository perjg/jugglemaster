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

void draw_juggler(int show_loadavg) {
	int color;
	int i;
	struct loadavg load;

	arm* ap = &(jmlib->ap);
	ball* rhand = &(jmlib->rhand);
	ball* lhand = &(jmlib->lhand);
	hand* handp = &(jmlib->handpoly);

	color = 64;
	// draw head
	aa_drawcircle(context,ap->hx, ap->hy,
			ap->hr, color, -1);

	aa_drawline(context, 0, 0, aa_imgwidth(context)/2, aa_imgheight(context)/2, 1);
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
		aa_imgwidth(context), aa_imgheight(context));

	aa_printf(context, 0, 0, AA_SPECIAL,
		"Site: %s    Style: %s    Balls: %i",
		jmlib->getSite(), jmlib->getStyle(), jmlib->balln);

	if(show_loadavg) {
		loadaverage(&load);
		aa_printf(context, 0, 1, AA_SPECIAL,
			"LoadAvg: %2.2f %2.2f %2.2f",
			load.one, load.five, load.fifteen);
	}
	if(jmlib->getStatus() == ST_PAUSE) {
		aa_puts(context, 1, 3, AA_SPECIAL, "Paused");
	}
	aa_flush(context);

	memset(context->imagebuffer,0,aa_imgwidth(context)*aa_imgheight(context));
}

/* Just for anyone not aware, /proc/loadavg gives load average over the
     last 1, 5, 15 minutes. "Load Average" is defined as the average
     runnable number of process at any given moment in time.
   If load average is greater than your number of processors, then your
     system has more work to do than it's capable of doing. */
void loadaverage(struct loadavg *load) {
	FILE *loadf;

	loadf = fopen("/proc/loadavg", "r");
	load->one = -1;
	load->five = -1;
	load->fifteen = -1;

	if (loadf) {
		fscanf(loadf, "%f %f %f",
			&load->one, &load->five, &load->fifteen);
		fclose(loadf);
	}
}

int startlistening(int port) {
	/* Canned start-socket-listening code */
	char myname[MAXHOSTNAME+1];
	int s;
	struct sockaddr_in sa;
	struct hostent *hp;
	memset(&sa, 0, sizeof(struct sockaddr_in));
	gethostname(myname, MAXHOSTNAME);
	if((hp = gethostbyname(myname)) == NULL) {
		return(-1);
	}

	sa.sin_family = hp->h_addrtype;
	sa.sin_port = htons(port);

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return(-1);
	}

	if (bind(s, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0) {
		close(s);
		return(-1);
	}
	listen(s, 1);
	return(s);
}

void stoplistening(int fd) {
	close(fd);
}

void resizehandler(aa_context *resized_context) {
	jmlib->setWindowSize(aa_imgwidth(resized_context),
			aa_imgheight(resized_context));
	context = resized_context;
}

void main_loop(int max_iterations, int delay,
		int loadavg_flag, int normal_load, int socket_fd) {
	struct timeval starttime, endtime, selecttime;
	struct loadavg load;
	long speed = DEFSPEED; /* microseconds between frames */
	long load_speed = 0; /* Speed adjustment, based on load */
	int loop_forever = 0;
	char c;
	int i;
	int tmp;
	char newsite[JML_MAX_SITELEN];
	char newstyle[2];

	int accepted_sock = 0; /* FD for accept()ed socket */
	int amount_read = 0; /* Amount read into buffer so far */
	int r; /* Num bytes read in last read() */
	char socket_buffer[MAX_SOCKET_BUFFER]; /* Buffer */
	char command[MAX_SOCKET_BUFFER];
	char data[MAX_SOCKET_BUFFER];
	fd_set socket_set; /* Used for select() */
	socklen_t sin_size; /* Used by accept() */
	struct sockaddr their_addr; /* Used by accept() */
	JML_CHAR **possible_styles;

	int newstyle_index;

	possible_styles = jmlib->getStyles();

	load.one = -1;
	load.five = -1;
	load.fifteen = -1;

	if(delay > 0) {
		speed = delay * 1000;
	}

	if(max_iterations <= 0) {
		loop_forever = 1;
	}

	memset((void *)socket_buffer,0,MAX_SOCKET_BUFFER);
	memset((void *)command,0,MAX_SOCKET_BUFFER);
	memset((void *)data,0,MAX_SOCKET_BUFFER);

	while (1) {
		gettimeofday(&starttime,NULL);
		jmlib->doJuggle();
		draw_juggler(loadavg_flag);

		if(loadavg_flag) {
			loadaverage(&load);
			if(load.one != -1) {
				tmp = (int)(load.one*100 - normal_load);
				load_speed = 5000 * tmp;
			}
		}

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
			for (i=0;i<jmlib->numStyles();i++) {
				aa_printf(context, 3, 5+i, AA_SPECIAL,
					"%i: %s",i+1,possible_styles[i]);
			}
			aa_flush(context);
			memset(newstyle,0,JML_MAX_SITELEN);
			aa_edit(context, 3, 5+i, 2, newstyle, 2);
			if(newstyle[0]!=0) {
				newstyle_index=atoi(newstyle)-1;
				if(newstyle_index>=0
				  && newstyle_index<jmlib->numStyles()) {
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
				"l - Toggle Load Monitoring");
			aa_puts(context, 3, 10, AA_SPECIAL,
				"q - Quit");
			aa_puts(context, 3, 11, AA_SPECIAL,
				"space - Pause");
			aa_puts(context, 3, 12, AA_SPECIAL,
				"+, -, enter - Speed up, down, reset");
			aa_puts(context, 3, 14, AA_SPECIAL,
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
		} else if(c=='l') {
			/* Toggle Load Monitoring */
			if(loadavg_flag == 1) {
				loadavg_flag = 0;
				load_speed = 0;
			} else {
				loadavg_flag = 1;
			}
		} else if(c==13) {
			/* Speed Reset */
			/* 13 == Enter */
			speed = DEFSPEED;
		}
		gettimeofday(&endtime,NULL);

		selecttime.tv_sec = endtime.tv_sec - starttime.tv_sec;
		if (selecttime.tv_sec != 0) {
			endtime.tv_usec += 1000000*selecttime.tv_sec;
			selecttime.tv_sec = 0;
		}
		selecttime.tv_usec = (speed + load_speed) - 
					(endtime.tv_usec - starttime.tv_usec);
		if(selecttime.tv_usec < 0) {
			selecttime.tv_usec = 1;
		}

		/* IPC Code begins here */
		if(socket_fd <= 0) {
			select(0,NULL,NULL,NULL,&selecttime);
		} else {

		FD_ZERO(&socket_set);
		FD_SET(socket_fd,&socket_set);
		/* accepted_sock is the fd we get
			from accept()ing the socket */
		if(accepted_sock > 0) {
			/* read just one byte */
			r = read(accepted_sock,
				(void *)&socket_buffer[amount_read],1);
			if(r == -1 && errno != EAGAIN) {
			/* EAGAIN means we need to try reading again,
			there wasn't an error, the descriptor just had
			nothing waiting in it
			Else, close the handle reset the other data stuff */
				memset((void *)socket_buffer,0,MAX_SOCKET_BUFFER-amount_read-1);
				close(accepted_sock);
				amount_read = 0;
				accepted_sock = 0;
				memset((void *)data,0,MAX_SOCKET_BUFFER);
				memset((void *)command,0,MAX_SOCKET_BUFFER);
			} else if(r > 0) {
			/* transfer stuff: command=data; If we see an
			'=', everything before it was a command. If we
			see a ';', the stuff before it was data */
				amount_read+=r;
				if(socket_buffer[amount_read-1] == '=') {
					memset((void *)command,0,MAX_SOCKET_BUFFER);
					memcpy((void *)command,(void *)socket_buffer, amount_read-1);
					memset((void *)socket_buffer,0,MAX_SOCKET_BUFFER);
					amount_read = 0;
				} else if(socket_buffer[amount_read-1] == ';') {
					memset((void *)data,0,MAX_SOCKET_BUFFER);
					memcpy((void *)data,(void *)socket_buffer, amount_read-1);
					memset((void *)socket_buffer,0,MAX_SOCKET_BUFFER);
					close(accepted_sock);
					amount_read = 0;
					accepted_sock = 0;
				}
			}
			if(amount_read >= MAX_SOCKET_BUFFER-1) {
			/* Too late, F**k 'em, they're trying
			to overflow us anyway */
				write(accepted_sock,OVERFLOW_ERROR,strlen(OVERFLOW_ERROR));
				memset((void *)socket_buffer,0,MAX_SOCKET_BUFFER);
				close(accepted_sock);
				amount_read = 0;
				accepted_sock = 0;
			}
		}

		if(command[0] != '\0' && data[0] != '\0') {
			if(!strcmp(command,"Style")) {
				jmlib->setStyle(data);
			} else if(!strcmp(command,"Site")) {
				jmlib->setPattern("Something",data,
					HR_DEF, DR_DEF);
				jmlib->setStyleDefault();
				jmlib->startJuggle();
			} else if(!strcmp(command,"Speed")) {
				if(!strcmp(data,"Up")) {
					speed -= 1500;
					if(speed < 0) speed = 0;
				} else if(!strcmp(data,"Down")) {
					speed += 1500;
				} else if(!strcmp(data,"Reset")) {
					speed = DEFSPEED;
				}
			}

			memset((void *)data,0,MAX_SOCKET_BUFFER);
			memset((void *)command,0,MAX_SOCKET_BUFFER);
		}
		if(!strcmp(command,"Quit")) {
			return;
		}
				
		if(select(socket_fd+1, &socket_set,
			NULL, NULL, &selecttime) > 0) {
			if(FD_ISSET(socket_fd,&socket_set) && accepted_sock) {
			/* Just close any subsequent conenctions until
				this one's done */
				close(accept(socket_fd,&their_addr,&sin_size));
			} else if(FD_ISSET(socket_fd,&socket_set) && !accepted_sock) {
			/* Else accept() the socket, and set it to not block */
				sin_size = sizeof(sockaddr);
				accepted_sock = accept(socket_fd,&their_addr, &sin_size);
				fcntl(accepted_sock, F_SETFL, O_NONBLOCK);
			}
		}
		}

		if(!loop_forever && max_iterations-- <= 0) {
			break;
		}
	}
}

int main(int argc, char **argv) {
	char options[] = "aljhip:n:d:m:t:s:";
	int help_flag = 0;
	int aahelp_flag = 0;
	int justoutput_flag = 0;
	int loadavg_flag = 0;
	int ipc_flag = 0;
	int port_num = DEFPORT;
	int max_iterations = 0;
	int delay = 0;
	int normal_load;
	int socket_fd = -1;

	normal_load = (int)(DEFLOAD * 100);
	static struct option long_options[] =
        {
		{"help", no_argument, &help_flag, 1},
		{"aahelp", no_argument, &aahelp_flag, 1},
		{"justoutput", no_argument, &justoutput_flag, 1},
		{"ipc", no_argument, &ipc_flag, 1},
		{"loadavg", no_argument, &loadavg_flag, 1},
		{"maxiterations", required_argument, 0, 'm'},
		{"port", required_argument, 0, 'p'},
		{"normalload", required_argument, 0, 'n'},
		{"delay", required_argument, 0, 'd'},
		{"siteswap", required_argument, 0, 's'},
		{"style", required_argument, 0, 't'},
		{0,0,0,0}
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
			case 'l':
				loadavg_flag=1;
				break;
			case 'n':
				normal_load = (int)(100*atof(optarg));
				break;
			case 'i':
				ipc_flag=1;
				break;
			case 'p':
				port_num=atoi(optarg);
				break;
		}

	if(aahelp_flag || help_flag) {
		printf("AAJM, An ASCII Art Juggling program\n");
		printf("Usage: %s [OPTIONS]\n",argv[0]);
	}
	if(help_flag) {
		printf("Jugglemaster Options:\n");
		printf("  -s, --siteswap=XX          show siteswap XX (3)\n");
		printf("  -t, --style=XX             use style XX (\"Normal\")\n");
		printf("  -d, --delay=XX             delay XX ms between frames (%i)\n", (int)DEFSPEED/1000);
		printf("  -m, --maxiterations=XX     do at most XX iterations\n");
		printf("  -j, --justoutput           only output [don't init kb or mouse]\n");
		printf("  -i, --ipc                  enable IPC\n");
		printf("  -p, --port=XX              use port XX for IPC (%i)\n",DEFPORT);
		printf("  -l, --loadavg              change speed based on load average\n");
		printf("  -n, --normalload=XX        a normal load average for your machine (%2.2f)\n",DEFLOAD);
		printf("  -h, --help                 get help [this screen]\n");
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
	jmlib->setWindowSize(aa_imgwidth(context),aa_imgheight(context));
	jmlib->startJuggle();

	aa_resizehandler(context, resizehandler);

	if(loadavg_flag) {
		/* If we're doing that thing where we care, then this
			can go down to as-low-as-possible priority */
		nice(19);
	}

	if(ipc_flag) {
		socket_fd = startlistening(port_num);
	}

	main_loop(max_iterations,delay,loadavg_flag,normal_load, socket_fd);

	if(socket_fd > 0) {
		stoplistening(socket_fd);
	}
	aa_close(context);

	delete jmlib;
	return 1;
}

