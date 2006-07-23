//----------------------------------------------------------------------------
//   nappopenaldemo_main.cc
//
//  (C)2005 Kim, Hyoun Woo
//----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "nopenaldemo/nappopenaldemo.h"
#include "nopenaldemo/nappopenalstate.h"

nNebulaScriptClass(nAppOpenALDemo, "application::napplication")

//----------------------------------------------------------------------------
/**
*/
nAppOpenALDemo::nAppOpenALDemo()
{
}

//----------------------------------------------------------------------------
/**
*/
nAppOpenALDemo::~nAppOpenALDemo()
{
}

//----------------------------------------------------------------------------
/**
*/
bool nAppOpenALDemo::Open()
{
    if (!nApplication::Open())
    {
        return false;
    }

    // specify the directory which contains sample sound files.
    nFileServer2::Instance()->SetAssign("oal", "home:code/contrib/nopenal/export/");

    // create edit state and specifies it to application.
    this->nappopenalstate = (nAppOpenALState*)this->CreateState("nappopenalstate", "nappopenalstate");
    this->SetState("nappopenalstate");

    return true;
}

//----------------------------------------------------------------------------
/**
*/
void nAppOpenALDemo::Close()
{
    //TODO: Add code for closing the application.

    nApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
nScriptServer*
nAppOpenALDemo::CreateScriptServer()
{
    return (nScriptServer*) kernelServer->New("ntclserver", "/sys/servers/script");
}

//------------------------------------------------------------------------------
/**
    Create nOpenAL audio server.
*/
nAudioServer3*
nAppOpenALDemo::CreateAudioServer()
{
    return (nAudioServer3*) kernelServer->New("nopenalserver", "/sys/servers/audio");
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
