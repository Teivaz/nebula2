//------------------------------------------------------------------------------
//  ndsoundserver3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/ndsoundserver3.h"
#include "kernel/nenv.h"
#include "kernel/ntimeserver.h"
#include "resource/nresourceserver.h"
#include "audio3/ndsound3.h"
#include "audio3/nlistener3.h"
#include "audio3/ndsound_wavfmt.h"

nNebulaClass(nDSoundServer3, "naudioserver3");

//------------------------------------------------------------------------------
/**
*/
nDSoundServer3::nDSoundServer3() :
    refResourceServer("/sys/servers/resource"),
    refHwnd("/sys/env/hwnd"),
    dsListener(0),
    m_pDS(0),
    lastStreamUpdateCheck(0.0)
{
}

//------------------------------------------------------------------------------
/**
*/
nDSoundServer3::~nDSoundServer3()
{
    n_assert(0 == this->dsListener);
}

//------------------------------------------------------------------------------
/**
    Open the audio server.
*/
bool
nDSoundServer3::Open()
{
    n_assert(!this->isOpen);
    n_assert(0 == this->dsListener);
    HRESULT hr;

    // get public window handle, which has been initialized by the gfx subsystem
    HWND hwnd = (HWND) this->refHwnd->GetI();

    // initialize the sound manager
    if( m_pDS ) m_pDS->Release();

    // Create IDirectSound using the primary sound device
    hr = DirectSoundCreate8( NULL, &m_pDS, NULL );
    n_assert(SUCCEEDED(hr));

    // Set DirectSound coop level 
    //#define DSSCL_NORMAL                0x00000001
    //#define DSSCL_PRIORITY              0x00000002
    //#define DSSCL_EXCLUSIVE             0x00000003
    //#define DSSCL_WRITEPRIMARY          0x00000004
    hr = m_pDS->SetCooperativeLevel( hwnd, DSSCL_PRIORITY );
    n_assert(SUCCEEDED(hr));

    hr = this->SetPrimaryBufferFormat(2, 22050, 16);
    n_assert(SUCCEEDED(hr));

    // obtain 3D listener interface
    hr = this->Get3DListenerInterface(&(this->dsListener));
    n_assert(SUCCEEDED(hr));

    memset(&(this->dsListenerProps), 0, sizeof(this->dsListenerProps));
    this->dsListenerProps.dwSize = sizeof(this->dsListenerProps);
    this->dsListener->GetAllParameters(&this->dsListenerProps);

    return nAudioServer3::Open();
}

//------------------------------------------------------------------------------
/**
    Close the audio server.
*/
void
nDSoundServer3::Close()
{
    n_assert(this->isOpen);
    n_assert(this->dsListener);

    // release sound resources
    this->refResourceServer->UnloadResources(nResource::SoundResource | nResource::SoundInstance);

    this->dsListener->Release();
    this->dsListener = 0;
        
    if( m_pDS ) m_pDS->Release();
    nAudioServer3::Close();
}

