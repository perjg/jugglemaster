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
#include "./patterns.h"


/*

From the default patterns.jm:
#GA=9.8         ;Gravity (0<f<=98) [meter/second^2]
#DR=0.50        ;Dwell ratio (0.10<=f<=0.90)
#HR=0.20        ;Height (0.01<=f<=1.00) [meter]
#SP=1.0         ;Speed ratio (0.1<=f<=2.0)
#BC=000         ;Background color  Red Green Blue (0<=R,G,B<=F)
#BP=1           ;Beep ON (n=0,1)
#HD=1           ;Hand ON (n=0,1)
#PD=1           ;Pattern ON (n=0,1)
#MR=0           ;Switch right and left (n=0,1)
*/

int ParsePatterns(FILE *input, 
	struct groups_t *groups, struct styles_t *styles) {

	float currhr,currdr;
	float currga,currsp;
	char currbc[4];
	int currbp,currhd,currpd,currmr;
	char buf[1024];
	char current_group[256];
	char current_style[256];
	char pattern_name[256];
	char pattern_data[256];
	int x1, x2, y1, y2;

	struct style_t *style = NULL;
	struct style_t *newstyle = NULL;
	struct pattern_t *patt = NULL;
	struct pattern_t *newpatt = NULL;
	struct pattern_group_t *group = NULL;
	struct pattern_group_t *newgroup = NULL;

	current_group[0] = '\0';
	strcpy(current_style, "Normal");
	pattern_data[0] = '\0';
	pattern_name[0] = '\0';

	if(input == NULL || styles == NULL || groups == NULL) {
		return(0);
	}

	while(!feof(input)) {
		fgets(buf, 1023, input);
		if(sscanf(buf, "#HR=%f",&currhr) == 1) {
			/* Height Ratio */
			if(currhr > HR_MAX) {
				currhr = HR_MAX;
			} else if (currhr < HR_MIN) {
				currhr = HR_MIN;
			}
		} else if(sscanf(buf, "#DR=%f",&currdr) == 1) {
			/* Dwell Ratio */
			if(currdr > DR_MAX) {
				currdr = DR_MAX;
			} else if (currdr < DR_MIN) {
				currdr = DR_MIN;
			}
		} else if(sscanf(buf, "#GA=%f",&currga) == 1) {
			/* Gravity */
		} else if(sscanf(buf, "#SP=%f",&currsp) == 1) {
			/* Speed */
		} else if(sscanf(buf, "#BC=%3s",currbc) == 1) {
			/* BG Color */
		} else if(sscanf(buf, "#BP=%i",&currbp) == 1) {
			/* Beep */
		} else if(sscanf(buf, "#HD=%i",&currhd) == 1) {
			/* Show Hand */
		} else if(sscanf(buf, "#PD=%i",&currpd) == 1) {
			/* Show Pattern */
		} else if(sscanf(buf, "#MR=%i",&currmr) == 1) {
			/* Mirror */
		} else if(sscanf(buf, "/[ %255[^]] ]",current_group) == 1) {
			/* New Group */
			strcpy(current_style, "Normal");
			currdr = DR_DEF;
			currhr = HR_DEF;
			newgroup = (struct pattern_group_t *)malloc(sizeof(struct pattern_group_t));
			if(groups->first == NULL) {
				groups->first = newgroup;
			} else if(group != NULL) {
				group->next = newgroup;
			}
			group = newgroup;
			group->name = (char *)malloc(strlen(current_group) + 1);
			strcpy(group->name, current_group);
			group->first_patt = NULL;
			group->next = NULL;
		} else if(sscanf(buf, "%%%255[^\n]",current_style) == 1) {
			/* New Style */
			if(Find_Style(styles,current_style) != NULL) continue;
			newstyle = (struct style_t *)malloc(sizeof(struct style_t));

			if(styles->first == NULL) {
				styles->first = newstyle;
			} else if(style != NULL) {
				style->next = newstyle;
			}
			style = newstyle;
			style->name = (char *)malloc(strlen(current_style) + 1);
			strncpy(style->name, current_style, strlen(current_style));
			style->length = 0;
			style->data = NULL;
		} else if(buf[0] == '{') {
			/* Style data */
			if(sscanf(buf, "{%d,%d}{%d,%d}",&x1,&y1,&x2,&y2) != 4) {
				if(sscanf(buf, "{%d,%d}",&x1,&y1) == 2) {
					/* Truncated Style data [extension by Chunky] */
					x2 = x1;
					y2 = y1;
				} else {
					continue;
				}
			}
			if(style == NULL) {
				/* Bail if we're not in a style. */
				continue;
			}
			style->length += 4;
			style->data = (int *)realloc((void *)style->data,
				style->length*sizeof(int));
			style->data[style->length-4] = x1;
			style->data[style->length-3] = y1;
			style->data[style->length-2] = x2;
			style->data[style->length-1] = y2;
			style->next = NULL;
		} else if (legal_pattern_first_char(buf[0])) {
			/* Pattern */
			if(sscanf(buf, "%255s %255[^\n]", pattern_data, pattern_name) != 2) {
				if(sscanf(buf, "%255[^\n]", pattern_data) == 1) {
					strncpy(pattern_name, pattern_data, 255);
				} else {
					continue;
				}
			}

			if(group == NULL) {
				/* Not currently in a group -
					bail, as we've noplace to put it */
				continue;
			}
			newpatt = (struct pattern_t *)malloc(sizeof(struct pattern_t));

			if(group->first_patt == NULL) {
				group->first_patt = newpatt;
			} else if(patt != NULL) {
				patt->next = newpatt;
			}
			patt = newpatt;
			patt->name = (char *)malloc(strlen(pattern_name) + 1);
			strncpy(patt->name, pattern_name, strlen(pattern_name));

			patt->data = (char *)malloc(strlen(pattern_data) + 1);
			strncpy(patt->data, pattern_data, strlen(pattern_data));

			patt->style = (char *)malloc(strlen(current_style) + 1);
			strncpy(patt->style, current_style, strlen(current_style));
			patt->hr = currhr;
			patt->dr = currdr;
			patt->next = NULL;
		}
	}
	return(1);
}


