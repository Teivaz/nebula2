#ifdef __LINUX__
#define N_IMPLEMENTS nGLServer2
//------------------------------------------------------------------------------
//  X & GLX specific window handling
//  17-May-2003 cubejk  ported form nebula1
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"


//-----------------------------------------------------------------------------
/**
*/
nGLServer2::DeviceClose()
{
    //clean GLX windows
    glXDestroyWindow(this->display, this->glx_ctx_win);
    glXDestroyWindow(this->display, this->glx_fs_win);
    this->glx_current_win = NULL;

    //destroy context
    glXDestroyContext(this->display, this->context);
}

//------------------------------------------------------------------------------
/**
    Open and initialize the opengl context, make the app window visible.
*/
bool
nGLServer2::WindowOpen()
{
    //open connection to Xserver default display
    this->display = XOpenDisplay(NULL);
    if (!this->display)
    {
        n_error("Can't open a connection to the XServer.\n");
        return;
    }

    //get the default screen of this display
    this->screen = DefaultScreen(this->display);

    // test, if X Server supports glX
    if (!glXQueryExtension(this->display,NULL,NULL))
    {
        n_error("XServer don't support GLX.\n");
        return;
    }

    //test GLX version
    int major;
    int minor;
    if (!glXQueryVersion(this->display, &major, &minor))
    {
        n_error("Can't get version of GLX. Failed!\n");
        return;
    }

    if ((minor < 3) || (major < 1))
    {
        char temp[255];
        sprintf(&temp[0],"GLX Version %i.%i not supported, you need GLX >= 1.3.\n",major,minor);
        n_error(&temp[0]);
        return;
    }

    //display GLX info
    //TODO: add a version, with lesser output
    this->showGLXInfo();

    ///minimum GLX FrameBufferConfig
    static const int minimumConfig[] = {
        GLX_RED_SIZE,   8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE,  8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 16,
        //GLX_DOUBLEBUFFER,
        GLX_X_VISUAL_TYPE , GLX_TRUE_COLOR,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        None,
    };

    //get all fbconfigs that have the minimum buffersizes
    int num;
    GLXFBConfig *possibleConfig = glXChooseFBConfig(
        this->display,
        this->screen,
        minimumConfig,
        &num);

    if (num < 1)
    {
        n_error("No valid frame buffer config found. Minmum is R8G8B8A8 D16S0. Failed!\n");
        return;
    }
    else
    {
        //the possibleConfigurations a nice sorted by GLX, so we can chose the 1. one
        this->selectedConfig = possibleConfig[0];//TODO/BUG: should I do a memcopy?
        XFree(possibleConfig);
        int r = this->getBufferAttr(this->selectedConfig, GLX_RED_SIZE);
        int g = this->getBufferAttr(this->selectedConfig, GLX_GREEN_SIZE);
        int b = this->getBufferAttr(this->selectedConfig, GLX_BLUE_SIZE);
        int a = this->getBufferAttr(this->selectedConfig, GLX_ALPHA_SIZE);
        int d = this->getBufferAttr(this->selectedConfig, GLX_DEPTH_SIZE);
        int s = this->getBufferAttr(this->selectedConfig, GLX_STENCIL_SIZE);
        n_printf("Select frame buffer config: R%iG%iB%iA%i D%iS%i.\n",r,g,b,a,d,s);
    }

    //get a XVisualInfo this FrameBuffer config
    this->xvi = glXGetVisualFromFBConfig(this->display, this->selectedConfig);

    if (this->xvi == NULL)
    {
        n_error("Can't get a appropriate RGB visual for this fbConfig.\n");
        return;
    }

    //create the OpenGL rendercontext with direct rendering, if possible
    this->context = glXCreateContext(this->display, this->xvi, NULL, GL_TRUE);
    if (this->context == NULL)
    {
       n_error("Can't create a OpenGL Context!\n");
       return;
    }

    if (glXIsDirect(this->display,this->context))
    {
        n_printf("Created a DirectRendering OpenGL context.\n");
    }
    else
    {
        n_printf("Created a OpenGL context. Notice this is not a DirectRendering context may you get low performance.\n");
    }

    //Check for XF86VIDMODE extension
    int opcode, evbase, everror;
    this->ext_XF86VIDMODE = XQueryExtension (
        this->display,
        "XFree86-VidModeExtension",
        &opcode, &evbase, &everror);

    this->XWindowOpen();

    //create a glxdrawable for the fullscreen window
    if (this->ext_XF86VIDMODE)
    {
        this->glx_fs_win = glXCreateWindow(this->display, this->selectedConfig, this->fs_win, NULL);
    }

    //create a glxdrawable for the windowed mode window
    this->glx_ctx_win = glXCreateWindow(this->display, this->selectedConfig, this->ctx_win, NULL);

    //preselect the windowed mode windows as current
    this->current_win = &this->ctx_win;
    this->glx_current_win = &this->glx_ctx_win;

    glXMakeContextCurrent(this->display, /*draw*/ *this->glx_current_win, /*read*/ *this->glx_current_win, this->context);
}

