// 	$Id$	


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

#include "jmlib.h"

JMLib::JMLib() {
  styledata = (JML_CHAR*)NULL;
  initialize();
  use_cpp_callback = 0;
  cb = (ERROR_CALLBACK*)NULL;
  mCallback = NULL;
}

JMLib::JMLib(ERROR_CALLBACK* _cb) {
  styledata = (JML_CHAR*)NULL;
  initialize();
  use_cpp_callback = 0;
  cb = _cb;
  mCallback = NULL;
}

JMLib::~JMLib() {
  shutdown();
}

void JMLib::initialize(void) {
  JML_CHAR *possible_styles_list[] = {
	"Normal",
	"Reverse",
	"Shower",
	"Mills Mess",
	"Center",
	"Windmill",
	"Random" };

  int i;

  num_possible_styles = (int)(sizeof(possible_styles_list)/sizeof(possible_styles_list[0]));
  possible_styles = (JML_CHAR **)malloc((size_t)num_possible_styles*sizeof(JML_CHAR *));
  for(i=0;i<num_possible_styles;i++) {
	possible_styles[i]=(JML_CHAR *)malloc(strlen(possible_styles_list[i])+1);
	strncpy(possible_styles[i],possible_styles_list[i],strlen(possible_styles_list[i])+1);
  }

  // Set default values
  ga = 9.8F;
  dwell_ratio = 0.5F;
  height_ratio = 0.20F;
  mirror = 0;
  speed = SPEED_DEF;
  syn = 0;
  hand_on = 1;
  hand_x = 0;
  hand_y = 0;
  // frameSkip = FS_DEF;
  
  smode = 50.0;
  fpu = 1;
  
  status = ST_NONE;
  
#ifdef _WIN32_WCE
  CTime t;
  srand((unsigned int)t.GetTime());
#else
  srand(time(NULL));
#endif

  if (styledata == NULL)
    styledata = new JML_CHAR[STYLEMAX*4];
  
  setWindowSize(480, 400);
  setPatternDefault();
  setStyleDefault();
}

void JMLib::shutdown(void) {
  int i;
  if (styledata != NULL) {
    delete styledata;
    styledata = (JML_CHAR*)NULL;
  }
  for(i=0;i<num_possible_styles;i++) {
    free((void *)possible_styles[i]);
  }
  free((void *)possible_styles);
}

void JMLib::setErrorCallback(void *aUData, void (*aCallback)
			(void *, JML_CHAR *)) {
  mUData = aUData;
  use_cpp_callback = 1;
  mCallback = aCallback;
}

void JMLib::setErrorCallback(ERROR_CALLBACK* _cb) {
  cb = _cb;
}

void JMLib::error(JML_CHAR* msg) {
  if(use_cpp_callback) {
    if(mCallback != NULL) {
      mCallback(mUData, msg);
    }
  } else if(cb != NULL) {
    cb(msg);
  }
}

JML_BOOL JMLib::setWindowSize(JML_INT32 width, JML_INT32 height) {
  if (width <= 0 || height <= 0)
    return false;
  
  // store current status and stop juggling
  JML_INT32 oldStatus = status;
  stopJuggle();
  
  // set size
  imageWidth = width;
  imageHeight = height;
  horCenter = imageWidth / 2;
  verCenter = imageHeight / 2;
  
  // recalculate pattern
  if (oldStatus != ST_NONE) startJuggle();
  
  // restore state
  status = oldStatus;
  
  return true;
}

void JMLib::setMirror(JML_BOOL mir) {
  // store current status and stop juggling
  JML_INT32 oldStatus = status;
  stopJuggle();
     
  // set mirror
  mirror = mir ? 1 : 0;
       
  // recalculate pattern
  if (oldStatus != ST_NONE) startJuggle();
  
  // restore state
  status = oldStatus;
}

JML_BOOL JMLib::setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr, JML_FLOAT dr) {
  JML_INT32 jml_errno;
  
  if (strlen(site) > JML_MAX_SITELEN) {
    error("Siteswap too long");
    return false;
  }
  
  if (strlen(name) > JML_MAX_NAMELEN) {
    error("Pattern name too long");
    return false;    
  }
  
  // Check for valid siteswap
  if (!JMSiteValidator::validateSite(site)) {
    error("Invalid siteswap");
    return false;
  }
  
  /* zero */
  memset(siteswap, 0, JML_MAX_SITELEN);
  
  /* Copy the siteswap */
  strcpy(siteswap, site);
  
  /* Copy the name */
  strcpy(pattname, name);
  
  /* Set ratios */
  height_ratio = hr;
  dwell_ratio = dr;

  /* Turn off beep */
  beep = 0;
  
  /* Check ratios */
  if (height_ratio < HR_MIN || height_ratio > HR_MAX)
    height_ratio = HR_DEF;
  if (dwell_ratio < DR_MIN || dwell_ratio > DR_MAX)
    dwell_ratio = DR_DEF;
  
  /* Set pattern */
  if ((jml_errno = set_patt(site)) == 0) {
    doStepcalc();
    return true;
  }
  else {
    switch (jml_errno) {
    case 4:
      error("Syntax error");
      break;
    case 5:
      error("Invalid pattern");
      break;
    case 6:
      error("Invalid character in pattern");
      break;
    case 7:
      error("Synchronous number must be even");
      break;
    case 8:
      error("Max 6 balls may be multiplexed");
      break;
    case 9:
      error("Too many balls in pattern");
      break;
    case 10:
      error("Pattern too long");
      break;
    case 13:
      error("0 inside [] is invalid");
      break;
    default:
      error("Unexpected error");
      break;
    }
    
    return false;
  }
}

