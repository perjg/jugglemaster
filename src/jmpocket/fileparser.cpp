/*
 * JMPocket - JuggleMaster for Pocket PC
 * Version 1.03
 * (C) Per Johan Groland 2002-2004
 *
 * Using JMLib 2.0 (C) Per Johan Groland 2000-2002
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPocket is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 */

#include "patternloader.h"

// Dirty macros :)
#define CHECK_INT if (numbuf.numType != NUMBER_INT) {\
error("Style data must be an integer\n");\
return;}

#define CHECK_LBRACE if (strcmp(tokbuf, "{") != 0){\
error("'{' expected\n");\
return;}

#define CHECK_RBRACE if (strcmp(tokbuf, "}") != 0){\
error("'}' expected\n");\
return;}

#define CHECK_COMMA if (strcmp(tokbuf, ",") != 0){\
error("',' expected\n");\
return;}

#define NUMBER_INT 0
#define NUMBER_FLOAT 1

JMFileParser::JMFileParser(FILE* file, ERROR_CALLBACK* _errCb = NULL) {
  lineno = 1;
  setFile(file);
  errCb = _errCb;
  errorCount = warningCount = 0;
}

JMFileParser::~JMFileParser() {
  
}

char* JMFileParser::sfgets(char* s, int size, FILE* stream) {
  int pos;
  s = fgets(s, size, stream);
  
  if (s == NULL)
    return NULL;
  
  if (s[0] == '\n')
    return s;
  
  if (strlen(s) == 0)
    s[0] = '\0';
  else {
    pos = strlen(s)-1;
    
    if (s[pos] != '\n')
      error("Internal compiler error (sfgets)\n");
    
    s[pos] = '\0';
  }
  
  return s;
}

// main parser routine
void  JMFileParser::parse(void) {
  char* ptr;
  int i = 0;
  
  lineno = 1;
  
  pptext = new char[1024];
  tokbuf = new char[1024];
  
  while ((pptext = sfgets(pptext, 1023, file)) != NULL) {
    // strip off leading and trailing whitespace
    //pptext = strim(pptext);
    
    // last line?
    if (strcmp(pptext, "/----------------- end ------------------") == 0)
      continue;
    
    // look for errors in characters
    ptr = pptext;
    while(*(ptr++) != '\0') {
      // look for comments after command
      if (i!= 0 && *ptr == ';')
        *ptr = '\0';
      
      // make sure we're dealing with 7-bit ascii
      if (!isascii(*ptr)) {
        error("Found illegal character '%c'\n", *ptr);
        continue;
      }
      
      i++;
    }
    
    // the first character determines what type of line we're dealing with
    switch (pptext[0]) {
    case '\n':
      break;
    case ';': // empty line or comment
      break;
    case '#': // parameter
      parseParam(pptext);
      break;
    case '%': // style
      parseStyle(pptext);
      break;
    case '{': // style data
      parseStyleData(pptext);
      break;
    case '/': // category
      parseCategory(pptext);
      break;
    default:
      parsePattern(pptext);
      break;
    }
    
    lineno++;
  }
}

