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

#ifndef COLORENTRY__HDR_
#define COLORENTRY__HDR_

#include "stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class JMColorEntry {
public:
  CBrush*  brush;
  CPen*    pen;
  COLORREF rgb;

  JMColorEntry(int r, int g, int b) {
    JMColorEntry(RGB(r, g, b));
    /*
    this->rgb = RGB(r, g, b);
    brush     = new CBrush(rgb);
    pen       = new CPen(PS_SOLID, 1, rgb);
    */
  }

  JMColorEntry(COLORREF rgb) {
    this->rgb = rgb;
    brush     = new CBrush(rgb);
    pen       = new CPen(PS_SOLID, 1, rgb);
  }

  ~JMColorEntry() {
    delete brush;
    delete pen;
  }

  void update(int r, int g, int b) {
    update(RGB(r, g, b));
  }

  void update(COLORREF rgb) {
    delete brush;
    delete pen;

    this->rgb = rgb;
    brush     = new CBrush(rgb);
    pen       = new CPen(PS_SOLID, 1, rgb);
  }
};

#endif
