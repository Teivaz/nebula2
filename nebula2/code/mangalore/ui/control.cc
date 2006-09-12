//------------------------------------------------------------------------------
//  ui/control.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/control.h"
#include "ui/server.h"
#include "input/server.h"
#include "input/event.h"
#include "input/mapping.h"
#include "input/priority.h"

namespace UI
{
ImplementRtti(UI::Control, UI::Element);
ImplementFactory(UI::Control);

//------------------------------------------------------------------------------
/**
*/
Control::Control() :
    enabled(false),
    tooltipDelay(0.1),
    mouseWithin(false),
    mouseWithinTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Control::~Control()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Parse GUI attributes on create.
*/
void
Control::OnCreate(Element* parent)
{
    Element::OnCreate(parent);
    this->SetEnabled(this->gfxNode->GetBoolAttr("rlGuiEnabled"));
    if (this->gfxNode->HasAttr("rlGuiTooltip"))
    {
        this->SetTooltip(this->gfxNode->GetStringAttr("rlGuiTooltip"));
    }

    // send an initial mouse move (so that highlights etc... will be switched
    // on if the mouse is already over the element)
    this->OnMouseMove(Server::Instance()->GetMousePosition());
}

//------------------------------------------------------------------------------
/**
*/
void
Control::OnDestroy()
{
    this->SetEnabled(false);

    // call parent
    Element::OnDestroy();
}

//------------------------------------------------------------------------------
/**
*/
void
Control::SetEnabled(bool b)
{
    if (this->enabled != b)
    {
        this->enabled = b;

        if (this->enabled)
        {
            this->AttachInput();
        }
        else
        {
            this->RemoveInput();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Control::AttachInput()
{
    // hook on input mappings
    Input::Server* inputServer = Input::Server::Instance();
    n_assert(inputServer->HasMapping("mousePosition"));
    n_assert(inputServer->HasMapping("mouseLMB"));
    n_assert(inputServer->HasMapping("mouseRMB"));

    inputServer->AttachInputSink("mouseLMB", Input::InputPriority_MangaloreUI, this);
    inputServer->AttachInputSink("mouseRMB", Input::InputPriority_MangaloreUI, this);
    inputServer->AttachInputSink("mousePosition", Input::InputPriority_MousePositionTracking, this);
}

//------------------------------------------------------------------------------
/**
*/
void
Control::RemoveInput()
{
    // remove from input mappings
    Input::Server* inputServer = Input::Server::Instance();
    n_assert(inputServer->HasMapping("mousePosition"));
    n_assert(inputServer->HasMapping("mouseLMB"));
    n_assert(inputServer->HasMapping("mouseRMB"));

    inputServer->RemoveInputSink("mousePosition", this);
    inputServer->RemoveInputSink("mouseLMB", this);
    inputServer->RemoveInputSink("mouseRMB", this);
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (this->IsEnabled() && this->IsVisible() && msg->CheckId(Input::Event::Id)) return true;
    return Message::Port::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
Control::HandleMessage(Message::Msg* msg)
{
    n_assert(msg);

    if (msg->CheckId(Input::Event::Id))
    {
        Input::Event* event = static_cast<Input::Event*>(msg);

        if (event->GetType() == Input::Event::MouseMoved)
        {
            this->OnMouseMove(event->GetRelMousePosition());
        }
        else
        {
            if (!event->Handled())
            {
                // handle mouse button events
                bool handled = false;
                if (event->HasMousePosition())
                {
                    switch (event->GetType())
                    {
                        case Input::Event::ButtonUp:
                        {
                            switch (event->GetButton())
                            {
                                case Input::Event::LeftMouseButton:
                                {
                                    handled = this->OnLeftButtonUp(event->GetMousePosition());
                                    break;
                                }
                                case Input::Event::RightMouseButton:
                                {
                                    handled = this->OnRightButtonUp(event->GetMousePosition());
                                    break;
                                }
                            }
                            break;
                        }

                        case Input::Event::ButtonDown:
                        {
                            switch (event->GetButton())
                            {
                                case Input::Event::LeftMouseButton:
                                {
                                    handled = this->OnLeftButtonDown(event->GetMousePosition());
                                    break;
                                }
                                case Input::Event::RightMouseButton:
                                {
                                    handled = this->OnRightButtonDown(event->GetMousePosition());
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                else
                {
                    // TODO: handle key events
                }

                if (handled)
                {
                    event->SetHandled();
                }
            }
        }
    }
    else
    {
        Message::Port::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Display tooltip if necessary.
*/
void
Control::OnFrame()
{
    Element::OnFrame();

    // activate tooltip after mouse was over element for a while
    if (this->IsVisible())
    {
        Server* uiServer = Server::Instance();
        if (this->IsEnabled() && this->GetTooltip().IsValid() && this->Inside(uiServer->GetMousePosition()))
        {
            nTime time = uiServer->GetTime();
            if (!this->mouseWithin)
            {
                this->mouseWithinTime = time;
                this->mouseWithin = true;
            }
            else if(time > this->mouseWithinTime + this->GetTooltipDelay())
            {
                uiServer->ShowToolTip(this->GetTooltip());
            }
        }
        else
        {
            this->mouseWithin = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::OnMouseMove(const vector2& mousePos)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::OnLeftButtonDown(const vector2& mousePos)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::OnLeftButtonUp(const vector2& mousePos)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::OnRightButtonDown(const vector2& mousePos)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::OnRightButtonUp(const vector2& mousePos)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::OnChar(uchar charCode)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::OnKeyDown(nKey key)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Control::OnKeyUp(nKey key)
{
    return false;
}

} // namespace UI