JML_BOOL JMLib::setStyle(JML_CHAR* name, JML_UINT8 length,
                     JML_INT8* data, JML_INT32 offset) {
  if (strlen((char*)data) > JML_MAX_STYLELEN) {
    error("Style too large");
    return false;
  }
  
  if (strlen(name) > JML_MAX_NAMELEN) {
    error("Style name too long");
    return false;
  }
  
  strcpy(stylename, name);
  memcpy(styledata, data + offset, length*4);
  style_len = length;
  
  return true;
}

JML_BOOL JMLib::setStyle(JML_CHAR* name) {
/* After adding a style here, also add to possible_styles_list at line 36 */

  if (strcmp(name, "Reverse") == 0 || strcmp(name, "reverse") == 0) {
    JML_INT8 style[] = { 4, 0, 13, 0 };
    setStyle("Reverse", 1, style);
  }
  else if (strcmp(name, "Shower") == 0 || strcmp(name, "shower") == 0) {
    JML_INT8 style[] = { 5, 0, 10, 0, 10, 0, 5, 0 };
    setStyle("Shower", 2, style);
  }
  else if (strcmp(name, "Mills Mess")== 0 || strcmp(name, "mills mess") == 0) {
    JML_INT8 style[] = { -1, 0, -12, 0, 0, 0, 12, 0, 1, 0, -12, 0 };
    setStyle("Mills Mess", 3, style);
  }
  else if (strcmp(name, "Center") == 0 || strcmp(name, "center") == 0) {
    JML_INT8 style[] = { 13, 0, 0, 0 };
    setStyle("Center", 1, style);
  }
  else if (strcmp(name, "Windmill") == 0 || strcmp(name, "windmill") == 0) {
    JML_INT8 style[] = { 10, 0, -8, 0, -8, 0, 10, 0 };
    setStyle("Windmill", 2, style);
  }
  else if (strcmp(name, "Random") == 0 || strcmp(name, "random") == 0) {
    int i;
    JML_INT8 *style;
    style = (JML_INT8 *)malloc((size_t)(4*sizeof(JML_INT8)*strlen(getSite())));
    if(style != NULL) {
	for(i=0;i<(int)strlen(getSite())*4;) {
	  style[i++] = (rand()%30)-15;
	  style[i++] = (rand()%10);
	}
	setStyle("Random", strlen(getSite()), style);
	free((void *)style);
	startJuggle();
    } else {
	setStyleDefault();
    }
  }
  else { // anything else is interpreted as "Normal"
    setStyleDefault();
  }
  return true;
}

JML_CHAR **JMLib::getStyles(void) {
  return possible_styles;
}

JML_INT32 JMLib::numStyles(void) {
  return num_possible_styles;
}

void JMLib::setPatternDefault(void) {
  setPattern("3 Cascade", "3", HR_DEF, DR_DEF);
  setStyleDefault();
}

void JMLib::setStyleDefault(void) {
  JML_INT8 defStyle[] = { 13, 0, 4, 0 };
  setStyle("Normal", 1, defStyle);
}


void JMLib::setHR(JML_FLOAT HR) {
	if (HR > HR_MAX) {
		height_ratio = HR_MAX;
	} else if (HR < HR_MIN) {
		height_ratio = HR_MIN;
	} else {
		height_ratio = HR;
	}
}

JML_FLOAT JMLib::getHR() {
	return(height_ratio);
}

void JMLib::setDR(JML_FLOAT DR) {
	if (DR > DR_MAX) {
		dwell_ratio = DR_MAX;
	} else if (DR < DR_MIN) {
		dwell_ratio = DR_MIN;
	} else {
		dwell_ratio = DR;
	}
}

JML_FLOAT JMLib::getDR() {
	return(dwell_ratio);
}


