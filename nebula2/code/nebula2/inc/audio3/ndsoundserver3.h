#ifndef N_DSOUNDSERVER3_H
#define N_DSOUNDSERVER3_H
//------------------------------------------------------------------------------
/**
    @class nDSoundServer3

    nAudioServer3 implemention using DirectSound.

    (C) 2003 RadonLabs GmbH
*/
#include "audio3/naudioserver3.h"
#include "kernel/nautoref.h"

#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

class nResourceServer;
class nEnv;
class nDSound3;
class nDSound_WavFmt;

//------------------------------------------------------------------------------
class nDSoundServer3 : public nAudioServer3
{
public:
    /// constructor
    nDSoundServer3();
    /// destructor
    virtual ~nDSoundServer3();
    /// open the audio device
    virtual bool Open();
    /// close the audio device
    virtual void Close();
    /// begin an audio frame
    virtual bool BeginScene();
    /// update listener attributes
    virtual void UpdateListener(const nListener3& l);
    /// start a sound
    virtual void StartSound(nSound3* s);
    /// update a sound
    virtual void UpdateSound(nSound3* s);
    /// stop a sound
    virtual void StopSound(nSound3* s);    
    /// end the audio frame
    virtual void EndScene();
    /// create a non-shared sound object
    virtual nSound3* NewSound();
    /// create a shared sound resource object
    virtual nSoundResource* NewSoundResource(const char* rsrcName);

    static nKernelServer* kernelServer;

protected:
    nAutoRef<nResourceServer> refResourceServer;
    nAutoRef<nEnv> refHwnd;
    IDirectSound3DListener8* dsListener;
    DS3DLISTENER dsListenerProps;
    nTime lastStreamUpdateCheck;

    // soundManager ports
public:
    /// set primary buffer to a specified format
    bool SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    /// returns the 3D listener interface associated with primary buffer.
    bool Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );
    /// create
    bool Create( nDSound3** ppSound, LPTSTR strWaveFileName, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    /// create from memory
    bool CreateFromMemory( nDSound3** ppSound, BYTE* pbData, ULONG ulDataSize, LPWAVEFORMATEX pwfx, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    /// create for streaming
    bool CreateStreaming( nDSound3** ppSound, LPTSTR strWaveFileName, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNotifyCount, DWORD dwNotifySize );

protected:
    LPDIRECTSOUND8  m_pDS;
};

//------------------------------------------------------------------------------
#endif
