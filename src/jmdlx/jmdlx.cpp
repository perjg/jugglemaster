/*
 * JMDeluxe - Portable JuggleMaster based on wxWindows
 * (C) Per Johan Persson 2002, Gary Briggs 2003
 *
 * JuggleMaster is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */ 

#include "jmdlx.h"
#include <wx/dcbuffer.h>    //wxBufferedDC

// JMLib* jmlib;

IMPLEMENT_APP(JMApp)

bool JMApp::OnInit() {
  // Create main window frame
  int windowx,windowy;

  windowx = min(480,wxGetDisplaySize().x);
  windowy = min(400,wxGetDisplaySize().y);

  jmlib = JMLib::alloc_JuggleMaster();
  //jmlib = JMLib::alloc_JuggleSaver();
  jmlib->setWindowSize(windowx, windowy);
  jmlib->setPatternDefault();
  jmlib->setStyleDefault();
  jmlib->setScalingMethod(SCALING_METHOD_DYNAMIC);
  jmlib->startJuggle();

  wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
  cmdline.SetLogo("JuggleMaster Deluxe");
  
  if(cmdline.Parse() == -1) {
	exit(0);
  }
  wxString initialsiteswap,initialstyle,named_pattern,semaphore;

  if(cmdline.Found("help")) {
	cmdline.Usage();
	printf("\n Style can be anything in the \"Change Style\" menu, eg \"Mills Mess\"\n");
	exit(0);
  }
  if(cmdline.Found("style",&initialstyle)) {
	jmlib->setStyle((JML_CHAR *)(const char *)initialstyle);
  }
  if(cmdline.Found("pattern",&named_pattern)) {
	/* FIXME */
	printf("Named Pattern: %s\n",(const char *)named_pattern);
  }
  if(cmdline.Found("semaphore",&semaphore)) {
	/* FIXME */
	printf("Semaphore Requested: %s\n",(const char *)semaphore);
  }
  if (cmdline.GetParamCount() > 0) {
    initialsiteswap = cmdline.GetParam(0);
    if(!initialsiteswap.IsEmpty()) {
  		jmlib->setPattern((JML_CHAR *)(const char *)initialsiteswap,
				(JML_CHAR *)(const char *)initialsiteswap,HR_DEF, DR_DEF);
    }
  }

	frame = new JMFrame(NULL, -1, "JuggleMaster Deluxe",
		wxDefaultPosition, wxSize(windowx,windowy), jmlib);

	// Show the frame
	frame->Show(true);

	return true;
}

int JMApp::OnExit() {
	delete jmlib;
	return 0;
}

BEGIN_EVENT_TABLE(JMFrame, wxFrame)
    EVT_MENU(ID_EXIT,  JMFrame::OnClose)
    EVT_MENU(ID_ABOUT, JMFrame::OnAbout)
    EVT_MENU(CHANGE_SITESWAP_S, JMFrame::changeSiteSwap)
    EVT_MENU(CHANGE_SITESWAP_A, JMFrame::changeSiteSwapAdvanced)
    EVT_MENU(CHANGE_SITESWAP_R, JMFrame::changeSiteSwapRandom)
    EVT_MENU(CHANGE_STYLE_S, JMFrame::changeStyle)
    EVT_MENU(CHOOSE_PATTERN, JMFrame::choosePattern)
    EVT_MENU(CHOOSE_SEMAPHORE, JMFrame::chooseSemaphore)
    EVT_MENU(PRINT_PS, JMFrame::print)
    EVT_MENU(OPTION_MIRROR, JMFrame::changeMirror)
    EVT_MENU(OPTION_REDOWNLOAD, JMFrame::reDownload)
    EVT_MENU(OPTION_PAUSE, JMFrame::changePause)
    EVT_MENU(SPEED_UP, JMFrame::speedUp)
    EVT_MENU(SPEED_DOWN, JMFrame::speedDown)
    EVT_MENU(SPEED_RESET, JMFrame::speedReset)
#ifdef OPENGL_SUPPORT
    EVT_MENU(OPTION_OGL_AUTOROTATE, JMFrame::changeAutoRotate)
    EVT_MENU(OPTION_OGL_3D_MODE, JMFrame::changeRenderMode)
    EVT_MENU(OPTION_COLORBALLS, JMFrame::changeColorballs)
