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

#ifndef MEMDC__HDR_
#define MEMDC__HDR_

class JMMemDC : public CDC {
 private:       
  CBitmap    m_bitmap;        // Offscreen bitmap
  CBitmap*   m_oldBitmap;     // bitmap originally found in CMemDC
  CDC*       m_pDC;           // Saves CDC passed in constructor
  CRect      m_rect;          // Rectangle of drawing area.
  bool       m_bMemDC;        // true if CDC really is a Memory DC.
 public:
  
  JMMemDC(CDC* pDC, const CRect* pRect = NULL, COLORREF* bgcolor = NULL) : CDC() {
    ASSERT(pDC != NULL); 
    
    // Some initialization
    m_pDC = pDC;
    m_oldBitmap = NULL;
    
    // Get the rectangle to draw
    if (pRect == NULL) {
      pDC->GetClipBox(&m_rect);
    }
    else {
      m_rect = *pRect;
    }
    
    // Create a Memory DC
    CreateCompatibleDC(pDC);
    pDC->LPtoDP(&m_rect);
    
    m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), 
                                    m_rect.Height());
    m_oldBitmap = SelectObject(&m_bitmap);
    
    pDC->DPtoLP(&m_rect);
    
    // Fill background 
    if (bgcolor == NULL)
      FillSolidRect(m_rect, pDC->GetBkColor());
    else
      FillSolidRect(m_rect, *bgcolor);
  }

  ~JMMemDC() {          
    if (m_bMemDC) {
      // Copy the offscreen bitmap onto the screen.
      m_pDC->BitBlt(m_rect.left, m_rect.top, 
                    m_rect.Width(),  m_rect.Height(),
                    this, m_rect.left, m_rect.top, SRCCOPY);            
      
      //Swap back the original bitmap.
      SelectObject(m_oldBitmap);        
    } 
    else {
      // All we need to do is replace the DC with an illegal
      // value, this keeps us from accidentally deleting the 
      // handles associated with the CDC that was passed to 
      // the constructor.              
      m_hDC = m_hAttribDC = NULL;
    }       
  }
  
  // Allow usage as a pointer    
  JMMemDC* operator->() {
    return this;
  }       
  
  // Allow usage as a pointer    
  operator JMMemDC*() {
    return this;
  }
};

#endif