//------------------------------------------------------------------------------
/**
    Begin the audio scene.
*/
bool
nDSoundServer3::BeginScene()
{
    return nAudioServer3::BeginScene();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::UpdateListener(const nListener3& l)
{
    n_assert(this->dsListener);

    const matrix44& m = l.GetTransform();
    const vector3& v  = l.GetVelocity();
    this->dsListenerProps.vPosition.x       = m.M41;
    this->dsListenerProps.vPosition.y       = m.M42;
    this->dsListenerProps.vPosition.z       = m.M43;
    this->dsListenerProps.vVelocity.x       = v.x;
    this->dsListenerProps.vVelocity.y       = v.y;
    this->dsListenerProps.vVelocity.z       = v.z;
    this->dsListenerProps.vOrientFront.x    = m.M31;
    this->dsListenerProps.vOrientFront.y    = m.M32;
    this->dsListenerProps.vOrientFront.z    = m.M33;
    this->dsListenerProps.vOrientTop.x      = m.M21;
    this->dsListenerProps.vOrientTop.y      = m.M22;
    this->dsListenerProps.vOrientTop.z      = m.M23;
    this->dsListenerProps.flDistanceFactor  = 1.0f;
    this->dsListenerProps.flRolloffFactor   = l.GetRollOffFactor();
    this->dsListenerProps.flDopplerFactor   = l.GetDopplerFactor();

    HRESULT hr = this->dsListener->SetAllParameters(&(this->dsListenerProps), DS3D_IMMEDIATE);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::StartSound(nSound3* sound)
{
    n_assert(sound);
    sound->Start();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::UpdateSound(nSound3* sound)
{
    n_assert(sound);
    sound->Update();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::StopSound(nSound3* sound)
{
    n_assert(sound);
    sound->Stop();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer3::EndScene()
{
    // handle stream updates (only every 1/20 seconds)
    nTime curTime = kernelServer->GetTimeServer()->GetTime();
    nTime diff = curTime - this->lastStreamUpdateCheck;
    if (diff > 0.05)
    {
        this->lastStreamUpdateCheck = curTime;
    
        nRoot* rsrcPool = this->refResourceServer->GetResourcePool(nResource::SoundResource);
        nRoot* cur;
        for (cur = rsrcPool->GetHead(); cur; cur = cur->GetSucc())
        {
            nDSoundResource* sound = (nDSoundResource*) cur;
            if (sound->GetStreaming())
            {
                nDSound3 *snd = sound->getSound3();
                if (snd->IsSoundPlaying())
                {
                    if (snd->CheckStreamUpdate())
                    {
                        snd->HandleWaveStreamNotification(sound->GetLooping());
                    }
                }
            }
        }
    }
    nAudioServer3::EndScene();
}

//------------------------------------------------------------------------------
/**
*/
nSoundResource*
nDSoundServer3::NewSoundResource(const char* rsrcName)
{
    return (nSoundResource*) this->refResourceServer->NewResource("ndsoundresource", rsrcName, nResource::SoundResource);
}

//------------------------------------------------------------------------------
/**
*/
nSound3*
nDSoundServer3::NewSound()
{
    return (nSound3 *) this->refResourceServer->NewResource("ndsound3", 0, nResource::SoundInstance);
}

//------------------------------------------------------------------------------
// SOUND MANAGER PORTS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*/
bool nDSoundServer3::Create( nDSound3** ppSound3, LPTSTR strWaveFileName, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNumBuffers )
{
    HRESULT hr;
    DWORD   i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    nDSound_WavFmt*      pWaveFile      = NULL;

    n_assert(0 != m_pDS);
    n_assert(0 != strWaveFileName);
    n_assert(0 != ppSound3);
    n_assert(dwNumBuffers > 1);

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
        goto LFail;
    }

    pWaveFile = new nDSound_WavFmt();
    if( pWaveFile == NULL )
    {
        goto LFail;
    }

    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );

    if( pWaveFile->GetSize() == 0 )
    {
        // Wave is blank, so don't create it.
        goto LFail;
    }

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = pWaveFile->GetSize();

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;
    
    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
    hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL );

    // Be sure to return this error code if it occurs so the
    // callers knows this happened.
    //if( hr == DS_NO_VIRTUALIZATION ) 
    //    hrRet = DS_NO_VIRTUALIZATION;
            
    if( FAILED(hr) )
    {
        // DSERR_BUFFERTOOSMALL will be returned if the buffer is
        // less than DSBSIZE_FX_MIN and the buffer is created
        // with DSBCAPS_CTRLFX.
        
        // It might also fail if hardware buffer mixing was requested
        // on a device that doesn't support it.
        n_printf("CreateSoundBuffer: Wave file must be mono for 3D control. or using ambient sound setting by 2D sound.\n");
                    
        goto LFail;
    }

    // Default to use DuplicateSoundBuffer() when created extra buffers since always 
    // create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
    // DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
    if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
    {
        for( i=1; i<dwNumBuffers; i++ )
        {
            if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
            {
                n_printf("DuplicateSoundBuffer");
                goto LFail;
            }
        }
    }
    else
    {
        for( i=1; i<dwNumBuffers; i++ )
        {
            hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
            if( FAILED(hr) )
            {
                n_printf("CreateSoundBuffer");
                goto LFail;
            }
        }
   }
    
    // Create the sound
    // *ppSound3 = new nDSound3();
    (*ppSound3)->createDSBuffer( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags );

    if( apDSBuffer ) delete[] (apDSBuffer);
    return true;

LFail:
    // Cleanup
    if( pWaveFile ) delete pWaveFile;
    if( apDSBuffer ) delete[] (apDSBuffer);
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool nDSoundServer3::CreateFromMemory( nDSound3** ppSound, BYTE* pbData, ULONG ulDataSize, LPWAVEFORMATEX pwfx, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNumBuffers )
{
    HRESULT hr;
    DWORD   i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    nDSound_WavFmt*     pWaveFile      = NULL;

    n_assert(0 != m_pDS);
    n_assert(0 != pbData);
    n_assert(0 != ppSound);
    n_assert(dwNumBuffers > 1);

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile = new nDSound_WavFmt();
    if( pWaveFile == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile->OpenFromMemory( pbData,ulDataSize, pwfx, WAVEFILE_READ );

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = ulDataSize;

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pwfx;

    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL ) ) )
    {
        n_printf("CreateSoundBuffer");
        goto LFail;
    }

    // Default to use DuplicateSoundBuffer() when created extra buffers since always 
    // create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
    // DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
    if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
    {
        for( i=1; i<dwNumBuffers; i++ )
        {
            if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
            {
                n_printf("DuplicateSoundBuffer");
                goto LFail;
            }
        }
    }
    else
    {
        for( i=1; i<dwNumBuffers; i++ )
        {
            hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
            if( FAILED(hr) )
            {
                n_printf("CreateSoundBuffer");
                goto LFail;
            }
        }
   }

    // Create the sound
    // *ppSound = new nDSound3();
    (*ppSound)->createDSBuffer( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags );

    if( apDSBuffer ) delete [] apDSBuffer;
    return true;

LFail:
    // Cleanup
    if( apDSBuffer ) delete [] apDSBuffer;
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool nDSoundServer3::CreateStreaming( nDSound3** ppStreamingSound, LPTSTR strWaveFileName, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNotifyCount, DWORD dwNotifySize )
{
    HRESULT hr;

    n_assert(0 != m_pDS);
    n_assert(0 != strWaveFileName);
    n_assert(0 != ppStreamingSound);

    LPDIRECTSOUNDBUFFER pDSBuffer      = NULL;
    DWORD               dwDSBufferSize = NULL;
    nDSound_WavFmt*          pWaveFile      = NULL;

    pWaveFile = new nDSound_WavFmt();
    if( pWaveFile == NULL )
        return false;
    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );

    // Figure out how big the DSound buffer should be 
    dwDSBufferSize = dwNotifySize * dwNotifyCount;

    // Set up the direct sound buffer.
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags | 
                           DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;

    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBuffer, NULL ) ) )
    {
        // If wave format isn't then it will return 
        // either DSERR_BADFORMAT or E_INVALIDARG
        if( hr == DSERR_BADFORMAT || hr == E_INVALIDARG )
            return false;

        return false;
    }

    // Create the sound
    // *ppStreamingSound = new CStreamingSound( pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize );
    (*ppStreamingSound)->createDSBufferStreaming( pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize );

    return true;
}

