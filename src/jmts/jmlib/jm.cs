//
// JMLib - Portable JuggleMaster Library
// Version 2.0                                                                  
// (C) Per Johan Persson 2000-2002, Gary Briggs 2003-2005
//                                                                              
// Based on JuggleMaster Version 1.60                                           
// Copyright (c) 1995-1996 Ken Matsuoka                                         
//                                                                              
// You may redistribute and/or modify JMLib under the terms of the              
// Modified BSD License as published in various places online or in the         
// COPYING.jmlib file in the package you downloaded.                            
//                                                                              
// This program is distributed in the hope that it will be useful, but          
// WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of          
// MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the              
// Modified BSD License for more details.
//

$KW=0.25;
$XR=1024;
$DW=290; //Max dpm

$BMAX=630; //Max Balls
$LMAX=76; // max column of a pattern
$MMAX=71; // Max multiplex, double+1

$OBJECT_HAND  = 1 << 0;
$OBJECT_UNDER = 1 << 1;
$OBJECT_MOVE  = 1 << 2;
$OBJECT_MOVE2 = 1 << 3;

$JML_MAX_SITELEN=56;
$JML_MAX_STYLELEN=500;

$SPEED_DEF=0.4;

$HR_MAX=1.00; // HR == "Height Ratio"
$HR_MIN=0.04;
$HR_DEF=0.17;

$DR_MAX=0.90; // DR == "Dwell Ratio"
$DR_MIN=0.10;
$DR_DEF=0.50;



function JMBall::OnAdd(%this) {
	%this.bh = 0;
	%this.gx = 0;
	%this.gy = 0;
	%this.c = 0;
	%this.c0 = 0;
	%this.chand = 0;
	%this.thand = 0;
	%this.st = 0;
}

function JMArm::OnAdd(%this) {
	%this.hx = 0;
	%this.hy = 0;
	%this.hr = 0;
	%this.st |= $OBJECT_HAND;
	for(%i=0;%i<6;%i++) {
		%this.rx[%i] = 0;
		%this.ry[%i] = 0;
		%this.lx[%i] = 0;
		%this.ly[%i] = 0;
	}
}

function JMLib::OnAdd(%this) {


	for(%i=0;%i<$BMAX;%i++) {
		%this.b[%i] = new ScriptObject(JMBall);
	}
	%this.lhand = new ScriptObject(JMBall);
	%this.rhand = new ScriptObject(JMBall);

	%this.ap = new ScriptObject(JMArm);


	%this.reset();

	%this.setWindowSize(360, 300);

	%this.setPatternDefault();
	%this.setStyleDefault();
	%this.startJuggle();
}

function JMLib::OnDelete(%this) {
	for(%i=0;%i<$BMAX;%i++) {
		%this.b[%i].delete();
	}
	%this.lhand.delete();
	%this.rhand.delete();
}

function JMLib::reset(%this) {
// Initial Variables. This was JMLib::initialize
	%this.ga = 9.8;
	%this.dwell_ratio = $DR_DEF;
	%this.height_ratio = $HR_DEF;
	%this.speed = $SPEED_DEF;
	%this.syn = 0;
	%this.hand_x = 0;
	%this.hand_y = 0;
	%this.smode = 50.0;
	%this.dpm = 0;
	%this.balln = 0;

	%this.gx_max = 0;
	%this.gx_min = 0;
	%this.gy_max = 0;
	%this.gy_min = 0;

	// %this.imageWidth = 0;
	// %this.imageHeight = 0;
	// %this.horCenter = 0;
	// %this.verCenter = 0;

	%this.arm_x = 0;
	%this.arm_y = 0;
	%this.style_len = 0;
	%this.base = 0;
	%this.time_count = 0;
	%this.tw = 0;
	%this.aw = 0;
	%this.pmax = 0;
	%this.pattw = 0;
	%this.kw0 = 0;       // XR/KW [m]

	for(%i=0;%i<$LMAX*2;%i++) {
		%this.r[%i] = 0;
	}

	for(%i=0;%i<$BMAX;%i++) {
		%this.b[%i].bh = 0;
		%this.b[%i].gx = 0;
		%this.b[%i].gy = 0;
		%this.b[%i].c = 0;
		%this.b[%i].c0 = 0;
		%this.b[%i].chand = 0;
		%this.b[%i].thand = 0;
		%this.b[%i].st = 0;
	}
	%this.lhand.bh = 0;
	%this.lhand.gx = 0;
	%this.lhand.gy = 0;
	%this.lhand.c = 0;
	%this.lhand.c0 = 0;
	%this.lhand.chand = 0;
	%this.lhand.thand = 0;
	%this.lhand.st = 0;

	%this.rhand.bh = 0;
	%this.rhand.gx = 0;
	%this.rhand.gy = 0;
	%this.rhand.c = 0;
	%this.rhand.c0 = 0;
	%this.rhand.chand = 0;
	%this.rhand.thand = 0;
	%this.rhand.st = 0;

	%this.ap.hx = 0;
	%this.ap.hy = 0;
	%this.ap.hr = 0;
	%this.ap.st |= $OBJECT_HAND;
	for(%i=0;%i<6;%i++) {
		%this.ap.rx[%i] = 0;
		%this.ap.ry[%i] = 0;
		%this.ap.lx[%i] = 0;
		%this.ap.ly[%i] = 0;
	}

	for(%i=0;%i<$BMAX+1;%i++) {
		%this.high[%i] = 0;
	}
	
	%this.siteswap = ""; // The current siteswap

	for(%i=0;%i<$JML_MAX_NAMELEN;%i++) {
		%this.stylename[%i] = 0; // The name of the current style
	}

	// These are grouped for initialisation convenience, not logical reason.
	for(%i=0;%i<$LMAX;%i++) {
		for(%j=0;%j<$MMAX;%j++) {
			%this.patt[%i,%j] = 0;
		}
		%this.patts[%i] = 0;
		%this.steps[%i] = -1; // used to print the site on screen
	}
	%this.pattname = ""; // The name of the current pattern

}

