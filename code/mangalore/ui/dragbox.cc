//------------------------------------------------------------------------------
//  ui/dragbox.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "ui/dragbox.h"
#include "gui/nguiserver.h"

#include "input/server.h"
#include "input/event.h"
#include "input/mapping.h"
#include "input/priority.h"

#include "msg/ui/dragboxupdated.h"
#include "msg/ui/dragboxstarted.h"
#include "msg/ui/dragboxready.h"
#include "msg/ui/dragboxcancelled.h"

namespace UI
{
	ImplementRtti(UI::DragBox,  Foundation::RefCounted);
	ImplementFactory(UI::DragBox);


//------------------------------------------------------------------------------
/**
*/
DragBox::DragBox()
{
	this->dragThreshold = (10.0f / 1024.0f);

	this->enabled = false;
	this->dragging = false;
	this->dragBoxValid = false;
	this->dragBoxStartedSent = false;
}

//------------------------------------------------------------------------------
/**
*/
DragBox::~DragBox()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void
DragBox::SetEnabled(bool b)
{
	// check if the new state is not equal to the old
    if (this->enabled != b)
    {
		// if not equal ...
		// set the new state
        this->enabled = b;
        
		// check if the dragbox is enabled
        if (this->enabled)
        {
			// if enabled ...
			// attach the dragbox to the inputserver
            this->AttachInput();
        }
        else
        {
			// if not enabled ...
			// deattach the dragbox from the inputserver
            this->RemoveInput();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
DragBox::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (this->IsEnabled() && msg->CheckId(Input::Event::Id)) return true;
	return Message::Port::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
DragBox::HandleMessage(Message::Msg* msg)
{
    n_assert(msg);

    if (msg->CheckId(Input::Event::Id))
    {
		// convert the message pointer to an event pointer
        Input::Event* event = static_cast<Input::Event*>(msg);
        
		// check if the eventtyp is MouseMoved
        if (event->GetType() == Input::Event::MouseMoved)
        {
			// if it ...
			// call the memberfunction OnMouseMove with the relative mouse position
            this->OnMouseMove(event->GetRelMousePosition());
        }
        else
        {
			// if not MouseMoved ...
			// check if the event is NOT handled
            if (!event->Handled())
            {
				// if not ...
                // handle mouse button events
				// init the handled flag with false
                bool handled = false;
				// check if the event contains the mouseposition and so mousedata
                if (event->HasMousePosition())
                {
					// if contains ...
					// switch the eventtype
                    switch (event->GetType())
                    {
						// in the case of ButtonUp
						case Input::Event::ButtonDoubleClicked:
						{
							this->dragging = false;
							break;
						}
						// in the case of ButtonUp
						case Input::Event::ButtonUp:
                        {
							// switch the botton
                            switch (event->GetButton())
                            {
								// in the case of LeftMouseButton
                                case Input::Event::LeftMouseButton:
                                {
									if( this->dragBoxCancelled )
									{
										this->dragBoxCancelled = false;
                                        
                                        // validity check is needed (drag box may be enabled while button is pressed!)
                                        if (this->buttonDown.isvalid())
                                        {
                                            this->buttonDown->SetHandled();
                                        }
									}
									this->buttonDown = 0;
									// call the memberfunction OnLeftButtonUp with the position 
									// of mouse where the button event occured. The return value 
									// sets the handled flag.
                                    handled = this->OnLeftButtonUp(event->GetMousePosition());
                                    break;
                                }
								// in the case of RightMouseButton
                                case Input::Event::RightMouseButton:
                                {
									// call the memberfunction OnRightButtonUp with the position 
									// of mouse where the button event occured. The return value 
									// sets the handled flag.
                                    handled = this->OnRightButtonUp(event->GetMousePosition());
                                    break;
                                }
                            }
                            break;
                        }

						// in the case of ButtonDown
                        case Input::Event::ButtonDown:
                        {
							// switch the botton
                            switch (event->GetButton())
                            {
								// in the case of LeftMouseButton
                                case Input::Event::LeftMouseButton:
                                {
									// store the buttonDown event
									this->buttonDown = event;
									// call the memberfunction OnLeftButtonUp with the position 
									// of mouse where the button event occured. The return value 
									// sets the handled flag.
                                    handled = this->OnLeftButtonDown(event->GetMousePosition());
                                    break;
                                }
								// in the case of RightMouseButton
                                case Input::Event::RightMouseButton:
                                {
									// call the memberfunction OnRightButtonUp with the position 
									// of mouse where the button event occured. The return value 
									// sets the handled flag.
                                    handled = this->OnRightButtonDown(event->GetMousePosition());
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                // check if the handled flag is set to true
                if (handled)
                {
					// if set true ...
					// set the event to handled
                    event->SetHandled();
                }
            }
        }
    }
    else
    {
		// call parent
		Message::Port::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
DragBox::AttachInput()
{
    // hook on input mappings
	// get a pointer to the input server
    Input::Server* inputServer = Input::Server::Instance();
	// check if the inputserver can map the mousePosition
    n_assert(inputServer->HasMapping("mousePosition"));
	// check if the inputserver can map the left mouse button
    n_assert(inputServer->HasMapping("mouseLMB"));
	// check if the inputserver can map the right mouse button
    n_assert(inputServer->HasMapping("mouseRMB"));

	// attach the port of the dragbox to the inputserver mapping of the left mouse button
    inputServer->AttachInputSink("mouseLMB", Input::InputPriority_MangaloreUI, this);
	// attach the port of the dragbox to the inputserver mapping of the right mouse button
    inputServer->AttachInputSink("mouseRMB", Input::InputPriority_MangaloreUI, this);
	// attach the port of the dragbox to the inputserver mapping of the mouse position
    inputServer->AttachInputSink("mousePosition", Input::InputPriority_MousePositionTracking, this);
}
    
//------------------------------------------------------------------------------
/**
*/
void
DragBox::RemoveInput()
{
    // remove from input mappings
	// get a pointer to the input server
    Input::Server* inputServer = Input::Server::Instance();
	// check if the inputserver can map the mousePosition
    n_assert(inputServer->HasMapping("mousePosition"));
	// check if the inputserver can map the left mouse button
    n_assert(inputServer->HasMapping("mouseLMB"));
	// check if the inputserver can map the right mouse button
    n_assert(inputServer->HasMapping("mouseRMB"));

	// remove the port of the dragbox from the inputserver mapping of the mouse position
    inputServer->RemoveInputSink("mousePosition", this);
	// remove the port of the dragbox from the inputserver mapping of the left mouse button
    inputServer->RemoveInputSink("mouseLMB", this);
	// remove the port of the dragbox from the inputserver mapping of the right mouse button
    inputServer->RemoveInputSink("mouseRMB", this);
}

//------------------------------------------------------------------------------
/**
*/
bool
DragBox::OnMouseMove(const vector2& mousePos)
{
	// check if the dragbox is enabled and dragging is on
    if (this->IsEnabled() && this->dragging)
    {
		// update the dragbox
        this->UpdateDragBox(mousePos);

		// check if the dragbox is valid (big enough)
        if (this->dragBoxValid)
        {
			// check if the message DragBoxStarted was sent
            if (!this->dragBoxStartedSent)
            {
				// if not ...
				// check if there is a dragboxdisplay
				if( this->display.isvalid() )
				{
					// check if the dragboxdisplay is not open
					if( !this->display->IsOpen() )
					{
						// if not ...
						this->display->Open();
					}
				}
				// check if there is a receiver
				if( this->receiver.isvalid() )
				{
					// create a DragBoxStarted message
					Ptr<Message::DragBoxStarted> dragBoxStarted = Message::DragBoxStarted::Create();
					// check if the message is handled
					if( this->receiver->Accepts( dragBoxStarted ) )
					{
						// send the message
						this->receiver->HandleMessage( dragBoxStarted );
					}
				}
                this->dragBoxStartedSent = true;
            }
			// check if there is a receiver
			if( this->receiver.isvalid() )
			{
				// create a DragBoxUpdated message
				Ptr<Message::DragBoxUpdated> dragBoxUpdated = Message::DragBoxUpdated::Create();
				// check if the message is handled
				if( this->receiver->Accepts( dragBoxUpdated ) )
				{
					// send the message
					this->receiver->HandleMessage( dragBoxUpdated );
				}
			}
        }
		else
		{
			// check if there is a dragboxdisplay
			if( this->display.isvalid() )
			{
				// check if the dragboxdisplay is open
				if( this->display->IsOpen() )
				{
					// if open ...
					this->display->Close();
				}
			}
		}
    }

	return false;    
}

//------------------------------------------------------------------------------
/**
*/
bool
DragBox::OnLeftButtonDown(const vector2& mousePos)
{
	// check if the dragbox is enabled
    if (this->IsEnabled())
    {
		// initialize some values for the dragbox start
        this->dragging = true;
        this->dragStartMousePos = mousePos;
        this->dragBoxValid = false;
        this->dragBoxStartedSent = false;
		this->dragBoxCancelled = false;

		//return true;
    }

    return false;    
}

//------------------------------------------------------------------------------
/**
*/
bool
DragBox::OnLeftButtonUp(const vector2& mousePos)
{
	// check if the dragbox is enabled and dragging is on
    if (this->IsEnabled() && this->dragging)
    {
		// update the dragbox
        this->UpdateDragBox(mousePos);
		// set dragging to off
        this->CancelDragging();

		// check if the dragbox is valid
        if (this->dragBoxValid)
        {
			// check if there is a receiver
			if( this->receiver.isvalid() )
			{
				// create a DragBoxReady message
				Ptr<Message::DragBoxReady> dragBoxReady = Message::DragBoxReady::Create();
				// check if the message is handled
				if( this->receiver->Accepts( dragBoxReady ) )
				{
					// send the message
					this->receiver->HandleMessage( dragBoxReady );
				}
			}

			// reset the dragBoxCancelled flag to false
			this->dragBoxCancelled = false;

			// return true to set the event as handled
			return true;
        }

    }

	// return false and do nothing !
	return false;  
}

//------------------------------------------------------------------------------
/**
*/
bool
DragBox::OnRightButtonDown(const vector2& mousePos)
{
	// check if the dragbox is enabled and dragging is on
    if (this->IsEnabled() && this->dragging)
    {
		// cancel dragging
        this->CancelDragging();

		// check if there is a receiver
		if( this->receiver.isvalid() )
		{
			// create a DragBoxCancelled message
			Ptr<Message::DragBoxCancelled> dragBoxCancelled = Message::DragBoxCancelled::Create();
			// check if the message is handled
			if( this->receiver->Accepts( dragBoxCancelled ) )
			{
				// send the message
				this->receiver->HandleMessage( dragBoxCancelled );
			}
		}

		return true;
    }

	return false;    
}

//------------------------------------------------------------------------------
/**
*/
bool
DragBox::OnRightButtonUp(const vector2& mousePos)
{
    return false;    
}


//------------------------------------------------------------------------------
/**
    Manually cancel the drag box.
*/
void
DragBox::CancelDragging()
{

	// check if the dragbox is enabled and dragging is on
    if (this->IsEnabled() && this->dragging)
    {
		// set dragging off
        this->dragging = false;
		// set the dragBoxCancelled flag to TRUE
		this->dragBoxCancelled = true;

		// check if there is a dragboxdisplay
		if( this->display.isvalid() )
		{
			// check if the dragboxdisplay is open
			if( this->display->IsOpen() )
			{
				// if open ...
				this->display->Close();
			}
		}
    }
}

//------------------------------------------------------------------------------
/**
    Update the drag box data. This will set the dragBoxValid member if the
    provided mouse position is not within the drag threshold from the start
    mouse position, and update the 2d- and 3d-drag box data.
*/
void
DragBox::UpdateDragBox(const vector2& mousePos)
{
    n_assert(this->dragging);

    // check mouse move distance
    vector2 mouseDist = mousePos - this->dragStartMousePos;
    if ((n_abs(mouseDist.x) > this->dragThreshold) &&
        (n_abs(mouseDist.y) > this->dragThreshold))
    {
        // we have a valid drag box
        this->dragBoxValid = true;

        // update the drag box rectangle
        this->dragBox2D.v0.x = n_saturate(n_min(mousePos.x, this->dragStartMousePos.x));
        this->dragBox2D.v0.y = n_saturate(n_min(mousePos.y, this->dragStartMousePos.y));
        this->dragBox2D.v1.x = n_saturate(n_max(mousePos.x, this->dragStartMousePos.x));
        this->dragBox2D.v1.y = n_saturate(n_max(mousePos.y, this->dragStartMousePos.y));
    }

	// check if there is a dragboxdisplay
	if( this->display.isvalid() )
	{
		// update the dragboxdisplay
		this->display->Update( this->dragBox2D );
	}
}

//------------------------------------------------------------------------------
/**
    Return true if a world space position is inside the current drag box.
*/
bool
DragBox::IsWorldCoordInDragBox(const vector3& pos) const
{
    if (this->dragBoxValid)
    {
        // transform vector to 2d screen space
        vector4 vec4(pos.x, pos.y, pos.z, 1.0f);
        vector4 proj4 = nGfxServer2::Instance()->GetTransform(nGfxServer2::ViewProjection) * vec4;
        vector2 screen2(((proj4.x / proj4.w) + 1.0f) * 0.5f, (((-proj4.y / proj4.w) + 1.0f) * 0.5f));
        return this->dragBox2D.inside(screen2);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    resets all smartpointers.
*/
void
DragBox::Deactivate()
{
	// reset the pointer for the receiver to zero
	this->receiver = 0;

	// check if there is a display
	if( this->display.isvalid() )
	{
		// check if the display is open
		if( this->display->IsOpen() )
		{
			// if open ...
			this->display->Close();
		}
		// reset the pointer for the display to zero
		this->display = 0;
	}
}


} // namespace UI