// Internal functions
void JMLib::arm_line(void){
  JML_INT32 mx,my;
  JML_INT32 sx,sy;
  
  if (hand_on) { // only bother calculating if hands are drawn
    // Notes:
    //  * gx/gy may need to be replaced by gx0/gy0 if erasing old values
    //    the method used in the X11 version
    //  * JMWin uses 11*dpm/DW instead of 11, which causes incorrect
    //    hand placement for some reason.
    ap.rx[0]= rhand.gx + 11 + arm_x;
    ap.ry[0]= rhand.gy + 11 + arm_y;
    ap.lx[0]= lhand.gx + 11 - arm_x;
    ap.ly[0]= lhand.gy + 11 + arm_y;
    
    sx=(JML_INT32)dpm*XR/kw0;
    sy=base-dpm/3;
    
    ap.rx[1]=(ap.rx[0]+(horCenter+sx)*2)/3+dpm/12;
    ap.lx[1]=(ap.lx[0]+(horCenter-sx)*2)/3-dpm/12;
    ap.ry[1]=(ap.ry[0]+sy)/2+dpm/8;
    ap.ly[1]=(ap.ly[0]+sy)/2+dpm/8;
    
    ap.rx[2]=(ap.rx[1]+(horCenter+sx)*3)/4;
    ap.lx[2]=(ap.lx[1]+(horCenter-sx)*3)/4;
    ap.ry[2]=(ap.ry[1]+sy*2)/3-dpm/25;
    ap.ly[2]=(ap.ly[1]+sy*2)/3-dpm/25;
    
    ap.rx[3]=(ap.rx[2]+(horCenter+sx)*2)/3-dpm/13;
    ap.lx[3]=(ap.lx[2]+(horCenter-sx)*2)/3+dpm/13;
    ap.ry[3]=(ap.ry[2]+sy*2)/3-dpm/40;
    ap.ly[3]=(ap.ly[2]+sy*2)/3-dpm/40;
    
    mx=(ap.rx[3]+ap.lx[3])/2;
    my=(ap.ry[3]+ap.ly[3])/2;
    
    ap.rx[4]=(mx*2+ap.rx[3])/3;
    ap.lx[4]=(mx*2+ap.lx[3])/3;
    ap.ry[4]=(my*2+ap.ry[3])/3;
    ap.ly[4]=(my*2+ap.ly[3])/3;
    
    ap.hx=mx;
    ap.hy=(my*2-dpm*2/3+base)/3;
    ap.hr=dpm/11;
    
    ap.rx[5]=ap.hx+dpm/20;
    ap.lx[5]=ap.hx-dpm/20;
    ap.ry[5]=ap.hy+dpm/13;
    ap.ly[5]=ap.ry[5];
  }
}

void JMLib::applyCorrections(void) {
  // Correct ball coordinates
  for(JML_INT32 i = balln - 1; i >= 0; i--) {
    b[i].gx += bm1;
    b[i].gy += bm1;
  }
}

void JMLib::hand_pos(JML_INT32 c, JML_INT32 h, JML_INT32* x, JML_INT32* z) {
  JML_UINT32 a;
  
  if(mirror){
    if(!syn &&  h) c--;
    if(c&1) a=((--c+h)%style_len)*4+2;
    else a=((c+h)%style_len)*4;
  }
  else{
    if(!syn && !h) c--;
    if(c&1) a=((c-h)%style_len)*4+2;
    else a=((c+1-h)%style_len)*4;
  }
  
  if(h) *x= styledata[a];
  else  *x=-styledata[a];
  
  *z=styledata[a+1];
}