void
nGLServer2::ContextOpen(void)
{
    n_assert(this->display);
    n_assert(this->context);
    n_assert(this->glx_current_win)
    //get the context, in the future this happens in BeginScene();
}

//-----------------------------------------------------------------------------
/**
    create and setup Xwindows
*/
void
nGLServer2::xWindowOpen()
{
    unsigned long valueMask, eventMask, selectinputMask;
    char* winTitle = (char*) this->windowTitle.Get();

    XSetWindowAttributes swa;
    memset(&swa,0,sizeof(swa));
    swa.colormap = this->getColormap();
    swa.border_pixel = 0;
    swa.override_redirect = true;

    if (this->ext_XF86VIDMODE)
    {
        n_printf("Fullscreen supported.\n");
        //create fullscreen window
        valueMask = (
            CWOverrideRedirect |
            CWBorderPixel |
            CWColormap);
        eventMask = 0;
        selectinputMask  = 0;

        swa.event_mask = eventMask;
        this->fs_win = XCreateWindow (
                        this->display,
                        RootWindow(this->display, this->xvi->screen),
                        0, 0, 1, 1, /*x, y, height, width*/
                        0 /*border*/,
                        this->xvi->depth,
                        InputOutput,
                        this->xvi->visual,
                        valueMask,
                        &swa);
        XSelectInput (this->display, this->fs_win, selectinputMask);

        //???
        XSetWindowBackground (
            this->display, this->fs_win,
            BlackPixel (this->display, this->xvi->screen));
    }
    else
    {
        n_printf("Fullscreen not supported.\n");
    }

    //--------------------------------------------------------------------
    //create windowmanager window
    // Technique inspired by GTK to ensure correct window behaviour
    // under some more pedantic window managers.
    // Create a group leading window for the window manager:
    valueMask  = (
        CWBorderPixel |
        CWColormap |
        CWEventMask );

    eventMask = (
        StructureNotifyMask |
        FocusChangeMask |
        KeyPressMask |
        KeyReleaseMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask );

    selectinputMask  = (
        StructureNotifyMask |
        FocusChangeMask |
        KeyPressMask |
        KeyReleaseMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask );

    swa.event_mask = eventMask;
    this->wm_win = XCreateWindow(
                    this->display,
                    RootWindow(this->display,this->xvi->screen), // parent
                    16, 16, 256, 256, // arbitary location, dimensions
                    4,                // resizable border width
                    this->xvi->depth,
                    InputOutput,
                    this->xvi->visual,
                    valueMask,
                    &swa);
    XSelectInput(this->display, this->wm_win, selectinputMask);

    this->wmDeleteWindow = XInternAtom(this->display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(this->display, this->wm_win, &wmDeleteWindow, 1);

    XClassHint *class_hint = XAllocClassHint();
    class_hint->res_name =  winTitle;
    class_hint->res_class = winTitle;
    XmbSetWMProperties(
        this->display, this->wm_win,
        NULL, NULL, NULL, 0,
        NULL, NULL, class_hint);

    XFree(class_hint);

    // Resiziing hints
    XSizeHints normal_hints;
    normal_hints.flags      = PMinSize | PMaxSize | PSize | PResizeInc;
    normal_hints.width      = 640;// arbitary init dimensions
    normal_hints.height     = 480;
    normal_hints.width_inc  = 1;  // step size of resize
    normal_hints.height_inc = 1;
    normal_hints.min_width  = 32;
    normal_hints.min_height = 32;
    normal_hints.max_width  = DisplayWidth (this->display, this->screen);
    normal_hints.max_height = DisplayHeight(this->display, this->screen);
    XSetWMNormalHints(this->display, this->wm_win, &normal_hints);

    // Window manager hints
    XWMHints wm_hints;
    wm_hints.flags = InputHint | StateHint | WindowGroupHint;
    wm_hints.input = True;
    wm_hints.window_group = wm_win;
    wm_hints.initial_state = NormalState;
    XSetWMHints(this->display, this->wm_win, &wm_hints);

    XmbSetWMProperties(
        this->display, this->wm_win,
        this->windowTitle.Get(), this->windowTitle.Get(),
        NULL, 0, NULL, NULL, NULL);

    //--------------------------------------------------------------------
    //create context window
    //the context window is where draw operations happens, if we are in windowed mode
    valueMask = (
        CWOverrideRedirect |
        CWBorderPixel |
        CWColormap |
        CWEventMask );

    eventMask  = (
        KeyPressMask |
        KeyReleaseMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask );

    selectinputMask  = (
        KeyPressMask |
        KeyReleaseMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask );

    swa.event_mask = eventMask;
    this->ctx_win = XCreateWindow(
                        this->display,    // display
                        this->wm_win, // make child of the window manager window
                        0, 0, 32, 32, // x, y, w, h
                        0,            // border_width
                        this->xvi->depth,  // depth
                        InputOutput,  // class
                        this->xvi->visual, // visual
                        valueMask,    // valuemask
                        &swa);        // attributes

    XSelectInput(this->display, this->ctx_win, selectinputMask);

    /*
    //FIXME: whats this??? I just copy and paste it from nebula1, don't know what this should do.
    Atom wm_client_leader = XInternAtom (this->display, "WM_CLIENT_LEADER", false);
    //XChangeProperty(display, w, property, type, format, mode, data, nelements)
    XChangeProperty (
        this->display, this->ctx_win,
        wm_client_leader, XA_WINDOW, 32,
        PropModeReplace, (const unsigned char*)&this->wm_win, 1);
    */

    XmbSetWMProperties(
        this->display, this->ctx_win,
        winTitle, winTitle,
        NULL, 0, NULL, NULL, NULL);
}

//------------------------------------------------------------------------------
/**
    Start rendering the scene.
*/
bool
nGLServer2::BeginScene()
{
    //check if the window must be updated
    if (this->updateWindowMask)
    {
        this->updateWindow();
    }
    //make shure that the right context is used.
    GLXContext active = glXGetCurrentContext(void);
    if (active != this->context)
    {
        if (glXMakeContextCurrent(this->display, /*draw*/ *this->glx_current_win, /*read*/ *this->glx_current_win, this->context))
        {
            return true;
        }
    }
    return false;
}

void
nGLServer2::PresentScene()
{
    glXWaitGL();//? must this be done?
    glXWaitX(); //? must this be done?
    glXSwapBuffers(this->display, *this->glx_current_win);
}


//--------------------------------------------------------------------
//  xwin_proc()
//
//  16-Nov-98   floh    created
//  08-Dec-98   floh    + Input-Events
//  30-Jul-01   samuel  + added window management and resizing
//  02-Aug-01   samuel  + added full screen extension support
//  02-Aug-01   samuel  + fixed mouse button support
//--------------------------------------------------------------------
bool
nGLServer2::Trigger()

    // Processes everything in the XEventQueue, when nothing is left
    // to be done, the routine returns TRUE. In case wmDeleteWindow
    // comes down the pipe, the routine returns FALSE, so that the
    // app knows, that it should stop.
    bool retval = true;
    while (retval && XPending(this->display))
    {
        XNextEvent(this->display,&(this->event));
        switch(this->event.type)
        {
            case ConfigureNotify:
                if (event.xconfigure.window == this->wm_win)
                {
                    //resize window if needed
                    if (this->height != (uint) this->event.xconfigure.height)
                    {
                        this->updateWindowMask |= CWHeight;
                        this->height = this->event.xconfigure.height;
                    }
                    if (this->width != (uint) this->event.xconfigure.width)
                    {
                        this->updateWindowMask |= CWWidth;
                        this->width = this->event.xconfigure.width;
                    }
                }
                break;
            case ClientMessage:
                if ((Atom)this->event.xclient.data.l[0] == this->wmDeleteWindow)
                {
                    n_printf("Window was killed externally\n");
                    if (this->displayOpen) this->CloseDisplay();
                    // the window itself stays alive and is only finally
                    // destroyed in the destructor!
                }
                retval = false;
                break;

            case KeyPress:
                if (this->refInputServer.isvalid())
                {
                    XKeyEvent *ke = (XKeyEvent *) &(this->event);
                    KeySym ks = XLookupKeysym(ke,0);
                    nKey nk   = this->translateKey(ks);
                    nInputEvent *ie = this->refInputServer->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_KEY_DOWN);
                        ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                        ie->SetKey(nk);
                        this->refInputServer->LinkEvent(ie);
                    }
                }
                break;

            case KeyRelease:
                if (this->refInputServer.isvalid())
                {
                    // can generate 2 events, a KeyUp and possibly a Char
                    XKeyEvent *ke = (XKeyEvent *) &(this->event);
                    unsigned char buf[32];
                    int l;
                    KeySym ks = XLookupKeysym(ke,0);
                    nKey nk   = this->translateKey(ks);
                    nInputEvent *ie = this->refInputServer->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_KEY_UP);
                        ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                        ie->SetKey(nk);
                        this->refInputServer->LinkEvent(ie);
                    }
                    l = XLookupString((XKeyEvent *)&(this->event), (char *) buf, sizeof(buf), NULL, NULL);
                    if (l==1)
                    {
                        ie = this->refInputServer->NewEvent();
                        if (ie)
                        {
                            ie->SetType(N_INPUT_KEY_CHAR);
                            ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                            ie->SetChar((int)(buf[0]));
                            this->refInputServer->LinkEvent(ie);
                        }
                    }
                }
                break;

            case ButtonPress:
                n_printf("ButtonPress:");
                if (this->refInputServer.isvalid())
                {
                    nInputEvent *ie = this->refInputServer->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_BUTTON_DOWN);
                        ie->SetDeviceId(N_INPUT_MOUSE(0));
                        ie->SetAbsPos(this->event.xbutton.x, this->event.xbutton.y);
                        switch(this->event.xbutton.button)
                        {
                            case Button1: ie->SetButton(0); break;   // LMB
                            case Button2: ie->SetButton(1); break;   // MMB
                            case Button3: ie->SetButton(2); break;   // RMB
                            case Button4: ie->SetButton(3); break;   // wheel - up?
                            case Button5: ie->SetButton(4); break;   // wheel - down?
                            //more???
                            default:      ie->SetButton(2); break;   // ???
                        }
                        float relX = float(this->event.xbutton.x) / this->height;
                        float relY = float(this->event.xbutton.y) / this->width;
                        ie->SetRelPos(relX, relY);
                        n_printf("%f,%f.\n",relX,relY);
                        this->refInputServer->LinkEvent(ie);
                    }
                }
                break;

            case ButtonRelease:
                n_printf("ButtonRelase:");
                if (this->refInputServer.isvalid())
                {
                    nInputEvent *ie = this->refInputServer->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_BUTTON_UP);
                        ie->SetDeviceId(N_INPUT_MOUSE(0));
                        ie->SetAbsPos(this->event.xbutton.x, this->event.xbutton.y);
                        switch(this->event.xbutton.button) {
                            case Button1: ie->SetButton(0); break;   // LMB
                            case Button2: ie->SetButton(1); break;   // MMB
                            case Button3: ie->SetButton(2); break;   // RMB
                            case Button4: ie->SetButton(3); break;   // wheel - up?
                            case Button5: ie->SetButton(4); break;   // wheel - down?
                            //more???
                            default:      ie->SetButton(2); break;   // ???
                        }
                        float relX = float(this->event.xbutton.x) / this->height;
                        float relY = float(this->event.xbutton.y) / this->width;
                        ie->SetRelPos(relX, relY);
                        n_printf("%f,%f.\n",relX,relY);
                        this->refInputServer->LinkEvent(ie);
                    }
                }
                break;

            case MotionNotify:
                /*
                if (this->ref_is.isvalid())
                {
                    short x = this->event.xmotion.x;
                    short y = this->event.xmotion.y;
                    nInputEvent *ie = this->ref_is->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_MOUSE_MOVE);
                        ie->SetDeviceId(N_INPUT_MOUSE(0));
                        ie->SetAbsPos(x, y);
                        float relX = float(this->event.xmotion.x) / this->render_width;
                        float relY = float(this->event.xmotion.y) / this->render_height;
                        ie->SetRelPos(relX, relY);
                        this->ref_is->LinkEvent(ie);
                    }
                }
                */
                //n_printf("FIXME: got 'MotionNotify' don't handle it.");
                break;
            case FocusIn:
                /*
                #ifndef DEBUG_FULL_SCREEN
                if (this->ext_XF86VIDMODE &&
                    !this->keyboard_grabbed && event.xfocus.window == wm_win) {
                    XGrabKeyboard (this->dsp,
                        this->ctx_win,
                        false,
                        GrabModeAsync,
                        GrabModeAsync,
                        CurrentTime);
                    this->focused = true;
                    this->keyboard_grabbed = 2;
                }
                #endif
                */
                //n_printf("FIXME: got 'FocusIn' don't handle it.");
                break;

            case FocusOut:
                /*
                #ifndef DEBUG_FULL_SCREEN
                if (this->ext_XF86VIDMODE &&
                    this->keyboard_grabbed && !--(this->keyboard_grabbed)) {
                    XUngrabKeyboard (this->dsp,
                            CurrentTime);
                    this->focused = false;
                }
                #endif
                */
                //n_printf("FIXME: got 'FocusOut' don't handle it.");
                break;

            case MappingNotify:   // Automatic message, standard response
                XRefreshKeyboardMapping (&(this->event.xmapping));
            break;

            case SelectionClear:  // Automatic message
                n_printf ("SelectionClear Received\n");
                break;

            case SelectionNotify:  // Automatic message
                n_printf ("SelectionNotify Received\n");
                break;

            case SelectionRequest:  // Automatic message
                n_printf ("SelectionRequest Received\n");
                break;

            case MapNotify:
            case UnmapNotify:
            case ReparentNotify:
                // Not needed
                break;

            default:
                // Make sure we know exactly what events are received
                n_assert (false);
            break;
        }
    }
    return retval;
}

