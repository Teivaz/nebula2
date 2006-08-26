#ifndef N_WINDOWHANDLER_H
#define N_WINDOWHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nWindowHandler
    @ingroup Gfx2

    A window handler class which cares about platform specifics of window
    handler code and feeds keyboard and mouse input events into the
    input server. An instance of a window handler subclass is used by
    the graphics servers to manage the application window. Subclasses
    of nWindowHandler implement window handling for specific
    platforms (Win32, Linux, MacOSX, ...).

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "gfx2/ndisplaymode2.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------
class nWindowHandler
{
public:
    /// constructor
    nWindowHandler();
    /// destructor
    ~nWindowHandler();
    /// update the display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get the display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// open the application window
    bool OpenWindow();
    /// close the application window
    void CloseWindow();
    /// prepare window for switching between windowd/fullscreen mode
    void AdjustWindowForChange();
    /// restore window from minimized state
    void RestoreWindow();
    /// minimize the window
    void MinimizeWindow();
    /// call this method per frame, returns false if app should shut down
    bool Trigger();
    /// return true if window is open
    bool IsWindowOpen() const;
    /// return true if window is minimized
    bool IsWindowMinimized() const;
    /// return true if quit has been requested
    bool IsQuitRequested() const;
    /// skip message loop in trigger
    void SetSkipMsgLoop(bool skip);
    /// check skip message loop.
    bool IsSkipMsgLoop() const;

public:
    // NOTE: public because WinProc needs access!
    nAutoRef<nInputServer> refInputServer;
    nDisplayMode2 displayMode;
    bool windowOpen;
    bool windowMinimized;
    bool quitRequested;

protected:
    /// skip message loop
    bool skipMsgLoop;

};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nWindowHandler::IsWindowOpen() const
{
    return this->windowOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nWindowHandler::IsWindowMinimized() const
{
    return this->windowMinimized;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nWindowHandler::IsQuitRequested() const
{
    return this->quitRequested;
}

//------------------------------------------------------------------------------
/**
    Updates the display mode member. This should be called from
    nGfxServer2::SetDisplayMode(), so that the window handler will
    always have the current display mode to look at.
*/
inline
void
nWindowHandler::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
nWindowHandler::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
    Specify to skip internal message loop of nWindowHandler::Trigger().

    - 24-Nov-04 kims added
*/
inline
void
nWindowHandler::SetSkipMsgLoop(bool skip)
{
    this->skipMsgLoop = skip;
}

//------------------------------------------------------------------------------
/**
    Return true, if skip message loop is set on.

    - 24-Nov-04 kims added
*/
inline
bool
nWindowHandler::IsSkipMsgLoop() const
{
    return this->skipMsgLoop;
}
//------------------------------------------------------------------------------
#endif