JML_INT32 JMLib::juggle(struct ball *d) {
  JML_INT32 tp,flag=0,i,tpox,rpox,tpoz,rpoz;
  JML_INT32 x,y;
  
  // Save old values
  d->gx1=d->gx0;
  d->gy1=d->gy0;
  
  d->gx0=d->gx;
  d->gy0=d->gy;
  
  if(d->c<0) if((JML_INT32)time_count>=-d->c*tw) d->c=-d->c;
  
  while(1){
    tp=time_count-tw*xabs(d->c);
    if(tp<aw) break;
    d->st&=~OBJECT_UNDER;
    d->c0=d->c;
    
    if(d->st&OBJECT_HAND){
      d->c+=2;
      flag=1;
    }
    else{
      JML_INT32 t;
      
      t=d->c;
      
      if(syn){
        if(mirror && !d->chand) t++;
        else if(!mirror && d->chand) t++;
      }
      
      t%=pattw;
      d->bh=patt[t][r[t]];
      d->c+=xabs(d->bh);
      if(++r[t]>=patts[t]) r[t]=0;
      
      d->thand=d->chand;
      if((d->bh&1) || d->bh<0) d->chand=1-d->chand;
      
      flag=1;
    }
  }
  
  if( d->c>=0 && tp>=0 && !(d->st&OBJECT_UNDER) ){
    d->st|=OBJECT_UNDER;
    
    if(d->st&OBJECT_HAND){
      if(d->st&OBJECT_MOVE2){
        d->st|=OBJECT_MOVE;
        d->st&=~OBJECT_MOVE2;
      }
      else{
        d->st&=~OBJECT_MOVE;
      }
    }
    else{
      JML_INT32 h;
      JML_INT32 t;
      
      t=d->c;
      
      if(syn){
        if(mirror && !d->chand) t++;
        else if(!mirror && d->chand) t++;
      }
      
      t%=pattw;
      
      if(d->bh==1) d->st|=OBJECT_MOVE;
      else d->st&=~OBJECT_MOVE;
      
      for(i=0;i<patts[t];i++){
        h=patt[t][i];
        if(h==1){
          if(d->chand) lhand.st|=OBJECT_MOVE2;
          else rhand.st|=OBJECT_MOVE2;
        }
        if(h!=2){
          if(d->chand) rhand.st|=OBJECT_MOVE2;
          else lhand.st|=OBJECT_MOVE2;
          d->st|=OBJECT_MOVE;
        }
      }
    }
  }
  
  if(!(d->st&OBJECT_MOVE)){
    if(d->c<0){
      hand_pos(-d->c,d->chand,&tpox,&tpoz);
      rpox=tpox;
      rpoz=tpoz;
    }
    else{
      if(d->st&OBJECT_UNDER){
        hand_pos(d->c  ,d->chand,&tpox,&tpoz);
        hand_pos(d->c+2,d->chand,&rpox,&rpoz);
        if(tpox!=rpox || tpoz!=rpoz){
          hand_pos(d->c+1,d->chand,&rpox,&rpoz);
          if(tpox!=rpox || tpoz!=rpoz) d->st|=OBJECT_MOVE;
        }
      }
      else{
        hand_pos(d->c-2,d->chand,&tpox,&tpoz);
        hand_pos(d->c  ,d->chand,&rpox,&rpoz);
        if(tpox!=rpox || tpoz!=rpoz){
          hand_pos(d->c-1,d->chand,&tpox,&tpoz);
          if(tpox!=rpox || tpoz!=rpoz) d->st|=OBJECT_MOVE;
        }
      }
    }
  }
  
  if(d->st&OBJECT_MOVE){
    if(d->bh==1){
      hand_pos(d->c0+1,d->thand,&tpox,&tpoz);
      hand_pos(d->c +1,d->chand,&rpox,&rpoz);
    }
    else if(d->st&OBJECT_UNDER){
      hand_pos(d->c   ,d->chand,&tpox,&tpoz);
      hand_pos(d->c+1 ,d->chand,&rpox,&rpoz);
    }
    else{
      hand_pos(d->c0+1,d->thand,&tpox,&tpoz);
      hand_pos(d->c   ,d->chand,&rpox,&rpoz);
    }
  }
  
  if(fpu){
    JML_FLOAT fx;
    
    if( !(d->st&OBJECT_HAND) && d->c<0 ){
      if(tpox==0){
        fx=0;
        y=(JML_INT32)((JML_FLOAT)tpoz*dpm/20-(JML_INT32)tp*dpm/12/tw);
      }
      else{
        if(tpox>0) fx=(JML_FLOAT)tpox/10-(JML_FLOAT)tp/6/tw;
        else fx=(JML_FLOAT)tpox/10+(JML_FLOAT)tp/6/tw;
        y=(JML_INT32)((JML_FLOAT)tpoz*dpm/20);
      }
    }
    else if( !(d->st&OBJECT_MOVE) ){
      fx=(JML_FLOAT)tpox/10;
      y=(JML_INT32)((JML_FLOAT)tpoz*dpm/20);
    }
    else{
      if(d->bh==1){
        fx=(JML_FLOAT)(tp-aw)/tw*2+1;
        y=(JML_INT32)((JML_FLOAT)high[1]*(1-jijo(fx)));
      }
      else if(d->st&OBJECT_UNDER){
        fx=(JML_FLOAT)tp/aw*2-1;
        y=(JML_INT32)((JML_FLOAT)high[0]*(1-jijo(fx)));
      }
      else{
        fx=(JML_FLOAT)tp/(tw*xabs(d->bh)-aw)*2+1;
        y=(JML_INT32)((JML_FLOAT)high[xabs(d->bh)]*(1-jijo(fx)));
      }
      
      y+=(JML_INT32)((JML_FLOAT)(fx*(rpoz-tpoz)+rpoz+tpoz)*dpm/40);
      fx=(fx*(rpox-tpox)+rpox+tpox)/20;
    }
    
    x=(JML_INT32)(fx*dpm*KW);
  }
  else{
    if( !(d->st&OBJECT_HAND) && d->c<0 ){
      if(tpox==0){
        x=0;
        y=tpoz*dpm/20-(JML_INT32)tp*dpm/12/tw;
      }
      else{
        if(tpox>0) x=XR*tpox/10-XR*tp/6/tw;
        else x=XR*tpox/10+XR*tp/6/tw;
        y=tpoz*dpm/20;
      }
    }
    else if( !(d->st&OBJECT_MOVE) ){
      x=XR*tpox/10;
      y=tpoz*dpm/20;
    }
    else{
      if(d->bh==1){
        x=XR*(tp-aw)*2/tw+XR;
        y=(jijo(XR)-jijo(x))/high0[1];
      }
      else if(d->st&OBJECT_UNDER){
        x=XR*tp*2/aw-XR;
        y=(jijo(XR)-jijo(x))/high0[0];
      }
      else{
        x=XR*tp*2/(tw*xabs(d->bh)-aw)+XR;
        y=(jijo(XR)-jijo(x))/high0[xabs(d->bh)];
      }
      
      y+=(x*(rpoz-tpoz)+XR*(rpoz+tpoz))*dpm/XR/40;
      x =(x*(rpox-tpox)+XR*(rpox+tpox))/20;
    }
    
    x=x*dpm/kw0;
  }
  
  // NOTE:
  //  * The alternative calulations of d->gx and gy below are
  //    from JMWin. They cause the entire pattern to be skewed.
  
  d->gx=horCenter + x - 11;
  //d->gx=horCenter + x - 11 * dpm / DW;
  
  if(d->st&OBJECT_HAND){
    if(d->chand) d->gx+=hand_x;
    else d->gx-=hand_x;
    y-=hand_y;
  }
  
  d->gy = base - y - 11;
  //d->gy = base - y - 11 * dpm / DW;
  
  return flag;
}

