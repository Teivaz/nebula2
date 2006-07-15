#ifndef N_GUIWIDGET_H
#define N_GUIWIDGET_H
//-----------------------------------------------------------------------------
/**
    @class nGuiWidget
    @ingroup Gui
    @brief An abstract gui widget (a rectangular click area)

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nscriptserver.h"
#include "kernel/nautoref.h"
#include "mathlib/vector.h"
#include "mathlib/rectangle.h"
#include "mathlib/matrix.h"
#include "util/nstring.h"
#include "kernel/nref.h"
#include "input/ninputevent.h"
#include "kernel/ndynautoref.h"
#include "gui/nguibrush.h"
#include "gui/nguieventhandler.h"

class nGuiServer;
class nGuiEvent;

//-----------------------------------------------------------------------------
class nGuiWidget : public nRoot
{
public:
    /// constructor
    nGuiWidget();
    /// destructor
    virtual ~nGuiWidget();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// set command to execute when the widget action happens
    void SetCommand(const char* cmd);
    /// get command to be executed.
    const char* GetCommand() const;
    /// set command to execute when widget is shown
    void SetShowCommand(const char* cmd);
    /// get command to execute on show
    const char* GetShowCommand() const;
    /// set command to execute when widget is hidden
    void SetHideCommand(const char* cmd);
    /// get command to execute on hide
    const char* GetHideCommand() const;
    /// set command to execute per frame while shown
    void SetFrameCommand(const char* cmd);
    /// get command to execute per frame while shown
    const char* GetFrameCommand() const;
    /// set command to execute on button down event
    void SetButtonDownCommand(const char* cmd);
    /// get command to execute on button down event
    const char* GetButtonDownCommand() const;
    /// set rect in parent widget's space (0.0 .. 1.0)
    void SetRect(const rectangle& r);
    /// get screen in parent widget's space
    const rectangle& GetRect() const;
    /// set the minimum size
    void SetMinSize(const vector2& s);
    /// get the minimum size
    const vector2& GetMinSize() const;
    /// set the maximum size
    void SetMaxSize(const vector2& s);
    /// get the maximum size
    const vector2& GetMaxSize() const;
    /// set optional tooltip string
    void SetTooltip(const char* str);
    /// get optional tooltip string
    const char* GetTooltip() const;
    /// show the widget and all its children
    void Show();
    /// hide the widget and all its children
    void Hide();
    /// currently shown?
    bool IsShown() const;
    /// enable the widget
    void Enable();
    /// disable the widget
    void Disable();
    /// currently enabled?
    virtual bool IsEnabled() const;
    /// has focus?
    bool HasFocus() const;
    /// widget blinking on/off
    void SetBlinking(bool b, nTime timeOut);
    /// set length of blink cycle in seconds
    void SetBlinkRate(double rate);
    /// get current blinking state
    bool GetBlinking() const;
    /// set sticky mouse behaviour
    void SetStickyMouse(bool b);
    /// get sticky mouse behaviour
    bool IsStickyMouse() const;
    /// rendering
    virtual bool Render();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up
    virtual bool OnButtonUp(const vector2& mousePos);
    /// handle right button down
    virtual bool OnRButtonDown(const vector2& mousePos);
    /// handle right button up
    virtual bool OnRButtonUp(const vector2& mousePos);
    /// handle double click on widget
    virtual void OnDoubleClick(const vector2& mousePos);
    /// handle character key event
    virtual void OnChar(uchar charCode);
    /// handle a key down event
    virtual bool OnKeyDown(nKey key);
    /// handle a key up event
    virtual void OnKeyUp(nKey key);
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// called when the widget's action should happen
    virtual void OnAction();
    /// called when widget becomes enabled
    virtual void OnEnabled();
    /// called when widget becomes disabled
    virtual void OnDisabled();
    /// called when widget position or size changes
    virtual void OnRectChange(const rectangle& newRect);
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// called when widget's window becomes focus window
    virtual void OnObtainFocus();
    /// called when widget's window loses focus window status
    virtual void OnLoseFocus();
    /// is position inside?
    virtual bool Inside(const vector2& p);
    /// get rectangle in screen space
    virtual rectangle GetScreenSpaceRect() const;
    /// get locale rectangle screen space 
    virtual rectangle GetScreenSpaceRect(const rectangle& src) const;
    /// get pointer to gui server
    nGuiServer* GetGuiServer();
    /// set the default brush
    void SetDefaultBrush(const char* name);
    /// get the default brush
    const char* GetDefaultBrush() const;
    /// set the pressed brush
    void SetPressedBrush(const char* name);
    /// get the pressed brush
    const char* GetPressedBrush() const;
    /// set the highlighted brush
    void SetHighlightBrush(const char* name);
    /// get the highlighted brush
    const char* GetHighlightBrush() const;
    /// set the disabled brush
    void SetDisabledBrush(const char* name);
    /// get the disabled brush
    const char* GetDisabledBrush() const;
    /// set clickthrough flag
    void SetBackground(bool b);
    /// get clickthrough flag
    bool IsBackground() const;
    /// set an optional click rect border
    void SetClickRectBorder(const vector2& b);
    /// get click rect border
    const vector2& GetClickRectBorder() const;
    /// set an external event handler to the widget (not owned by widget!)
    void SetEventHandler(nGuiEventHandler* h);
    /// get external event handler
    nGuiEventHandler* GetEventHandler() const;

protected:
    /// get owner-window of widget
    nGuiWidget* GetOwnerWindow();

    nString command;
    nString showCommand;
    nString hideCommand;
    nString frameCommand;
    nString buttonDownCommand;
    nString tooltip;
    static nClass* widgetClass;
    static nClass* windowClass;
    nTime lastButtonDownTime;
    nTime mouseWithinTime;
    nGuiEventHandler* eventHandler;

    bool shown;
    bool enabled;
    bool stickyMouse;
    bool hasFocus;
    bool backGround;
    bool mouseWithin;

    rectangle rect;
    vector2 clickRectBorder;
    vector2 minSize;
    vector2 maxSize;

    bool blinking;
    nTime blinkStarted;
    nTime blinkTimeOut;
    double blinkRate;

    nGuiBrush defaultBrush;
    nGuiBrush pressedBrush;
    nGuiBrush highlightBrush;
    nGuiBrush disabledBrush;
};

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWidget::SetClickRectBorder(const vector2& b)
{
    this->clickRectBorder = b;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiWidget::GetClickRectBorder() const
{
    return this->clickRectBorder;
}

//-----------------------------------------------------------------------------
/**
    Set the minimum size for the widget. NOTE: this is not enforced, 
    but merely a hint for layouter objects.
*/
inline
void
nGuiWidget::SetMinSize(const vector2& s)
{
    this->minSize = s;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiWidget::GetMinSize() const
{
    return this->minSize;
}

//-----------------------------------------------------------------------------
/**
    Set the maximum size for the widget. NOTE: this is not enforced, 
    but merely a hint for layouter objects.
*/
inline
void
nGuiWidget::SetMaxSize(const vector2& s)
{
    this->maxSize = s;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiWidget::GetMaxSize() const
{
    return this->maxSize;
}

//-----------------------------------------------------------------------------
/**
    Set the optional tooltip string. When the mouse is over this
    widget, the tooltip string will be set as the global tooltip string
    in the gui server object.

    @param  str     the tooltip string    
*/
inline
void
nGuiWidget::SetTooltip(const char* str)
{
    n_assert(str);
    this->tooltip = str;
}

//-----------------------------------------------------------------------------
/**
    Get the tooltip string.
*/
inline
const char*
nGuiWidget::GetTooltip() const
{
    return this->tooltip.Get();
}

//-----------------------------------------------------------------------------
/**
   Set command that should be executed if menu entry is activated.

   @param cmd Command to be executed.
*/
inline
void
nGuiWidget::SetCommand(const char* cmd)
{
    this->command = cmd;
}

//-----------------------------------------------------------------------------
/**
   Command to be executed.

   @return Command to be executed.
*/
inline
const char*
nGuiWidget::GetCommand() const
{
    return this->command.IsEmpty() ? 0 : this->command.Get();
}

//-----------------------------------------------------------------------------
/**
    Set script command to execute on shown.
*/
inline
void
nGuiWidget::SetShowCommand(const char* cmd)
{
    this->showCommand = cmd;
}

//-----------------------------------------------------------------------------
/**
    Get script command to execute on show.
*/
inline
const char*
nGuiWidget::GetShowCommand() const
{
    return this->showCommand.Get();
}

//-----------------------------------------------------------------------------
/**
    Set script command to execute on hide.
*/
inline
void
nGuiWidget::SetHideCommand(const char* cmd)
{
    this->hideCommand = cmd;
}

//-----------------------------------------------------------------------------
/**
    Get script command to execute on hide.
*/
inline
const char*
nGuiWidget::GetHideCommand() const
{
    return this->hideCommand.Get();
}

//-----------------------------------------------------------------------------
/**
    Set script command to execute per frame while visible.
*/
inline
void
nGuiWidget::SetFrameCommand(const char* cmd)
{
    this->frameCommand = cmd;
}

//-----------------------------------------------------------------------------
/**
    Get script command to execute per frame while visible.
*/
inline
const char* 
nGuiWidget::GetFrameCommand() const
{
    return this->frameCommand.Get();
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWidget::SetButtonDownCommand(const char* cmd)
{
    n_assert(cmd);
    buttonDownCommand = cmd;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiWidget::GetButtonDownCommand() const
{
    return this->buttonDownCommand.Get();
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
nGuiWidget::HasFocus() const
{
    return this->hasFocus;
}

//-----------------------------------------------------------------------------
/**
    Set sticky mouse behaviour. If sticky mouse is enabled, the widget
    will remain focused even when the mouse has moved outside the
    widget's area. Default is false.
*/
inline
void
nGuiWidget::SetStickyMouse(bool b)
{
    this->stickyMouse = b;
}

//-----------------------------------------------------------------------------
/**
    Get the sticky mouse behaviour of the widget.
*/
inline
bool
nGuiWidget::IsStickyMouse() const
{
    return this->stickyMouse;
}

//-----------------------------------------------------------------------------
/**
   Set screen space rectangle.
*/
inline
void
nGuiWidget::SetRect(const rectangle& r)
{
    this->OnRectChange(r);
}

//-----------------------------------------------------------------------------
/**
   Get screen space rectangle
*/
inline
const rectangle&
nGuiWidget::GetRect() const
{
    return this->rect;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWidget::SetBlinkRate(double rate)
{
    this->blinkRate = rate;
}
//-----------------------------------------------------------------------------
/**
*/
inline
bool
nGuiWidget::GetBlinking() const
{
    return this->blinking;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWidget::SetDefaultBrush(const char* name)
{
    this->defaultBrush.SetName(name);
}

//-----------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiWidget::GetDefaultBrush() const
{
    return this->defaultBrush.GetName().IsEmpty() ? 0 : this->defaultBrush.GetName().Get();
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWidget::SetPressedBrush(const char* name)
{
    this->pressedBrush.SetName(name);
}

//-----------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiWidget::GetPressedBrush() const
{
    return this->pressedBrush.GetName().IsEmpty() ? 0 : this->pressedBrush.GetName().Get();
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWidget::SetHighlightBrush(const char* name)
{
    this->highlightBrush.SetName(name);
}

//-----------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiWidget::GetHighlightBrush() const
{
    return this->highlightBrush.GetName().IsEmpty() ? 0 : this->highlightBrush.GetName().Get();
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWidget::SetDisabledBrush(const char* name)
{
    this->disabledBrush.SetName(name);
}

//-----------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiWidget::GetDisabledBrush() const
{
    return this->disabledBrush.GetName().IsEmpty() ? 0 : this->disabledBrush.GetName().Get();
}

//-----------------------------------------------------------------------------
/**
    Shows the widget, invokes OnShow() on itself and all visible children.
*/
inline
void
nGuiWidget::Show()
{
    this->OnShow();
    this->shown = true;
}

//-----------------------------------------------------------------------------
/**
    Hide the widget, invokes OnHide() on itself and all visible children.
*/
inline
void
nGuiWidget::Hide()
{
    this->OnHide();
    this->shown = false;
}

//-----------------------------------------------------------------------------
/**
    Widget currently visible?
*/
inline
bool
nGuiWidget::IsShown() const
{
    return this->shown;
}

//-----------------------------------------------------------------------------
/**
    Enables the widget. Enabled widgets react to the user's input.
*/
inline
void
nGuiWidget::Enable()
{
    this->enabled = true;
    this->OnEnabled();
}

//-----------------------------------------------------------------------------
/**
    Disable the widget. Disabled widgets are greyed out and don't react
    to user input.
*/
inline
void
nGuiWidget::Disable()
{
    this->enabled = false;
    this->OnDisabled();
}

//-----------------------------------------------------------------------------
/**
    Return true if the widget is currently enabled.
*/
inline
bool
nGuiWidget::IsEnabled() const
{
    return this->enabled;
}

//------------------------------------------------------------------------------
/**
    A background window will always be behind all other windows.
*/
inline
void
nGuiWidget::SetBackground(bool b)
{
    this->backGround = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiWidget::IsBackground() const
{
    return this->backGround;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiWidget::SetEventHandler(nGuiEventHandler* h)
{
    this->eventHandler = h;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nGuiEventHandler*
nGuiWidget::GetEventHandler() const
{
    return this->eventHandler;
}

//-----------------------------------------------------------------------------
#endif

