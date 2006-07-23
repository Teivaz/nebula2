//----------------------------------------------------------------------------
//  appname_main.cc
//
//  (C)2005 Kim, Hyoun Woo
//----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "nopenaldemo/nappopenalstate.h"
#include "application/nappcamera.h"

#include "nopenal/nopenalserver.h"

nNebulaScriptClass(nAppOpenALState, "application::nappstate")

//----------------------------------------------------------------------------
/**
*/
nAppOpenALState::nAppOpenALState() :
    audioHelper(0)
{

}

//----------------------------------------------------------------------------
/**
*/
nAppOpenALState::~nAppOpenALState()
{
    if (this->audioHelper)
        n_delete(this->audioHelper);
}

//----------------------------------------------------------------------------
/**
*/
void nAppOpenALState::OnStateEnter(const nString &prevState)
{
    float pos[3];

    // create and initialize audio helper.
    this->audioHelper = n_new(nOpenALHelper);

    // load and play background music with the given ogg file.
    this->oggObject = this->audioHelper->PlayFile("bgmusic", "oal:music/test.ogg");
    this->oggObject->SetVolume(1.f);
    this->oggObject->SetLooping(true);
    this->oggObject->Update();

    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
    this->audioHelper->SetSourcePosition(this->oggObject, pos);

    this->wavObject = this->audioHelper->PlayFile("pingping", "oal:sound/test.wav");

    this->wavObject->SetVolume(1.0f);
    this->wavObject->SetLooping(true);
    this->wavObject->Update();

    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
    this->audioHelper->SetSourcePosition(this->wavObject, pos);

}

//----------------------------------------------------------------------------
/**
*/
void nAppOpenALState::OnStateLeave(const nString& nextState)
{
    nOpenALServer* audioServer = nOpenALServer::Instance();

    audioServer->StopSound(this->oggObject.get());
}

//----------------------------------------------------------------------------
/**
*/
void nAppOpenALState::OnRender3D()
{
    //TODO: put application rendering code.

}

//----------------------------------------------------------------------------
/**
*/
void nAppOpenALState::OnFrameBefore()
{

}

//----------------------------------------------------------------------------
/**
*/
void nAppOpenALState::OnFrameRendered()
{
    //nOpenALServer* audioServer = nOpenALServer::Instance();
    //n_assert(audioServer != 0);

    //matrix44 m;
    //m.set(viewMatrix);
    //this->listener.SetTransform(m);
    //audioServer->UpdateSound(this->oggObject.get());
    //audioServer->UpdateSound(p_wav);

}

//----------------------------------------------------------------------------
/**
     Renders objects in screen coordinate.
*/
void nAppOpenALState::OnRender2D()
{
}

//----------------------------------------------------------------------------
/**
    Calls simulation stuff.
*/
void nAppOpenALState::OnFrame()
{
    nAppCamera* camera = this->app->GetAppCamera();
    transform44& tm = camera->Transform();
    tm.setmatrix(this->viewMatrix);

}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