JML_INT32 JMLib::set_ini(JML_INT32 rr) {
  JML_INT32 i,j,k;
  JML_FLOAT tw0,aw0;
  
  balln=0;
  pmax=0;
  
  if(pattw>LMAX) return 10;
  
  if (pattw == 0) return 1;
		    
		    for(i=0;i<pattw;i++){
		      for(j=0;j<patts[i];j++){
			balln+=xabs(patt[i][j]);
			pmax=max(pmax,xabs(patt[i][j]));
		      }
		    }
		    
		    if(balln%pattw) return 5;
		    balln /= pattw;
		    if(balln==0) return 9;
		    if(balln>BMAX) return 9;
		    
		    for(i=0;i<LMAX*2;i++) r[i]=0;
		    
		    for(i=0;i<=balln;i++){
		      j=0;
		      while(r[j]==patts[j%pattw] && j<pattw+pmax) j++;
		      if(i==balln){
			if(j==pattw+pmax) break;
			else return 5;
		      }
		      
		      b[i].st=0;
		      
		      if(mirror){
			if((j+syn)%2){b[i].thand=1;b[i].chand=1;}
			else {b[i].thand=0;b[i].chand=0;}
		      }
		      else{
			if((j+syn)%2){b[i].thand=0;b[i].chand=0;}
			else {b[i].thand=1;b[i].chand=1;}
		      }
		      
		      if(syn) b[i].c=-((JML_INT32)(j/2))*2;
		      else b[i].c=-j;
		      
		      while(j<pattw+pmax){
			if(r[j]==patts[j%pattw]) return 5;
			else r[j]++;
			k=patt[j%pattw][patts[j%pattw]-r[j]];
			if(syn && k<0){
			  if(j%2==0) j+=-k+1;
			  else j+=-k-1;
			}
			else j+=k;
		      }
		    }
		    
		    if(rr==0) return 0;
		    
		    if(pmax<3) pmax=3;
		    tw0 = newton_sqrt(2/ga*pmax*height_ratio)*2/(pmax-dwell_ratio*2)*smode/speed;
		    tw = (JML_INT32)fadd(tw0,0,0);
		    if(tw==0) return 15;
		    aw0 = tw0*dwell_ratio*2;
		    aw = (JML_INT32)fadd(aw0,0,0);
		    if(aw<1) aw=1;
		    if(aw>tw*2-1) aw=tw*2-1;
		    
		    kw0=(JML_INT32)((JML_FLOAT)XR/KW);
		    
		    if(fpu){
		      high[0]=(JML_FLOAT)(-.2*dpm);
		      high[1]=ga*jijo(tw0/smode*speed)/8*dpm;
		      for(i=2;i<=pmax;i++)
			high[i]=ga*jijo((tw0*i-aw0)/smode*speed)/8*dpm;
		    }
		    else{
		      high0[0]=(JML_INT32)(-jijo(XR)/(JML_FLOAT)(.2*dpm));
		      high0[1]=(JML_INT32)(jijo(XR)/(JML_FLOAT)(ga*jijo(tw0/smode*speed)/8*dpm));
		      for(i=2;i<=pmax;i++)
			high0[i]=(JML_INT32)(jijo(XR)/
					(JML_FLOAT)(ga*jijo((tw0*i-aw0)/smode*speed)/8*dpm));
		    }
		    
		    for(i=0;i<balln;i++){
		      b[i].bh =0;
		      b[i].gx =horCenter;b[i].gy =verCenter;
		      b[i].gx0=horCenter;b[i].gy0=verCenter;
		      b[i].gx1=horCenter;b[i].gy1=verCenter;
		    }
		    
		    if(mirror){
		      lhand.c=0;
		      if(syn) rhand.c=0;
		      else rhand.c=-1;
		    }
		    else{
		      rhand.c=0;
		      if(syn) lhand.c=0;
		      else lhand.c=-1;
		    }
		    
		    rhand.bh=2;
		    rhand.st=OBJECT_HAND;
		    rhand.thand=1;rhand.chand=1;
		    rhand.gx =horCenter;rhand.gy =verCenter;
		    rhand.gx0=horCenter;rhand.gy0=verCenter;
		    rhand.gx1=horCenter;rhand.gy1=verCenter;
		    
		    lhand.bh=2;
		    lhand.st=OBJECT_HAND;
		    lhand.thand=0;lhand.chand=0;
		    lhand.gx =horCenter;lhand.gy =verCenter;
		    lhand.gx0=horCenter;lhand.gy0=verCenter;
		    lhand.gx1=horCenter;lhand.gy1=verCenter;
		    
		    for(i=0;i<pattw;i++) r[i]=0;
		    
		    return 0;
}

