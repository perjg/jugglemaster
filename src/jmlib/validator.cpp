// 	$Id$	 

/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Groland 2000-2002, Gary Briggs 2003
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

#include "validator.h"

/*
 * Some notes on the implementation:
 *
 * - The site validator is entirely independent from the rest of jmlib.
 * - Notation:
 *   VSS  = Vanilla Siteswap (e.g. 534)
 *   MSS  = Multiplexed Siteswap (e.g. 25[75]51)
 *   SSS  = Sync Siteswap (e.g. (4,2x)(2x,4) and ([6x6],2)(2,[6x6]) )
 *          (The last example is a sync multiplexed site.)
 *
 * - The implementation is done in two parts:
 *   * Validate syntax
 *   * Validate site
 *   This is not the most effective methods (that would be to validate syntax
 *   on the fly when validating the site). This method is however more flexible
 *   in that it allows the syntax to be validated separately.
 *
 */

JML_INT32 JMSiteValidator::siteDigit(JML_INT8 s) {
  if (s >= '0' && s <= '9')
    return (JML_INT32)(s - '0');
  else if (s >= 'a' && s <= 'z')
    return (JML_INT32)(s - 'a' + 10);
  else if (s >= 'A' && s <= 'Z')
    return (JML_INT32)(s - 'A' + 10);
  else
    return -1;
}

JML_INT8 JMSiteValidator::siteChar(JML_INT32 c) {
  if (c >= 0 && c <= 9)
    return (JML_CHAR)(c + '0');
  else if (c >= 10 && c <= 35)
    return (JML_CHAR)(c - 10 + 'a');
  else
    return -1;
}

JML_INT32 JMSiteValidator::ballCount = -1;

// Returns the ball count of the last verified pattern, or -1 if the
// pattern was invalid or no pattern has been validated yet
JML_INT32 JMSiteValidator::getBallCount() {
  return ballCount;
}

// Scans a MSS for valid syntax
bool JMSiteValidator::scanMSS(JML_CHAR*& site) {
  //printf("scanMSS('%s')\n", site);

  site++;

  if (*site == ']') // empty multiplex throw
    return false;

  while (*site != ']') {
    //printf("scanning: %c\n", *site);
    
    if (*site == '\0') return false; // premature end of site

    if (siteDigit(*site) == -1) // only single site throws may appear
      return false;

    site++;
  }

  return true;
}

// Scans a SSS for valid syntax
bool JMSiteValidator::scanSSS(JML_CHAR*& site) {
  //printf("scanSSS('%s')\n", site);

  //printf("scanning: %c\n", *site);
  site++; // skip '('
 
  if (*site == '\0') return false; // premature end of site

  if (*site == '[') { // multiplex throw
    if (!scanMSS(site)) return false;
    //printf("multiplex ok -> now at %c\n", *site);
  }
  else if (siteDigit(*site) == -1) // single throw
    return false;
  
  //printf("scanning: %c\n", *site);
  site++; // skip to ', or 'x'
  //printf("now at %c\n", *site);


  // skip x if present
  if (*site == 'x') {
    //printf("scanning: %c\n", *site);
    site++;
  }

  if (*site != ',') // ',' expected
    return false;

  //printf("scanning: %c\n", *site);
  site++;
  if (*site == '\0') return false; // premature end of site

  if (*site == '[') { // multiplex throw
    if (!scanMSS(site)) return false;
  }
  else if (siteDigit(*site) == -1) // single throw
    return false;

  //printf("scanning: %c\n", *site);
  site++; // skip to ')' or x

  // skip x if present
  if (*site == 'x') {
    //printf("scanning: %c\n", *site);
    site++;
  }

  if (*site != ')') // ')' expected
    return false;

  //printf("scanning: %c\n", *site);
  site++;

  return true;
}