#endif
END_EVENT_TABLE()

JMFrame::JMFrame(wxWindow* parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size, JMLib *j) :
                       wxFrame(parent,id,title,pos,size) {
	SetIcon(wxIcon("IDI_WIZICON"));

  fileMenu = new wxMenu();
  optionsMenu = new wxMenu();
  helpMenu = new wxMenu();
  speedMenu = new wxMenu();

  fileMenu->Append(CHANGE_SITESWAP_S, "Change &SiteSwap (Simple)");
  fileMenu->Append(CHANGE_SITESWAP_A, "Change SiteSwap (Advanced)");
  fileMenu->Append(CHANGE_SITESWAP_R, "Change SiteSwap (Random)");
  fileMenu->AppendSeparator();
  fileMenu->Append(CHANGE_STYLE_S, "Change S&tyle");
  fileMenu->AppendSeparator();
  fileMenu->Append(CHOOSE_PATTERN, "Choose P&attern");
  fileMenu->Append(CHOOSE_SEMAPHORE, "Show Se&maphore");
  fileMenu->AppendSeparator();
  fileMenu->Append(PRINT_PS, "&Print...");
  fileMenu->AppendSeparator();
  fileMenu->Append(ID_EXIT, "E&xit");

  optionsMenu->AppendCheckItem(OPTION_MIRROR, "&Mirror");
  optionsMenu->AppendCheckItem(OPTION_PAUSE, "&Pause");
  optionsMenu->AppendCheckItem(OPTION_COLORBALLS, "&Color Balls");
  optionsMenu->Append(OPTION_REDOWNLOAD, "Re&Download Patterns");

  // OpenGL specific options
#ifdef OPENGL_SUPPORT
  optionsMenu->AppendCheckItem(OPTION_OGL_AUTOROTATE, "&Rotate Automatically");
  optionsMenu->AppendCheckItem(OPTION_OGL_3D_MODE, "3&D Mode");

  optionsMenu->Check(OPTION_OGL_AUTOROTATE, TRUE);
  optionsMenu->Check(OPTION_OGL_3D_MODE, TRUE);
#endif

  speedMenu->Append(SPEED_UP,"&Up");
  speedMenu->Append(SPEED_DOWN,"&Down");
  speedMenu->Append(SPEED_RESET,"&Reset");

  helpMenu->Append(ID_ABOUT, "&About");

  // The menu bar
  wxMenuBar* menuBar = new wxMenuBar();

  menuBar->Append(fileMenu, "&File");
  menuBar->Append(optionsMenu, "&Options");
  menuBar->Append(speedMenu, "&Speed");
  menuBar->Append(helpMenu, "&Help");
  SetMenuBar(menuBar);

  // Initialize jmlib
  jmlib = j;
  jmlib->setErrorCallback(this, ErrorCallBack);

  // The draw canvas
#ifdef OPENGL_SUPPORT
  if (OpenGLSupported()) {
    glCanvas = new JMOpenGLCanvas(this, jmlib);
    timer = new JMTimer(NULL, glCanvas, jmlib);
  }
  else {
    canvas = new JMCanvas(this, jmlib);
    timer = new JMTimer(canvas, NULL, jmlib);
  }
#else
  canvas = new JMCanvas(this, jmlib);
  timer = new JMTimer(canvas, jmlib);
#endif

  // Initialise Pattern Loader
  patterns = new PatternLoader();
  semaphores = new PatternLoader(NULL, DEFAULT_SEMAPHOREFILE);

}

JMFrame::~JMFrame() {
  delete timer;
  delete patterns;
  delete semaphores;
}

void JMFrame::OnAbout(wxCommandEvent &WXUNUSED(event)) {
	wxMessageBox("(C) Ken Matsuoka 1995-6, Per Johan Groland 2002, Gary Briggs 2003", "About JMDeluxe", wxOK, this);
}

void JMFrame::changeMirror(wxCommandEvent& WXUNUSED(event)) {
	jmlib->setMirror(optionsMenu->IsChecked(OPTION_MIRROR));
}

void JMFrame::reDownload(wxCommandEvent& WXUNUSED(event)) {
	delete patterns;
	delete semaphores;
	patterns = new PatternLoader(this, DEFAULT_PATTERNFILE, 1);
	semaphores = new PatternLoader(this, DEFAULT_SEMAPHOREFILE, 1);
}

