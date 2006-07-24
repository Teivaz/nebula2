//------------------------------------------------------------------------------
//  nguiwindow_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiwindow.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"
#include "kernel/ntimeserver.h"

nNebulaScriptClass(nGuiWindow, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiWindow::nGuiWindow() :
    nGuiWidget(),
    modal(false),
    dismissed(false),
    closeRequested(false),
    fadeInTime(0.0),
    fadeOutTime(0.0),
    openedTime(0.0),
    closeRequestTime(0.0),
    windowColor(1.0f, 1.0f, 1.0f, 1.0f),
    openFirstFrame(false)
{
    // windows are by default hidden
    this->shown = false;
    this->windowClass = kernelServer->FindClass("nguiwindow");
    n_assert(this->windowClass);
}

//------------------------------------------------------------------------------
/**
*/
nGuiWindow::~nGuiWindow()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Moves the window in front of all other child windows.
*/
void
nGuiWindow::SetFocusWindow(nGuiWindow* window)
{
    n_assert(window);
    n_assert(window->IsA(this->windowClass));
    if (!window->IsBackground())
    {
        n_assert(this == window->GetParent());
        n_assert(this != window);
        nGuiWindow* oldFocusWindow = this->GetTopMostWindow();
        if (oldFocusWindow && oldFocusWindow->HasFocus())
        {
            oldFocusWindow->OnLoseFocus();
        }
        window->Remove();
        this->AddTail(window);
        window->OnObtainFocus();
    }
}

//------------------------------------------------------------------------------
/**
    Returns the topmost visible child window, if one such exists.
*/
nGuiWindow*
nGuiWindow::GetTopMostWindow()
{
    nGuiWidget* widget;
    for (widget = (nGuiWidget*) this->GetTail(); widget; widget = (nGuiWidget*) widget->GetPred())
    {
        if (widget->IsA(this->windowClass) && widget->IsShown())
        {
            return (nGuiWindow*) widget;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiWindow::OnShow()
{
    nGuiWidget::OnShow();
    this->openFirstFrame = true;
}

//------------------------------------------------------------------------------
/**
    Called per frame. This just makes sure that the topmost window is
    also the focus window (this may change if the previous focus
    window has disappeared.

    - 07-Jun-05    kims    Added close event handler to provides fading and prevents
                           crashes when a custom nguibutton tries to close parent window.
*/
void
nGuiWindow::OnFrame()
{
    nGuiWindow* topMostWindow = this->GetTopMostWindow();
    if (topMostWindow && (!topMostWindow->HasFocus()))
    {
        topMostWindow->OnObtainFocus();
    }
    nTime time = nTimeServer::Instance()->GetTime();

    // catch time exceptions
    if (time < this->openedTime)
    {
        this->openedTime = time - this->fadeInTime;
    }
    if (this->closeRequested)
    {
        if (time < this->closeRequestTime)
        {
            // a timer exception! (time has been reset?)
            this->closeRequestTime = time;
        }
        // dismiss window when fade out time is reached
        // NOTE: the use on nTimeServer is intentional, as the current
        // frame time stamp is useless (would give fade delays when
        // resources are loaded)
        if (time > (this->closeRequestTime + this->fadeOutTime))
        {
            this->SetDismissed(true);

            // execute 'close' event handler
            if (!this->closeCommand.IsEmpty())
            {
                nGuiServer::Instance()->RunCommand(this, this->closeCommand);
            }
        }
    }
    nGuiWidget::OnFrame();
}

//------------------------------------------------------------------------------
/**
    OnMouseMoved goes to ALL child widgets, not just the focus window!
*/
bool
nGuiWindow::OnMouseMoved(const vector2& mousePos)
{
    if (this->IsShown())
    {
        return nGuiWidget::OnMouseMoved(mousePos);
        /*
        nGuiWindow* focusWindow = this->GetTopMostWindow();
        if (focusWindow)
        {
            return focusWindow->OnMouseMoved(mousePos);
        }
        else
        {
            return nGuiWidget::OnMouseMoved(mousePos);
        }
        */
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiWindow::OnButtonDown(const vector2& mousePos)
{
    if (this->IsShown() && this->Inside(mousePos))
    {
        // if the button click happened over the focus window,
        // route the click to the focus window, otherwise
        // check if button click happened over child windows
        // and declare a new focus window
        nGuiWindow* focusWindow = this->GetTopMostWindow();
        if (focusWindow)
        {
            if (focusWindow->Inside(mousePos))
            {
                focusWindow->OnButtonDown(mousePos);
                return true;
            }
            else if (!focusWindow->IsModal())
            {
                // check all child windows for new focus window
                nGuiWindow* childWindow;
                for (childWindow = (nGuiWindow*) this->GetTail();
                     childWindow;
                     childWindow = (nGuiWindow*) childWindow->GetPred())
                {
                    if ((childWindow != focusWindow) &&
                        childWindow->IsA(this->windowClass) &&
                        childWindow->IsShown() &&
                        childWindow->Inside(mousePos))
                    {
                        this->SetFocusWindow(childWindow);
                        childWindow->OnButtonDown(mousePos);
                        return true;
                    }
                }
            }
        }
        else
        {
            // no focus window, just deliver the button click to the child widgets
            return nGuiWidget::OnButtonDown(mousePos);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiWindow::OnButtonUp(const vector2& mousePos)
{
    if (this->IsShown())
    {
        nGuiWindow* focusWindow = this->GetTopMostWindow();
        if (focusWindow)
        {
            focusWindow->OnButtonUp(mousePos);
        }
        else
        {
            nGuiWidget::OnButtonUp(mousePos);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiWindow::OnRButtonDown(const vector2& mousePos)
{
    if (this->IsShown() && this->Inside(mousePos))
    {
        nGuiWindow* focusWindow = this->GetTopMostWindow();
        if (focusWindow)
        {
            focusWindow->OnRButtonDown(mousePos);
        }
        else
        {
            nGuiWidget::OnRButtonDown(mousePos);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiWindow::OnRButtonUp(const vector2& mousePos)
{
    if (this->IsShown())
    {
        nGuiWindow* focusWindow = this->GetTopMostWindow();
        if (focusWindow)
        {
            focusWindow->OnRButtonUp(mousePos);
        }
        else
        {
            nGuiWidget::OnRButtonUp(mousePos);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiWindow::OnChar(uchar charCode)
{
    if (this->IsShown())
    {
        nGuiWindow* focusWindow = this->GetTopMostWindow();
        if (focusWindow)
        {
            focusWindow->OnChar(charCode);
        }
        else
        {
            nGuiWidget::OnChar(charCode);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiWindow::OnKeyDown(nKey key)
{
    bool handled = false;
    if (this->IsShown())
    {
        nGuiWindow* focusWindow = this->GetTopMostWindow();
        if (focusWindow)
        {
            handled |= focusWindow->OnKeyDown(key);
        }
        else
        {
            handled |= nGuiWidget::OnKeyDown(key);
        }

        if (!handled)
        {
            if (N_KEY_ESCAPE == key)
            {
                // invoke the escape key handler
                if (!this->escapeCommand.IsEmpty())
                {
                    nGuiServer::Instance()->RunCommand(this, this->escapeCommand);
                    handled = true;
                }
            }
        }
    }
    return handled;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiWindow::OnKeyUp(nKey key)
{
    if (this->IsShown())
    {
        nGuiWindow* focusWindow = this->GetTopMostWindow();
        if (focusWindow)
        {
            focusWindow->OnKeyUp(key);
        }
        else
        {
            nGuiWidget::OnKeyUp(key);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This computes the window color (takes fade in and fade out effect into
    account).
*/
void
nGuiWindow::UpdateWindowColor()
{
    vector4 activeWindowColor(1.0f, 1.0f, 1.0f, 1.0f);
    vector4 inactiveWindowColor(1.0f, 1.0f, 1.0f, 1.0f);
    nGuiSkin* curSkin = nGuiServer::Instance()->GetSkin();
    if (curSkin)
    {
        activeWindowColor = curSkin->GetActiveWindowColor();
        inactiveWindowColor = curSkin->GetInactiveWindowColor();
    }

    // NOTE: the use on nTimeServer is intentional, as the current
    // frame time stamp is useless (would give fade delays when
    // resources are loaded)
    nTime time = nTimeServer::Instance()->GetTime();

    // active or inactive window color?
    if (this->HasFocus() || (this == nGuiServer::Instance()->GetRootWindowPointer()))
    {
        this->windowColor = activeWindowColor;
    }
    else
    {
        this->windowColor = inactiveWindowColor;
    }

    // to obscure resource loading delays, the actual fadein snapshot
    // is only taken after the first rendering (where resources
    // are demand-loaded)
    if (this->openFirstFrame && (this->fadeInTime > 0.0f))
    {
        // window is always invisible in first frame
        this->windowColor.w = 0;
    }
    else
    {
        if (this->closeRequested)
        {
            // fadeout?
            if ((this->fadeOutTime > 0.01f) && (time > this->closeRequestTime))
            {
                // fade alpha
                float lerp = 1.0f - n_saturate((float) ((time - this->closeRequestTime) / this->fadeOutTime));
                this->windowColor.w *= lerp;
            }
        }
        else
        {
            // fade in?
            if ((this->fadeInTime > 0.01f) && (time < (this->openedTime + this->fadeInTime)))
            {
                // fade alpha
                float lerp = n_saturate((float) ((time - this->openedTime) / this->fadeInTime));
                this->windowColor.w *= lerp;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiWindow::Render()
{
    if (this->IsShown())
    {
        nGuiServer* guiServer = nGuiServer::Instance();

        this->UpdateWindowColor();
        vector4 globalColor = guiServer->GetGlobalColor();
        guiServer->SetGlobalColor(this->windowColor);
        guiServer->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);

        // take opened time stamp AFTER first rendering to take resource
        // loading delays into account
        if (this->openFirstFrame)
        {
            // NOTE: the use on nTimeServer is intentional, as the current
            // frame time stamp is useless (would give fade delays when
            // resources are loaded)
            this->openedTime = nTimeServer::Instance()->GetTime();
            this->openFirstFrame = false;
        }

        // render contained widgets
        nGuiWidget::Render();

        // restore previous global color
        guiServer->SetGlobalColor(globalColor);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Set the close requested flag. This will start the fade out.
*/
void
nGuiWindow::SetCloseRequested(bool /*b*/)
{
    if (!this->closeRequested)
    {
        this->closeRequested = true;

        // NOTE: the use on nTimeServer is intentional, as the current
        // frame time stamp is useless (would give fade delays when
        // resources are loaded)
        this->closeRequestTime = nTimeServer::Instance()->GetTime();
    }
}
