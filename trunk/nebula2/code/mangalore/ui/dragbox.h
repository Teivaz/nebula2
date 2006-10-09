#ifndef UI_DRAGBOX_H
#define UI_DRAGBOX_H
//------------------------------------------------------------------------------
/**
    @class UI::DragBox


    (C) 2006 RadonLabs GmbH
*/
#include "message/msg.h"
#include "ui/dragboxdisplay.h"
#include "mathlib/rectangle.h"
#include "input/event.h"

namespace Managers
{
    class FactoryManager;
}

namespace Message
{
    class Msg;
	class Port;
};

namespace Input
{
    class Mapping;
}

//------------------------------------------------------------------------------
namespace UI
{

class DragBox : public Message::Port
{
    DeclareRtti;
	DeclareFactory(DragBox);

public:

    /// constructor
    DragBox();
    /// destructor
    virtual ~DragBox();
    /// enable/disable the control
    void SetEnabled(bool b);
    /// get enabled state of control
    bool IsEnabled() const;
    /// set the drag size threshold in screen space units
    void SetDragThreshold(float s);
    /// get the drag size threshold in screen space units
    float GetDragThreshold() const;

    /// set the receiver for all the messages this class will send
	void SetMsgReceiver(Message::Port* p);
    /// get the receiver for all the messages this class will send
    Message::Port* GetMsgReceiver() const;

	/// set the dragboxdisplay (will need to visualize the dimensions of the dragbox)
	void SetDragBoxDisplay( UI::DragBoxDisplay* d );
	/// get the dragboxdisplay (will need to visualize the dimensions of the dragbox)
	UI::DragBoxDisplay* GetDragBoxDisplay() const;

    /// accept input events
    virtual bool Accepts(Message::Msg* msg);
    /// handle input events
    virtual void HandleMessage(Message::Msg* msg);

    /// handle mouse move
    virtual bool OnMouseMove(const vector2& mousePos);
    /// handle left button down
    virtual bool OnLeftButtonDown(const vector2& mousePos);
    /// handle left button Up
    virtual bool OnLeftButtonUp(const vector2& mousePos);
    /// handle right button down
    virtual bool OnRightButtonDown(const vector2& mousePos);
    /// handle right button Up
    virtual bool OnRightButtonUp(const vector2& mousePos);

	/// resets all smartpointers
	/// after the call, the settings for the message receiver and the display was lost
	/// before you can destroy the message receiver you must call this function
	void Deactivate();

    /// return true if currently dragging
    bool IsDragging() const;
    /// return true if the drag box data is valid
    bool IsDragBoxValid() const;
    /// return the current drag box in screen space
    const rectangle& GetDragBox2D() const;
    /// return true if world space position is inside current drag box
    bool IsWorldCoordInDragBox(const vector3& pos) const;
    /// manually cancel the current dragging operation
    void CancelDragging();

private:

    /// attach to input mappings
    void AttachInput();
    /// remove from input mappings
    void RemoveInput();

    /// update 2d and 3d drag box data
    void UpdateDragBox(const vector2& mousePos);

    float dragThreshold;
    bool enabled;
    bool dragging;
    Ptr<Input::Event> buttonDown;
	bool dragBoxCancelled;
    bool dragBoxValid;
    bool dragBoxStartedSent;
    vector2 dragStartMousePos;
    rectangle dragBox2D;


	Ptr<UI::DragBoxDisplay> display;

	Ptr<Message::Port> receiver;

};

RegisterFactory(DragBox);


//------------------------------------------------------------------------------
/**
    set the for all the messages this class will send.
*/
inline
void
DragBox::SetMsgReceiver(Message::Port* p)
{
	// check if there is a receiver
    n_assert(p);
	// set the receiver
	this->receiver = p;
}

//------------------------------------------------------------------------------
/**
    get the for all the messages this class will send.
*/
inline
Message::Port*
DragBox::GetMsgReceiver() const
{
	// check if there is receiver
	n_assert( this->receiver.isvalid() );
	// return the receiver
	return this->receiver;
}

//------------------------------------------------------------------------------
/**
    set the dragboxdisplay (will need to visualize the dimensions of the dragbox).
*/
inline
void
DragBox::SetDragBoxDisplay( UI::DragBoxDisplay* d )
{
	// check if there is a dragboxdisplay
    n_assert(d);
	// set the dragboxdisplay
	this->display = d;
}

//------------------------------------------------------------------------------
/**
    get the dragboxdisplay (will need to visualize the dimensions of the dragbox).
*/
inline
UI::DragBoxDisplay*
DragBox::GetDragBoxDisplay() const
{
	// check if there is a dragboxdisplay
	n_assert( this->display.isvalid() );
	// return the dragboxdisplay
	return this->display;
}

//------------------------------------------------------------------------------
/**
    Set the mouse-move distance from the start point until a drag operation
    is valid in screen space. To set the threshold in pixels, compute
    numPixels / displayResolution.
*/
inline
void
DragBox::SetDragThreshold(float s)
{
    n_assert(s > 0.0f);
    this->dragThreshold = s;
}

//------------------------------------------------------------------------------
/**
    Returns the drag threshold distance.
*/
inline
float
DragBox::GetDragThreshold() const
{
    return this->dragThreshold;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
DragBox::IsEnabled() const
{
    return this->enabled;
}


//------------------------------------------------------------------------------
/**
*/
inline
bool
DragBox::IsDragging() const
{
    return this->dragging;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
DragBox::IsDragBoxValid() const
{
    return this->dragBoxValid;
}

//------------------------------------------------------------------------------
/**
*/
inline
const rectangle&
DragBox::GetDragBox2D() const
{
    n_assert(this->dragBoxValid);
    return this->dragBox2D;
}



} // namespace UI
//------------------------------------------------------------------------------
#endif