void JMFrame::setPause() {
	optionsMenu->Check(OPTION_PAUSE,TRUE);
	jmlib->setPause(optionsMenu->IsChecked(OPTION_PAUSE));
}

void JMFrame::unPause() {
	optionsMenu->Check(OPTION_PAUSE,FALSE);
	jmlib->setPause(optionsMenu->IsChecked(OPTION_PAUSE));
}

void JMFrame::togglePause() {
  if (optionsMenu->IsChecked(OPTION_PAUSE))
    unPause();
  else
    setPause();
}

void JMFrame::changePause(wxCommandEvent& WXUNUSED(event)) {
	jmlib->setPause(optionsMenu->IsChecked(OPTION_PAUSE));
}

void JMFrame::changeSiteSwapAdvanced(wxCommandEvent& WXUNUSED(event)) {
  new AdvancedSiteSwap(this,jmlib);
  // unPause();
}

void JMFrame::changeSiteSwapRandom(wxCommandEvent& WXUNUSED(event)) {
  new RandomSiteSwap(this,jmlib);
}
void JMFrame::setSiteSwap(wxString *newsite) {
  jmlib->setPattern((JML_CHAR *)(const char *)*newsite,
		(JML_CHAR *)(const char *)*newsite,HR_DEF, DR_DEF);
}

void JMFrame::setStyle(wxString *newstyle) {
  jmlib->setStyle((JML_CHAR *)(const char *)*newstyle);
}

void JMFrame::changeSiteSwap(wxCommandEvent& WXUNUSED(event))
{
  JML_CHAR *newpattern;
  wxTextEntryDialog dialog(this,
                           "Change SiteSwap",
                           "Please Enter SiteSwap Here",
			   jmlib->getSite(),
                           wxOK | wxCANCEL | wxCENTRE);

  if (dialog.ShowModal() == wxID_OK)
  {
	newpattern = (JML_CHAR *)(const char *)dialog.GetValue();
        jmlib->stopJuggle();
	jmlib->setPattern(newpattern,newpattern,HR_DEF, DR_DEF);
        jmlib->setStyleDefault();
        jmlib->startJuggle();
	unPause();
  }
  else {
      unPause();
  }
}

void JMFrame::changeStyle(wxCommandEvent& WXUNUSED(event))
{
  new ChooseStyle(this,jmlib);
}

void JMFrame::choosePattern(wxCommandEvent& WXUNUSED(event))
{
  if(patterns) {
	if(patterns->Usable()) {
  		new ChoosePatt(this,jmlib,patterns);
		return;
	}
  }
  wxMessageDialog *popup = new wxMessageDialog(this, "No Patterns Loaded!", "Error", wxOK|wxICON_ERROR);
  popup->ShowModal();
}

void JMFrame::chooseSemaphore(wxCommandEvent& WXUNUSED(event))
{
  if(semaphores) {
	if(semaphores->Usable()) {
		new ChooseSemaphore(this,jmlib,semaphores);
		return;
	}
  }
  wxMessageDialog *popup = new wxMessageDialog(this, "No Semaphores Loaded!", "Error", wxOK|wxICON_ERROR);
  popup->ShowModal();
}

void JMFrame::print(wxCommandEvent& WXUNUSED(event))
{
	new Print(this, jmlib);
}

#ifdef OPENGL_SUPPORT
void JMFrame::changeAutoRotate(wxCommandEvent& WXUNUSED(event)) {
  if (optionsMenu->IsChecked(OPTION_OGL_AUTOROTATE))
    glCanvas->enableAutoRotate();
  else
    glCanvas->disableAutoRotate();
}
#endif


#ifdef OPENGL_SUPPORT
void JMFrame::changeRenderMode(wxCommandEvent& WXUNUSED(event)) {
  if (optionsMenu->IsChecked(OPTION_OGL_3D_MODE))
    glCanvas->setRenderMode3D();
  else
    glCanvas->setRenderModeFlat();
}
#endif

#ifdef OPENGL_SUPPORT
void JMFrame::changeColorballs(wxCommandEvent& WXUNUSED(event)) {
  glCanvas->ballColors(optionsMenu->IsChecked(OPTION_COLORBALLS));
}
#endif

