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
#endif