function JMLib::setWindowSize(%this, %width, %height) {
	if (%width <= 0 || %height <= 0) {
		return;
	}

	%this.imageWidth = %width;
	%this.imageHeight = %height;
	%this.horCenter = %this.imageWidth / 2;
	%this.verCenter = %this.imageHeight / 2;

	return;
}

function JMLib::arm_line(%this) {
	%this.ap.rx[0]= %this.rhand.gx + 11 + %this.arm_x;
	%this.ap.ry[0]= %this.rhand.gy + 11 + %this.arm_y;
	%this.ap.lx[0]= %this.lhand.gx + 11 - %this.arm_x;
	%this.ap.ly[0]= %this.lhand.gy + 11 + %this.arm_y;

	%sx=mFloor(%this.dpm*$XR/%this.kw0);
	%sy=%this.base-%this.dpm/3;

	%this.ap.rx[1]=(%this.ap.rx[0]+(%this.horCenter+%sx)*2)/3+%this.dpm/12;
	%this.ap.lx[1]=(%this.ap.lx[0]+(%this.horCenter-%sx)*2)/3-%this.dpm/12;
	%this.ap.ry[1]=(%this.ap.ry[0]+%sy)/2+%this.dpm/8;
	%this.ap.ly[1]=(%this.ap.ly[0]+%sy)/2+%this.dpm/8;

	%this.ap.rx[2]=(%this.ap.rx[1]+(%this.horCenter+%sx)*3)/4;
	%this.ap.lx[2]=(%this.ap.lx[1]+(%this.horCenter-%sx)*3)/4;
	%this.ap.ry[2]=(%this.ap.ry[1]+%sy*2)/3-%this.dpm/25;
	%this.ap.ly[2]=(%this.ap.ly[1]+%sy*2)/3-%this.dpm/25;

	%this.ap.rx[3]=(%this.ap.rx[2]+(%this.horCenter+%sx)*2)/3-%this.dpm/13;
	%this.ap.lx[3]=(%this.ap.lx[2]+(%this.horCenter-%sx)*2)/3+%this.dpm/13;
	%this.ap.ry[3]=(%this.ap.ry[2]+%sy*2)/3-%this.dpm/40;
	%this.ap.ly[3]=(%this.ap.ly[2]+%sy*2)/3-%this.dpm/40;

	%mx=(%this.ap.rx[3]+%this.ap.lx[3])/2;
	%my=(%this.ap.ry[3]+%this.ap.ly[3])/2;

	%this.ap.rx[4]=(%mx*2+%this.ap.rx[3])/3;
	%this.ap.lx[4]=(%mx*2+%this.ap.lx[3])/3;
	%this.ap.ry[4]=(%my*2+%this.ap.ry[3])/3;
	%this.ap.ly[4]=(%my*2+%this.ap.ly[3])/3;

	%this.ap.hx=%mx;
	%this.ap.hy=(%my*2-%this.dpm*2/3+%this.base)/3;
	%this.ap.hr=%this.dpm/11;

	%this.ap.rx[5]=%this.ap.hx+%this.dpm/20;
	%this.ap.lx[5]=%this.ap.hx-%this.dpm/20;
	%this.ap.ry[5]=%this.ap.hy+%this.dpm/13;
	%this.ap.ly[5]=%this.ap.ry[5];
}

