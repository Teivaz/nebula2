//----------------------------------------------------------------------------
//  %(appNameL)s_main.cc
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "%(subDirL)s/%(appStateNameL)s.h"
#include "application/nappcamera.h"

nNebulaScriptClass(%(appStateName)s, "nappstate")

//----------------------------------------------------------------------------
/**
*/
%(appStateName)s::%(appStateName)s() 
{
    
}

//----------------------------------------------------------------------------
/**
*/
%(appStateName)s::~%(appStateName)s()
{
}

//----------------------------------------------------------------------------
/**
*/
void %(appStateName)s::OnStateEnter(const nString &prevState)
{
    //TODO: put application initialization code.

}

//----------------------------------------------------------------------------
/**
*/
void %(appStateName)s::OnRender3D()
{
    //TODO: put application rendering code.

}

//----------------------------------------------------------------------------
/**
*/
void %(appStateName)s::OnFrameBefore()
{

}

//----------------------------------------------------------------------------
/**
*/
void %(appStateName)s::OnFrameRendered()
{

}

//----------------------------------------------------------------------------
/**
     Renders objects in screen coordinate.
*/
void %(appStateName)s::OnRender2D()
{
}

//----------------------------------------------------------------------------
/**
    Calls simulation stuff.
*/
void %(appStateName)s::OnFrame()
{
    //TODO: put application input handling code.

    nAppCamera* camera = this->app->GetAppCamera();
    transform44& tm = camera->Transform();
    tm.setmatrix(this->viewMatrix);

}
//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
