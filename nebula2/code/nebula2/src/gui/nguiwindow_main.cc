//------------------------------------------------------------------------------
//  nguiwindow_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiwindow.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"

nNebulaScriptClass(nGuiWindow, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiWindow::nGuiWindow() :
    nGuiWidget(),
    modal(false)
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
    Called per frame. This just makes sure that the topmost window is
    also the focus window (this may change if the previous focus
    window has disappeared.
*/
void
nGuiWindow::OnFrame()
{
    nGuiWindow* topMostWindow = this->GetTopMostWindow();
    if (topMostWindow && (!topMostWindow->HasFocus()))
    {
        topMostWindow->OnObtainFocus();
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
                bool handled = this->refGuiServer->RunCommand(this, this->escapeCommand);
                handled = true;
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
*/
bool
nGuiWindow::Render()
{
    if (this->IsShown())
    {
        nGuiServer* guiServer = this->refGuiServer.get();
        vector4 activeWindowColor(1.0f, 1.0f, 1.0f, 1.0f);
        vector4 inactiveWindowColor(1.0f, 1.0f, 1.0f, 1.0f);
        nGuiSkin* curSkin = this->refGuiServer->GetSkin();
        if (curSkin)
        {
            activeWindowColor = curSkin->GetActiveWindowColor();
            inactiveWindowColor = curSkin->GetInactiveWindowColor();
        }
        if (this->HasFocus() || (this == this->refGuiServer->GetRootWindowPointer()))
        {
            this->refGuiServer->SetGlobalColor(activeWindowColor);
        }
        else
        {
            this->refGuiServer->SetGlobalColor(inactiveWindowColor);
        }

        this->refGuiServer->DrawBrush(this->GetScreenSpaceRect(), this->GetDefaultBrush());

        // render contained widgets
        nGuiWidget::Render();
        return true;
    }
    return false;
}
