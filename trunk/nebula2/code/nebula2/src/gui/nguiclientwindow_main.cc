//------------------------------------------------------------------------------
//  nguiclientwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiclientwindow.h"
#include "gui/nguibutton.h"
#include "gui/nguitextbutton.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"

nNebulaScriptClass(nGuiClientWindow, "nguiwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiClientWindow::nGuiClientWindow() :
    title("NoTitle"),
    flags(Movable | Resizable | CloseButton | TitleBar),
    dragging(false),
    resizing(false),
    titleHeight(0.0f),
    hasBorder(false)
{
    this->shown = false;
    this->SetRect(rectangle(vector2(0.1f, 0.1f), vector2(0.6f, 0.6f)));
    this->SetFadeInTime(0.2f);
    this->SetFadeOutTime(0.2f);
}

//------------------------------------------------------------------------------
/**
*/
nGuiClientWindow::~nGuiClientWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Update child widget rectangles.
*/
void
nGuiClientWindow::UpdateLayout(const rectangle& newRect)
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    rectangle closeRect;
    rectangle sizeRect;
    rectangle titleRect;
    rectangle clientRect;

    // update close button
    if (this->refCloseButton.isvalid())
    {
        vector2 closeSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("close_n");
        closeRect.v0.x = newRect.width() - (closeSize.x + 0.005f);
        closeRect.v0.y = (this->titleHeight - closeSize.y) * 0.5f;
        closeRect.v1.x = newRect.width() - 0.005f;
        closeRect.v1.y = closeRect.v0.y + closeSize.y;
        this->refCloseButton->SetRect(closeRect);
    }

    // update size button
    if (this->refSizeButton.isvalid())
    {
        vector2 sizeSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("size_n");
        sizeRect.v0 = newRect.size() - sizeSize;
        sizeRect.v1 = newRect.size();
        this->refSizeButton->SetRect(sizeRect);
    }

    // update title bar
    if (this->refTitleBar.isvalid())
    {
        titleRect.v0.x = 0.0f;
        titleRect.v0.y = 0.0f;
        titleRect.v1.x = newRect.width();
        titleRect.v1.y = this->titleHeight;
        this->refTitleBar->SetRect(titleRect);
    }

    // update client area
    const rectangle& border = this->GetBorder();
    clientRect.v0.x = border.v0.x;
    clientRect.v0.y = titleRect.height() + border.v0.y;
    clientRect.v1.x = newRect.width() - border.v1.x;
    clientRect.v1.y = newRect.height() - border.v1.y;
    if (this->refSizeButton.isvalid())
    {
        clientRect.v1.y = sizeRect.v0.y - border.v1.y;
    }
    this->refFormLayout->SetRect(clientRect);
}

//------------------------------------------------------------------------------
/**
    Create and initialize child widgets.
*/
void
nGuiClientWindow::OnShow()
{
    n_assert(!this->refCloseButton.isvalid());
    n_assert(!this->refSizeButton.isvalid());
    n_assert(!this->refTitleBar.isvalid());
    n_assert(!this->refFormLayout.isvalid());
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    // set own background resource and initial size
    if (!this->GetDefaultBrush())
    {
        this->SetDefaultBrush("window");
    }

    // update vertical text extent
    vector2 titleSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("titlebar");
    this->titleHeight = titleSize.y;
    this->SetMinSize(vector2(5.0f * this->titleHeight, 2.0f * this->titleHeight));

    // create child widgets
    kernelServer->PushCwd(this);

    // create and configure the title bar button (also used to move window)
    if (this->HasTitleBar())
    {
        nGuiTextButton* btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", "TitleBar");
        n_assert(btn);
        btn->SetDefaultBrush("titlebar");
        btn->SetPressedBrush("titlebar");
        btn->SetHighlightBrush("titlebar");
        btn->SetText(this->GetTitle());
        btn->SetColor(skin->GetTitleTextColor());
        btn->SetPressedOffset(vector2::zero);
        if (this->IsMovable())
        {
            btn->SetStickyMouse(true);
        }
        btn->OnShow();
        this->refTitleBar = btn;
    }

    // create and configure close button
    if (this->HasCloseButton())
    {
        nGuiButton* btn = (nGuiButton*) kernelServer->New("nguibutton", "CloseButton");
        n_assert(btn);
        btn->SetDefaultBrush("close_n");
        btn->SetPressedBrush("close_p");
        btn->SetHighlightBrush("close_h");
        btn->OnShow();
        this->refCloseButton = btn;
    }

    // create and configure size button
    if (this->IsResizable())
    {
        nGuiButton* btn = (nGuiButton*) kernelServer->New("nguibutton", "SizeButton");
        n_assert(btn);
        btn->SetDefaultBrush("size_n");
        btn->SetPressedBrush("size_p");
        btn->SetHighlightBrush("size_h");
        btn->SetStickyMouse(true);
        btn->OnShow();
        this->refSizeButton = btn;
    }

    // create and configure the client area form layout
    nGuiFormLayout* layout = (nGuiFormLayout*) kernelServer->New("nguiformlayout", "Layout");
    n_assert(layout);
    this->refFormLayout = layout;

    // update child widget rectangles
    this->SetRect(this->rect);

    nGuiServer::Instance()->RegisterEventListener(this);
    nGuiWindow::OnShow();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiClientWindow::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);

    if (this->refCloseButton.isvalid())
    {
        this->refCloseButton->Release();
        n_assert(!this->refCloseButton.isvalid());
    }
    if (this->refSizeButton.isvalid())
    {
        this->refSizeButton->Release();
        n_assert(!this->refSizeButton.isvalid());
    }
    if (this->refTitleBar.isvalid())
    {
        this->refTitleBar->Release();
        n_assert(!this->refTitleBar.isvalid());
    }
    if (this->refFormLayout.isvalid())
    {
        this->refFormLayout->Release();
        n_assert(!this->refFormLayout.isvalid());
    }
    nGuiWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
    The mouse handler will care about window movement and resizing.