function JMLib::error(%this,%errormesg) {
	echo("Error from JMlib:" SPC %errormesg);
	MessageBoxOK("Error from JMLib", %errormesg, "");
}

// The clusterfuck function category
function JMLib::fadd(%x, %k, %t) {
	return ((%x+%t)*mPow(10,%k)+0.5)/mPow(10,%k);
}

function squared(%x) {
	return %x * %x;
}

function max(%a,%b) {
	if(%a > %b) {
		return %a;
	} else {
		return %b;
	}
}

function min(%a,%b) {
	if(%a < %b) {
		return %a;
	} else {
		return %b;
	}
}
// End clusterfucking stupid category

function JMLib::set_ini(%this, %rr) { // No idea what rr is for
	%this.balln=0;
	%this.pmax=0;

	if(%this.pattw>$LMAX) {
		%this.error("Pattern too long");
		return 1;
	}

	if(%this.pattw==0) {
		%this.error("Pattern zero length");
		return 1;
	}

	for(%i=0;%i<%this.pattw;%i++){
		for(%j=0;%j<%this.patts[%i];%j++){
			%this.balln+=mAbs(%this.patt[%i,%j]);
			%this.pmax=max(%this.pmax,mAbs(%this.patt[%i,%j]));
		}
	}

	if(%this.balln % %this.pattw) { // TorqueScript hurts my face
		%this.error("Invalid Pattern");
		return 1;
	}

	%this.balln = mFloor(%this.balln / %this.pattw);

	if(%this.balln==0) {
		%this.error("No Balls in Pattern");
		return 1;
	}

	if(%this.balln>$BMAX) {
		%this.error("Too Many Balls in Pattern");
		return 1;
	}


	for(%i=0;%i<$LMAX*2;%i++) {
		%this.r[%i]=0;
	}

	for(%i=0;%i<=%this.balln;%i++) {
		%j=0;
		while(%this.r[%j]==%this.patts[%j % %this.pattw] && %j<%this.pattw+%this.pmax) {
			%j++;
		}

		if(%i==%this.balln){
			if(%j==%this.pattw+%this.pmax) {
				break;
			} else {
				%this.error("Invalid Pattern");
				return 1;
			}
		}

		%this.b[%i].st=0;

		if((%j+%this.syn)%2) {
			%this.b[%i].thand=0;
			%this.b[%i].chand=0;
		} else {
			%this.b[%i].thand=1;
			%this.b[%i].chand=1;
		}


		if(%this.syn) {
			%this.b[%i].c=-(2 * mFloor(%j/2));
		} else {
			%this.b[%i].c=-%j;
		}


		while(%j<%this.pattw+%this.pmax){
			if(%this.r[%j]==%this.patts[%j % %this.pattw]) {
				%this.error("Invalid Pattern");
				return 1;
			}

			%this.r[%j]++;

			%l=%j % %this.pattw;
			%k=%this.patt[%l, %this.patts[%l] - %this.r[%j]];
			if(%this.syn != 0 && %k < 0){
				if(%j%2==0) {
					%j+=-%k+1;
				} else {
					%j+=-%k-1;
				}
			} else {
				%j+=%k;
			}
		}
	}

	if(%rr==0) {
		return 0;
	}

	if(%this.pmax<3) {
		%this.pmax=3;
	}

	%tw0 = mSqrt(2/%this.ga*%this.pmax*%this.height_ratio) * 2 /
			(%this.pmax-%this.dwell_ratio*2)*%this.smode/%this.speed;
	%this.tw = mFloor(JMLib::fadd(%tw0,0,0));

	if(%this.tw==0)  {
		%this.error("Some Sort of Error");
		return 1;
	}

	%aw0 = %tw0*%this.dwell_ratio*2;
	%this.aw = mFloor(JMLib::fadd(%aw0,0,0));
	if(%this.aw<1) {
		%this.aw=1;
	} else if(%this.aw>%this.tw*2-1) {
		%this.aw=%this.tw*2-1;
	}

	%this.kw0=mFloor($XR/$KW);

	%this.high[0]=-0.2 * %this.dpm;
	%this.high[1]=%this.ga*squared(%tw0/%this.smode*%this.speed)/8*%this.dpm;

	for(%i=2;%i<=%this.pmax;%i++) {
		%this.high[%i]=%this.ga*squared((%tw0*%i-%aw0)/%this.smode*%this.speed)/8*%this.dpm;
	}

	for(%i=0;%i<%this.balln;%i++){
		%this.b[%i].bh =0;
		%this.b[%i].gx =%this.horCenter;
		%this.b[%i].gy =%this.verCenter;
		%this.b[%i].st =$OBJECT_MOVE;
	}

	%this.rhand.c=0;
	if(%this.syn) {
		%this.lhand.c=0;
	} else {
		%this.lhand.c=-1;
	}

	%this.rhand.bh=2;
	%this.rhand.st=$OBJECT_HAND;
	%this.rhand.thand=1;
	%this.rhand.chand=1;
	%this.rhand.gx = %this.horCenter;
	%this.rhand.gy = %this.verCenter;

	%this.lhand.bh=2;
	%this.lhand.st=$OBJECT_HAND;
	%this.lhand.thand=0;
	%this.lhand.chand=0;
	%this.lhand.gx =%this.horCenter;
	%this.lhand.gy =%this.verCenter;

	for(%i=0;%i<%this.pattw;%i++) {
		%this.r[%i]=0;
	}

	return 0;
}

