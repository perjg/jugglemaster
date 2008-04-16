#if 0

/*******************************************************************************
 *
 *  XScreenSaver Configuration
 *
 ******************************************************************************/

typedef struct
{
    GLXContext* glxContext;
    RENDER_STATE RenderState;
    float CurrentFrameRate;
    unsigned FramesSinceSync;
    unsigned LastSyncTime;
} JUGGLER3D_CONFIG;


static JUGGLER3D_CONFIG* pConfigInfo = NULL;
static int MaxObjects;
static int MinObjects;
static int MaxHeightInc;
static int MinHeightInc;
static float SpinSpeed;
static float TranslateSpeed;
static float JuggleSpeed;

static XrmOptionDescRec Options[] =
{
    {"-spin", ".spinSpeed", XrmoptionSepArg, 0},
    {"-trans", ".translateSpeed", XrmoptionSepArg, 0},
    {"-speed", ".juggleSpeed", XrmoptionSepArg, 0},
    {"-maxobjs", ".maxObjs", XrmoptionSepArg, 0},
    {"-minobjs", ".minObjs", XrmoptionSepArg, 0},
    {"-maxhinc", ".maxHInc", XrmoptionSepArg, 0},
    {"-minhinc", ".minHInc", XrmoptionSepArg, 0},
};


static argtype Vars[] = 
{
    {&MaxObjects, "maxObjs", "MaxObjs", "8", t_Int},
    {&MinObjects, "minObjs", "MinObjs", "3", t_Int},
    {&MaxHeightInc, "maxHInc", "MaxHInc", "6", t_Int},
    {&MinHeightInc, "minHInc", "MinHInc", "2", t_Int},
    {&JuggleSpeed, "juggleSpeed", "JuggleSpeed", "2.2", t_Float},
    {&TranslateSpeed, "translateSpeed", "TranslateSpeed", "0.1", t_Float},
    {&SpinSpeed, "spinSpeed", "SpinSpeed", "20.0", t_Float},
};


ENTRYPOINT ModeSpecOpt juggler3d_opts = {countof(Options), Options, countof(Vars), Vars};


ENTRYPOINT void reshape_juggler3d(ModeInfo *mi, int width, int height)
{
    JUGGLER3D_CONFIG* pConfig = &pConfigInfo[MI_SCREEN(mi)];
    ResizeGL(&pConfig->RenderState, width, height);
}


ENTRYPOINT void init_juggler3d(ModeInfo* mi)
{
    JUGGLER3D_CONFIG* pConfig;
    
    if (pConfigInfo == NULL)
    {
        /* Apply suitable bounds checks to the input parameters */
        MaxObjects = max(3, min(MaxObjects, 36));
        MinObjects = max(3, min(MinObjects, MaxObjects));

        MaxHeightInc = max(1, min(MaxHeightInc, 32));
        MinHeightInc = max(1, min(MinHeightInc, MaxHeightInc));
            
        pConfigInfo = (JUGGLER3D_CONFIG*) calloc(
            MI_NUM_SCREENS(mi), sizeof(JUGGLER3D_CONFIG));
        if (pConfigInfo == NULL)
        {
            fprintf(stderr, "%s: out of memory\n", progname);
            exit(1);
        }
    }
    
    pConfig = &pConfigInfo[MI_SCREEN(mi)];
    pConfig->glxContext = init_GL(mi);
    pConfig->CurrentFrameRate = 0.0f;
    pConfig->FramesSinceSync = 0;
    pConfig->LastSyncTime = 0;
    InitGLSettings(&pConfig->RenderState, MI_IS_WIREFRAME(mi));

    UpdatePattern(&pConfig->RenderState, MinObjects, MaxObjects, 
        MinHeightInc, MaxHeightInc);
    
    reshape_juggler3d(mi, MI_WIDTH(mi), MI_HEIGHT(mi));
}