void JMFileParser::parseParam(char* buf) {
  char* ptr;
  
  ptr = buf+1;
  ptr = nextToken(tokbuf, ptr);
  
  if (strlen(tokbuf) != 2) {
    error("Incorrect parameter\n");
    return;
  }
  
  if (!(buf[3] == '=')) {
    error("error in parameter\n");
    return;
  }
  ptr = &buf[4];
  
  // next, we have a float or an integer
  ptr = scanNumber(&numbuf, ptr);
  
  // check for valid parameter and set
  if (strcmp(tokbuf, "GA") == 0 || strcmp(tokbuf, "ga") == 0) {
    if (!numbuf.numType == NUMBER_FLOAT)
      error("Floating point number expected\n");
    else
      pl->setGA(numbuf.val.floatval);
  }
  else if (strcmp(tokbuf, "DR") == 0 || strcmp(tokbuf, "dr") == 0) {
    if (!numbuf.numType == NUMBER_FLOAT)
      error("Floating point number expected\n");
    else {
      pl->setDR(numbuf.val.floatval);
    }
  }
  else if (strcmp(tokbuf, "HR") == 0 || strcmp(tokbuf, "hr") == 0) {
    if (!numbuf.numType == NUMBER_FLOAT)
      error("Floating point number expected\n");
    else
      pl->setHR(numbuf.val.floatval);
  }
  else if (strcmp(tokbuf, "SP") == 0 || strcmp(tokbuf, "sp") == 0) {
    if (!numbuf.numType == NUMBER_FLOAT)
      error("Floating point number expected\n");
    else
      pl->setSP(numbuf.val.floatval);
  }
  else if (strcmp(tokbuf, "BC") == 0 || strcmp(tokbuf, "bc") == 0) {
    if (!numbuf.numType == NUMBER_INT)
      error("Integer expected\n");
    // bc is ignored
  }
  else if (strcmp(tokbuf, "BP") == 0 || strcmp(tokbuf, "bp") == 0) {
    if (!numbuf.numType == NUMBER_INT)
      error("Integer expected\n");
    else
      pl->setBP(numbuf.val.intval);
  }
  else if (strcmp(tokbuf, "HD") == 0 || strcmp(tokbuf, "hd") == 0) {
    if (!numbuf.numType == NUMBER_INT)
      error("Integer expected\n");
    else
      pl->setHD(numbuf.val.intval);
  }
  else if (strcmp(tokbuf, "PD") == 0 || strcmp(tokbuf, "pd") == 0) {
    if (!numbuf.numType == NUMBER_INT)
      error("Integer expected\n");
    else
      pl->setPD(numbuf.val.intval);
  }
  else if (strcmp(tokbuf, "MR") == 0 || strcmp(tokbuf, "mr") == 0) {
    if (!numbuf.numType == NUMBER_INT)
      error("Integer expected\n");
    else
      pl->setMR(numbuf.val.intval);
  }
  else
    error("Incorrect parameter\n");
}

// style: % name
void JMFileParser::parseStyle(char* buf) {
  /* the name is everything from character 1 */
  pl->setStyle(buf+1);
}

// style data: {number,number}{number,number}
void JMFileParser::parseStyleData(char* buf) {
  int s1, s2, s3, s4;
  
  char* ptr = buf+1;
  
  /* read number */
  ptr = scanNumber(&numbuf, ptr);
  CHECK_INT;
  s1 = numbuf.val.intval;
  
  /* read , */
  ptr = nextToken(tokbuf, ptr);
  CHECK_COMMA;
  
  /* read number */
  ptr = scanNumber(&numbuf, ptr);
  CHECK_INT;
  s2 = numbuf.val.intval;
  
  /* read } */
  ptr = nextToken(tokbuf, ptr);
  CHECK_RBRACE;
  
  /* read { */
  ptr = nextToken(tokbuf, ptr);
  CHECK_LBRACE;
  
  /* read number */
  ptr = scanNumber(&numbuf, ptr);
  CHECK_INT;
  s3 = numbuf.val.intval;
  
  /* read , */
  ptr = nextToken(tokbuf, ptr);
  CHECK_COMMA;
  
  /* read number */
  ptr = scanNumber(&numbuf, ptr);
  CHECK_INT;
  s4 = numbuf.val.intval;
  
  /* read } */
  ptr = nextToken(tokbuf, ptr);
  CHECK_RBRACE;
  
  pl->addToStyle(s1, s2, s3, s4);
}

// category: /[ name ] or / name
void JMFileParser::parseCategory(char* buf) {
  int cbracfound = 0;
  char* ptr = buf+1;
  
  // check for [
  if (*ptr == '[') {
    while (*(ptr++) != '\0') {
      if (*ptr == ']') {
	cbracfound = 1;
	*ptr = '\0';
	break;
      }
    }
    if (!cbracfound)
      warning("missing ']' in category\n");
    ptr = buf+2;
  }
  
  // everything up to nullbyte is the name
  pl->setCategory(ptr);
}