//-----------------------------------------------------------------------------
/**
    Setup the XWindow to the current displayMode and show it.
*/
void
nGLServer2::SetDisplayMode(const nDisplayMode2 &mode)
{
    n_assert(!this->displayOpen);
    nGfxServer2::SetDisplayMode(mode);

    //check mode for fullscreen support
    if (this->displayMode.GetType() != nDisplayMode2::WINDOWED)
    {
        //if (!this->ext_XF86VIDMODE)
        //{
            n_printf("Fullscreen not supported! Fake fullscreen by creating a window with the size of the current screen resolution.\n");
            this->displayMode.Set(nDisplayMode2::WINDOWED, DisplayHeight(this->display, this->screen), DisplayWidth(this->display, this->screen));
        //}
    }

    //switch windows to mode if needed
    if (this->displayMode.GetType() != nDisplayMode2::WINDOWED)
    {
        //fullscreen
        if (!this->isFullscreen) //only update if needed
        {
            this->current_win     = &this->fs_win;
            this->glx_current_win = &this->glx_fs_win;
            this->isFullscreen = true;
        }
    }
    else
    {
        if (this->isFullscreen) //only update if needed
        {
            this->current_win     = &this->ctx_win;
            this->glx_current_win = &this->glx_ctx_win;
            this->isFullscreen = false;
        }
    }

    Window root_return;
    int x_return, y_return;
    unsigned int width_return, height_return, border_return, depth_return;

    XGetGeometry(
        this->display, *this->current_win,
        &root_return,
        &x_return, &y_return,
        &width_return,  &height_return,
        &border_return, &depth_return
        );

    if (this->isFullscreen)
    {
        this->height = DisplayHeight(this->display, this->screen);
        this->width  = DisplayWidth (this->display, this->screen);
        this->border = 0;
        this->x = 0;
        this->y = 0;

        //resize window if needed
        if (this->height != height_return)
            this->updateWindowMask |= CWHeight;
        if (this->width != width_return)
            this->updateWindowMask |= CWWidth;
        //move the window if needed
        if (this->x != x_return)
            this->updateWindowMask |= CWX;
        if (this->y != y_return)
            this->updateWindowMask |= CWY;
        //rescale border if needed
        if (this->border != border_return)
            this->updateWindowMask |= CWBorderWidth;
    }
    else
    {
        this->border = border_return;
        this->x = x_return;
        this->y = y_return;
        unsigned int dispModeWidth  = this->displayMode.GetWidth();
        unsigned int dispModeHeight = this->displayMode.GetHeight();
        unsigned int screenWidth    = DisplayWidth(this->display, this->screen)  - this->border;
        unsigned int screenHeight   = DisplayHeight(this->display, this->screen) - this->border;
        if (dispModeWidth <= screenWidth )
        {
            this->width  = dispModeWidth;
        }
        else
        {
            n_printf("The requested window width (%i) is not possible. Using resolution width(%i).", dispModeWidth, screenWidth);
            this->width = screenWidth;
        }
        if (dispModeHeight <= screenHeight)
        {
            this->height = dispModeHeight;
        }
        else
        {
            n_printf("The requested window height (%i) is not possible. Using resolution height(%i).", dispModeHeight, screenHeight);
            this->height = screenHeight;
        }

        //resize window if needed
        if (this->height != height_return)
            this->updateWindowMask |= CWHeight;
        if (this->width != width_return)
            this->updateWindowMask |= CWWidth;
    }
}

