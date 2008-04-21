/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
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

#include "js_validator.h"

int JSValidator::siteDigit(char s) {
  if (s >= '0' && s <= '9')
    return (int)(s - '0');
  else if (s >= 'A' && s <= 'Z')
    return (int)(s - 'A' + 10);
  else if (s >= 'a' && s <= 'z')
    return (int)(s - 'a' + 10 + 25);
  else
    return -1;
}

char JSValidator::siteChar(int c) {
  if (c >= 0 && c <= 9)
    return (char)(c + '0');
  else if (c >= 10 && c <= 35)
    return (char)(c - 10 + 'A');
  else if (c >= 36 && c <= 62)
    return (char)(c - 10 + 'a');
  else
    return -1;
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
bool JSValidator::validateVSS(char* site) {
  int i, x;
  int len = (int)strlen(site);
  char* ls;
  bool ret = true;

  if(1 == len && -1 != siteDigit(site[0])) return true;

  ls = new char[len];
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

/* examples of JuggleSaver patterns
 *
 * 5c, 5c3b4r - vanilla siteswap with object designation
 * 9b@(-2.5,0,-70,40)>(2.5,0,70)*2 1b@(1,0,10)>(-1,0,-10)
 */
bool JSValidator::validateJSPattern(char* patt) {
    int ExtSiteLen;
    EXT_SITE_INFO* pExtInfo = ParsePattern(patt, &ExtSiteLen);
    if (pExtInfo == NULL) return false;

    char* site = GetCurrentSite();
    if (!JSValidator::validateVSS(site)) return false;

    return true;
}

#ifdef VALIDATOR_STANDALONE
void test_js_validator() {
  char* tmp;

  validate("3c@(2,0)>(-2,2,-30,30)/(5,-10) 3c@(2,1)>(-2,1.5,-30,30)/(5,10)");
  tmp = GetCurrentSite();
  validate("3c@(2,0,90)>(-2,2,-90,30)/(5,-10) 3c@(2,1,90)>(-2,1.5,-90,30)/(5,10)");
  tmp = GetCurrentSite();
  validate("9c@(-2.5,0,-70,40)>(2.5,0,70)*2 1c@(1,0,10)>(-1,0,-10)");
  tmp = GetCurrentSite();
  validate("1c23c45c6060606060");
  tmp = GetCurrentSite();
  validate("3c0c3c456060606060");
  tmp = GetCurrentSite();
  validate("6c1c61c601");
  tmp = GetCurrentSite();
  validate("3c@(-0.5,0,30,-720)>(2,2.5,-10,-45)*-1 3c@(0.5,0,-30,-720)>(-2,2.5,10,-45)*-1");
  tmp = GetCurrentSite();
  //validate("3");
  //validate("645346346");
  //validate("5c");
  //validate("9b@(-2.5,0,-70,40)>(2.5,0,70)*2 1b@(1,0,10)>(-1,0,-10)");
  //validate("9@(3,4,5,4)>(3,2,3)");
  //validate("@9)<*}*");
  //validate("9(3,4,3,2)");
  validate("3b>");
  tmp = GetCurrentSite();
  validate("9@(1,2,3,4)>(1,2,3)* 4");
  tmp = GetCurrentSite();
  validate("9@(4,3)>(4)/(4,5,6)*5");
  tmp = GetCurrentSite();
  validate("9@foo");
  tmp = GetCurrentSite();
  validate("9@(3,4,5,6)(f,r,3)");
  tmp = GetCurrentSite();
  validate("9@(3,5,4)>(3,2,3)");
  tmp = GetCurrentSite();
  validate("(4,3,2,3,4)");
  tmp = GetCurrentSite();
}
#endif
