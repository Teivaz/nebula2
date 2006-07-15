//-----------------------------------------------------------------------------
// File: DSUtil.h
//
// Desc: 
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef DSUTIL_H
#define DSUTIL_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <msacm.h>

#include <vorbis/codec.h>
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/nautoref.h"
#include "audio3/nwavfile.h"
#include "audio3/noggfile.h"

//-----------------------------------------------------------------------------
// Classes used by this header
//-----------------------------------------------------------------------------
class CSoundManager;
class CSound;
class CStreamingSound;

//-----------------------------------------------------------------------------
// Typing macros 
//-----------------------------------------------------------------------------
//#define WAVEFILE_READ   1
//#define WAVEFILE_WRITE  2

#define DSUtil_StopSound(s)         { if(s) s->Stop(); }
#define DSUtil_PlaySound(s)         { if(s) s->Play( 0, 0 ); }
#define DSUtil_PlaySoundLooping(s)  { if(s) s->Play( 0, DSBPLAY_LOOPING ); }

//-----------------------------------------------------------------------------
// Name: class CSoundManager
// Desc: 
//-----------------------------------------------------------------------------
class CSoundManager
{
protected:
    LPDIRECTSOUND8 m_pDS;

public:
    CSoundManager();
    ~CSoundManager();

    HRESULT Initialize( HWND hWnd, DWORD dwCoopLevel );
    inline  LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
    HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    HRESULT Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );

    HRESULT Create( CSound** ppSound, LPTSTR strWaveFileName, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    HRESULT CreateFromMemory( CSound** ppSound, BYTE* pbData, ULONG ulDataSize, LPWAVEFORMATEX pwfx, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    HRESULT CreateStreaming( CStreamingSound** ppStreamingSound, LPTSTR strWaveFileName, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNotifyCount, DWORD dwNotifySize );
    HRESULT CreateStreamingOgg( CStreamingSound** ppStreamingSound, LPTSTR strOggFileName, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNotifyCount, DWORD dwNotifySize );
};

//-----------------------------------------------------------------------------
// Name: class CSound
// Desc: Encapsulates functionality of a DirectSound buffer.
//-----------------------------------------------------------------------------
class CSound
{
protected:
    nAudioFile*          m_pWaveFile;
    LPDIRECTSOUNDBUFFER* m_apDSBuffer;
    DWORD                m_dwDSBufferSize;
    DWORD                m_dwNumBuffers;
    DWORD                m_dwCreationFlags;

    HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

public:
    CSound( LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, nAudioFile* pWaveFile, DWORD dwCreationFlags );
    virtual ~CSound();

    virtual HRESULT Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
    virtual HRESULT FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );
    virtual LPDIRECTSOUNDBUFFER GetFreeBuffer(DWORD& index);
    virtual LPDIRECTSOUNDBUFFER GetBuffer( DWORD dwIndex );

    virtual HRESULT Play( DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, LONG lPan, DWORD& outIndex);
    virtual HRESULT Play3D( LPDS3DBUFFER p3DBuffer, DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, DWORD& outIndex);
    virtual HRESULT Stop();                     // stops all sound buffers
    virtual HRESULT Stop(DWORD index);          // stops only indexed sound buffer
    virtual HRESULT Reset();                    // resets all sound buffers
    virtual HRESULT Reset(DWORD index);         // resets only indexed sound buffer
    virtual BOOL IsSoundPlaying();              // returns true if any sound buffer is playing
    virtual BOOL IsSoundPlaying(DWORD index);   // returns true if a specific sound buffer is playing
};




//-----------------------------------------------------------------------------
// Name: class CStreamingSound
// Desc: Encapsulates functionality to play a wave file with DirectSound.  
//       The Create() method loads a chunk of wave file into the buffer, 
//       and as sound plays more is written to the buffer by calling 
//       HandleWaveStreamNotification() whenever hNotifyEvent is signaled.
//-----------------------------------------------------------------------------
class CStreamingSound : public CSound
{
protected:
  
    DWORD m_dwLastPlayPos;
    DWORD m_dwPlayProgress;
    DWORD m_dwNotifySize;
    DWORD m_dwTriggerWriteOffset;
    DWORD m_dwNextWriteOffset;
    BOOL  m_bFillNextNotificationWithSilence;
    DWORD stopCount;

public:
    CStreamingSound( LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, nAudioFile* pWaveFile, DWORD dwNotifySize, DWORD dwCreationFlags );
    virtual ~CStreamingSound();

    virtual HRESULT HandleWaveStreamNotification( BOOL bLoopedPlay );
    virtual HRESULT Reset();
    virtual BOOL IsSoundPlaying();              // returns true if any sound buffer is playing

    /// FLOH extended: return true when HandleWaveStreamNotification() must be called
    bool CheckStreamUpdate();
    bool Inside(DWORD pos, DWORD start, DWORD end);
};

#endif // DSUTIL_H