// Scans a site for valid syntax
bool JMSiteValidator::scanSite(JML_CHAR* site) {
  if (*site == '(') { // sync site
    while (*site == '(') {
      if (!scanSSS(site)) return false;
    }
    if (*site != '\0') // rubbish after site end
      return false;
  }
  else {
    while (*site != '\0') {
      //printf("scanning: %c\n", *site);
      if (*site == '[') {
	if (!scanMSS(site)) return false;
	site++;
	//printf("read: %c\n", *site);
      }
      else if (siteDigit(*site) == -1) // only single site throws may appear
	return false;
      else
	site++;
    }
  }
  return true;
}

bool JMSiteValidator::validateSiteSyntax(JML_CHAR* site) {
  // Check for valid characters
  for (JML_INT32 i = 0; i < (JML_INT32)strlen(site); i++) {
    if (site[i] != '[' && site[i] != ']' && site[i] != '(' &&
	site[i] != ')' && site[i] != ',') {
      if (siteDigit(site[i]) == -1)
	return false;
    }
  }

  return scanSite(site);
}

bool JMSiteValidator::validateSite(JML_CHAR* site) {
  if (site == 0 || *site == 0)
    return false;

  if (validateSiteSyntax(site)) {
    if (site[0] == '(') // SSS
      return validateSSS(site);
    else // VSS or MSS
      return validateMSS(site);
  }
  return false;
}

/*
 * Let VSS be the siteswap to validate.
 * Let LS be a sequence of 1's the length of VSS
 *
 * Algorithm:
 *
 * for (i = 0; i <= len(VSS)-1; i++)
 *   x = VSS(i);
 *   LS((i+x) mod len(VSS))--;
 *
 * VSS is valid iff LS is a sequence of zeros
 *
 * Note: The method validateMSS validates VSSs as well.
 */
bool JMSiteValidator::validateVSS(JML_CHAR* site) {
  JML_INT32 i, x;
  JML_INT32 len = (JML_INT32)strlen(site);
  JML_CHAR* ls;
  bool ret = true;

  if(1 == len && -1 != siteDigit(site[0])) return true;

  ls = new JML_CHAR[len];
  memset(ls, 1, len);

  for (i = 0; i < len; i++) {
    x = siteDigit(site[i]);
    ls[(i+x)%len]--;
  }
  
  for (i = 0; i < len; i++) {
    if (ls[i] != 0) {
      ret = false;
    }
  }

  delete ls;
  return ret;
}

bool JMSiteValidator::validateSSS(JML_CHAR* site) {
  //char* MSS = new char(len+1); will crash! why?
  JML_CHAR MSS[1024];

  if (transSyncMSS(MSS, site)) {
    bool ret = validateMSS(MSS);
    //delete MSS;
    return ret;
  }
  else
    return false;
}

/*
 * The throw capacity LS of a MSS is the amount of balls multiplexed for each throw.
 * e.g. for [54]24 the throw capacity is 211.
 *
 * A MSS can be validated by calculating the throw capacity and then verifying it using
 * the same algorithm as used for a VSS with the exception that the modulo must be calculated
 * from the throw count of the pattern (since this will be different from the length).
 *
 * This implementation can also be used to validate a VSS.
 *
 */
bool JMSiteValidator::validateMSS(JML_CHAR* site) {
  //MSS* mss = new MSS(site);
  JML_INT32 i, x;
  JML_INT32 len = (JML_INT32)strlen(site);
  JML_CHAR* ls;
  JML_INT32 lsLen = 0;
  JML_INT32 throwCount = 0;

  ballCount = 0;

  if(1 == len && -1 != siteDigit(site[0])) return true;

  ls = new JML_CHAR[len]; // Guaranteed to be long enough
  memset(ls, 0, len);
  
  for (i = 0; i < len; i++) {
    if (site[i] == '[') {
      while (site[++i] != ']') {
        ls[lsLen]++;
      }
      lsLen++;
      continue;
    }
    else {
      ls[lsLen++]++;
    }
  }

  /*
  for (i = 0; i < lsLen; i++) {
    printf("ls[%d] = %d\n", i, ls[i]);
  }
  */

  for (i = 0; i < len; i++) {
    if (site[i] == '[') {
      while (site[++i] != ']') {
        x = siteDigit(site[i]);
        ls[(throwCount+x)%lsLen]--;

        ballCount += x;

        /*
        printf("throw %d lands at position %d (%d+%d mod %d)\n", x, (throwCount+x)%lsLen,
               throwCount, x, lsLen);
        for (int j = 0; j < lsLen; j++)
          printf("ls[%d] = %d\n", j, ls[j]);
        */
      }
      throwCount++;
      continue;
    }
    else {
      x = siteDigit(site[i]);
      ls[(throwCount+x)%lsLen]--;
      ballCount += x;
      
      /*
      printf("throw %d lands at position %d (%d+%d mod %d)\n", x, (throwCount+x)%lsLen,
	     throwCount, x, lsLen);
      for (int j = 0; j < lsLen; j++)
	printf("ls[%d] = %d\n", j, ls[j]);
      */

      throwCount++;
    }
  }

  if (throwCount != 0)
    ballCount = ballCount / throwCount;

  for (i = 0; i < lsLen; i++) {
    if (ls[i] != 0) {
      delete ls;
      return false;
    }
  }

  delete ls;
  return true;
}

