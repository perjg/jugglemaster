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

#ifdef RAND_STANDALONE
#include "./validator.h"
#endif

/* All maths in this file courtesy of Aidan Burns,
http://www.geocities.com/aidanjburns/ */

/*

ASYNCHRONOUS PATTERNS

Seed a buffer of pattern_length with throws of height num_balls, then:

Rule 1,  The Downright Rule: to swap any two throws in a pattern, the
throw that moves to the right decreases with each step, and the throw
that moves to the left increases with each step. A throw may not move
if it would decrease below zero.

Rule 2,  The Circular Rule: a throw may be moved from the front to the
back of a pattern.

Implemenation note: Rule 2 is implied by doing all swapping arithmetic
   modulo patt_len.


SYNCHRONOUS PATTERNS

Seed a buffer of pattern_length with pairs of throws of height num_balls,
then:

Rule 3,  The Handedness Rule: when you change a throws handedness, you
change its crossedness.

Rule 4,  The Two Up, Two Down Rule: to swap any two throws in a synchronous
pattern, the throw that moves to the right decreases by two with each
bracket, and the throw that moves to the left increases by two with each
bracket. A throw may not move if it would decrease below zero.

Rule 2 is also still used, and is also still implied by the implemenation


MULTIPLEX PATTERNS

Multiplex patterns can be created by combining same-length patterns of
the either other two flavers. Example here is easier than anything else:

  5  0  4 +    ( 3 balls )
  4  2  0      ( 2 balls )
-----------
[54] 2  4      ( 5 balls )


(  4    ,   4   ) +  ( 4 balls )
(  6x   ,   6x  )    ( 6 balls )
-----------------
( [46x] , [46x] )    ( 10 balls )
*/

JML_CHAR *jm_rand(JML_INT8 numballs, JML_INT8 pattlen,
		JML_INT8 transformations,
		JML_BOOL synchronous, JML_INT8 multiplex) {
	if(synchronous) {
		return jm_rand_sync(numballs, (pattlen + 1)/2, transformations);
	} else {
		return jm_rand_async(numballs, pattlen, transformations);
	}
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

		first = rand()%pattlen;
		distance = rand()%pattlen;

		if((buf[first] - distance >= 0) &&
			(buf[(first+distance)%pattlen] + distance >= 0) &&
			(buf[first] - distance <= 35) &&
			(buf[(first+distance)%pattlen] + distance <= 35)) {

		/* RULE 1 */

			tmp = buf[first] - distance;
			buf[first] = buf[(first+distance)%pattlen] + distance;
			buf[(first+distance)%pattlen] = tmp;
			i++;
		} else {
			j++;
		}
	}
	returnvalue = (JML_CHAR *)malloc(sizeof(JML_CHAR) * pattlen + 1);
	memset(returnvalue, '\0', sizeof(JML_CHAR) * pattlen + 1);

	for(i = 0; i < pattlen; i++) {
		if(buf[i] >= 0 && buf[i] <= 9) {
			returnvalue[i] = '0' + buf[i];
		} else if(buf[i] > 9 && buf[i] <= 35) {
			returnvalue[i] = 'a' + buf[i] - 10;
		} else {
			returnvalue[i] = 0;
		}
	}
	free(buf);
	return returnvalue;
}