function JMLib::juggle(%this, %ball) {
	if(%ball.c<0) {
		if(%this.time_count>=-%ball.c*%this.tw) {
			%ball.c=-%ball.c;
		}
	}

	while(1){
		%tp = %this.time_count-%this.tw*mAbs(%ball.c);

		if(%tp<%this.aw) {
			break;
		}

		%ball.st &= ~$OBJECT_UNDER;
		%ball.c0 = %ball.c;

		if(%ball.st & $OBJECT_HAND){
			%ball.c+=2;
		} else {
			%t=%ball.c;

			if(%this.syn && %ball.chand) {
				%t++;
			}

			%t %= %this.pattw;
			%ball.bh = %this.patt[%t,%this.r[%t]];
			%ball.c += mAbs(%ball.bh);
			%this.r[%t]++;
			if(%this.r[%t]>=%this.patts[%t]) {
				%this.r[%t]=0;
			}

			%ball.thand=%ball.chand;
			if((%ball.bh & 1) || %ball.bh<0) {
				%ball.chand = 1-%ball.chand;
			}
		}
	}

	if( %ball.c>=0 && %tp>=0 && (%ball.st & $OBJECT_UNDER)==0 ){
		%ball.st |= $OBJECT_UNDER;
	
		if(%ball.st & $OBJECT_HAND){
			if((%ball.st & $OBJECT_MOVE2) != 0){
				%ball.st |= $OBJECT_MOVE;
				%ball.st &= ~$OBJECT_MOVE2;
			} else {
				%ball.st &= ~$OBJECT_MOVE;
			}
		} else {
			%t=%ball.c;
	
			if(%this.syn && %ball.chand) {
				%t++;
			}
	
			%t %= %this.pattw;
	
			if(%ball.bh==1) {
				%ball.st |= $OBJECT_MOVE;
			} else {
				%ball.st &= ~$OBJECT_MOVE;
			}
	
			for(%i=0;%i<%this.patts[%t];%i++){
				%h=%this.patt[%t,%i];
				if(%h==1){
					if(%ball.chand) {
						%this.lhand.st |= $OBJECT_MOVE2;
					} else {
						%this.rhand.st |= $OBJECT_MOVE2;
					}
				}
				if(%h!=2){
					if(%ball.chand) {
						%this.rhand.st|= $OBJECT_MOVE2;
					} else {
						%this.lhand.st|= $OBJECT_MOVE2;
					}
					%ball.st|= $OBJECT_MOVE;
				}
			}
		}
	}


	if((%ball.st & $OBJECT_MOVE) == 0){
		if(%ball.c < 0){
			%tpox = %this.hand_pos_x(-%ball.c,%ball.chand);
			%tpoz = %this.hand_pos_z(-%ball.c,%ball.chand);
			%rpox = %tpox;
			%rpoz = %tpoz;
		} else {
			if((%ball.st & $OBJECT_UNDER) != 0) {
				%tpox = %this.hand_pos_x(%ball.c  , %ball.chand);
				%tpoz = %this.hand_pos_z(%ball.c  , %ball.chand);
				%rpox = %this.hand_pos_x(%ball.c+2, %ball.chand);
				%rpoz = %this.hand_pos_z(%ball.c+2, %ball.chand);
				if(%tpox != %rpox || %tpoz != %rpoz) {
					%rpox = %this.hand_pos_x(%ball.c+1, %ball.chand);
					%rpoz = %this.hand_pos_z(%ball.c+1, %ball.chand);
					if(%tpox != %rpox || %tpoz != %rpoz) {
						%ball.st |= $OBJECT_MOVE;
					}
				}
			} else {
				%tpox = %this.hand_pos_x(%ball.c-2, %ball.chand);
				%tpoz = %this.hand_pos_z(%ball.c-2, %ball.chand);
				%rpox = %this.hand_pos_x(%ball.c  , %ball.chand);
				%rpoz = %this.hand_pos_z(%ball.c  , %ball.chand);
				if(%tpox != %rpox || %tpoz != %rpoz) {
					%tpox = %this.hand_pos_x(%ball.c-1, %ball.chand);
					%tpoz = %this.hand_pos_z(%ball.c-1, %ball.chand);
					if(%tpox != %rpox || %tpoz != %rpoz) {
						%ball.st |= $OBJECT_MOVE;
					}
				}
			}
		}
	}

	if((%ball.st & $OBJECT_MOVE) != 0) {
		if(%ball.bh == 1) {
			%tpox = %this.hand_pos_x(%ball.c0+1, %ball.thand);
			%tpoz = %this.hand_pos_z(%ball.c0+1, %ball.thand);
			%rpox = %this.hand_pos_x(%ball.c+1 , %ball.chand);
			%rpoz = %this.hand_pos_z(%ball.c+1 , %ball.chand);
		} else if((%ball.st & $OBJECT_UNDER) != 0) {
			%tpox = %this.hand_pos_x(%ball.c   , %ball.chand);
			%tpoz = %this.hand_pos_z(%ball.c   , %ball.chand);
			%rpox = %this.hand_pos_x(%ball.c+1 , %ball.chand);
			%rpoz = %this.hand_pos_z(%ball.c+1 , %ball.chand);
		} else {
			%tpox = %this.hand_pos_x(%ball.c0+1, %ball.thand);
			%tpoz = %this.hand_pos_z(%ball.c0+1, %ball.thand);
			%rpox = %this.hand_pos_x(%ball.c   , %ball.chand);
			%rpoz = %this.hand_pos_z(%ball.c   , %ball.chand);
		}
	}


	if( (%ball.st & $OBJECT_HAND) == 0 && %ball.c<0 ) {
		if(%tpox==0) {
			%fx=0;
			%y= %tpoz*%this.dpm/20-%tp*%this.dpm/12/%this.tw;
		} else {
			if(%tpox>0) {
				%fx=%tpox/10-%tp/6/%this.tw;
			} else {
				%fx=%tpox/10+%tp/6/%this.tw;
			}
			%y=%tpoz*%this.dpm/20;
		}
	} else if( (%ball.st & $OBJECT_MOVE) == 0 ) {
		%fx=%tpox/10;
		%y=%tpoz*%this.dpm/20;
	} else {
		if(%ball.bh==1) {
				%fx=(%tp-%this.aw)/%this.tw*2+1;
				%y=%this.high[1]*(1-squared(%fx));
		} else if((%ball.st & $OBJECT_UNDER) != 0) {
				%fx=%tp/%this.aw*2-1;
				%y=%this.high[0]*(1-squared(%fx));
		} else {
				%fx=%tp/(%this.tw*mAbs(%ball.bh)-%this.aw)*2+1;
				%y=%this.high[mAbs(%ball.bh)]*(1-squared(%fx));
		}
		%y+=(%fx*(%rpoz-%tpoz)+%rpoz+%tpoz)*%this.dpm/40;
		%fx=(%fx*(%rpox-%tpox)+%rpox+%tpox)/20;
	}

	%x = %fx * %this.dpm * $KW;

	%ball.gx=%this.horCenter + %x - 11;

	if((%ball.st & $OBJECT_HAND) != 0){
		if(%ball.chand) {
			%ball.gx += %this.hand_x;
		} else {
			%ball.gx -= %this.hand_x;
		}
		%y -= %this.hand_y;
	}

	%ball.gy = %this.base - %y - 11;
	return 0;

}