void JMFrame::OnClose(wxCommandEvent &WXUNUSED(event)) {
	Close(TRUE);
}

void JMFrame::OnPaint(wxPaintEvent &WXUNUSED(event)) {

}

void JMFrame::speedUp(wxCommandEvent& WXUNUSED(event)) {
	jmlib->speedUp();
}

void JMFrame::speedDown(wxCommandEvent& WXUNUSED(event)) {
	jmlib->speedDown();
}

void JMFrame::speedReset(wxCommandEvent& WXUNUSED(event)) {
	jmlib->speedReset();
}

void JMFrame::ErrorCallBack(void *aUData, JML_CHAR *aErrMsg) {
  /* Massive thanks go to Colin Bayer for his teaching me how this works */
  wxString error_message;
  error_message = aErrMsg;
  wxMessageDialog *message = new wxMessageDialog((JMFrame *)aUData, error_message, "Error", wxOK|wxICON_ERROR);
  message->ShowModal();
}

BEGIN_EVENT_TABLE(JMCanvas, wxScrolledWindow)
  EVT_PAINT(JMCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(JMCanvas::OnEraseBackground) 
  EVT_SIZE(JMCanvas::OnSize)
  EVT_LEFT_DOWN(JMCanvas::OnLMouseDown)
END_EVENT_TABLE()


JMCanvas::JMCanvas(JMFrame *p, JMLib *j) : 
  wxScrolledWindow( p, -1, wxDefaultPosition,
		wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE ) {
	jmlib = j;
	parent = p;
	ball_colors[0] = (wxBrush*)wxRED_BRUSH;
	ball_colors[1] = (wxBrush*)wxGREEN_BRUSH;
	ball_colors[2] = (wxBrush*)wxBLUE_BRUSH;
	ball_colors[3] = (wxBrush*)wxWHITE_BRUSH;
	ball_colors[4] = (wxBrush*)wxBLACK_BRUSH;
	ball_colors[5] = (wxBrush*)wxCYAN_BRUSH;
	ball_colors[6] = (wxBrush*)wxGREY_BRUSH;
	ball_colors[7] = (wxBrush*)wxLIGHT_GREY_BRUSH;
	backBuffer = new wxBitmap(wxGetDisplaySize().x, wxGetDisplaySize().y);
}

JMCanvas::~JMCanvas() {
	delete backBuffer;
}

/*
    Refactor to use wxBufferedPaintDC
*/
void JMCanvas::OnPaint(wxPaintEvent &WXUNUSED(event)) {
  
  //wxMemoryDC dc;
  int i;
  // wxPaintDC dc(this);
  //dc.SelectObject(*backBuffer);
  //PrepareDC(dc);
  wxBufferedPaintDC dc(this);
  arm* ap = &(jmlib->ap);
  ball* rhand = &(jmlib->rhand);
  ball* lhand = &(jmlib->lhand);
  hand* handp = &(jmlib->handpoly);

  dc.SetBackground(*wxWHITE_BRUSH);
  // dc.SetBackground(wxBrush(wxColor(r,g,b),wxSOLID);
  dc.SetPen(*wxBLACK_PEN);
  dc.Clear();

  // draw head
  dc.DrawEllipse(ap->hx - ap->hr, ap->hy - ap->hr, ap->hr*2, ap->hr*2);

  // draw juggler
  for(i=0;i<5;i++){
    dc.DrawLine(ap->rx[i], ap->ry[i], ap->rx[i+1], ap->ry[i+1]);
    dc.DrawLine(ap->lx[i], ap->ly[i], ap->lx[i+1], ap->ly[i+1]);
  }

  // hands
  for (i=0; i <= 8; i++) {
    dc.DrawLine(rhand->gx + handp->rx[i], rhand->gy + handp->ry[i],
                rhand->gx + handp->rx[i+1], rhand->gy + handp->ry[i+1]);
    dc.DrawLine(lhand->gx + handp->lx[i], lhand->gy + handp->ly[i],
                lhand->gx + handp->lx[i+1], lhand->gy + handp->ly[i+1]);
  }

  dc.DrawLine(rhand->gx + handp->rx[9], rhand->gy + handp->ry[9],
              rhand->gx + handp->rx[0], rhand->gy + handp->ry[0]);
  dc.DrawLine(lhand->gx + handp->lx[9], lhand->gy + handp->ly[9],
              lhand->gx + handp->lx[0], lhand->gy + handp->ly[0]);

  dc.SetBrush(*wxRED_BRUSH);

  // draw balls
  int diam = jmlib->getBallRadius()*2;
  for(i=jmlib->numBalls()-1;i>=0;i--) {
    if(parent->optionsMenu->IsChecked(OPTION_COLORBALLS)) {
      dc.SetBrush(*ball_colors[i%NUMBALLCOLORS]);
    }

    dc.DrawEllipse(jmlib->b[i].gx, jmlib->b[i].gy, diam, diam);
  }

  // Draw pattern info on screen
  int start = (int)jmlib->getSiteposStart(); 
  int stop  = (int)jmlib->getSiteposStop();
  int diff = stop - start;
  wxCoord x = 10;
  wxCoord y = 10;
  wxCoord strWidth = 0;
  wxCoord strHeight = 0;

  wxString sitetext("Site: ");
  wxString site(jmlib->getSite());

  dc.DrawText(sitetext, x, y);
  dc.GetTextExtent(sitetext, &strWidth, &strHeight);
  x += strWidth;

  // First part of string
  if (start > 0) {
    dc.DrawText(site.Mid(0, start), x, y);
    dc.GetTextExtent(site.Mid(0, start), &strWidth, &strHeight);
	x += strWidth;
  }

  // Active part of string
  dc.SetTextForeground(wxColour(255, 0, 0));
  dc.DrawText(site.Mid(start, diff), x, y);
  dc.GetTextExtent(site.Mid(start, diff), &strWidth, &strHeight);
  x += strWidth;

  // Last part of string
  dc.SetTextForeground(wxColour(0, 0, 0));
  dc.DrawText(site.Mid(stop), x, y);
  dc.GetTextExtent(site.Mid(stop), &strWidth, &strHeight);
  x += strWidth;

  // More pattern information
  wxString balltext;
  balltext.Printf("    Style: %s    Balls: %i", jmlib->getStyle(), jmlib->numBalls());
  dc.DrawText(balltext, x, y);

  // flip
 // wxPaintDC sdc(this);
  //PrepareDC(sdc);

  //sdc.Blit(0, 0, GetSize().x, GetSize().y, &dc, 0, 0);
}

void JMCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {

}

void JMCanvas::OnSize(wxSizeEvent &event) {
	jmlib->setWindowSize((JML_INT32)event.GetSize().x,(JML_INT32)event.GetSize().y);
	if(backBuffer) {
		backBuffer->SetWidth(event.GetSize().x);
		backBuffer->SetHeight(event.GetSize().y);
	}
	event.Skip();
}

void JMCanvas::OnLMouseDown(wxMouseEvent& event) {
  parent->togglePause();
  event.Skip();
}

#ifdef OPENGL_SUPPORT
JMTimer::JMTimer(JMCanvas *c, JMOpenGLCanvas* glc, JMLib *j) : wxTimer() {
	canvas = c;
	glCanvas = glc;
	jmlib = j;
	current_delay = 30;
	Start(current_delay);
}
#else
JMTimer::JMTimer(JMCanvas *c, JMLib *j) : wxTimer() {
	canvas = c;
	jmlib = j;
	current_delay = 30;
	Start(current_delay);
}
#endif


void JMTimer::Notify() {
  jmlib->doJuggle();
  if (canvas)   canvas->Refresh();
#ifdef OPENGL_SUPPORT
  if (glCanvas) glCanvas->Refresh();
#endif
  wxWakeUpIdle();
  wxYieldIfNeeded();
}

int JMTimer::SpeedUp() {
	if(current_delay <= JMTIMER_MIN) {
		return(0);
	} else {
		current_delay -= 10;
		Start(current_delay);
		return(1);
	}
}

int JMTimer::SpeedDown() {
	if(current_delay >= JMTIMER_MAX) {
		return(0);
	} else {
		current_delay += 10;
		Start(current_delay);
		return(1);
	}
}

void JMTimer::SpeedReset() {
	current_delay = 30;
	Start(current_delay);
}
