//------------------------------------------------------------------------------
//  ui/server.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/server.h"
#include "ui/eventhandler.h"
#include "ui/event.h"
#include "ui/factorymanager.h"
#include "foundation/factory.h"
#include "input/ninputserver.h"
#include "input/event.h"
#include "input/server.h"
#include "input/mapping.h"
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

    // set the projection matrix for gui elements, this must be
    // the same as used in the gui vertex shader!
    this->guiProjMatrix.set(0.5f, 0.0f,    0.0f, 0.0f,
                            0.0f, 0.6667f, 0.0f, 0.0f,
                            0.0f, 0.0f,   -0.5f, 0.0f,
                            0.0f, 0.0f,    0.5f, 1.0f);
    this->invGuiProjMatrix = this->guiProjMatrix;
    this->invGuiProjMatrix.invert();
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

    // hook on input mappings
    Input::Server* inputServer = Input::Server::Instance();
    n_assert(inputServer->HasMapping("mousePosition"));
    inputServer->AttachInputSink("mousePosition", 100, this);

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
    for (int i = 0; i < this->windows.Size(); i++)
    {
        if (this->windows[i].isvalid() && this->windows[i]->IsOpen())
        {
            this->windows[i]->Close();
        }
    }
    this->lightEntity->OnDeactivate();
    this->lightEntity = 0;
    this->factoryManager = 0;

    // remove from input mappings
    Input::Server* inputServer = Input::Server::Instance();
    n_assert(inputServer->HasMapping("mousePosition"));
    inputServer->RemoveInputSink("mousePosition", this);

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

    // find a free slot in the window array
    int freeIndex = this->windows.FindIndex(0);
    if (-1 != freeIndex)
    {
        this->windows[freeIndex] = window;
    }
    else
    {
        this->windows.Append(window);
    }
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
    int index = this->windows.FindIndex(window);
    if (index != -1)
    {
        this->windows[index] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Server::Render()
{
    if (this->windows.Size() > 0)
    {
        this->lightEntity->Render();
    }
    for (int i = 0; i < this->windows.Size(); i++)
    {
        if (this->windows[i].isvalid())
        {
            this->windows[i]->OnRender();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (this->isOpen && msg->CheckId(Input::Event::Id)) return true;
    return Message::Port::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
Server::HandleMessage(Message::Msg* msg)
{
    n_assert(msg);

    if (this->isOpen && msg->CheckId(Input::Event::Id))
    {
        Input::Event* event = static_cast<Input::Event*>(msg);
        if (event->GetType() == Input::Event::MouseMoved)
        {
            this->mousePos = event->GetRelMousePosition();
        }
    }
    else
    {
        Message::Port::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Trigger the ui server. This distributes input to the current window.
*/
void
Server::Trigger()
{
    // HACK, don't collide with the old style Nebula2 gui
    if (nGuiServer::Instance()->IsMouseOverGui())
    {
        return;
    }

    Ptr<Window> activeWindow = this->GetActiveWindow();
    this->inTrigger = true;

    // call the per-frame method
    int num = this->windows.Size();
    for (int i = 0; i < num; i++)
    {
        if (this->windows[i].isvalid())
        {
            // note: window may have been closed when this method returns!
            this->windows[i]->OnFrame();
        }
    }
    this->inTrigger = false;
}

//------------------------------------------------------------------------------
/**
    Creates a GUI event and sends it to the currently set event handler.
*/
void
Server::PutEvent(Event* guiEvent)
{
    n_assert(0 != guiEvent);
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
    if (this->GetActiveWindow() != 0)
    {
        // TODO: because the canvas always is fullscreen at the moment and
        // because there only one canvas is allowed, the inside check is useless
        // if a canvas is open the mouse will be over gui.
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    This takes a 2d screen space position and converts it into a in view space.
    This is a slightly modified version of nGfxServer2::ComputeWorldMouseRay().
*/
vector3
Server::ScreenSpacePosToViewSpace(const vector2& screenSpacePos)
{
    // Compute mouse position in world coordinates.
    vector3 screenCoord3D((screenSpacePos.x - 0.5f) * 2.0f, (screenSpacePos.y - 0.5f) * 2.0f, 1.0f);
    vector3 viewCoord = this->GetInvGuiProjectionMatrix() * screenCoord3D;
    viewCoord.y = -viewCoord.y;
    viewCoord.z = 0.0f;
    return viewCoord;
}

} // namespace UI