/* original set_dpm
   void JMLib::set_dpm(void){
   float speed0;
   int i;
   
   speed0=speed;
   speed=2;
   base=0;
   dpm=400;
   //dpm = (int)(400 * 400 / imageHeight);
   
   gy_max=80-11;
   gy_min=-200-11;
   gx_max = -1000;
   gx_min = 1000;
   
   if(set_ini(1)==0){
   for(time_count=0;(long)time_count<tw*(pattw+pmax+style_len);time_count++){
   for(i=0;i<balln;i++){
   juggle(&b[i]);
   gy_max=max(gy_max,b[i].gy);
   gy_min=min(gy_min,b[i].gy);
   gx_max = max(gx_max,b[i].gx);
   gx_min = min(gx_min,b[i].gx);
   }
   
   juggle(&rhand);
   juggle(&lhand);
   
   gy_max=max(gy_max,rhand.gy);
   gy_min=min(gy_min,rhand.gy);
   gy_max=max(gy_max,lhand.gy);
   gy_min=min(gy_min,lhand.gy);
   
   gx_max = max(gx_max,rhand.gx);
   gx_min = min(gx_min,rhand.gx);
   gx_max = max(gx_max,lhand.gx);
   gx_min = min(gx_min,lhand.gx);
   
   arm_line();
   
   for ( i = 0 ; i < 5 ; ++i ) {
   gx_max = max(gx_max, ap.rx[i]);
   gx_max = max(gx_max, ap.lx[i]);
   }
   
   }
   }
   
   if (gy_max-gy_min>0) {
   if (imageWidth <= 160 && imageHeight <= 160) {
   dpm=(int)((float)imageWidth*280/(gy_max-gy_min));
   base = imageHeight - imageHeight/4;
   
   //dpm=(int)((float)imageHeight*340/(gy_max-gy_min));
   //base = (int)(180 - //imageHeight/5 -// (float)gy_max*dpm/imageHeight);
   }
   else {
   dpm=(int)((float)imageHeight*340/(gy_max-gy_min));
   if(dpm>DW) dpm=DW;
   base=(int)((int)((float)imageHeight*0.925F)-
   (float)gy_max*dpm/imageHeight);
   }

   // original statements
   //dpm=(int)((float)400*340/(gy_max-gy_min));
   //if(dpm>DW) dpm=DW;
   //base=(int)(370-(float)gy_max*dpm/400);
   
   }
   
   speed=speed0;
   }
*/