//-----------------------------------------------------------------------------
/**
    Restore the minimized app window to the size requested by SetDisplayMode(),
    Must be called from within DeviceOpen() after setup the the render context.

    Sets windowMinimized to false.
*/
nGLServer2::RestoreWindow()
{
    n_assert(this->current_win);
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);

    //update the window if needed
    if (this->updateWindowMask)
    {
        this->updateWindow();
    }
    //show window
    if (this->isFullscreen)
    {
        XMapWindow(this->display, this->fs_win);
    }
    else
    {
        XMapWindow(this->display, this->wm_win);
        XMapWindow(this->display, this->ctx_win);
    }
    //wait for X
    XSync(this->display, false);

    this->windowMinimized = false;
}

//-----------------------------------------------------------------------------
/**
    Minimize the app window. Must be called from within DeviceClose() after
    the context has been destroyed.

    Sets windowMinimized to true.
*/
void
nGLServer2::MinimizeWindow()
{
    n_assert(this->current_win);
    n_assert(this->windowOpen);
    n_assert(!this->windowMinimized);

    //close/hide window
    if (this->isFullscreen)
    {
        XUnmapWindow(this->display, this->fs_win);
    }
    else
    {
        XUnmapWindow(this->display, this->ctx_win);
        XUnmapWindow(this->display, this->wm_win);
    }
    //wait for X
    XSync(this->display, false);

    this->windowMinimized = true;
}

