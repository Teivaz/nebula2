#ifndef N_GUICLIENTWINDOW_H
#define N_GUICLIENTWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiClientWindow
    @ingroup NebulaGuiSystem
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

    /// update the child widget layout, when position or size changes
    virtual void UpdateLayout(const rectangle& newRect);
protected:

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
#endif    