ENTRYPOINT void draw_juggler3d(ModeInfo* mi)
{
    JUGGLER3D_CONFIG* pConfig = &pConfigInfo[MI_SCREEN(mi)];
    Display* pDisplay = MI_DISPLAY(mi);
    Window hwnd = MI_WINDOW(mi);

    if (pConfig->glxContext == NULL)
        return;

    glXMakeCurrent(MI_DISPLAY(mi), MI_WINDOW(mi), *(pConfig->glxContext));
    
    /* While drawing, keep track of the rendering speed so we can adjust the
     * animation speed so things appear consistent.  The basis of the this
     * code comes from the frame rate counter (fps.c) but has been modified
     * so that it reports the initial frame rate earlier (after 0.02 secs
     * instead of 1 sec). */
    
    if (pConfig->FramesSinceSync >=  1 * (int) pConfig->CurrentFrameRate)
    {
        struct timeval tvnow;
        unsigned now;
            
        # ifdef GETTIMEOFDAY_TWO_ARGS
            struct timezone tzp;
            gettimeofday(&tvnow, &tzp);
        # else
            gettimeofday(&tvnow);
        # endif
        
        now = (unsigned) (tvnow.tv_sec * 1000000 + tvnow.tv_usec);
        if (pConfig->FramesSinceSync == 0)
        {
            pConfig->LastSyncTime = now;
        }
        else
        {
            unsigned Delta = now - pConfig->LastSyncTime;
            if (Delta > 20000)
            {
                pConfig->LastSyncTime = now;
                pConfig->CurrentFrameRate = 
                    (pConfig->FramesSinceSync * 1.0e6f) / Delta;
                pConfig->FramesSinceSync = 0;
            }
        }
    }
    
    pConfig->FramesSinceSync++;
    
    if (pConfig->RenderState.Time > 150.0f)
    {
        UpdatePattern(&pConfig->RenderState, MinObjects, MaxObjects, 
            MinHeightInc, MaxHeightInc);
    }
    DrawGLScene(&pConfig->RenderState);
    
    if (pConfig->CurrentFrameRate > 1.0e-6f)
    {
        pConfig->RenderState.Time += JuggleSpeed / pConfig->CurrentFrameRate;
        pConfig->RenderState.SpinAngle += SpinSpeed / pConfig->CurrentFrameRate;
        pConfig->RenderState.TranslateAngle += 
            TranslateSpeed / pConfig->CurrentFrameRate;
    }
    
    if (mi->fps_p)
        do_fps(mi);
  
    glFinish();
    glXSwapBuffers(pDisplay, hwnd);
}


ENTRYPOINT Bool juggler3d_handle_event(ModeInfo* mi, XEvent* pEvent)
{
  JUGGLER3D_CONFIG* pConfig = &pConfigInfo[MI_SCREEN(mi)];
  RENDER_STATE* pState = &pConfig->RenderState;

    if (pEvent->xany.type == ButtonPress &&
        pEvent->xbutton.button == Button1)
    {
      pState->button_down_p = True;
      gltrackball_start (pState->trackball,
                         pEvent->xbutton.x, pEvent->xbutton.y,
                         MI_WIDTH (mi), MI_HEIGHT (mi));
      return True;
    }
    else if (pEvent->xany.type == ButtonRelease &&
             pEvent->xbutton.button == Button1)
    {
      pState->button_down_p = False;
      return True;
    }
    else if (pEvent->xany.type == ButtonPress &&
             (pEvent->xbutton.button == Button4 ||
              pEvent->xbutton.button == Button5 ||
              pEvent->xbutton.button == Button6 ||
              pEvent->xbutton.button == Button7))
    {
      gltrackball_mousewheel (pState->trackball, pEvent->xbutton.button, 2,
                              !pEvent->xbutton.state);
      return True;
    }
    else if (pEvent->xany.type == MotionNotify &&
             pState->button_down_p)
    {
      gltrackball_track (pState->trackball,
                         pEvent->xmotion.x, pEvent->xmotion.y,
                         MI_WIDTH (mi), MI_HEIGHT (mi));
      return True;
    }
    else if (pEvent->xany.type == KeyPress)
    {
        char str[20];
        KeySym Key = 0;
        int count = XLookupString(&pEvent->xkey, str, 20, &Key, 0);
        str[count] = '\0';
        if (*str == ' ')
        {
            UpdatePattern(&pConfig->RenderState, MinObjects, MaxObjects, 
                MinHeightInc, MaxHeightInc);
        }
    }
    
    return False;
}

XSCREENSAVER_MODULE ("Juggler3D", juggler3d)

#endif