bool JMSiteValidator::transSyncMSS(JML_CHAR* MSS, JML_CHAR* SSS) {
  JML_INT32 i, x;
  JML_INT32 len = (JML_INT32)strlen(SSS);
  JML_INT32 add = 1;
  JML_INT32 mssPos = 0;

  memset(MSS, 0, len+1);

  for (i = 0; i < len; i++) {
    //printf("SSS[%d] = %c\n", i, SSS[i]);

    if (SSS[i] == '(' || SSS[i] == ',') { // start of sync throw or second in pair
      if (SSS[i] == '(') add = 1;
      else if (SSS[i] == ',') add = -1;

      i++; // skip '('
      
      if (SSS[i] == '[') { // multiplex in sync throw
	MSS[mssPos++] = '[';
	i++; // skip '['

	while (SSS[i] != ']') {
	  x = siteDigit(SSS[i]);
	  if (x % 2 != 0) return false;

	  if (SSS[i+1] == 'x') {
	    MSS[mssPos++] = siteChar(x+add);
	    i++; // skip 'x'
	  }
	  else
	    MSS[mssPos++] = siteChar(x);
	  
	  i++; // skip to next
	}

	MSS[mssPos++] = ']';
      }
      else { // single sync throw
	x = siteDigit(SSS[i]);
	if (x % 2 != 0) return false;

	if (SSS[i+1] == 'x') {
	  MSS[mssPos++] = siteChar(x+add);
	  i++; // skip 'x'
	}
	else
	  MSS[mssPos++] = siteChar(x);
      }
    }
  }

  return true;
}

#ifdef VALIDATOR_STANDALONE
void foo(char* site) {
  printf("Site '%s' is ", site);

  //if (v->validateSite(site))
  if (JMSiteValidator::validateSite(site))
    printf("valid\n");
  else
    printf("invalid\n");
}

int main(void) {
  foo("633");
  foo("4[45]3");
  foo("2[]3");
  foo("2[4]3");
  foo("2[345");
  foo("2]345");
  foo("(2x,4)(4x,2)");
  foo("(2x,4)(4x,2)7");
  foo("8(2x,4)(4x,2)");
  foo("(2x,4)6(4x,2)");
  foo("(2x(4),(4x,2)");
  foo("([2x4],4)([24x],2)");
  foo("([2x4]4,4)([24x],2)");
  foo("([2x4,4)([24x],2)");
  foo("44453");
  foo("6337");
  foo("b");
  foo("b1");
  foo("c1");
  foo("[54]24");
  foo("(6x,4)(4,6x)");
  foo("(7,4)(4,5)");
  foo("(4,2x)(2x,4)");
  foo("(7x,2)(2,5x)");
  foo("spam spam spam!\"#!#¤#\"#¤%");
  foo("");
  foo(NULL);
  foo("[543]3[45][27]2");
  foo("(4,4)(0,4x)(4,4)(4x,0)");
  foo("([yxy],2)(2,[22])(2,[yxy])([22],2)");
}
#endif
