//-----------------------------------------------------------------------------
//  nguiwidget_main.cc
//  (C) 2003 RadonLabs GmbH
//-----------------------------------------------------------------------------
#include "gui/nguiwidget.h"
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "gui/nguiserver.h"
#include "gui/nguievent.h"
#include "gui/nguiresource.h"

nNebulaScriptClass(nGuiWidget, "nroot");

nClass* nGuiWidget::widgetClass=0;
nClass* nGuiWidget::windowClass=0;

//-----------------------------------------------------------------------------
/**
*/
nGuiWidget::nGuiWidget() :
    rect(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f)),
    lastButtonDownTime(0.0),
    mouseWithinTime(0.0),
    shown(true),
    enabled(true),
    stickyMouse(false),
    hasFocus(false),
    backGround(false),
    mouseWithin(false),
    minSize(0.0f, 0.0f),
    maxSize(1.0f, 1.0f),
    blinking(false),
    blinkStarted(0.0),
    blinkTimeOut(0.0),
    blinkRate(1.0),
    eventHandler(0)
{
    this->widgetClass = kernelServer->FindClass("nguiwidget");
    this->windowClass = kernelServer->FindClass("nguiwindow");
}

//-----------------------------------------------------------------------------
/**
*/
nGuiWidget::~nGuiWidget()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Get the owner window if any, if not returns itself
*/
nGuiWidget*
nGuiWidget::GetOwnerWindow()
{
    if (this->IsA(windowClass))
    {
        // returns itself, if the widget is a window
        return this;
    }
    else
    {
        // search for the first parent, which is a window
        nGuiWidget* parent = (nGuiWidget*) this->GetParent();
        while(parent && !parent->IsA(windowClass))
        {
            parent = (nGuiWidget*) parent->GetParent();
        }

        if(!parent)
        {
            // no parent window found
            return 0;
        }
        
        // return owner-window of this widget
        return parent;
    }
}