function JMLib::set_dpm(%this) {
	%speed0 = %this.speed;
	%this.speed = 2;

	%this.base=0;
	%this.dpm=400;

	%this.gy_max=80-11;
	%this.gy_min=-200-11;
	%this.gx_max = -1000;
	%this.gx_min = 1000;

	if(%this.set_ini(1) == 0) {

		for (%this.time_count=0;
			%this.time_count < %this.tw*(%this.pattw+%this.pmax+%this.style_len);
			%this.time_count++) {

			for (%i=0;%i<%this.balln;%i++) {
				%this.juggle(%this.b[%i]);
				%this.gy_max=max(%this.gy_max,%this.b[%i].gy);
				%this.gy_min=min(%this.gy_min,%this.b[%i].gy);
				%this.gx_max=max(%this.gx_max,%this.b[%i].gx+2*11*%this.dpm/$DW);
				%this.gx_min=min(%this.gx_min,%this.b[%i].gx);

				%this.gy_max = mFloor(%this.gy_max);
				%this.gy_min = mFloor(%this.gy_min);
				%this.gx_max = mFloor(%this.gx_max);
				%this.gx_min = mFloor(%this.gx_min);
			}

			// %this.juggle(%this.rhand);
			// %this.juggle(%this.lhand);

			%this.gy_max=max(%this.gy_max,%this.rhand.gy);
			%this.gy_min=min(%this.gy_min,%this.rhand.gy);
			%this.gy_max=max(%this.gy_max,%this.lhand.gy);
			%this.gy_min=min(%this.gy_min,%this.lhand.gy);

			%this.gx_max=max(%this.gx_max,%this.rhand.gx);
			%this.gx_min=min(%this.gx_min,%this.rhand.gx);
			%this.gx_max=max(%this.gx_max,%this.lhand.gx);
			%this.gx_min=min(%this.gx_min,%this.lhand.gx);

			%this.arm_x = mFloor((22-11)*%this.dpm/$DW);
			%this.arm_y = mFloor((16-11)*%this.dpm/$DW);

			%this.arm_line();

			for (%i=0; %i<5 ;%i++ ) {
				%this.gx_max = max(%this.gx_max, %this.ap.rx[%i]);
				%this.gx_max = max(%this.gx_max, %this.ap.lx[%i]);
				%this.gx_min = min(%this.gx_min, %this.ap.rx[%i]);
				%this.gx_min = min(%this.gx_min, %this.ap.lx[%i]);
			}
		}
	}
	// %this.gx_min = 53;
	// %this.gx_max = 457;
	// %this.gy_min = -215;
	// %this.gy_max = 69;

	%this.dpm=mFloor(400 * (%this.imageHeight - 30*2)/(%this.gy_max-%this.gy_min));
	%this.base=mFloor(%this.imageHeight - 30 - %this.gy_max*%this.dpm/400);

	%this.gx_min = mFloor(%this.horCenter - (%this.horCenter - %this.gx_min) * %this.dpm/400);
	%this.gx_max = mFloor(%this.horCenter - (%this.horCenter - %this.gx_max) * %this.dpm/400);

	%this.speed = %speed0;

}

