//------------------------------------------------------------------------------
//  nopenalserver_main.cc
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenal/nopenalserver.h"
#include "kernel/nenv.h"
#include "resource/nresourceserver.h"
#include "audio3/nlistener3.h"

#include <vorbis/codec.h>

nNebulaScriptClass(nOpenALServer, "naudioserver3");

nOpenALServer * nOpenALServer::m_pSelf = 0;

//-----------------------------------------------------------------------------
/**
    Constructor.
*/
nOpenALServer::nOpenALServer() :
    refResourceServer("/sys/servers/resource"),
    refHwnd("/sys/env/hwnd"),
    m_pDevice(0),
    nextBuffer(0),
    nextSource(0),
    //EAXlevel(0),
    lastStreamUpdateCheck(0.0)
{
    n_printf( "nOpenALServer Constructor\n" );

    ZeroMemory(&buffer[0], 256);
    ZeroMemory(&source[0], 256);

    m_pSelf = this;

    get_info();
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

    //alutInit (NULL, 0); // init OpenAL
    ALCcontext * pContext = 0;
	
    m_pDevice = alcOpenDevice(0); // this is supposed to select the "preferred device"
    n_assert(m_pDevice && "Default sound device not present");

	pContext = alcCreateContext(m_pDevice, 0);
    n_assert(pContext);
    check_alc_error();

	alcMakeContextCurrent(pContext);
	check_alc_error();
	
    //

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

    for( ; nextBuffer > 0; --nextBuffer )
    {
        alDeleteBuffers(1, &buffer[nextBuffer-1]);
    }
    for( ; nextSource > 0; --nextSource )
    {
        alDeleteSources(1, &source[nextSource-1]);
    }
    //alutExit();
    ALCcontext * pContext = 0;
	pContext = alcGetCurrentContext();
	alcMakeContextCurrent(0);
	alcDestroyContext(pContext);
	alcCloseDevice(m_pDevice);
    //

    // release sound resources
    this->refResourceServer->UnloadResources(nResource::SoundResource | nResource::SoundInstance);

    n_printf("Successfully shut down OpenAL\n");

    nAudioServer3::Close();
}


void
nOpenALServer::get_info()
{
	n_printf("ALC info: \n");
	n_printf("DEFAULT_DEVICE_SPECIFIER: %s\n", (char *)alcGetString(m_pDevice, ALC_DEFAULT_DEVICE_SPECIFIER));
	n_printf("DEVICE_SPECIFIER: %s\n",         (char *)alcGetString(m_pDevice, ALC_DEVICE_SPECIFIER));
	n_printf("EXTENSIONS: %s\n",               (char *)alcGetString(m_pDevice, ALC_EXTENSIONS));
	n_printf("AL info: \n");
	n_printf("VERSION: %s\n",                  (char *)alGetString(AL_VERSION));
	n_printf("RENDERER: %s\n",                 (char *)alGetString(AL_RENDERER));
	n_printf("VENDOR: %s\n",                   (char *)alGetString(AL_VENDOR));
	n_printf("EXTENSIONS: %s\n",               (char *)alGetString(AL_EXTENSIONS));
}

void
nOpenALServer::check_alc_error()
{
	ALenum ErrCode;
	nString Err = "ALC error: ";
	if ((ErrCode = alcGetError(m_pDevice)) != ALC_NO_ERROR)
	{
		Err += (char*)alcGetString(m_pDevice, ErrCode);
        n_error(Err.Get());
	}
}

void
nOpenALServer::check_al_error()
{
	ALenum ErrCode;
	nString Err = "OpenAL error: ";
	if ((ErrCode = alGetError()) != AL_NO_ERROR)
	{
		Err += (char*)alGetString(ErrCode);
        n_error(Err.Get());
	}
}

void
nOpenALServer::check_ov_error(const nString& msg, const int& ret)
{
    if (ret >= 0)
    {
        return;
    }

	nString err  = "OggVorbis error: ";
    err         += msg;
    err         += " ";
    switch(ret)
    {
    case OV_FALSE:
        err += "OV_FALSE";
        break;
    case OV_HOLE:
        err += "OV_HOLE";
        break;
    case OV_EREAD:
        err += "OV_EREAD";
        break;
    case OV_EFAULT:
        err += "OV_EFAULT";
        break;
    case OV_EIMPL:
        err += "OV_EIMPL";
        break;
    case OV_EINVAL:
        err += "OV_EINVAL";
        break;
    case OV_ENOTVORBIS:
        err += "OV_ENOTVORBIS";
        break;
    case OV_EBADHEADER:
        err += "OV_HOLE";
        break;
    case OV_EVERSION:
        err += "OV_EVERSION";
        break;
    case OV_EBADLINK:
        err += "OV_EBADLINK";
        break;
    case OV_ENOSEEK:
        err += "OV_ENOSEEK";
        break;
    default:
        err += "...";
        break;
    }
	n_error(err.Get());
}