//-----------------------------------------------------------------------------
/**
    Render the widget and all its child widgets.
*/
bool
nGuiWidget::Render()
{
    if (this->IsShown())
    {
        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            cur->Render();
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
   Is coordinate in mouse sensitive area.
*/
bool
nGuiWidget::Inside(const vector2& p)
{
    // FIXME: make sure this has no unwanted side effects
    if (this->IsBackground())
    {
        return false;
    }
    else if (this->IsShown())
    {
        rectangle screenSpaceRect = this->GetScreenSpaceRect();
        screenSpaceRect.v0 += this->clickRectBorder;
        screenSpaceRect.v1 -= this->clickRectBorder;
        if (screenSpaceRect.inside(p))
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiWidget::OnMouseMoved(const vector2& mousePos)
{
    if (this->IsShown())
    {
        // distribute to children
        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            if (cur->IsEnabled())
            {
                cur->OnMouseMoved(mousePos);
            }
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiWidget::OnButtonDown(const vector2& mousePos)
{
    bool retval = false;
    if (this->IsShown())
    {
        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetTail(); cur; cur = (nGuiWidget*) cur->GetPred())
        {
            if (cur->IsEnabled())
            {
                if (cur->OnButtonDown(mousePos))
                {
                    retval = true;
                    break;  // FIXME: does this interfere with other window code?
                }
            }
        }

        if (this->Inside(mousePos))
        {
            nGuiEvent event(this, nGuiEvent::ButtonDown);
            nGuiServer::Instance()->PutEvent(event);
            nGuiServer::Instance()->RunCommand(this, this->buttonDownCommand);

            // store button down time, and probably shoot a double click event
            nTime time = nGuiServer::Instance()->GetTime();
            nTime timeDiff = time - this->lastButtonDownTime;
            if ((timeDiff > 0.0) && (timeDiff < 0.2))
            {
                this->OnDoubleClick(mousePos);
            }
            this->lastButtonDownTime = time;
        }
    }
    return retval;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiWidget::OnButtonUp(const vector2& mousePos)
{
    if (this->IsShown())
    {
        nGuiEvent event(this, nGuiEvent::ButtonUp);
        nGuiServer::Instance()->PutEvent(event);

        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            if (cur->IsEnabled())
            {
                cur->OnButtonUp(mousePos);
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiWidget::OnRButtonDown(const vector2& mousePos)
{
    bool retval = false;
    if (this->IsShown())
    {
        nGuiEvent event(this, nGuiEvent::RButtonDown);
        nGuiServer::Instance()->PutEvent(event);

        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            if (cur->IsEnabled())
            {
                retval |= cur->OnRButtonDown(mousePos);
            }
        }
    }
    return retval;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiWidget::OnRButtonUp(const vector2& mousePos)
{
    if (this->IsShown())
    {
        nGuiEvent event(this, nGuiEvent::RButtonUp);
        nGuiServer::Instance()->PutEvent(event);

        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            if (cur->IsEnabled())
            {
                cur->OnRButtonUp(mousePos);
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    This method is called by the widget itself when 2 OnButtonDown()
    events arrive within a double click time.
*/
void
nGuiWidget::OnDoubleClick(const vector2& /*mousePos*/)
{
    if (this->IsShown())
    {
        nGuiEvent event(this, nGuiEvent::DoubleClick);
        nGuiServer::Instance()->PutEvent(event);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnChar(uchar charCode)
{
    if (this->IsShown())
    {
        nGuiEvent event(this, nGuiEvent::Char);
        nGuiServer::Instance()->PutEvent(event);

        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            if (cur->IsEnabled())
            {
                cur->OnChar(charCode);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiWidget::OnKeyDown(nKey key)
{
    bool handled = false;
    if (this->IsShown())
    {
        nGuiEvent event(this, nGuiEvent::KeyDown);
        nGuiServer::Instance()->PutEvent(event);

        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            if (cur->IsEnabled())
            {
                handled |= cur->OnKeyDown(key);
            }
        }
    }
    return handled;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnKeyUp(nKey key)
{
    if (this->IsShown())
    {
        nGuiEvent event(this, nGuiEvent::KeyUp);
        nGuiServer::Instance()->PutEvent(event);

        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            if (cur->IsEnabled())
            {
                cur->OnKeyUp(key);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnEnabled()
{
    nGuiEvent event(this, nGuiEvent::Enabled);
    nGuiServer::Instance()->PutEvent(event);
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnDisabled()
{
    nGuiEvent event(this, nGuiEvent::Disabled);
    nGuiServer::Instance()->PutEvent(event);
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnShow()
{
    nGuiServer::Instance()->RunCommand(this, this->showCommand);

    nGuiEvent event(this, nGuiEvent::Show);
    nGuiServer::Instance()->PutEvent(event);

    nGuiWidget* cur;
    for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
    {
        if (cur->IsShown())
        {
            cur->OnShow();
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnHide()
{
    nGuiServer::Instance()->RunCommand(this, this->hideCommand);

    nGuiWidget* cur;
    for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
    {
        if (cur->IsShown())
        {
            cur->OnHide();
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnFrame()
{
    if (this->IsShown())
    {
        // activate tooltip after mouse was over widget for a while
        if (this->Inside(nGuiServer::Instance()->GetMousePos())/* && this->HasFocus()*/ && !this->tooltip.IsEmpty())            
        {
            nTime time = nTimeServer::Instance()->GetTime();
            if(!this->mouseWithin)
            {
                this->mouseWithinTime = time;
                this->mouseWithin = true;
            }
            else if(time > this->mouseWithinTime + nGuiServer::Instance()->GetToolTipActivationTime())
            {
                nGuiServer::Instance()->ShowToolTip(this->tooltip.Get(), vector4(0.0f, 0.0f, 0.0f, 1.0f));
            }
        }
        else
        {
            this->mouseWithin = false;
        }

        // Set blinking time
        if (this->blinking)
        {
            if (this->blinkTimeOut > 0.0)
            {
                nTime curTime = nGuiServer::Instance()->GetTime();
                if (curTime >= (this->blinkStarted + this->blinkTimeOut))
                {
                    this->SetBlinking(false, 0.0);
                }
            }
        }

        // run the per-frame command
        nGuiServer::Instance()->RunCommand(this, this->frameCommand);

        // distribute to children
        nGuiWidget* cur;
        for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
        {
            cur->OnFrame();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    This method is called when the widget's action should happen.
*/
void
nGuiWidget::OnAction()
{
    nGuiServer::Instance()->RunCommand(this, this->command);
    nGuiEvent event(this, nGuiEvent::Action);
    nGuiServer::Instance()->PutEvent(event);
}

//-----------------------------------------------------------------------------
/**
    This method is called when the widget's rectangle is going to change.
*/
void
nGuiWidget::OnRectChange(const rectangle& newRect)
{
    this->rect = newRect;
}

//-----------------------------------------------------------------------------
/**
    This method is called by the gui server when the PutEvent()
    method is invoked on all registered event listeners.
*/
void
nGuiWidget::OnEvent(const nGuiEvent& event)
{
    if (this->eventHandler)
    {
        this->eventHandler->HandleEvent(event);
    }
}

//-----------------------------------------------------------------------------
/**
    This method is called when the widget's owner window becomes the
    focus window.
*/
void
nGuiWidget::OnObtainFocus()
{
    this->hasFocus = true;

    // distribute to children
    nGuiWidget* cur;
    for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
    {
        cur->OnObtainFocus();
    }
}

//-----------------------------------------------------------------------------
/**
    This method is called when the widget's owner window loses focus.
*/
void
nGuiWidget::OnLoseFocus()
{
    this->hasFocus = false;

    // distribute to children
    nGuiWidget* cur;
    for (cur = (nGuiWidget*) this->GetHead(); cur; cur = (nGuiWidget*) cur->GetSucc())
    {
        cur->OnLoseFocus();
    }
}

//-----------------------------------------------------------------------------
/**
    Return this widget's rectangle in screen space. Widget rectangles
    are defined in the coordinate space of their parent widget.
*/
rectangle
nGuiWidget::GetScreenSpaceRect() const
{
    rectangle r = this->rect;
    nGuiWidget* widget = (nGuiWidget*) this->GetParent();
    while (widget && widget->IsA(this->widgetClass))
    {
        const rectangle pr = widget->GetRect();
        r.v0 += pr.v0;
        r.v1 += pr.v0;
        widget = (nGuiWidget*) widget->GetParent();
    }
    return r;
}

//-----------------------------------------------------------------------------
/**
    Return a screen space rectangle which has been defined in this widgets
    coordinate space.
*/
rectangle
nGuiWidget::GetScreenSpaceRect(const rectangle& src) const
{
    rectangle r = src;
    nGuiWidget* widget = (nGuiWidget*) this;
    while (widget && widget->IsA(this->widgetClass))
    {
        const rectangle pr = widget->GetRect();
        r.v0 += pr.v0;
        r.v1 += pr.v0;
        widget = (nGuiWidget*) widget->GetParent();
    }
    return r;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::SetBlinking(bool b, nTime timeOut)
{
    this->blinking = b;
    this->blinkStarted = nGuiServer::Instance()->GetTime();
    this->blinkTimeOut = timeOut;
}