function JMLib::setPatternDefault(%this) {
	%this.setPattern("3", "3", $HR_DEF, $DR_DEF);
}

// Always use JMLib::setPattern, never set_patt
function JMLib::setPattern(%this, %name, %site, %hr, %dr) {
  
	if(%hr $= "") {
		%hr = $HR_DEF;
	}
	if(%dr $= "") {
		%dr = $DR_DEF;
	}
 // This is completely backwards, but it retains compatability with the C++
	if(strlen(%site) == 0) {
		%site = %name;
	}
	if (strlen(%site) > $JML_MAX_SITELEN) {
		%this.error("Siteswap too long");
		return 1;
	}
  
	// Check for valid siteswap
	if (JMSiteValidator::validateSite(%site) != 0) {
		%this.error("Invalid siteswap");
		return 1;
	}


	%this.siteswap = %site;
	%this.pattname = %name;

	%this.setHR(%hr);
	%this.setDR(%dr);

	if (%this.set_patt(%site) == 0) {
		%this.doStepcalc();
		return 0;
	} else {
		return 1;
	}
}

function JMLib::set_patt(%this, %site) {
	%sync_flag = 0;
// during parsing, sync_flag is:
//   0 between the ) and the (
//   1 when you're on the first throw inside a (throw,throw) pair
//   1 turns to 2 on the next iteration of %i
//   4 when you're on the second throw inside a (throw,throw) pair
//   4 turns to 5 on the next iteration of %i
//   There is no 3.

	%in_multiplex = 0;
	%a = 0;
	%l = strlen(%site);

	if(%l > $LMAX) {
		%this.error("Pattern too long");
		return 1;
	}

	%this.pattw=0;
	%this.balln=0;

	if(getSubStr(%site,0,1) $="(") {
		%this.syn=1;
	} else {
		%this.syn=0;
	}

	for(%i=0;%i<$LMAX;%i++){
		%c = getSubStr(%site,0,1);

		if(%i >= %l) {
			if(%sync_flag!=0 || %in_multiplex!=0) {
				%this.error("Syntax error");
				return 1;
			}
			break;
		}
		if(%c $= "[") {
			%in_multiplex=1;
			%this.patts[%this.pattw]=0;
			%site=getSubStr(%site, 1, strlen(%site)-1);
			continue;
		}
		if(%c $= "]"){
			if(%in_multiplex == 0) {
				%this.error("Syntax error");
				return 1;
			}
			%in_multiplex=0;
			%this.pattw++;
			%site=getSubStr(%site, 1, strlen(%site)-1);
			continue;
		}

		if(%this.syn == 1) {
			switch$(%c) {
				case "(":
					if(%sync_flag != 0) {
						%this.error("Syntax error");
						return 1;
					}
					%sync_flag = 1;
					%site=getSubStr(%site, 1, strlen(%site)-1);
					continue;
				case ")":
					if(%sync_flag!=5) {
						%this.error("Syntax error");
						return 1;
					}
					%sync_flag=0;
					%site=getSubStr(%site, 1, strlen(%site)-1);
					continue;
				case ",":
					if(%sync_flag!=2)  {
						%this.error("Syntax error");
						return 1;
					}
					%sync_flag=4;
					%site=getSubStr(%site, 1, strlen(%site)-1);
					continue;
				case "x": // FIXME ! Add case "X";
					if(%sync_flag!=2 && %sync_flag!=5)  {
						%this.error("Syntax error");
						return 1;
					}
					if(%in_multiplex) {
						%this.patt[%this.pattw,%this.patts[%this.pattw]-1]=-%a;
					} else {
						%this.patt[%this.pattw-1,0]=-%a;
					}
					%site=getSubStr(%site, 1, strlen(%site)-1);
					continue;
			}
		}

		%c = getSubStr(%site,0,1);
		%a = JMLib::siteDigit(%c);
		if(%a == 0 && %c !$= "0") {
			%this.error("Invalid character in pattern");
			return 1;
		}

		if(%this.syn){
			if(%a % 2) {
				%this.error("Synchronous number must be even");
				return 1;
			}
			if(%in_multiplex == 0 && %sync_flag != 1 && %sync_flag != 4) {
				%this.error("Syntax error");
				return 1;
			}
			if(%sync_flag == 1) {
				%sync_flag = 2;
			}
			if(%sync_flag == 4) {
				%sync_flag = 5;
			}
		}

		if(%in_multiplex != 0) {
			if(%a == 0) {
				%this.error("0 inside [] is invalid");
				return 1;
			}
			%this.patt[%this.pattw,%this.patts[%this.pattw]]=%a;
			%this.patts[%this.pattw]++;
			if(%this.patts[%this.pattw] > $MMAX) {
				%this.error("Max " @ $MMAX @ " balls may be multiplexed");
				return 1;
			}
		} else {
			%this.patt[%this.pattw,0]=%a;
			if(%a == 0) {
				%this.patts[%this.pattw]=0;
				%this.pattw++;
			} else {
				%this.patts[%this.pattw]=1;
				%this.pattw++;
			}
		}
		%site=getSubStr(%site, 1, strlen(%site)-1);
		%this.balln += %a;
	}

	if(%this.pattw==0) {
		%this.error("Too many balls in pattern");
		return 1;
	}
	%this.balln /= %this.pattw;

	if(%this.balln > $BMAX) {
		%this.error("Too many balls in pattern");
		return 1;
	}

	return 0;
}