*/
bool
nGuiClientWindow::OnMouseMoved(const vector2& mousePos)
{
    if (this->dragging)
    {
        vector2 dragDist = mousePos - this->startMousePos;
        rectangle r = this->startRect;
        r.v0 += dragDist;
        r.v1 += dragDist;
        vector2 size = r.v1 - r.v0;
        if (r.v0.x < 0.0f)
        {
            r.v0.x = 0.0f;
            r.v1.x = size.x;
        }
        else if (r.v1.x > 1.0f)
        {
            r.v1.x = 1.0f;
            r.v0.x = 1.0f - size.x;
        }
        if (r.v0.y < 0.0f)
        {
            r.v0.y = 0.0f;
            r.v1.y = size.y;
        }
        else if (r.v1.y > 1.0f)
        {
            r.v1.y = 1.0f;
            r.v0.y = 1.0f - size.y;
        }
        this->SetRect(r);
    }
    else if (this->resizing)
    {
        vector2 dragDist = mousePos - this->startMousePos;
        rectangle r = this->startRect;
        r.v1 += dragDist;
        vector2 size = r.size();
        if (size.x < this->minSize.x)
        {
            r.v1.x = r.v0.x + this->minSize.x;
        }
        if (size.y < this->minSize.y)
        {
            r.v1.y = r.v0.y + this->minSize.y;
        }
        if (r.v1.x > 1.0f)
        {
            r.v1.x = 1.0f;
        }
        if (r.v1.y > 1.0f)
        {
            r.v1.y = 1.0f;
        }
        this->SetRect(r);
    }
    return nGuiWindow::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
    Called when the client window rectangle changes. This repositions
    the child widget.
*/
void
nGuiClientWindow::OnRectChange(const rectangle& newRect)
{
    if (this->IsShown())
    {
        this->UpdateLayout(newRect);
    }
    nGuiWindow::OnRectChange(newRect);
}

//------------------------------------------------------------------------------
/**
    Check for events from our child widgets and handle them accordingly.
*/
void
nGuiClientWindow::OnEvent(const nGuiEvent& event)
{
    if (this->refTitleBar.isvalid() && (event.GetWidget() == this->refTitleBar.get()))
    {
        // handle events from our title bar
        if (this->IsMovable())
        {
            if (event.GetType() == nGuiEvent::ButtonDown)
            {
                this->BeginDrag();
            }
            else if (event.GetType() == nGuiEvent::ButtonUp)
            {
                this->EndDrag();
            }
        }
    }
    else if (this->refCloseButton.isvalid() && (event.GetWidget() == this->refCloseButton.get()))
    {
        // handle events from our close button
        if (event.GetType() == nGuiEvent::ButtonUp)
        {
            this->SetCloseRequested(true);
        }
    }
    else if (this->refSizeButton.isvalid() && (event.GetWidget() == this->refSizeButton.get()))
    {
        // handle events from our size button
        if (event.GetType() == nGuiEvent::ButtonDown)
        {
            this->BeginResize();
        }
        else if (event.GetType() == nGuiEvent::ButtonUp)
        {
            this->EndResize();
        }
    }
    nGuiWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Begin dragging the window. This is usually invoked by the
    title bar button through the script interface.
*/
void
nGuiClientWindow::BeginDrag()
{
    n_assert(this->IsMovable());
    this->dragging      = true;
    this->startMousePos = nGuiServer::Instance()->GetMousePos();
    this->startRect     = this->rect;
}

//------------------------------------------------------------------------------
/**
    Finish dragging. This is usually invoked by the title bar button
    through the script interface.
*/
void
nGuiClientWindow::EndDrag()
{
    this->dragging = false;
}

//------------------------------------------------------------------------------
/**
    Finish dragging. This is usually invoked by the title bar button
    through the script interface.
*/
void
nGuiClientWindow::CancelDrag()
{
    this->dragging = false;
    this->rect = this->startRect;
}

//------------------------------------------------------------------------------
/**
    Begin resizing. This is usually invoked by the resize button
    through the script interface.
*/
void
nGuiClientWindow::BeginResize()
{
    n_assert(this->IsResizable());
    this->resizing      = true;
    this->startMousePos = nGuiServer::Instance()->GetMousePos();
    this->startRect     = this->rect;
}

//------------------------------------------------------------------------------
/**
    End resizing. This is usually invoked by the resize button
    through the script interface.
*/
void
nGuiClientWindow::EndResize()
{
    this->resizing = false;
}

//------------------------------------------------------------------------------
/**
    Cancel resizing. This is usually invoked by the resize button
    through the script interface.
*/
void
nGuiClientWindow::CancelResize()
{
    this->resizing = false;
    this->rect = this->startRect;
}

//------------------------------------------------------------------------------
/**
    Close all sibling windows which are of the same class of me. Can be used
    to implement Singleton windows.
*/
void
nGuiClientWindow::CloseSiblings()
{
    nGuiWindow* window;
    for (window = (nGuiWindow*) this->GetParent()->GetHead();
         window;
         window = (nGuiWindow*) window->GetSucc())
    {
        if ((window != this) && window->IsInstanceOf(this->GetClass()))
        {
            window->SetDismissed(true);
        }
    }
}

