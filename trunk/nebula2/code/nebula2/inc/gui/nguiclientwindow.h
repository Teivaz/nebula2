#ifndef N_GUICLIENTWINDOW_H
#define N_GUICLIENTWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiClientWindow
    @ingroup Gui
    @brief A window with optional titlebar, close button, size handles and a
    client area.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiwindow.h"
#include "gui/nguitextbutton.h"
#include "gui/nguiformlayout.h"

class nGuiButton;

//------------------------------------------------------------------------------
class nGuiClientWindow : public nGuiWindow
{
public:
    /// constructor
    nGuiClientWindow();
    /// destructor
    virtual ~nGuiClientWindow();
    /// handle mouse move, route to focus window if exists
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called when widget position or size changes
    virtual void OnRectChange(const rectangle& newRect);
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// set movable flag
    void SetMovable(bool b);
    /// get movable flag
    bool IsMovable() const;
    /// set resize flag
    void SetResizable(bool b);
    /// get resizeable flag
    bool IsResizable() const;
    /// enable/disable close button
    void SetCloseButton(bool b);
    /// has close button?
    bool HasCloseButton() const;
    /// has the window a title bar (default yes)
    void SetTitleBar(bool b);
    /// return if title bar is enabled
    bool HasTitleBar() const;
    /// set title string
    void SetTitle(const char* s);
    /// get title string
    const char* GetTitle() const;
    /// close all sibling windows of the same class
    void CloseSiblings();
    /// set border (overrides the default border setup of the skin)
    void SetBorder(const rectangle& border);
    /// get border
    const rectangle& GetBorder() const;
    /// get the window's form layout object
    nGuiFormLayout* GetFormLayout() const;

protected:
    /// update the child widget layout, when position or size changes
    virtual void UpdateLayout(const rectangle& newRect);
    /// begin dragging
    void BeginDrag();
    /// end dragging
    void EndDrag();
    /// cancel dragging
    void CancelDrag();
    /// begin sizing
    void BeginResize();
    /// end resizing
    void EndResize();
    /// cancel resizing
    void CancelResize();

    enum Flags
    {
        Movable = (1<<0),
        Resizable = (1<<1),
        CloseButton = (1<<2),
        TitleBar = (1<<3),
    };
    nString title;
    ushort flags;
    nRef<nGuiButton>     refCloseButton;
    nRef<nGuiButton>     refSizeButton;
    nRef<nGuiTextButton> refTitleBar;
    nRef<nGuiFormLayout> refFormLayout;
    bool dragging;
    bool resizing;
    vector2 startMousePos;
    rectangle startRect;
    float titleHeight;
    bool hasBorder;
    rectangle border;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiClientWindow::SetTitleBar(bool b)
{
    if (b) this->flags |= TitleBar;
    else   this->flags &= ~TitleBar;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiClientWindow::HasTitleBar() const
{
    return (0 != (this->flags & TitleBar));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiClientWindow::SetMovable(bool b)
{
    if (b) this->flags |= Movable;
    else   this->flags &= ~Movable;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiClientWindow::IsMovable() const
{
    return (0 != (this->flags & Movable));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiClientWindow::SetResizable(bool b)
{
    if (b) this->flags |= Resizable;
    else   this->flags &= ~Resizable;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiClientWindow::IsResizable() const
{
    return (0 != (this->flags & Resizable));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiClientWindow::SetCloseButton(bool b)
{
    if (b) this->flags |= CloseButton;
    else   this->flags &= ~CloseButton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiClientWindow::HasCloseButton() const
{
    return (0 != (this->flags & CloseButton));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiClientWindow::SetTitle(const char* s)
{
    this->title = s;
    if (this->refTitleBar.isvalid())
    {
        this->refTitleBar->SetText(s);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiClientWindow::GetTitle() const
{
    return this->title.IsEmpty() ? 0 : this->title.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiClientWindow::SetBorder(const rectangle& b)
{
    this->hasBorder = true;
    this->border = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const rectangle&
nGuiClientWindow::GetBorder() const
{
    if (this->hasBorder)
    {
        return this->border;
    }
    // fallback to the skin border
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);
    return skin->GetWindowBorder();
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiFormLayout*
nGuiClientWindow::GetFormLayout() const
{
    return this->refFormLayout;
}

//------------------------------------------------------------------------------
#endif