function JMLib::setStyle(%this, %name, %length, %data) {
	if (%length * 4 > $JML_MAX_STYLELEN) {
		%this.error("Style too large");
		return 1;
	}

	%this.stylename = %name;
	for(%i = 0; %i < %length * 4; %i++) {
		%this.styledata[%i] = getWord(%data,%i);
	}
	%this.style_len = %length;

	return 0;
}

function JMLib::setStyleNamed(%this, %name) {
	switch$(%name) {
		case "Normal":
			%data= "13 0 4 0";
			%length = 1;
		case "Reverse":
			%data = "4 0 13 0";
			%length = 1;
		case "Shower":
			%data = "5 0 10 0 10 0 5 0";
			%length = 2;
		case "Mills Mess":
			%data = "-1 0 -12 0 0 0 12 0 1 0 -12 0";
			%length = 3;
		case "Windmill":
			%data = "10 0 -8 0 -8 0 10 0";
			%length = 2;
		case "Random":
			%length = strlen(%this.getSite());
			%i = %length * 4;
			%data = "";
			while(%i >= 0) {
				%data = %data SPC getRandom(-15,15);
				%data = %data SPC getRandom(0,10);
				%i-=2;
			}
		default:
			%this.setStyleDefault();
			return 1;
	}
	%this.setStyle(%name, %length, %data);
	return 0;
}

