//------------------------------------------------------------------------------
//  nwindowhandler.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nwindowhandler.h"

//------------------------------------------------------------------------------
/**
*/
nWindowHandler::nWindowHandler() :
    refInputServer("/sys/servers/input"),
    windowOpen(false),
    windowMinimized(false),
    quitRequested(false),
    skipMsgLoop(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nWindowHandler::~nWindowHandler()
{
    n_assert(!this->windowOpen);
}

//------------------------------------------------------------------------------
/**
    Open the application window. Override this method in a platform specific
    subclass. The window will live during the entire life span of the
    graphics server. It should be opened in a minimized or invisible
    state, until nGfxServer2::OpenDisplay() calls AdjustWindowForChange() and
    RestoreWindow().
*/
bool
nWindowHandler::OpenWindow()
{
    n_assert(!this->windowOpen);
    this->windowOpen = true;
    this->windowMinimized = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the application window. This method will be called from the
    gfx server's destructor. Override this method in a platform
    specific subclass.
*/
void
nWindowHandler::CloseWindow()
{
    this->windowOpen = false;
}

//------------------------------------------------------------------------------
/**
    Adjust the window for change. This will be called by the gfx server
    before switching between fullscreen and windowed mode. Subclasses
    should inspect the displayMode member for information about the
    new window behaviour.
*/
void
nWindowHandler::AdjustWindowForChange()
{   
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);

    // empty
}

//------------------------------------------------------------------------------
/**
    This restores the minimized app window to the size requested in
    the current display mode member.
*/
void
nWindowHandler::RestoreWindow()
{
    n_assert(this->windowOpen);
}

//------------------------------------------------------------------------------
/**
    This method is called when the application window should be minimized.
*/
void
nWindowHandler::MinimizeWindow()
{
    n_assert(this->windowOpen);
}

//------------------------------------------------------------------------------
/**
    The trigger method is called from nGfxServer2::Trigger(). The window
    handler should do its message handling and return false when the
    application should shut down (for instance because an Alt-F4 has been
    received).
*/
bool
nWindowHandler::Trigger()
{
    return true;
}






    
