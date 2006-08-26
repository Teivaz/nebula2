//------------------------------------------------------------------------------
//  nappstate_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "application/nappstate.h"

nNebulaClass(nAppState, "nroot");

//------------------------------------------------------------------------------
/**
*/
nAppState::nAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAppState::~nAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnCreate(nApplication* application)
{
    n_assert(application);
    n_assert(!this->app.isvalid());
    this->app = application;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnStateEnter(const nString& prevState)
{
    n_printf("nAppState::OnStateEnter(%s) called on state %s\n", prevState.Get(), this->GetName());
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnStateLeave(const nString& nextState)
{
    n_printf("nAppState::OnStateLeave(%s) called on state %s\n", nextState.Get(), this->GetName());
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnFrame()
{
    n_printf("nAppState::OnFrame() called on state %s\n", this->GetName());
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnRender3D()
{
    n_printf("nAppState::OnRender3D() called on state %s\n", this->GetName());
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnRender2D()
{
    n_printf("nAppState::OnRender2D() called on state %s\n", this->GetName());
}

//------------------------------------------------------------------------------
/**
    Callback method which is called in the render loop before
    nSceneServer::BeginScene() is called. Overwrite this method
    in a subclass as your necessary.

    - 08-Jun-05    kims    Added
*/
void
nAppState::OnFrameBefore()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Callback method which is called in the render loop after
    nSceneServer::RenderScene() is called. Overwrite this method
    in a subclass as your necessary.

    - 08-Jun-05    kims    Added
*/
void
nAppState::OnFrameRendered()
{
    // empty
}

