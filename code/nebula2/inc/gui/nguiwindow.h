#ifndef N_GUIWINDOW_H
#define N_GUIWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiWindow
    @ingroup Gui
    @brief A window that defines a rectangular area on screen and may contain
    other widgets.

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"
#include "gui/nguiserver.h"

//------------------------------------------------------------------------------
class nGuiWindow : public nGuiWidget
{
public:
    /// constructor
    nGuiWindow();
    /// destructor
    virtual ~nGuiWindow();
    /// render the window and all contained widgets
    virtual bool Render();
    /// make window the new focus window
    virtual void SetFocusWindow(nGuiWindow* window);
    /// get topmost visible child window
    virtual nGuiWindow* GetTopMostWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// handle mouse move, route to focus window if exists
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down, route to focus window if exists
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up, route to focus window if exists
    virtual bool OnButtonUp(const vector2& mousePos);
    /// handle right button down, route to focus window if exists
    virtual bool OnRButtonDown(const vector2& mousePos);
    /// handle right button up, route to focus window if exists
    virtual bool OnRButtonUp(const vector2& mousePos);
    /// handle character key event, route to focus window if exists
    virtual void OnChar(uchar charCode);
    /// handle a key down event, route to focus window if exists
    virtual bool OnKeyDown(nKey key);
    /// handle a key up event, route to focus window if exists
    virtual void OnKeyUp(nKey key);
    /// called per frame
    virtual void OnFrame();
    /// set modal flag
    void SetModal(bool b);
    /// get modal flag
    bool IsModal() const;
    /// set escape key script handler
    void SetEscapeCommand(const char* cmd);
    /// get escape key script handler
    const char* GetEscapeCommand() const;
    /// set dismissed flag
    void SetDismissed(bool b);
    /// is dismissed?
    bool IsDismissed() const;
    /// set close request
    void SetCloseRequested(bool b);
    /// get close request
    bool IsCloseRequested() const;
    /// set fadein time
    void SetFadeInTime(nTime t);
    /// get fadein time
    nTime GetFadeInTime() const;
    /// set fadeout time
    void SetFadeOutTime(nTime t);
    /// get fadeout time
    nTime GetFadeOutTime() const;
    /// get window color
    const vector4& GetWindowColor() const;
    /// set window color
    void SetWindowColor(const vector4&);
    /// set 'close' event script handler
    void SetCloseCommand(const char* cmd);
    /// get 'close' event script handler
    const char* GetCloseCommand() const;

protected:
    /// compute current window color
    virtual void UpdateWindowColor();

    nString closeCommand;
    nString escapeCommand;
    nClass* windowClass;
    
    bool modal;
    bool dismissed;
    bool closeRequested;

    bool openFirstFrame;
    nTime fadeInTime;
    nTime fadeOutTime;
    nTime openedTime;
    nTime closeRequestTime;
    vector4 windowColor;
};

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiWindow::GetWindowColor() const
{
    return this->windowColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiWindow::SetWindowColor(const vector4& c)
{
    this->windowColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiWindow::SetEscapeCommand(const char* cmd)
{
    this->escapeCommand = cmd;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiWindow::GetEscapeCommand() const
{
    return this->escapeCommand.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiWindow::SetModal(bool b)
{
    this->modal = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiWindow::IsModal() const
{
    return this->modal;
}

//-----------------------------------------------------------------------------
/**
    Set the dismiss flag. The gui server will check for dismissed windows
    under the current root window and release them.
*/
inline
void
nGuiWindow::SetDismissed(bool b)
{
    this->dismissed = b;
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
nGuiWindow::IsDismissed() const
{
    return this->dismissed;
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
nGuiWindow::IsCloseRequested() const
{
    return this->closeRequested;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWindow::SetFadeInTime(nTime t)
{
    this->fadeInTime = t;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nTime
nGuiWindow::GetFadeInTime() const
{
    return this->fadeInTime;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWindow::SetFadeOutTime(nTime t)
{
    this->fadeOutTime = t;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nTime
nGuiWindow::GetFadeOutTime() const
{
    return this->fadeOutTime;
}

//------------------------------------------------------------------------------
/**

- 07-Jun-05    kims    Added 'close', 'setclosecommand' and 'getclosecommand'
                       to provide fading and prevent crashes when a custom 
                       nguibutton tries to close parent window.
*/
inline
void
nGuiWindow::SetCloseCommand(const char* cmd)
{
    this->closeCommand = cmd;
}

//------------------------------------------------------------------------------
/**
    - 07-Jun-05    kims    Added 'close', 'setclosecommand' and 'getclosecommand'
                           to provide fading and prevent crashes when a custom 
                           nguibutton tries to close parent window.
*/
inline
const char*
nGuiWindow::GetCloseCommand() const
{
    return this->closeCommand.Get();
}
//------------------------------------------------------------------------------
#endif