//-----------------------------------------------------------------------------
/**
    Set the window title
*/
void
nGLServer2::SetWindowTitle(const char* title)
{
    nGfxServer2::SetWindowTitle(title);
    if (this->isFullscreen)
    {
        XStoreName(this->display, this->fs_win, (char*) this->windowTitle.Get());
    }
    else
    {
        XStoreName(this->display, this->ctx_win,(char*) this->windowTitle.Get());
        XStoreName(this->display, this->wm_win, (char*) this->windowTitle.Get());
    }
}

//--------------------------------------------------------------------
/**
    17-May-2003 cubejk  ported from nebula1
*/
Colormap
nGLServer2::getColormap()
{
    Status status;
    XStandardColormap *standardCmaps;
    Colormap cmap;
    int i, numCmaps;

    if (this->xvi->c_class!=TrueColor)
    {
        n_error("nGlXServer: no support for non-TrueColor visual");
    }
    //try to reuse a colormap
    status = XmuLookupStandardColormap(
        this->display,
        this->screen,
        this->xvi->visualid,
        this->xvi->depth,
        XA_RGB_DEFAULT_MAP,
        false,/* replace */
        true);/* retain */

    if (status == 1)
    {
        status = XGetRGBColormaps(
                    this->display,
                    RootWindow(this->display, this->xvi->screen),
                    &standardCmaps,
                    &numCmaps,
                    XA_RGB_DEFAULT_MAP);

        if (status == 1)
        {
            for (i = 0; i < numCmaps; i++)
            {
                if (standardCmaps[i].visualid == this->xvi->visualid)
                {
                    cmap = standardCmaps[i].colormap;
                    XFree(standardCmaps);
                    return cmap;
                }
            }
        }
    }
    //if there is not colormap we can reuse, create a new one
    cmap = XCreateColormap(
            this->display,
            RootWindow(this->display, this->xvi->screen),
            this->xvi->visual,
            AllocNone);
    return cmap;
}