//------------------------------------------------------------------------------
/**
    Set primary buffer to a specified format 
        !WARNING! - Setting the primary buffer format and then using this
                    same dsound object for DirectMusic messes up DirectMusic! 
        For example, to set the primary buffer format to 22kHz stereo, 16-bit
        then:   dwPrimaryChannels = 2
                dwPrimaryFreq     = 22050, 
                dwPrimaryBitRate  = 16
*/
bool nDSoundServer3::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                             DWORD dwPrimaryFreq, 
                                             DWORD dwPrimaryBitRate )
{
    HRESULT hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    n_assert(0 != m_pDS);

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return false;

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return false;

    if( pDSBPrimary ) pDSBPrimary->Release();

    return true;
}

//------------------------------------------------------------------------------
/**
    Returns the 3D listener interface associated with primary buffer.
*/
bool nDSoundServer3::Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener )
{
    HRESULT             hr;
    DSBUFFERDESC        dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    n_assert(0 != ppDSListener);
    n_assert(0 != m_pDS);

    *ppDSListener = NULL;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
        return false;

    if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)ppDSListener ) ) )
    {
        if( pDSBPrimary ) pDSBPrimary->Release();
        return false;
    }

    // Release the primary buffer, since it is not need anymore
    if( pDSBPrimary ) pDSBPrimary->Release();

    return true;
}
