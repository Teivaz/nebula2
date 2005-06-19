#include "napplication_tutorial/ntutorialappstate.h"

nNebulaClass(nTutorialAppState, "nappstate");

//----------------------------------------------------------------------------
nTutorialAppState::nTutorialAppState()
{
    this->world = 0;
    this->testObject = 0;
}

//----------------------------------------------------------------------------
nTutorialAppState::~nTutorialAppState()
{
    if (this->testObject)
    {
        n_delete(this->testObject);
        this->testObject = 0;
    }
        
    if (this->world)
    {
        n_delete(this->world);
        this->world = 0;
    }
}

//----------------------------------------------------------------------------
void nTutorialAppState::OnStateEnter(const nString& prevState)
{
    this->world = n_new(nGfxObject);
    this->world->SetResourceName("gfxlib:stdlight.n2");
    this->world->Load();
    this->testObject = n_new(nGfxObject);
    this->testObject->SetResourceName("gfxlib:examples/torus.n2");
    this->testObject->Load();  
}

//----------------------------------------------------------------------------
void nTutorialAppState::OnRender3D()
{
    this->world->Render();
    this->testObject->Render();
}

//----------------------------------------------------------------------------
void nTutorialAppState::OnRender2D()
{
    // empty
}

//----------------------------------------------------------------------------
void nTutorialAppState::OnFrame()
{
    // empty
}