void JMLib::set_dpm(void) {
  JML_FLOAT speed0;
  JML_INT32 i;
  
  speed0=speed;
  speed=(JML_FLOAT)2;
  base=0;
  dpm=400;
  
  gy_max=80-11;
  gy_min=-200-11;
  gx_max = -1000;
  gx_min = 1000;
  
  if (set_ini(1)==0) {
    for (time_count=0;(JML_INT32)time_count<tw*(pattw+pmax+style_len);time_count++){
      for (i=0;i<balln;i++) {
        juggle(&b[i]);
        gy_max=max(gy_max,b[i].gy);
        gy_min=min(gy_min,b[i].gy);
        gx_max = max(gx_max,b[i].gx+2*11*dpm/DW); // changed from X11 version
        gx_min = min(gx_min,b[i].gx);
      }
      
      juggle(&rhand);
      juggle(&lhand);
      
      gy_max=max(gy_max,rhand.gy);
      gy_min=min(gy_min,rhand.gy);
      gy_max=max(gy_max,lhand.gy);
      gy_min=min(gy_min,lhand.gy);
      
      gx_max = max(gx_max,rhand.gx);
      gx_min = min(gx_min,rhand.gx);
      gx_max = max(gx_max,lhand.gx);
      gx_min = min(gx_min,lhand.gx);
      
      arm_x = (22-11)*dpm/DW;
      arm_y = (16-11)*dpm/DW;
      
      // from JMWin:
      //ap.rx[0]=rhand.gx +11*dpm/DW+arm_x;
      //ap.ry[0]=rhand.gy +11*dpm/DW+arm_y;
      //ap.lx[0]=lhand.gx +11*dpm/DW-arm_x;
      //ap.ly[0]=lhand.gy +11*dpm/DW+arm_y;
      //
      
      arm_line();
      
      for ( i = 0 ; i < 5 ; ++i ) {
        gx_max = max(gx_max, ap.rx[i]);
        gx_max = max(gx_max, ap.lx[i]);
        gx_min = min(gx_min, ap.rx[i]);
        gx_min = min(gx_min, ap.lx[i]);
      }
    }
  }
  
  if (gy_max-gy_min>0) {
    // special handling for smaller screens
    if (imageWidth <= 320) {
      if (imageWidth > 160) { // 160-320 width
        dpm=(JML_INT32)((JML_FLOAT)imageHeight*280/(gy_max-gy_min));
        base = (JML_INT32)((JML_INT32)((JML_FLOAT)imageHeight) -
               (JML_FLOAT)gy_max * dpm / imageHeight) - 5;
      }
      else { // 0-160 width
        dpm=(JML_INT32)((JML_FLOAT)imageHeight*280/(gy_max-gy_min));
        base = imageHeight - imageHeight/4;
      }
    }
    else {
      dpm=(JML_INT32)(400.0* (imageHeight - 30*2)/(gy_max-gy_min));
      base=imageHeight - 30 -(JML_INT32)( (JML_FLOAT)gy_max*dpm/400 );
    }
    
    gx_min = horCenter - (JML_INT32)( (horCenter - gx_min) * dpm/400 );
    gx_max = horCenter - (JML_INT32)( (horCenter - gx_max) * dpm/400 );
    
    // original version
    //dpm=(JML_INT32)(400.0*340/(gy_max-gy_min));
    //if(dpm>DW) dpm=DW;
    //base=370-(JML_INT32)( (JML_FLOAT)gy_max*dpm/400 );
  }
  
  speed=speed0;
}

JML_INT32 JMLib::set_patt(JML_CHAR* s) {
  JML_INT32 i, l, flag = 0, flag2 = 0, a = 0;
  
  l=strlen(s);
  
  if(l>LMAX) return 10;
  
  pattw=0;
  balln=0;
  
  if(*s=='(')syn=1;
  else syn=0;
  
  for(i=0;i<LMAX;i++){
    if(i>=l){
      if(flag!=0 || flag2!=0) return 4;
      break;
    }
    if(*s=='['){
      flag2=1;
      patts[pattw]=0;
      s++;continue;
    }
    if(*s==']'){
      if(flag2==0) return 4;
      flag2=0;
      pattw++;
      s++;continue;
    }
    
    if(syn==1) switch(*s){
    case '(':
      if(flag!=0) return 4;
      flag=1;
      s++;continue;
    case ')':
      if(flag!=5) return 4;
      flag=0;
      s++;continue;
    case ',':
      if(flag!=2) return 4;
      flag=4;
      s++;continue;
    case 'X':
    case 'x':
      if(flag!=2 && flag!=5) return 4;
      if(flag2) patt[pattw][patts[pattw]-1]=-a;
      else patt[pattw-1][0]=-a;
      s++;continue;
    }
    
    a = ctod(*s);
    if(a==-1) return 6;
    
    if(syn){
      if(a%2) return 7;
      if(flag2==0 && flag!=1 && flag!=4) return 4;
      if(flag==1) flag=2;
      if(flag==4) flag=5;
    }
    
    if(flag2){
      if(a==0) return 13;
      patt[pattw][patts[pattw]++]=a;
      if(patts[pattw] >MMAX) return 8;
    }
    else{
      patt[pattw][0]=a;
      if(a==0) patts[pattw++]=0;
      else patts[pattw++]=1;
    }
    s++;
    balln+=a;
  }
  
  if(pattw==0) return 9;
  balln /= pattw;

  if(balln > BMAX) return  9;
  
  return 0;
}

JML_INT32 JMLib::ctod(JML_CHAR c){
  if(c>='0' && c<='9') return c-'0';
  else if(c>='a' && c<='z') return c-'a'+10;
  else if(c>='A' && c<='Z') return c-'A'+10;
  
  return -1;
}

void JMLib::startJuggle(void) {
  set_dpm();
  if (set_ini(1) != 0)
    return;
  xbitset();
  
  // apply corrections
  bm1=11-11*dpm/DW;
  time_count=0;
  time_period=0;
  
  status = ST_JUGGLE;
}

