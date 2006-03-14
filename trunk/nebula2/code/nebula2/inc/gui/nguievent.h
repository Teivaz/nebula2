#ifndef N_GUIEVENT_H
#define N_GUIEVENT_H
//------------------------------------------------------------------------------
/**
    @class nGuiEvent
    @ingroup Gui
    @brief A gui event class.

    Gui events are fired through the nGuiServer::PutEvent() method. Widgets
    can register with the nGuiServer to be notified through the
    nGuiWidget::OnEvent() method when events occur.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiEvent
{
public:
    /// event types
    enum Type
    {
        InvalidType,
        ButtonDown,
        ButtonUp,
        RButtonDown,
        RButtonUp,
        DoubleClick,
        Char,
        KeyDown,
        KeyUp,
        Show,
        Action,
        Enabled,
        Disabled,
        SliderChanged,
        SelectionChanged,
        SelectionDblClicked,
        DialogCancel,
        DialogOk,
        WheelUp,
        WheelDown,
        MenuEntrySelected,
        DragBoxStarted,
        DragBoxFinished,
        DragBoxCancelled,
        DragBoxUpdated,
    };

    /// default constructor
    nGuiEvent();
    /// constructor
    nGuiEvent(nGuiWidget* w, Type t);
    /// set the originator widget
    void SetWidget(nGuiWidget* w);
    /// get the originator widget
    nGuiWidget* GetWidget() const;
    /// set the event type
    void SetType(Type t);
    /// get the event type
    Type GetType() const;
    /// convert event type to string
    static const char* TypeToString(Type t);
    /// convert string to event type
    static nGuiEvent::Type StringToType(const char* str);

private:
    nRef<nGuiWidget> refWidget;
    Type type;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGuiEvent::nGuiEvent() :
    type(InvalidType)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiEvent::nGuiEvent(nGuiWidget* w, Type t) :
    refWidget(w),
    type(t)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiEvent::SetWidget(nGuiWidget* w)
{
    n_assert(w);
    this->refWidget = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiWidget*
nGuiEvent::GetWidget() const
{
    return this->refWidget;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiEvent::SetType(Type t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiEvent::Type
nGuiEvent::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiEvent::TypeToString(Type t)
{
    switch(t)
    {
        case ButtonDown:          return "ButtonDown";
        case ButtonUp:            return "ButtonUp";
        case RButtonDown:         return "RButtonDown";
        case RButtonUp:           return "RButtonUp";
        case DoubleClick:         return "DoubleClick";
        case Char:                return "Char";
        case KeyDown:             return "KeyDown";
        case KeyUp:               return "KeyUp";
        case Show:                return "Show";
        case Action:              return "Action";
        case Enabled:             return "Enabled";
        case Disabled:            return "Disabled";
        case SliderChanged:       return "SliderChanged";
        case SelectionChanged:    return "SelectionChanged";
        case SelectionDblClicked: return "SelectionDblClicked";
        case DialogCancel:        return "DialogCancel";
        case DialogOk:            return "DialogOk";
        case WheelUp:             return "WheelUp";
        case WheelDown:           return "WheelDown";
        case MenuEntrySelected:   return "MenuEntrySelected";
        case DragBoxStarted:      return "DragBoxStarted";
        case DragBoxFinished:     return "DragBoxFinished";
        case DragBoxCancelled:    return "DragBoxCancelled";
        case DragBoxUpdated:      return "DragBoxUpdated";
        default:                  return "InvalidType";
    }
}

//------------------------------------------------------------------------------
/**
    Convert given string to type.

    27-Aug-04   kims   created.
*/
inline
nGuiEvent::Type
nGuiEvent::StringToType(const char* str)
{
    if( strcmp(str, "ButtonDown") == 0)          return ButtonDown;
    else
    if( strcmp(str, "ButtonUp") == 0)            return ButtonUp;
    else
    if( strcmp(str, "RButtonDown") == 0)         return RButtonDown;
    else
    if( strcmp(str, "RButtonUp") == 0)           return RButtonUp;
    else
    if( strcmp(str, "DoubleClick") == 0)         return DoubleClick;
    else
    if( strcmp(str, "Char") == 0)                return Char;
    else
    if( strcmp(str, "KeyDown") == 0)             return KeyDown;
    else
    if( strcmp(str, "KeyUp") == 0)               return KeyUp;
    else
    if( strcmp(str, "Show") == 0)                return Show;
    else  
    if( strcmp(str, "Action") == 0)              return Action;
    else
    if( strcmp(str, "Enabled") == 0)             return Enabled;
    else
    if( strcmp(str, "Disabled") == 0)            return Disabled;
    else
    if( strcmp(str, "SliderChanged") == 0)       return SliderChanged;
    else
    if( strcmp(str, "SelectionChanged") == 0)    return SelectionChanged;
    else
    if( strcmp(str, "SelectionDblClicked") == 0) return SelectionDblClicked;
    else
    if( strcmp(str, "DialogCancel") == 0)        return DialogCancel;
    else
    if( strcmp(str, "DialogOk") == 0)            return DialogOk;
    else
    if( strcmp(str, "WheelUp") == 0)             return WheelUp;
    else
    if( strcmp(str, "WheelDown") == 0)           return WheelDown;
    else
    if( strcmp(str, "MenuEntrySelected") == 0)   return MenuEntrySelected;
    else
    if( strcmp(str, "DragBoxFinished") == 0)     return DragBoxFinished;
    else
    if( strcmp(str, "DragBoxCancelled") == 0)    return DragBoxCancelled;
    else
    if( strcmp(str, "DragBoxUpdated") == 0)      return DragBoxUpdated;
    else
    if( strcmp(str, "DragBoxStarted") == 0)      return DragBoxStarted;
    else                                         return InvalidType;
}
//------------------------------------------------------------------------------
#endif
