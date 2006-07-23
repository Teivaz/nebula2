//------------------------------------------------------------------------------
//  nexitappstate_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "application/nexitappstate.h"
#include "application/napplication.h"

nNebulaClass(nExitAppState, "application::nappstate");

//------------------------------------------------------------------------------
/**
*/
nExitAppState::nExitAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nExitAppState::~nExitAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nExitAppState::OnStateEnter(const nString& prevState)
{
    this->app->SetQuitRequested(true);
}
