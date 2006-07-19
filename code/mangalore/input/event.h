#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H
//------------------------------------------------------------------------------
/**
    @class Input::Event

    Input::Event's are Messages that where emited from the Input::Source
    to the Input::Sinks - the attached Msg::Ports
    
    (C) 2006 RadonLabs GmbH
*/
#include "message/msg.h"

namespace Input
{
class Mapping;

class Event : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(Event);
    DeclareMsgId;

public:
    /// constructor
    Event();
    /// destructor
    virtual ~Event();

    /// is this event already handled?
    bool Handled(int recursive = 3) const;
    /// set this event was handled
    void SetHandled();

    /// connect the event to other events (so a button click is able to now if up or down already was handled) 
    void Connect(Event* relatedEvent);
    /// disconnect all references to other events !! needed to broke up the circular references !!
    void Disconnect();

    /// set the mapping from where the event comes
    void SetMapping(Mapping* mapping);
    /// get the mapping from where the event comes
    Mapping* GetMapping() const;

    /// set the input priority of the chain element that just get this event
    void SetPriority(int priority);
    /// get the input priority of the chain element that just get this event
    int GetPriority() const;

    /// the type of the event
    enum Type
    {
        AxisMoved = 0,          ///< a axis has changed
        MouseMoved,             ///< windows mouse has moved, and the win proc has generated a event for

        ButtonDown,             ///< a button gone down this frame
        ButtonUp,               ///< a button gone up this frame
        ButtonClicked,          ///< a button was clicked this frame
        ButtonDoubleClicked,    ///< a button was double clicked this frame
        
        ButtonPressed,          ///< a button is pressed this frame

        Character,              ///< a character/key was pressed, text input

        BeginFrame,             ///< a new frame begins
        EndFrame,               ///< the frame ends

        NumEventType,
        InvalidEventType,
    };

    /// get event type
    Type GetType() const;
    /// set event type
    void SetType(Type t);

    /// the button that was pressed
    enum Button
    {
        LeftMouseButton = 0,    // the left mouse button
        MiddleMouseButton,      // the middle mouse button
        RightMouseButton,       // the right mouse buttons
        UndefinedButton,        // any other button
        NumButtonTypes,         // number of button types
    };

    /// --- data of button events ---
    /// does this event have a mouse position (special events from win proc have a mouse position)
    bool HasMousePosition() const;
    /// set position of mouse where the button event occured
    void SetMousePosition(const vector2& pos);
    /// get position of mouse where the button event occured
    const vector2& GetMousePosition() const;

    /// get the pressed button (to easy know the mouse buttons)
    void SetButton(Button b);
    /// get the pressed button (to easy know the mouse buttons)
    Button GetButton() const;

    /// --- data of mouse moved events ---
    /// get absolute mouse position (screen pixels)
    int GetAbsMousePositionX() const;
    /// get absolute mouse position (screen pixels)
    int GetAbsMousePositionY() const;
    /// get relative mouse position (between 0. and 1.)
    const vector2& GetRelMousePosition() const;

    /// set absolute mouse position (screen pixels)
    void SetAbsMousePosition(int x, int y);
    /// set relative mouse position (between 0. and 1.)
    void SetRelMousePosition(const vector2& pos);

    /// --- data of axis events ---
    /// get axis value
    float GetAxisValue() const;
    /// set axis value
    void SetAxisValue(float val);

    /// --- data of character events ---
    /// get character code
    int GetCharacterCode() const;
    /// set character code
    void SetCharacterCode(int charCode);

private:
    /// the type of the event
    Type type; 
    /// button that was used in this event (to identity the mouse buttons easy)
    Button button;
    /// pointer to the input mapping that has created the event
    Mapping* mapping;

    // the event data: TODO: union?
    vector2 mousePosition;
    bool hasMousePosition;
    float axisValue;
    int characterCode;
    vector2 relMousePosition;
    int absMousePositionX;
    int absMousePositionY;

    // array of connected events, used to detect a handled button up in the related button clicked event
    nArray<Ptr<Input::Event> > connectedEvents;

    // was this event handled?
    bool handled;

    /// the priority of the sink in the chain that just get this event
    int priority;
};

}; // namespace Input
//------------------------------------------------------------------------------
#endif