//-----------------------------------------------------------------------------
/**
    Update the current window
*/
nGLServer2::updateWindow()
{
    //update window properties
    XWindowChanges values;

    if (this->updateWindowMask & CWWidth)
        values.width = this->width;
    if (this->updateWindowMask & CWHeight)
        values.height = this->height;
    if (this->updateWindowMask & CWX)
        values.x = this->x;
    if (this->updateWindowMask & CWY)
        values.y = this->y;
    if (this->updateWindowMask & CWBorderWidth)
        values.border_width = this->border;

    //update window
    if (this->isFullscreen)
    {
        XConfigureWindow(this->display, this->fs_win, this->updateWindowMask, &values);
        //glXDrawable update
        glXDestroyWindow(this->display, this->glx_fs_win);
        this->glx_fs_win = glXCreateWindow(this->display, this->selectedConfig, this->fs_win, NULL);

    }
    else
    {
        XConfigureWindow(this->display, this->wm_win,  this->updateWindowMask, &values);
        XConfigureWindow(this->display, this->ctx_win, this->updateWindowMask, &values);
        //glXDrawable update
        glXDestroyWindow(this->display, this->glx_ctx_win);
        this->glx_ctx_win = glXCreateWindow(this->display, this->selectedConfig, this->ctx_win, NULL);
    }
    glViewport(0 /*x*/, 0 /*y*/, this->width, this->height);
    this->updateWindowMask = 0;
}

//--------------------------------------------------------------------
/**
    Get the attribute value from this FBConfig.
    @param  GLXFBConfig     a valid GLXFrameBufferConfig
    @param  int             GLX_RED_SIZE, GLX_GREEN_SIZE, GLX_BLUE_SIZE,
                            GLX_ALPHA_SIZE, GLX_DEPTH_SIZE, GLX_STENCIL_SIZE
    @return int             on error -1, else value
*/
int
nGLServer2::getBufferAttr(GLXFBConfig config,const int attribute)
{
    int value;
    int error = glXGetFBConfigAttrib(this->display, config, attribute, &value);

    if (error == GLX_BAD_ATTRIBUTE)
    {
        n_error("BadAttributeType: %i \n",attribute);
    }
    else
    {
        return value;
    }
    return -1;
}

//--------------------------------------------------------------------
/**
    Display informations about GLX.
*/
void
nGLServer2::showGLXInfo()
{
    //glx
    n_printf("GLX - Extensions\n");
    n_printf("supported Extensions:\n");
    this->printExtensions(glXQueryExtensionsString(this->display,this->screen));

    //glx client
    n_printf("GLX Client\nVendor: %s\n",
        glXGetClientString(this->display, GLX_VENDOR));
    n_printf("Version: %s\n",
        glXGetClientString(this->display, GLX_VERSION));
    n_printf("supported Extensions:\n");
    this->printExtensions(glXGetClientString(this->display, GLX_EXTENSIONS));

    //glx server
    n_printf("GLX Server\nVendor: %s\n",
        glXQueryServerString(this->display, this->screen, GLX_VENDOR));
    n_printf("Version: %s\n",
        glXQueryServerString(this->display, this->screen, GLX_VERSION));
    n_printf("supported Extensions:\n");
    this->printExtensions(glXQueryServerString(this->display, this->screen, GLX_EXTENSIONS));
}

