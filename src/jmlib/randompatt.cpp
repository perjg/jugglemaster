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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "./randompatt.h"

/* All maths in this file courtesy of Aidan Burns,
http://www.geocities.com/aidanjburns/contents.html */

/*
Transforming Asynchronous Patterns

Seed a buffer of pattern_length with throws of height num_balls, then:

Rule 1  the Downright Rule: to swap any two throws in a pattern, the
throw that moves to the right decreases with each step, and the throw
that moves to the left increases with each step. A throw may not move
if it would decrease below zero.

Rule 2  the Circular Rule: a throw may be moved from the front to the
back of a pattern.

*/

JML_CHAR *jm_rand(JML_INT8 numballs, JML_INT8 pattlen,
        JML_INT8 transformations, JML_BOOL synchronous) {
	return jm_rand_async(numballs, pattlen, transformations);
}

JML_CHAR *jm_rand_async(JML_INT8 numballs, JML_INT8 pattlen,
		JML_INT8 transformations) {
	int i,j; /* Swaps, failed swaps */
	int *buf;
	JML_CHAR *returnvalue;

	if(numballs < 0 || numballs > 35 || pattlen == 0) {
		/* This is a silly pattern. Let's not generate it */
		return NULL;
	}

	buf = (int *)malloc(sizeof(int) * pattlen);

	for(i = 0; i < pattlen; i++) {
		buf[i]=numballs;
	}

	i = 0;
	j = 0;

	while(i < transformations && j < transformations * 5) {
			/* the * 5 is a silly get-out in case we're not
			getting stuff swapped */
		int distance, first;
		int tmp;

		first = rand()%(pattlen-1);
		distance = rand()%(pattlen-1);

		if((buf[first] - distance >= 0) &&
			(buf[(first+distance)%pattlen] + distance >= 0) &&
			(buf[first] - distance <= 35) &&
			(buf[(first+distance)%pattlen] + distance <= 35)) {

			tmp = buf[first] - distance;
			buf[first] = buf[(first+distance)%pattlen] + distance;
			buf[(first+distance)%pattlen] = tmp;
			i++;
		} else if((buf[first] + (numballs - distance) >= 0) &&
			(buf[(first+distance)%pattlen] - (numballs - distance) >= 0) &&
			(buf[first] + (numballs - distance) <= 35) &&
			(buf[(first+distance)%pattlen] - (numballs - distance) <= 35)) {

			tmp = buf[first] + (numballs - distance);
			buf[first] = buf[(first+distance)%pattlen] - (numballs - distance);
			buf[(first+distance)%pattlen] = tmp;
			i++;
		} else {
			j++;
		}
	}
	returnvalue = (JML_CHAR *)malloc(sizeof(JML_CHAR) * pattlen);

	for(i = 0; i < pattlen; i++) {
		if(buf[i] >= 0 && buf[i] <= 9) {
			returnvalue[i] = '0' + buf[i];
		} else if(buf[i] > 9 && buf[i] <= 35) {
			returnvalue[i] = 'a' + buf[i] - 10;
		} else {
			returnvalue[i] = 0;
		}
	}
	return returnvalue;
}

#ifdef RAND_STANDALONE
int main(int argc, char *argv[]) {
	int numballs, pattlen, transformations;
	JML_CHAR *pattern;
	char line[100];

	FILE *inputfile;

	if(argc < 2 || strcmp(argv[1],"-h") == 0) {
		printf("Usage: %s <inputfile>\n"
			" Where inputfile contains lines of the form:\n"
			" numballs=<integer> pattlen=<integer> transformations=<integer>\n",
			argv[0]);
		exit(0);
	}

	inputfile = fopen(argv[1], "r");
	if(inputfile == NULL) {
		printf("Couldn't open input file\n");
	}
	
	while(NULL != fgets(line, sizeof(line), inputfile)) {
		if(3 == sscanf(line,
			"numballs=%i pattlen=%i transformations=%i",&numballs,
			&pattlen, &transformations)) {

			pattern = jm_rand(numballs, pattlen, transformations, 0);
			printf("Balls: %i, Pattlen: %i, Transformations: %i\n",
				numballs, pattlen, transformations);
			if(pattern != NULL) {
				printf("Pattern: %s\n",pattern);
				free(pattern);
			}
		}
	}
	return 0;
}
#endif