void JMLib::stopJuggle(void) {
  status = ST_NONE;
}

void JMLib::togglePause(void) {
  if (status == ST_JUGGLE)
    status = ST_PAUSE;
  else if (status == ST_PAUSE)
    status = ST_JUGGLE;
}

void JMLib::setPause(JML_BOOL pauseOn) {
  if (status != ST_NONE) {
    if (pauseOn)
      status = ST_PAUSE;
    else
      status = ST_JUGGLE;
  }
}

JML_INT32 JMLib::getStatus(void) {
  return status;
}

JML_FLOAT JMLib::fadd(JML_FLOAT x, JML_INT32 k, JML_FLOAT t) {
  return (JML_FLOAT)(JML_INT32((x+t)*xpow(10,k)+.5)/xpow(10,k));
  //return (JML_FLOAT)(floor((x+t)*xpow(10,k)+.5)/xpow(10,k));
}

/* JML_BOOL JMLib::setFrameskip(JML_INT32 fs) {
	frameSkip = fs;
	if(fs > FS_MAX) {
		frameSkip = FS_MAX;
		return(false);
	} else if(fs < FS_MIN) {
		frameSkip = FS_MIN;
		return(false);
	}
	return(true);
} */

/* FIXME */
void JMLib::speedUp(void) {
	speed = SPEED_MAX;
	set_ini(0);
}

/* FIXME */
void JMLib::speedDown(void) {
	speed = SPEED_MIN;
	set_ini(0);
}

/* FIXME */
void JMLib::speedReset(void) {
	speed = SPEED_DEF;
	set_ini(0);
}

JML_INT32 JMLib::doJuggle(void) {
  JML_INT32 i;
  JML_INT32 tone = 0;
  
  if (status == ST_PAUSE || status == ST_NONE) {
    // frameSkip=0;
    tone=0;
    return 0;
  }
  
  // time_count += frameSkip;
  time_count++;
  // frameSkip = 0;
  
  if ((JML_INT32)time_count < aw) time_count = aw;

  time_period = (time_count - aw)/tw;
  time_period %= pattw;
  //time_period = time_count % pattw;
  
  for (i=0; i < balln; i++)
    if(juggle(&b[i]) && beep) tone = max(tone, xabs(b[i].bh));
  
  if(juggle(&rhand) + juggle(&lhand) > 0){
    //if (back_on==1) patt_print(1);
  }
  
  arm_line();
  applyCorrections();

  return tone;
}

// Modify to include hand calculations
void JMLib::xbitset(void) {
  JML_INT32 i, j = 0;
  
  // DATA is used to create the hand bitmaps
  static JML_INT32 DATA[] = {
    0,18, 0,23,17,23,20,22,
    22,20,23,17,23,12,18,12,
    18,16,16,18, 0,18,
    12,15,23,17,99,99
  };
  
  JML_INT32 data[sizeof(DATA)];
  
  // initialize the DATA array.
  memcpy(data, DATA, sizeof(DATA));
  for(i=0;data[i]<99;i++) data[i]=(data[i]-11)*dpm/DW;
  
  // apply hand placement offsets
  hand_x=data[i-4]+2;
  hand_y=data[i-3]+2;
  arm_x=data[i-2];
  arm_y=data[i-1];
  
  // calculate hand polygons
  for(i=0;data[i+6]<99;i+=2) {
    if (j > 9) break;
    
    handpoly.rx[j] = 11 + data[i];
    handpoly.ry[j] = 10 + data[i+1];
    
    handpoly.lx[j] = 12 - data[i];
    handpoly.ly[j] = 10 + data[i+1];
    
    j++;
  }
}

void JMLib::doStepcalc(void) {
  JML_INT32 i;
  /* pos: position in string
     stp: position in steps array */
  JML_UINT8 stp = 0, pos = 0;
  JML_CHAR* s;
  
  s = siteswap;
  
  /* reset */
  for (i = 0; i < LMAX; i++) {
    steps[i] = -1;
  }
  
  /* Synchronous pattern */
  if (syn) {
    while (pos <= strlen(siteswap)) {
      if (*s == '(') {
        steps[stp] = pos;
        stp+=2;
        while(*s != ')') { s++; pos++; }	
        pos++;
        s++;
      }
      else if (*s == '\0') {
        steps[stp] = pos;
        break;
      }
      else {
        error("Internal error");
        return;
      }
    }
  }
  else {
    while (pos <= strlen(siteswap)) {
      if (*s == '(') {
        error("Internal error");
        return;
      }
      /* Multiplex */
      else if (*s == '[') {
        steps[stp++] = pos;
        while(*s != ']') { s++; pos++; }
        pos++;
        s++;
      }
      /* Normal throw */
      else {
        steps[stp++] = pos++;
        s++;
      }
    }
  }
}
