//------------------------------------------------------------------------------
//  input/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/server.h"
#include "input/ninputserver.h"
#include "message/server.h"
#include "kernel/nscriptserver.h"
#include "gui/nguiserver.h"
#include "ui/server.h"
#include "ceui/server.h"

// HACK
#include "application/app.h"

namespace Input
{
ImplementRtti(Input::Server, Foundation::RefCounted);
ImplementFactory(Input::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    time(0.0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the input subsystem.
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);

    nInputServer::Instance()->Open();

    // define input mapping by calling external script function
    nScriptServer* scriptServer = Foundation::Server::Instance()->GetScriptServer();
    nString result;
    scriptServer->Run("OnMapInput", result);

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
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Set the current time stamp.
*/
void
Server::SetTime(double t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
    The per frame trigger method of the input server.
*/
void
Server::Trigger()
{
    n_assert(this->isOpen);
    nInputServer* inputServer = nInputServer::Instance();
    nGuiServer* guiServer = nGuiServer::Instance();

    // trigger the input server
    inputServer->Trigger(this->time);

    // FIXME: trigger Nebula GUI server
    // THIS IS DIRTY, BUT CURRENTLY NECESSARY, BECAUSE THE InputServer FLUSHES
    // THE NEBULA2 INPUT EVENT AT THE END OF THE FRAME
    guiServer->Trigger();
    UI::Server::Instance()->Trigger();
#ifdef MANGALORE_USE_CEGUI
    CEUI::Server::Instance()->Trigger();
#endif

    // flush Nebula input events
    inputServer->FlushEvents();
}

//------------------------------------------------------------------------------
/**
*/
float
Server::GetSlider(const char* name) const
{
    n_assert(name);
    n_assert(this->isOpen);
    return nInputServer::Instance()->GetSlider(name);
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::GetButton(const char* name) const
{
    n_assert(name);
    n_assert(this->isOpen);
    return nInputServer::Instance()->GetButton(name);
}

//------------------------------------------------------------------------------
/**
*/
const vector2&
Server::GetMousePos() const
{
    return nInputServer::Instance()->GetMousePos();
}

} // namespace Graphics
