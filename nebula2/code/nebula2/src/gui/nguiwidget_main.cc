//-----------------------------------------------------------------------------
//  nguiwidget_main.cc
//  (C) 2003 RadonLabs GmbH
//-----------------------------------------------------------------------------
#include "gui/nguiwidget.h"
#include "kernel/nkernelserver.h"
#include "gui/nguiserver.h"
#include "gui/nguievent.h"

nNebulaScriptClass(nGuiWidget, "nroot");

//-----------------------------------------------------------------------------
/**
*/
nGuiWidget::nGuiWidget() :
    refGuiServer("/sys/servers/gui"),
    rect(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f)),
    shown(true),
    triggerSound(false),
    blinking(false),
    enabled(true),
    dismissed(false),
    stickyMouse(false),
    hasFocus(false),
    minSize(0.0f, 0.0f),
    maxSize(1.0f, 1.0f)
{
    this->widgetClass = kernelServer->FindClass("nguiwidget");
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
    Render the widget and all it's child widgets.
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
    if (this->IsShown())
    {
        return this->GetScreenSpaceRect().inside(p);
    }
    else
    {
        return false;
    }
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
            this->refGuiServer->PutEvent(event);
            this->refGuiServer->RunCommand(this, this->buttonDownCommand);
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
        this->refGuiServer->PutEvent(event);

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
        this->refGuiServer->PutEvent(event);

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
        this->refGuiServer->PutEvent(event);

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
*/
void
nGuiWidget::OnChar(uchar charCode)
{
    if (this->IsShown())
    {
        nGuiEvent event(this, nGuiEvent::Char);
        this->refGuiServer->PutEvent(event);

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
        this->refGuiServer->PutEvent(event);

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
        this->refGuiServer->PutEvent(event);

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
    this->refGuiServer->PutEvent(event);
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnDisabled()
{
    nGuiEvent event(this, nGuiEvent::Disabled);
    this->refGuiServer->PutEvent(event);
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiWidget::OnShow()
{
    this->refGuiServer->RunCommand(this, this->showCommand);

    nGuiEvent event(this, nGuiEvent::Show);
    this->refGuiServer->PutEvent(event);

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
    this->refGuiServer->RunCommand(this, this->hideCommand);

    nGuiEvent event(this, nGuiEvent::Hide);
    this->refGuiServer->PutEvent(event);

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
        // activate tooltip if mouse is over widget
        if (this->Inside(this->refGuiServer->GetMousePos()) && (!this->tooltip.IsEmpty()))
        {
            this->refGuiServer->ShowToolTip(this->tooltip.Get(), vector4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        // run the per-frame command
        this->refGuiServer->RunCommand(this, this->frameCommand);

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
    this->refGuiServer->RunCommand(this, this->command);
    nGuiEvent event(this, nGuiEvent::Action);
    this->refGuiServer->PutEvent(event);
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
    // empty
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
*/
bool
nGuiWidget::RenderAudio()
{
    return false;
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