void FreeGroups(struct groups_t *groups) {
	struct pattern_t *patt = NULL;
	struct pattern_t *tmppatt = NULL;
	struct pattern_group_t *group = NULL;
	struct pattern_group_t *tmpgroup = NULL;

	group = groups->first;
	while(group) {
		patt = group->first_patt;
		while(patt) {
			if(patt->name != NULL) {
				free((void *)patt->name);
			}
			if(patt->data != NULL) {
				free((void *)patt->data);
			}
			if(patt->style != NULL) {
				free((void *)patt->style);
			}
			tmppatt = patt;
			patt = patt->next;
			free((void *)tmppatt);
		}
		if(group->name != NULL) {
			free((void *)group->name);
		}
		tmpgroup = group;
		group = group->next;
		free((void *)tmpgroup);
	}
	groups->first = NULL;
}

void FreeStyles(struct styles_t *styles) {
	struct style_t *style = NULL;
	struct style_t *tmpstyle = NULL;

	if(styles == NULL) return;
	style = styles->first;
	while(style) {
		if(style->name != NULL) {
			free((void *)style->name);
		}
		if(style->data != NULL) {
			free((void *)style->data);
		}
		tmpstyle = style;
		style = style->next;
		free((void *)tmpstyle);
	}
	styles->first = NULL;
}


int legal_pattern_first_char(char c) {
	return( c == '[' || c == '(' ||
			( c >= 'a' && c <= 'z' ) ||
			( c >= 'A' && c <= 'Z' ) ||
			( c >= '0' && c <= '9' ));
}


struct pattern_group_t *FirstGroup(struct groups_t *g) {
	return g->first;
}

struct pattern_group_t *NextGroup(struct pattern_group_t *g) {
	return g->next;
}

const char *Group_GetName(struct pattern_group_t *g) {
	return g->name;
}

struct pattern_t *Group_GetPatterns(struct pattern_group_t *g) {
	return g->first_patt;
}


struct pattern_t *NextPatt(struct pattern_t *p) {
	return p->next;
}

const char *Patt_GetName(struct pattern_t *p) {
	return p->name;
}

const char *Patt_GetData(struct pattern_t *p) {
	return p->data;
}

const char *Patt_GetStyle(struct pattern_t *p) {
	return p->style;
}


struct style_t *FirstStyle(struct styles_t *f) {
	return f->first;
}

struct style_t *NextStyle(struct style_t *p) {
	return p->next;
}

const char *Style_GetName(struct style_t *s) {
	return s->name;
}

int *Style_GetData(struct style_t *s) {
	return s->data;
}

unsigned int Style_GetLength(struct style_t *s) {
	return s->length;
}

struct style_t *Find_Style(styles_t *style_list, const char *name) {
	struct style_t *styles;
	
	if(style_list == NULL || name == NULL) return NULL;
	styles = style_list->first;
	while(styles) {
		if(styles->name != NULL) {
			if(strcasecmp(styles->name,name) == 0) return styles;
		}
		styles = styles->next;
	}
	return NULL;
}


#ifdef PATTERNS_STANDALONE

int main(int argc, char *argv[]) {
	FILE *f;
	const char *filename;
	struct groups_t g;
	struct styles_t s;
	struct pattern_group_t *groups;
	struct style_t *styles;
	struct pattern_t *patterns;
	int quiet = 0;

	s.first = NULL;
	g.first = NULL;

	if((argc == 3 && argv[1][0] != '-') || (argc < 2 || argc > 3)) {
		printf("Format: %s [-q[uiet]] <filename>\n", argv[0]);
		return 1;
	}

	if(strncmp(argv[1],"-q",2) == 0) {
		quiet = 1;
		filename = argv[2];
	} else {
		filename = argv[1];
	}
	if((f = fopen(filename,"r")) == NULL) {
		perror("Couldn't open file");
		return 1;
	}

	ParsePatterns(f,&g,&s);
	fclose(f);

	if(!quiet) {
		groups = FirstGroup(&g);
		while(groups) {
			printf("Group Name: %s\n",Group_GetName(groups));
	
			patterns = Group_GetPatterns(groups);
			while(patterns) {
				printf("  Pattern Name: %s\n",Patt_GetName(patterns));
				printf("   Style: %s\n",Patt_GetStyle(patterns));
				printf("   Data: %s\n",Patt_GetData(patterns));
				patterns = NextPatt(patterns);
			}

			groups = NextGroup(groups);
			printf("\n");
		};

		printf("\n\n\n");
		styles = FirstStyle(&s);
		if(styles == NULL) printf("Great Scott!\n");
		while(styles) {
			int i;
			printf("Style Name: %s\n",Style_GetName(styles));
			printf(" Length: %i\n",Style_GetLength(styles));
			printf(" Data:\n");
			for(i=0;i<(int)Style_GetLength(styles);i++) {
				if((i%4) == 0) {
					printf("  {");
				} else if ((i%4) == 2) {
					printf("}{");
				}
				printf(" %i",Style_GetData(styles)[i]);
	
				if ((i%4) == 0 || (i%4) == 2) {
					printf(", ");
				} else if ((i%4) == 3) {
					printf("}\n");
				}
			}
			styles = NextStyle(styles);
			printf("\n");
		}
	}

	//FreeGroups(&g);
	FreeStyles(&s);

	return 0;
}

#endif
