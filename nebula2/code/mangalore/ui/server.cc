//------------------------------------------------------------------------------
//  ui/server.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/server.h"
#include "ui/canvas.h"
#include "ui/eventhandler.h"
#include "ui/event.h"
#include "ui/factorymanager.h"
#include "foundation/factory.h"
#include "input/ninputserver.h"
#include "gui/nguiserver.h"
#include "ui/window.h"

namespace UI
{
ImplementRtti(UI::Server, Foundation::RefCounted);
ImplementFactory(UI::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    time(0.0),
    frameTime(0.0),
    inTrigger(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);

    // create factory manager
    this->factoryManager = FactoryManager::Create();

    // setup a light entity for the user interface
    nLight light;
    light.SetType(nLight::Directional);
    light.SetDiffuse(vector4(1.0f, 1.0f, 1.0f, 1.0f));
    light.SetSpecular(vector4(1.0f, 1.0f, 1.0f, 1.0f));
    light.SetAmbient(vector4(0.2f, 0.2f, 0.2f, 1.0f));
    light.SetCastShadows(false);
	this->lightEntity = Graphics::LightEntity::Create();

    matrix44 m;
    m.rotate_x(-45.0f);
    m.rotate_y(45.0f);
    this->lightEntity->SetTransform(m);
    this->lightEntity->SetLight(light);
    this->lightEntity->OnActivate();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::Close()
{
    n_assert(this->isOpen);
    this->HideGui();
    this->windows.Clear();
    this->lightEntity->OnDeactivate();
    this->lightEntity = 0;
    this->factoryManager = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Attaches a new window to the server. This method is exclusively called
    by Window::Open()!
*/
void
Server::AttachWindow(Window* window)
{
    n_assert(window);
    n_assert(this->windows.Find(window) == 0);
    this->windows.Append(window);
}

//------------------------------------------------------------------------------
/**
    Removes a window from the server. This method is exclusively called
    by Window::Close()!
*/
void
Server::RemoveWindow(Window* window)
{
    n_assert(window);
    n_assert(this->windows.Find(window) != 0);
    nArray<Ptr<Window> >::iterator iter = this->windows.Find(window);
    n_assert(iter);
    this->windows.Erase(iter);
}

//------------------------------------------------------------------------------
/**
    *** OBSOLETE ***

    Displays a new gui defined by a Nebula2 resource and sets
    the (optional) associated event handler.
*/
void
Server::DisplayGui(const nString& resName, Message::Port* handler)
{
    this->curCanvas = (Canvas*) FactoryManager::Instance()->CreateElement("Canvas");
    n_assert(this->curCanvas->IsInstanceOf(Canvas::RTTI));
    this->curCanvas->SetResourceName(resName);
    this->curCanvas->OnCreate(0);
    this->curEventHandler = handler;
}

//------------------------------------------------------------------------------
/**
    *** OBSOLETE ***

    Hides the currently displayed 2D GUI.
*/
void
Server::HideGui()
{
    if (this->curCanvas.isvalid())
    {
        if (this->inTrigger)
        {
            this->curCanvas->SetDismissed(true);
        }
        else
        {
            this->curCanvas->OnDestroy();
            this->curCanvas = 0;
        }
        this->curEventHandler = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Server::Render()
{
    if (this->curCanvas.isvalid() || this->windows.Size() > 0)
    {
        this->lightEntity->Render();
    }

    // OBSOLETE
    if (this->curCanvas.isvalid() && this->curCanvas->IsValid())
    {
        this->curCanvas->OnRender();
    }
    int i;
    for (i = 0; i < this->windows.Size(); i++)
    {
        this->windows[i]->OnRender();
    }
}

//------------------------------------------------------------------------------
/**
    Trigger the ui server. This distributes input to the current canvas.
*/
void
Server::Trigger()
{
    // HACK
    if (nGuiServer::Instance()->IsMouseOverGui())
    {
        return;
    }

    Ptr<Window> activeWindow = this->GetActiveWindow();
    Ptr<Canvas> activeCanvas = this->GetToplevelCanvas();
    this->inTrigger = true;

    // process raw input from the Nebula2 input server
    nInputServer* inputServer = nInputServer::Instance();
    nInputEvent* ie;
    for (ie = inputServer->FirstEvent(); ie; ie = inputServer->NextEvent(ie))
    {
        // handle mouse input
        if (ie->GetDeviceId() == N_INPUT_MOUSE(0))
        {
            switch (ie->GetType())
            {
                case N_INPUT_MOUSE_MOVE:
                    {
                        vector2 mousePos(ie->GetRelXPos(), ie->GetRelYPos());
                        this->mousePos = mousePos;
                        if (activeCanvas.isvalid() && activeCanvas->IsValid())
                        {
                            activeCanvas->OnMouseMove(mousePos);
                        }
                        if (activeWindow.isvalid())
                        {
                            activeWindow->OnMouseMove(mousePos);
                        }
                    }
                    break;

                case N_INPUT_BUTTON_DOWN:
                    {
                        vector2 mousePos(ie->GetRelXPos(), ie->GetRelYPos());
                        if (ie->GetButton() == 0)
                        {
                            if (activeCanvas.isvalid() && activeCanvas->IsValid())
                            {
                                activeCanvas->OnLeftButtonDown(mousePos);
                            }
                            if (activeWindow.isvalid())
                            {
                                activeWindow->OnLeftButtonDown(mousePos);
                            }
                        }
                        else if (ie->GetButton() == 1)
                        {
                            if (activeCanvas.isvalid() && activeCanvas->IsValid())
                            {
                                activeCanvas->OnRightButtonDown(mousePos);
                            }
                            if (activeWindow.isvalid())
                            {
                                activeWindow->OnRightButtonDown(mousePos);
                            }
                        }
                    }
                    break;

                case N_INPUT_BUTTON_UP:
                    {
                        vector2 mousePos(ie->GetRelXPos(), ie->GetRelYPos());
                        if (ie->GetButton() == 0)
                        {
                            if (activeCanvas.isvalid() && activeCanvas->IsValid())
                            {
                                activeCanvas->OnLeftButtonUp(mousePos);
                            }
                            if (activeWindow.isvalid())
                            {
                                activeWindow->OnLeftButtonUp(mousePos);
                            }
                        }
                        else if (ie->GetButton() == 1)
                        {
                            if (activeCanvas.isvalid() && activeCanvas->IsValid())
                            {
                                activeCanvas->OnRightButtonUp(mousePos);
                            }
                            if (activeWindow.isvalid())
                            {
                                activeWindow->OnRightButtonUp(mousePos);
                            }
                        }
                    }
                    break;
            }
        }

        // handle keyboard input
        if (ie->GetDeviceId() == N_INPUT_KEYBOARD(0))
        {
            switch (ie->GetType())
            {
                case N_INPUT_KEY_CHAR:
                    if (activeCanvas.isvalid() && activeCanvas->IsValid())
                    {
                        activeCanvas->OnChar(ie->GetChar());
                    }
                    if (activeWindow.isvalid())
                    {
                        activeWindow->OnChar(ie->GetChar());
                    }
                    break;

                case N_INPUT_KEY_DOWN:
                    if (activeCanvas.isvalid() && activeCanvas->IsValid())
                    {
                        activeCanvas->OnKeyDown(ie->GetKey());
                    }
                    if (activeWindow.isvalid())
                    {
                        activeWindow->OnKeyDown(ie->GetKey());
                    }
                    break;

                case N_INPUT_KEY_UP:
                    if (activeCanvas.isvalid() && activeCanvas->IsValid())
                    {
                        activeCanvas->OnKeyUp(ie->GetKey());
                    }
                    if (activeWindow.isvalid())
                    {
                        activeWindow->OnKeyUp(ie->GetKey());
                    }
                    break;
            }
        }
    }

    // call the per-frame method
    if (activeCanvas.isvalid() && activeCanvas->IsValid())
    {
        activeCanvas->OnFrame();
    }
    nArray<Ptr<Window> > copyOfWindowArray = this->windows;
    int i;
    int num = copyOfWindowArray.Size();
    for (i = 0; i < num; i++)
    {
        copyOfWindowArray[i]->OnFrame();
    }
    if (this->curCanvas.isvalid() && this->curCanvas->IsDismissed())
    {
        this->curCanvas->OnDestroy();
        this->curCanvas = 0;
    }
    this->inTrigger = false;
}

//------------------------------------------------------------------------------
/**
    Creates a GUI event and sends it to the currently set event handler.
*/
void
Server::PutEvent(const nString& eventName)
{
    Ptr<Event> guiEvent = Event::Create();
    guiEvent->SetEventName(eventName);
    if (this->curEventHandler.isvalid())
    {
        guiEvent->SendSync(this->curEventHandler);
    }
    Ptr<Window> activeWindow = this->GetActiveWindow();
    if (activeWindow.isvalid())
    {
        activeWindow->HandleEvent(guiEvent);
    }
}

//------------------------------------------------------------------------------
/**
    Display a tooltip.
*/
void
Server::ShowToolTip(const nString& tooltip)
{
    nGuiServer::Instance()->ShowToolTip(tooltip.Get(), vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

//-----------------------------------------------------------------------------
/**
    Check if the mouse is currently over an GUI element.
*/
bool
Server::IsMouseOverGui() const
{
    if (this->curCanvas.isvalid() || (this->windows.Size() > 0))
    {
        // TODO: because the canvas always is fullscreen at the moment and
        // because there only one canvas is allowed, the inside check is useless
        // if a canvas is open the mouse will be over gui.
        return true;
    }
    // fallthrough: mouse not over any gui element
    return false;
}

} // namespace UI