//--------------------------------------------------------------------
//  ngl_TranslateKey()
//  Uebersetzt X-Keys nach Nebula-Keys.
//  08-Dec-98   floh    created
//  02-Jul-98   samuel  added further keycodes
//  14-Jul-03   cubejk  copy and paste from nebula1
//--------------------------------------------------------------------
nKey
nGLServer2::translateKey(KeySym xkey)
{
    nKey nk;
    switch(xkey) {
        case XK_BackSpace:      nk=N_KEY_BACK; break;
        case XK_Tab:            nk=N_KEY_TAB; break;
        case XK_Linefeed:       nk=N_KEY_RETURN; break;
        case XK_Return:         nk=N_KEY_RETURN; break;
        case XK_Pause:          nk=N_KEY_PAUSE; break;
        case XK_Scroll_Lock:    nk=N_KEY_SCROLL; break;
        case XK_Sys_Req:        nk=N_KEY_PRINT; break;
        case XK_Escape:         nk=N_KEY_ESCAPE; break;
        case XK_Delete:         nk=N_KEY_DELETE; break;
        case XK_Home:           nk=N_KEY_HOME; break;
        case XK_Left:           nk=N_KEY_LEFT; break;
        case XK_Up:             nk=N_KEY_UP; break;
        case XK_Right:          nk=N_KEY_RIGHT; break;
        case XK_Down:           nk=N_KEY_DOWN; break;
        case XK_Page_Up:        nk=N_KEY_PRIOR; break;
        case XK_Page_Down:      nk=N_KEY_NEXT; break;
        case XK_End:            nk=N_KEY_END; break;
        case XK_Begin:          nk=N_KEY_HOME; break;
        case XK_Select:         nk=N_KEY_SELECT; break;
        case XK_Print:          nk=N_KEY_PRINT; break;
        case XK_Execute:        nk=N_KEY_EXECUTE; break;
        case XK_Insert:         nk=N_KEY_INSERT; break;
        case XK_Menu:           nk=N_KEY_MENU; break;
        case XK_Help:           nk=N_KEY_HELP; break;
        case XK_Num_Lock:       nk=N_KEY_NUMLOCK; break;
        case XK_KP_Multiply:    nk=N_KEY_MULTIPLY; break;
        case XK_KP_Add:         nk=N_KEY_ADD; break;
        case XK_KP_Separator:   nk=N_KEY_SEPARATOR; break;
        case XK_KP_Subtract:    nk=N_KEY_SUBTRACT; break;
        case XK_KP_Decimal:     nk=N_KEY_DECIMAL; break;
        case XK_KP_Divide:      nk=N_KEY_DIVIDE; break;
        case XK_KP_0:           nk=N_KEY_NUMPAD0; break;
        case XK_KP_1:           nk=N_KEY_NUMPAD1; break;
        case XK_KP_2:           nk=N_KEY_NUMPAD2; break;
        case XK_KP_3:           nk=N_KEY_NUMPAD3; break;
        case XK_KP_4:           nk=N_KEY_NUMPAD4; break;
        case XK_KP_5:           nk=N_KEY_NUMPAD5; break;
        case XK_KP_6:           nk=N_KEY_NUMPAD6; break;
        case XK_KP_7:           nk=N_KEY_NUMPAD7; break;
        case XK_KP_8:           nk=N_KEY_NUMPAD8; break;
        case XK_KP_9:           nk=N_KEY_NUMPAD9; break;

        case XK_KP_Space:       nk=N_KEY_SPACE; break;
        case XK_KP_Tab:         nk=N_KEY_TAB; break;
        case XK_KP_Enter:       nk=N_KEY_RETURN; break;
        case XK_KP_F1:          nk=N_KEY_F1; break;
        case XK_KP_F2:          nk=N_KEY_F2; break;
        case XK_KP_F3:          nk=N_KEY_F3; break;
        case XK_KP_F4:          nk=N_KEY_F4; break;
        case XK_KP_Home:        nk=N_KEY_HOME; break;
        case XK_KP_Left:        nk=N_KEY_LEFT; break;
        case XK_KP_Up:          nk=N_KEY_UP; break;
        case XK_KP_Right:       nk=N_KEY_RIGHT; break;
        case XK_KP_Down:        nk=N_KEY_DOWN; break;
        case XK_KP_End:         nk=N_KEY_END; break;
        case XK_KP_Begin:       nk=N_KEY_HOME; break;
        case XK_KP_Insert:      nk=N_KEY_INSERT; break;
        case XK_KP_Delete:      nk=N_KEY_DELETE; break;
        case XK_KP_Page_Up:     nk=N_KEY_PRIOR; break;
        case XK_KP_Page_Down:   nk=N_KEY_NEXT; break;

        case XK_F1:             nk=N_KEY_F1; break;
        case XK_F2:             nk=N_KEY_F2; break;
        case XK_F3:             nk=N_KEY_F3; break;
        case XK_F4:             nk=N_KEY_F4; break;
        case XK_F5:             nk=N_KEY_F5; break;
        case XK_F6:             nk=N_KEY_F6; break;
        case XK_F7:             nk=N_KEY_F7; break;
        case XK_F8:             nk=N_KEY_F8; break;
        case XK_F9:             nk=N_KEY_F9; break;
        case XK_F10:            nk=N_KEY_F10; break;
        case XK_F11:            nk=N_KEY_F11; break;
        case XK_F12:            nk=N_KEY_F12; break;
        case XK_F13:            nk=N_KEY_F13; break;
        case XK_F14:            nk=N_KEY_F14; break;
        case XK_F15:            nk=N_KEY_F15; break;
        case XK_F16:            nk=N_KEY_F16; break;
        case XK_F17:            nk=N_KEY_F17; break;
        case XK_F18:            nk=N_KEY_F18; break;
        case XK_F19:            nk=N_KEY_F19; break;
        case XK_F20:            nk=N_KEY_F20; break;
        case XK_F21:            nk=N_KEY_F21; break;
        case XK_F22:            nk=N_KEY_F22; break;
        case XK_F23:            nk=N_KEY_F23; break;
        case XK_F24:            nk=N_KEY_F24; break;
        case XK_Shift_L:        nk=N_KEY_SHIFT; break;
        case XK_Shift_R:        nk=N_KEY_SHIFT; break;
        case XK_Control_L:      nk=N_KEY_CONTROL; break;
        case XK_Control_R:      nk=N_KEY_CONTROL; break;
        case XK_space:          nk=N_KEY_SPACE; break;
        case XK_0:              nk=N_KEY_0; break;
        case XK_1:              nk=N_KEY_1; break;
        case XK_2:              nk=N_KEY_2; break;
        case XK_3:              nk=N_KEY_3; break;
        case XK_4:              nk=N_KEY_4; break;
        case XK_5:              nk=N_KEY_5; break;
        case XK_6:              nk=N_KEY_6; break;
        case XK_7:              nk=N_KEY_7; break;
        case XK_8:              nk=N_KEY_8; break;
        case XK_9:              nk=N_KEY_9; break;
        case XK_A:              nk=N_KEY_A; break;
        case XK_B:              nk=N_KEY_B; break;
        case XK_C:              nk=N_KEY_C; break;
        case XK_D:              nk=N_KEY_D; break;
        case XK_E:              nk=N_KEY_E; break;
        case XK_F:              nk=N_KEY_F; break;
        case XK_G:              nk=N_KEY_G; break;
        case XK_H:              nk=N_KEY_H; break;
        case XK_I:              nk=N_KEY_I; break;
        case XK_J:              nk=N_KEY_J; break;
        case XK_K:              nk=N_KEY_K; break;
        case XK_L:              nk=N_KEY_L; break;
        case XK_M:              nk=N_KEY_M; break;
        case XK_N:              nk=N_KEY_N; break;
        case XK_O:              nk=N_KEY_O; break;
        case XK_P:              nk=N_KEY_P; break;
        case XK_Q:              nk=N_KEY_Q; break;
        case XK_R:              nk=N_KEY_R; break;
        case XK_S:              nk=N_KEY_S; break;
        case XK_T:              nk=N_KEY_T; break;
        case XK_U:              nk=N_KEY_U; break;
        case XK_V:              nk=N_KEY_V; break;
        case XK_W:              nk=N_KEY_W; break;
        case XK_X:              nk=N_KEY_X; break;
        case XK_Y:              nk=N_KEY_Y; break;
        case XK_Z:              nk=N_KEY_Z; break;

        case XK_a:              nk=N_KEY_A; break;
        case XK_b:              nk=N_KEY_B; break;
        case XK_c:              nk=N_KEY_C; break;
        case XK_d:              nk=N_KEY_D; break;
        case XK_e:              nk=N_KEY_E; break;
        case XK_f:              nk=N_KEY_F; break;
        case XK_g:              nk=N_KEY_G; break;
        case XK_h:              nk=N_KEY_H; break;
        case XK_i:              nk=N_KEY_I; break;
        case XK_j:              nk=N_KEY_J; break;
        case XK_k:              nk=N_KEY_K; break;
        case XK_l:              nk=N_KEY_L; break;
        case XK_m:              nk=N_KEY_M; break;
        case XK_n:              nk=N_KEY_N; break;
        case XK_o:              nk=N_KEY_O; break;
        case XK_p:              nk=N_KEY_P; break;
        case XK_q:              nk=N_KEY_Q; break;
        case XK_r:              nk=N_KEY_R; break;
        case XK_s:              nk=N_KEY_S; break;
        case XK_t:              nk=N_KEY_T; break;
        case XK_u:              nk=N_KEY_U; break;
        case XK_v:              nk=N_KEY_V; break;
        case XK_w:              nk=N_KEY_W; break;
        case XK_x:              nk=N_KEY_X; break;
        case XK_y:              nk=N_KEY_Y; break;
        case XK_z:              nk=N_KEY_Z; break;
        default:                nk=N_KEY_NONE; break;
    }
    return nk;
}
#endif __LINUX__
