/*
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2000-2008, 2025-2026, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * JuggleSaver support based on Juggler3D
 * Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
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
#include "../validator.h"

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

  ls = new char[len+1];
  memset(ls, 1, len);

  for (i = 0; i < len; i++) {
    x = siteDigit(site[i]);

		if (x == -1) {
			delete ls;
			return false;
		}

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
    PASSING_LAYOUT Layout;
    EXT_SITE_INFO* pExtInfo = ParsePatternEx(patt, &ExtSiteLen, &Layout);
    if (pExtInfo == NULL) return false;

    // With several jugglers, or hands throwing together, a throw must outlast
    // the carry, and a pass must stay in the air at least as long as it is carried
    int J = Layout.Jugglers, hands = 2 * J;
    bool shifted = false, ok = true;
    for (int h = 0; h < hands; h++) shifted = shifted || Layout.HandShift[h] != 0;
    if (J > 1 || shifted) {
        float carry = CARRY_TIME * J;
        int a = ExtSiteLen, b = hands;
        while (b != 0) { int t = a % b; a = b; b = t; }
        int period = ExtSiteLen / a * hands;
        for (int s = 0; s < period; s++) {
            int w = pExtInfo[s % ExtSiteLen].Weight;
            if (w == 0) continue;
            int flight = w + Layout.HandShift[(s + w) % hands] - Layout.HandShift[s % hands];
            bool pass = (s % hands) % J != ((s + w) % hands) % J;
            if (flight <= carry || (pass && flight < 2.0f * carry)) ok = false;
        }
    }
    free(pExtInfo);

    char* site = GetCurrentSite();
    ok = ok && JSValidator::validateVSS(site);
    free(site);
    return ok;
}

bool JSValidator::isPlainSolo(const char* patt) {
    int ExtSiteLen;
    PASSING_LAYOUT Layout;
    EXT_SITE_INFO* pExtInfo = ParsePatternEx(patt, &ExtSiteLen, &Layout);
    if (pExtInfo == NULL) return false;
    free(pExtInfo);
    bool shifted = false;
    for (int h = 0; h < JS_MAX_HANDS; h++) shifted = shifted || Layout.HandShift[h] != 0;
    return Layout.Jugglers == 1 && !shifted;
}

int JSValidator::jugglerCount(const char* patt) {
    int ExtSiteLen;
    PASSING_LAYOUT Layout;
    EXT_SITE_INFO* pExtInfo = ParsePatternEx(patt, &ExtSiteLen, &Layout);
    if (pExtInfo == NULL) return 0;
    free(pExtInfo);
    return Layout.Jugglers;
}

bool JSValidator::isJSOnly(char* patt) {
  bool valid_js = validateJSPattern(patt);
  bool valid_jm = JMSiteValidator::validateSite(patt);

  if (valid_js && !valid_jm)
    return true;
  return false;
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