function JMLib::setStyleDefault(%this) {
	%this.setStyleNamed("Normal");
}

function JMLib::setHR(%this,%hr) {
	if (%hr > $HR_MAX) {
		%this.height_ratio = $HR_MAX;
	} else if (%hr < $HR_MIN) {
		%this.height_ratio = $HR_MIN;
	} else {
		%this.height_ratio = %hr;
	}
}

function JMLib::getHR(%this) {
	return(%this.height_ratio);
}

function JMLib::setDR(%this,%dr) {
	if (%dr > $DR_MAX) {
		%this.dwell_ratio = $DR_MAX;
	} else if (%dr < $DR_MIN) {
		%this.dwell_ratio = $DR_MIN;
	} else {
		%this.dwell_ratio = %dr;
	}
}

function JMLib::getDR(%this) {
	return(%this.dwell_ratio);
}

function JMLib::getSite(%this) {
	return(%this.siteswap);
}

function JMLib::getPattName(%this) {
	return(%this.pattname);
}

function JMLib::getStyle(%this) {
	return(%this.stylename);
}

function JMLib::getImageWidth(%this) {
	return(%this.imageWidth);
}

function JMLib::getImageHeight(%this) {
	return(%this.imageHeight);
}

function JMLib::doStepcalc(%this) {
	// pos: position in string
	// stp: position in steps array
	%pos = 0;
	%stp = 0;

	%s = %this.siteswap;
  
	// reset
	for (%i = 0; %i < $LMAX; %i++) {
		%this.steps[%i] = -1;
	}
  
	// Synchronous pattern
	if (%this.syn) {
		while (%pos <= strlen(%this.siteswap)) {
			%c = getSubStr(%s, 0, 1);
			if (%c $= "(") {
				%this.steps[%stp] = %pos;
				%stp+=2;
				while(%c !$= ")") {
					%s = getSubStr(%s, 1, strlen(%s)-1);
					%c = getSubStr(%s, 0, 1);
					%pos++;
				}        
				%pos++;
				%s = getSubStr(%s, 1, strlen(%s)-1);
			} else if (strlen(%s) == 0) {
				%this.steps[%stp] = %pos;
				break;
			} else {
				%this.error("Internal error; synchronous pattern not");
				return;
			}
		}
	} else {
		while (%pos <= strlen(%this.siteswap)) {
			%c = getSubStr(%s, 0, 1);
			if (%c $= "(") {
				%this.error("Internal error; non-synchronous pattern is");
				return;
			} else if (%c $= "[") {
				// Multiplex
				%this.steps[%stp] = %pos;
				%stp++;
				while(%c !$= "]") {
					%s = getSubStr(%s, 1, strlen(%s)-1);
					%c = getSubStr(%s, 0, 1);
					%pos++;
				}
				%pos++;
				if(%pos > strlen(%this.siteswap)) {
					break;
				}
				%s = getSubStr(%s, 1, strlen(%s)-1);
			} else {
				// Normal throw
				%this.steps[%stp] = %pos;
				%pos++;
				%stp++;
				if(%pos > strlen(%this.siteswap)) {
					break;
				}
				%s = getSubStr(%s, 1, strlen(%s)-1);
			}
		}
	}
}

function JMLib::hand_pos_x(%this, %c, %h) {
	if(%this.syn == 0 && %h == 0) {
		%c--;
	}

	if(%c & 1) {
		%a=((%c-%h) % %this.style_len)*4+2;
	} else {
		%a=((%c+1-%h) % %this.style_len)*4;
	}
  
	if(%h != 0) {
		return %this.styledata[%a];
	} else {
		return -%this.styledata[%a];
	}
}

function JMLib::hand_pos_z(%this, %c, %h) {
	if(%this.syn == 0 && %h == 0) {
		%c--;
	}

	if(%c & 1) {
		%a=((%c-%h) % %this.style_len)*4+2;
	} else {
		%a=((%c+1-%h) % %this.style_len)*4;
	}

	return %this.styledata[%a+1];
}

function JMLib::startJuggle(%this) {
	%this.set_dpm();
	if (%this.set_ini(1) != 0) {
		return;
	}

	// apply corrections
	%this.time_count=0;
	%this.time_period=0;
}

function JMLib::doJuggle(%this) {
	%this.time_count++;
	if (%this.time_count < %this.aw) {
		%this.time_count = %this.aw;
	}

	for (%i=0; %i < %this.balln; %i++) {
		%this.juggle(%this.b[%i]);
	}

	%this.juggle(%this.rhand);
	%this.juggle(%this.lhand);
  
	%this.arm_line();
}

