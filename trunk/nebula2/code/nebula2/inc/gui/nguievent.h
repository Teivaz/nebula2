#ifndef N_GUIEVENT_H
#define N_GUIEVENT_H
//------------------------------------------------------------------------------
/**
    @class nGuiEvent
    @ingroup NebulaGuiSystem
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
        Hide,
        Action,
        Enabled,
        Disabled,
        SliderChanged,
        SelectionChanged,
        DialogCancel,
        DialogOk,
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
    return this->refWidget.get();
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
        case ButtonDown:       return "ButtonDown";
        case ButtonUp:         return "ButtonUp";
        case RButtonDown:      return "RButtonDown";
        case RButtonUp:        return "RButtonUp";
        case DoubleClick:      return "DoubleClick";
        case Char:             return "Char";
        case KeyDown:          return "KeyDown";
        case KeyUp:            return "KeyUp";
        case Show:             return "Show";
        case Hide:             return "Hide";
        case Action:           return "Action";
        case Enabled:          return "Enabled";
        case Disabled:         return "Disabled";
        case SliderChanged:    return "SliderChanged";
        case SelectionChanged: return "SelectionChanged";
        case DialogCancel:     return "DialogCancel";
        case DialogOk:         return "DialogOk";
        default:               return "InvalidType";
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
    if( strcmp(str, "ButtonDown") == 0)       return ButtonDown;
    else
    if( strcmp(str, "ButtonUp") == 0)         return ButtonUp;
    else
    if( strcmp(str, "RButtonDown") == 0)      return RButtonDown;
    else
    if( strcmp(str, "RButtonUp") == 0)        return RButtonUp;
    else
    if( strcmp(str, "DoubleClick") == 0)      return DoubleClick;
    else
    if( strcmp(str, "Char") == 0)             return Char;
    else
    if( strcmp(str, "KeyDown") == 0)          return KeyDown;
    else
    if( strcmp(str, "KeyUp") == 0)            return KeyUp;
    else
    if( strcmp(str, "Show") == 0)             return Show;
    else
    if( strcmp(str, "Hide") == 0)             return Hide;
    else
    if( strcmp(str, "Action") == 0)           return Action;
    else
    if( strcmp(str, "Enabled") == 0)          return Enabled;
    else
    if( strcmp(str, "Disabled") == 0)         return Disabled;
    else
    if( strcmp(str, "SliderChanged") == 0)    return SliderChanged;
    else
    if( strcmp(str, "SelectionChanged") == 0) return SelectionChanged;
    else
    if( strcmp(str, "DialogCancel") == 0)     return DialogCancel;
    else
    if( strcmp(str, "DialogOk") == 0)         return DialogOk;
    else                                      return InvalidType;
}
//------------------------------------------------------------------------------
#endif
