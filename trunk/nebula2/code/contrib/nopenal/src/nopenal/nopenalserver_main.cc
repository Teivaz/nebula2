//------------------------------------------------------------------------------
//  nopenalserver_main.cc
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenal/nopenalserver.h"
#include "kernel/nenv.h"
#include "resource/nresourceserver.h"
#include "audio3/nlistener3.h"

nNebulaScriptClass(nOpenALServer, "naudioserver3");

//-----------------------------------------------------------------------------
/**
    Constructor.
*/
nOpenALServer::nOpenALServer() :
    refResourceServer("/sys/servers/resource"),
    refHwnd("/sys/env/hwnd"),
    nextBuffer(0),
    nextSource(0),
    EAXlevel(0),
    lastStreamUpdateCheck(0.0)
{
    n_printf( "nOpenALServer Constructor\n" );

    ZeroMemory(&buffer[0], 256);
    ZeroMemory(&source[0], 256);
}

//-----------------------------------------------------------------------------
/**
    Destructor.  
*/
nOpenALServer::~nOpenALServer()
{
    n_printf( "nOpenALServer Destructor\n" );
}

//------------------------------------------------------------------------------
/**
    Open the audio server.
*/
bool
nOpenALServer::Open()
{
    n_printf( "nOpenALServer Open\n" );
    n_assert(!this->isOpen);

    // get public window handle, which has been initialized by the gfx subsystem
    HWND hwnd = (HWND) this->refHwnd->GetI();
    n_assert2( hwnd, "/sys/env/hwnd is NULL -- perhaps nWin32LogHandler::OpenWindow() has not been called." )

    alutInit (NULL, 0); // init OpenAL

    alDopplerFactor(1.0); // don't exaggerate doppler shift
    alDopplerVelocity(343); // using meters/second

    //
    // Set Listener attributes
    //

    // defaults
    ALfloat listenerPos[]={0.0,0.0,0.0};
    ALfloat listenerVel[]={0.0,0.0,0.0};
    ALfloat listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};  // Listener facing into the screen

    // error
    ALint   error;

    // Position ...
    alListenerfv(AL_POSITION,listenerPos);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        n_printf("alListenerfv POSITION : %s\n", alGetString(error));
        return false;
    }

    // Velocity ...
    alListenerfv(AL_VELOCITY,listenerVel);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        n_printf("alListenerfv VELOCITY : %s\n", alGetString(error));
        return false;
    }

    // Orientation ...
    alListenerfv(AL_ORIENTATION,listenerOri);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        n_printf("alListenerfv ORIENTATION : %s\n", alGetString(error));
        return false;
    }

    n_printf("Successfully initialized OpenAL\n");

    return nAudioServer3::Open();
}

//------------------------------------------------------------------------------
/**
    Close the audio server.
*/
void
nOpenALServer::Close()
{
    n_printf( "nOpenALServer Close\n" );

    n_assert(this->isOpen);

    alutExit();

    // release sound resources
    this->refResourceServer->UnloadResources(nResource::SoundResource | nResource::SoundInstance);

    n_printf("Successfully shut down OpenAL\n");

    nAudioServer3::Close();
}