// pattern: site name or site
void JMFileParser::parsePattern(char* buf) {
  char* ptr = buf;
  // the site is everything up to the first
  // blank or everything if no blank found
  
  //printf("pptext[0] = '%c' %d\n", buf[0], buf[0]);
  
  while (*(ptr++) != '\0') {
    // found a blank, which means that the rest of the line is a name
    if (isblank(*ptr)) {
      *ptr = '\0';
      if (isblank(*(ptr+1))) ptr++;
      // read remaining blanks
      while (*(ptr++) != '\0')
	if (!isblank(*ptr))
	  break;
      
      pl->addPattern(buf, ptr);
      return;
    }
  }
  
  // if we get here, no space was found, which means
  // the pattern is unnamed, so we use the site for a name
  pl->addPattern(buf, buf);
}

/* utility functions */

// gets the next token, returns position in buffer,
// tok_dest is set to the token, tok_dest must be
// at least strlen(buf) long
char* JMFileParser::nextToken(char* tok_dest, char* buf) {
  //fprintf(stderr, "Scanning '%s'\n", buf);
  
  // skip whitespace
  if (isblank(*buf))
    while (*(buf++) != '\0')
      if (!isblank(*buf))
	break;
  
  // alphanumeric characters are appended to tok_dest
  if (isalnum(*buf))
    while (isalnum(*buf) && *buf != '\0')
      *(tok_dest++) = *(buf++);
  // any other character is returned 
  else
    *(tok_dest++) = *(buf++);
  
  *tok_dest = '\0';
  
  return buf;
}

// scans for a number, returned in num_dest.
// returns the position in the buffer
char* JMFileParser::scanNumber(number* num_dest, char* buf) {
  char nbuf[1024];
  char* pbuf = nbuf;
  int type_is_float = 0;
  
  // skip whitespace
  if (isblank(*buf))
    while (*(buf++) != '\0')
      if (!isblank(*buf))
	break;
  
  // check for negative
  if (*buf == '-')
    *(pbuf++) = *(buf++);
  
  // must read at least one digit
  if (!isdigit(*buf))
    error("number expected ('%c', %d)\n");
  
  // scan number
  while (*buf != '\0' && *buf != '\n') {
    if (isdigit(*buf))
      *(pbuf++) = *buf;
    else if (*buf == '.') {
      type_is_float = 1;
      *(pbuf++) = *buf;
    }
    else if (isblank(*buf))
      break;
    else {
      break;
    }
    
    buf++;
  }
  
  *pbuf = '\0';
  
  // scanned a float
  if (type_is_float) {
    num_dest->numType = NUMBER_FLOAT;
    sscanf(nbuf, "%f", &(num_dest->val.floatval));    
  }
  // scanned an integer
  else {
    num_dest->numType = NUMBER_INT;
    sscanf(nbuf, "%d", &(num_dest->val.intval));
  }
  
  return buf;
}

#ifdef _WIN32
int JMFileParser::isblank(int c) {
  if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
    return true;
  else
    return false;
}
#endif

#if _WIN32_WCE < 300
int JMFileParser::isascii(int c) {
  if (c >= 0 && c <= 127)
    return true;
  else
    return false;
}

int JMFileParser::isalnum(int c) {
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9'))
    return true;
  else
    return false;
}

int JMFileParser::isdigit(int c) {
  if (c >= '0' && c <= '9')
    return true;
  else
    return false;
}

#endif

void JMFileParser::errPrint(const char* fmt, va_list ap) {
  if (errCb) {
    char buf[512];
    vsprintf(buf, fmt, ap);
    errCb(buf);
  }
}

void JMFileParser::error(const char* fmt, ...) {
  va_list ap;
  
  errorCount++;
  
  va_start(ap, fmt);
  errPrint(fmt, ap);
  va_end(ap);
}

/* Writes a warning message to stderr */
void JMFileParser::warning(const char* fmt, ...) {
  va_list ap;
  
  warningCount++;
  
  va_start(ap, fmt);
  errPrint(fmt, ap);
  va_end(ap);
}
