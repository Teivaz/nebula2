#include "napplication_tutorial/ntutorialapp.h"
#include "napplication_tutorial/ntutorialappstate.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nTutorialApp, "napplication")

//----------------------------------------------------------------------------
nTutorialApp::nTutorialApp()
{
}

//----------------------------------------------------------------------------
nTutorialApp::~nTutorialApp()
{
}

//----------------------------------------------------------------------------
bool nTutorialApp::Open()
{
    nApplication::Open();
    this->gameState = (nTutorialAppState*)this->CreateState("ntutorialappstate", "gamestate");
    this->SetState("gamestate");
    return true;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