JML_CHAR *jm_rand_sync(JML_INT8 numballs, JML_INT8 pattlen,
		JML_INT8 transformations) {
	int i,j; /* Swaps, failed swaps */
	int *left, *right; /* Left and right hands. left[1] and right[1]
					are thrown at the same instant */
	int *leftcross, *rightcross;
					/* buffers that say either a throw is, or isn't, crossing.
					if leftcross[1] is set, then left[1] must be a crossing
					throw. */
	int leftcrosses = 0;
	int rightcrosses = 0;
	int totalcrosses = 0;
	JML_CHAR *returnvalue;
	JML_CHAR *returntmp;

	if(numballs < 0 || numballs > 35 || pattlen == 0) {
		/* This is a silly pattern. Let's not generate it */
		return NULL;
		
	}

	if(numballs & 1 && pattlen & 1) {
		pattlen++;
	}

	left = (int *)malloc(sizeof(int) * pattlen);
	right = (int *)malloc(sizeof(int) * pattlen);
	leftcross = (int *)malloc(sizeof(int) * pattlen);
	rightcross = (int *)malloc(sizeof(int) * pattlen);

	/* To seed the pattern:
		Even numbers of balls go (n,n)<rinse, repeat>
		Odd numbers of balls go ((2n-2)x,2)(2,(2n-2)x)<rinse, repeat> */
	for(i = 0; i < pattlen; i++) {
		if(0 == (numballs & 1)) {
			left[i] = numballs;
			right[i] = numballs;
			leftcross[i] = 0;
			rightcross[i] = 0;
		} else {
			if(i&1) {
				left[i] = 2 * (numballs - 1);
				right[i] = 2;
				leftcross[i] = 1;
				rightcross[i] = 0;
			} else {
				left[i] = 2;
				right[i] = 2 * (numballs - 1);
				leftcross[i] = 0;
				rightcross[i] = 1;
			}
		}
	}

	i = 0;
	j = 0;

	while(i < transformations && j < transformations * 5) {
			/* the * 5 is a silly get-out in case we're not
			getting stuff swapped */

		/* Make rule3 considerably less likely than rule4 */
		if(rand()%6 == 1) {

			/* RULE 3 */

			int pos;
			int tmp;

			pos = rand()%pattlen;

			if(leftcross[pos] != 0) leftcross[pos] = 0;
			else leftcross[pos] = 1;

			if(rightcross[pos] != 0) rightcross[pos] = 0;
			else rightcross[pos] = 1;

			tmp = left[pos];
			left[pos] = right[pos];
			right[pos] = tmp;

			i++;

		} else {

			/* RULE 4 */

			int distance, first;
			int tmp;
			int *switchbuffer;
			int *switchbuffercross;

			first = rand()%pattlen;
			distance = rand()%pattlen;

			if(rand()%1) {
				switchbuffer = left;
				switchbuffercross = leftcross;
			} else {
				switchbuffer = right;
				switchbuffercross = rightcross;
			}

			if((switchbuffer[first] - distance*2 >= 0) &&
				(switchbuffer[(first+distance)%pattlen] + distance*2 >= 0) &&
				(switchbuffer[first] - distance*2 <= 35) &&
				(switchbuffer[(first+distance)%pattlen] + distance*2 <= 35)) {

				tmp = switchbuffer[first] - distance*2;
				switchbuffer[first] = switchbuffer[(first+distance)%pattlen] + distance*2;
				switchbuffer[(first+distance)%pattlen] = tmp;

				tmp = switchbuffercross[first];
				switchbuffercross[first] = switchbuffercross[(first+distance)%pattlen];
				switchbuffercross[(first+distance)%pattlen] = tmp;
				i++;
			} else {
				j++;
			}
		}
	}

	/* Format: (something,something)...
		where something can have an x on the end */

	for(i=0;i<pattlen;i++) {
		if(leftcross[i]) leftcrosses++;
		if(rightcross[i]) rightcrosses++;
	}

	totalcrosses = leftcrosses + rightcrosses;
	returnvalue = (JML_CHAR *)malloc(sizeof(JML_CHAR) * pattlen * 2 +
			sizeof(JML_CHAR) * pattlen * 3 +
			sizeof(JML_CHAR) * totalcrosses + 1);
	memset(returnvalue, '\0', sizeof(JML_CHAR) * pattlen * 2 + /* Numbers */
			sizeof(JML_CHAR) * pattlen * 3 + /* Brackets and commas */
			sizeof(JML_CHAR) * totalcrosses + 1); /* x's */

	for(returntmp=returnvalue,i=0;i<pattlen;i++) {
		*returntmp++ = '(';

		if(left[i] >= 0 && left[i] <= 9) {
			*returntmp++ = '0' + left[i];
		} else if(left[i] > 9 && left[i] <= 35) {
			*returntmp++ = 'a' + left[i] - 10;
		} else {
			*returntmp++ = '0';
		}

		if(leftcross[i]) {
			*returntmp++ = 'x';
		}

		*returntmp++ = ',';

		if(right[i] >= 0 && right[i] <= 9) {
			*returntmp++ = '0' + right[i];
		} else if(right[i] > 9 && right[i] <= 35) {
			*returntmp++ = 'a' + right[i] - 10;
		} else {
			*returntmp++ = '0';
		}

		if(rightcross[i]) {
			*returntmp++ = 'x';
		}

		*returntmp++ = ')';
	}

	free(left);
	free(right);
	free(leftcross);
	free(rightcross);

	return returnvalue;
}

#ifdef RAND_STANDALONE
int main(int argc, char *argv[]) {
	int numballs, pattlen, transformations, sync;
	JML_CHAR *pattern;
	JMSiteValidator val;

	/* FILE *inputfile;
	char line[100];

	if(argc < 2 || strcmp(argv[1],"-h") == 0) {
		printf("Usage: %s <inputfile>\n"
			" Where inputfile contains lines of the form:\n"
			" numballs=<integer> pattlen=<integer> transformations=<integer> sync=<0|1>\n",
			argv[0]);
		exit(0);
	}

	inputfile = fopen(argv[1], "r");
	if(inputfile == NULL) {
		printf("Couldn't open input file\n");
	}
	
	while(NULL != fgets(line, sizeof(line), inputfile)) {
		if(4 == sscanf(line,
			"numballs=%i pattlen=%i transformations=%i sync=%i",&numballs,
			&pattlen, &transformations, &sync)) {

			pattern = jm_rand(numballs, pattlen, transformations, sync);
			printf("Balls: %i, Pattlen: %i, Transformations: %i Sync: %i\n",
				numballs, pattlen, transformations, sync);
			if(pattern != NULL) {
				printf("Pattern: %s\n",pattern);
				free(pattern);
			}
		}
	} */

	for(sync = 0; sync <= 1; sync++) {
		for(numballs = 0; numballs<=35; numballs++) {
			for(pattlen = 1; pattlen < 50; pattlen++) {
				for(transformations = 0; transformations < 20; transformations ++) {
					int showpat = 0;
					pattern = jm_rand(numballs, pattlen,
								transformations, sync, 0);
					if(pattern != NULL) {
						if(val.validateSite(pattern)) {
							printf("SUCCESS");
						} else {
							printf("FAIL");
							showpat++;
						}
					} else {
						printf("NULL RETURNED ");
					}
					printf(" - balls: %i len: %i, tran: %i, sync: %i\n",
						numballs, pattlen, transformations, sync);
					if(showpat) {
						printf("     %s\n", pattern);
					}
					if(pattern != NULL) {
						free(pattern);
						pattern = NULL;
					}
				}
			}
		}
	}
	return 0;
}
#endif
