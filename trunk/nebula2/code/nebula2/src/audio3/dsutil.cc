//-----------------------------------------------------------------------------
// File: DSUtil.cpp
//
// Desc: DirectSound framework classes for reading and writing wav files and
//       playing them in DirectSound buffers. Feel free to use this class
//       as a starting point for adding extra functionality.
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <mmsystem.h>
#include <dxerr9.h>
#include <dsound.h>
#include "audio3/DSUtil.h"
#include "audio3/DXUtil.h"

//-----------------------------------------------------------------------------
// Name: CSoundManager::CSoundManager()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSoundManager::CSoundManager()
{
    m_pDS = NULL;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::~CSoundManager()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
    SAFE_RELEASE(m_pDS);
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Initialize(HWND  hWnd, DWORD dwCoopLevel)
{
    HRESULT hr;

    SAFE_RELEASE(m_pDS);

    // Create IDirectSound using the primary sound device
    if (FAILED(hr = DirectSoundCreate8(NULL, &m_pDS, NULL)))
        return DXTRACE_ERR(TEXT("DirectSoundCreate8"), hr);

    // Set DirectSound coop level
    if (FAILED(hr = m_pDS->SetCooperativeLevel(hWnd, dwCoopLevel)))
        return DXTRACE_ERR(TEXT("SetCooperativeLevel"), hr);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format
//       !WARNING! - Setting the primary buffer format and then using this
//                   same dsound object for DirectMusic messes up DirectMusic!
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050,
//               dwPrimaryBitRate  = 16
//-----------------------------------------------------------------------------
HRESULT CSoundManager::SetPrimaryBufferFormat(DWORD dwPrimaryChannels,
                                              DWORD dwPrimaryFreq,
                                              DWORD dwPrimaryBitRate)
{
    HRESULT hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if (m_pDS == NULL)
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;

    if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL)))
        return DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);

    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag      = (WORD)WAVE_FORMAT_PCM;
    wfx.nChannels       = (WORD)dwPrimaryChannels;
    wfx.nSamplesPerSec  = (DWORD)dwPrimaryFreq;
    wfx.wBitsPerSample  = (WORD)dwPrimaryBitRate;
    wfx.nBlockAlign     = (WORD)(wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD)(wfx.nSamplesPerSec * wfx.nBlockAlign);

    if (FAILED(hr = pDSBPrimary->SetFormat(&wfx)))
        return DXTRACE_ERR(TEXT("SetFormat"), hr);

    SAFE_RELEASE(pDSBPrimary);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Get3DListenerInterface()
// Desc: Returns the 3D listener interface associated with primary buffer.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Get3DListenerInterface(LPDIRECTSOUND3DLISTENER* ppDSListener)
{
    HRESULT hr;
    DSBUFFERDESC dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if (ppDSListener == NULL)
        return E_INVALIDARG;
    if (m_pDS == NULL)
        return CO_E_NOTINITIALIZED;

    *ppDSListener = NULL;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL)))
        return DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);

    if (FAILED(hr = pDSBPrimary->QueryInterface(IID_IDirectSound3DListener,
                                                  (VOID**)ppDSListener)))
    {
        SAFE_RELEASE(pDSBPrimary);
        return DXTRACE_ERR(TEXT("QueryInterface"), hr);
    }

    // Release the primary buffer, since it is not need anymore
    SAFE_RELEASE(pDSBPrimary);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Create()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Create(CSound** ppSound,
                              LPTSTR strWaveFileName,
                              DWORD dwCreationFlags,
                              GUID guid3DAlgorithm,
                              DWORD dwNumBuffers)
{
    HRESULT hr;
    HRESULT hrRet = S_OK;
    DWORD i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    nAudioFile*          pWaveFile      = NULL;

    if (m_pDS == NULL)
        return CO_E_NOTINITIALIZED;
    if (strWaveFileName == NULL || ppSound == NULL || dwNumBuffers < 1)
        return E_INVALIDARG;

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if (apDSBuffer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    // identify file type by extension
    char* ext = strdup(strrchr(strWaveFileName, '.'));
    for (char* c = ext; *c != 0; ++c)
        *c = tolower(*c);

    if (!strcmp(ext, ".wav"))
    {
        pWaveFile = new nWavFile();
    }
    else if (!strcmp(ext, ".ogg"))
    {
        pWaveFile = new nOggFile();
    }
    else
    {
        n_error("Audio file format not supported: %s\n", strWaveFileName);
    }

    free(ext);

    if (pWaveFile == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile->Open(strWaveFileName);

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = pWaveFile->GetSize();

    // Minimal required buffer size
    dwDSBufferSize = n_max(dwDSBufferSize , DSBSIZE_FX_MIN);

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->GetFormat();

    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
    hr = m_pDS->CreateSoundBuffer(&dsbd, &apDSBuffer[0], NULL);

    // Be sure to return this error code if it occurs so the
    // callers knows this happened.
    if (hr == DS_NO_VIRTUALIZATION)
        hrRet = DS_NO_VIRTUALIZATION;

    if (FAILED(hr))
    {
        // The usual Error in this case here is if a stereo sound is
        // tried to be played as 3D-Sound
        // only MONO is allowed in this case !!!!

        // DSERR_BUFFERTOOSMALL will be returned if the buffer is
        // less than DSBSIZE_FX_MIN and the buffer is created
        // with DSBCAPS_CTRLFX.

        // It might also fail if hardware buffer mixing was requested
        // on a device that doesn't support it.
        nString msg;
        msg.Format("CSoundManager::Create(): CreateSoundBuffer for '%s' failed! Make sure you do not try to play a stereo sound as 3D sound!", strWaveFileName);
        n_dxtrace(hr, msg.Get());

        goto LFail;
    }

    // Default to use DuplicateSoundBuffer() when created extra buffers since always
    // create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if
    // DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
    if ((dwCreationFlags & DSBCAPS_CTRLFX) == 0)
    {
        for (i = 1; i < dwNumBuffers; i++)
        {
            hr = m_pDS->DuplicateSoundBuffer(apDSBuffer[0], &apDSBuffer[i]);
            if (FAILED(hr))
            {
                n_dxtrace(hr, "CSoundManager::Create(): DuplicateSoundBuffer");
                goto LFail;
            }
        }
    }
    else
    {
        for (i = 1; i < dwNumBuffers; i++)
        {
            hr = m_pDS->CreateSoundBuffer(&dsbd, &apDSBuffer[i], NULL);
            if (FAILED(hr))
            {
                n_dxtrace(hr, "CSoundManager::Create(): CreateSoundBuffer");
                goto LFail;
            }
        }
   }

    // Create the sound
    *ppSound = new CSound(apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags);

    SAFE_DELETE(pWaveFile);
    SAFE_DELETE(apDSBuffer);
    return hrRet;

LFail:
    // Cleanup
    SAFE_DELETE(pWaveFile);
    SAFE_DELETE(apDSBuffer);
    return hr;
}


//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateFromMemory()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateFromMemory(CSound** ppSound,
                                        BYTE* pbData,
                                        ULONG  ulDataSize,
                                        LPWAVEFORMATEX pwfx,
                                        DWORD dwCreationFlags,
                                        GUID guid3DAlgorithm,
                                        DWORD dwNumBuffers)
{
    n_error("Sorry - not implemented yet");
    return E_NOTIMPL;
}





//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateStreaming()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateStreaming(CStreamingSound** ppStreamingSound,
                                       LPTSTR strWaveFileName,
                                       DWORD dwCreationFlags,
                                       GUID guid3DAlgorithm,
                                       DWORD dwNotifyCount,
                                       DWORD dwNotifySize)
{
    HRESULT hr;

    // CO_E_NOTINITIALIZED ?
    n_assert(m_pDS != NULL) ;

    // Check for zero pointer
    n_assert(strWaveFileName != NULL);
    n_assert(ppStreamingSound != NULL);

    LPDIRECTSOUNDBUFFER pDSBuffer      = NULL;
    DWORD               dwDSBufferSize = NULL;
    nAudioFile*         pWaveFile      = NULL;

    // identify file type by extension
    char* ext = strdup(strrchr(strWaveFileName, '.'));
    for (char* c = ext; *c != 0; ++c)
        *c = tolower(*c);

    if (!strcmp(ext, ".wav"))
    {
        pWaveFile = new nWavFile();
    }
    else if (!strcmp(ext, ".ogg"))
    {
        pWaveFile = new nOggFile();
    }
    else
    {
        n_error("Audio file format not supported: %s\n", strWaveFileName);
    }

    free(ext);

    // out of memory ?
    n_assert (pWaveFile != NULL);

    pWaveFile->Open(strWaveFileName);

    // Figure out how big the DSound buffer should be
    dwDSBufferSize = dwNotifySize * dwNotifyCount;

    // Set up the direct sound buffer.
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags |
                           DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->GetFormat();

    // n_assert(pWaveFile->m_pwfx->nChannels==1); ?
    // in case of 3D-sound, mono is absolutely necessary!
    // otherwise CreateSoundBuffer will
    // return DSERR_BADFORMAT !!!!

    if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL)))
    {
        // If wave format isn't then it will return
        // either DSERR_BADFORMAT or E_INVALIDARG
        if (hr == DSERR_BADFORMAT || hr == E_INVALIDARG)
            n_error("CreateSoundBuffer");

        n_error("CreateSoundBuffer");
    }
    n_assert (SUCCEEDED (hr));

    // Create the sound
    *ppStreamingSound = new CStreamingSound(pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize, dwCreationFlags);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CSound::CSound()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSound::CSound(LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize,
               DWORD dwNumBuffers, nAudioFile* pWaveFile, DWORD dwCreationFlags)
{
    DWORD i;

    m_apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if (NULL != m_apDSBuffer)
    {
        for (i=0; i<dwNumBuffers; i++)
            m_apDSBuffer[i] = apDSBuffer[i];

        m_dwDSBufferSize  = dwDSBufferSize;
        m_dwNumBuffers    = dwNumBuffers;
        m_pWaveFile       = pWaveFile;
        m_dwCreationFlags = dwCreationFlags;

        FillBufferWithSound(m_apDSBuffer[0], FALSE);
    }
}

//-----------------------------------------------------------------------------
// Name: CSound::~CSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSound::~CSound()
{
    for (DWORD i=0; i<m_dwNumBuffers; i++)
    {
        SAFE_RELEASE(m_apDSBuffer[i]);
    }
    SAFE_DELETE_ARRAY(m_apDSBuffer);
}


//-----------------------------------------------------------------------------
// Name: CSound::FillBufferWithSound()
// Desc: Fills a DirectSound buffer with a sound file
//-----------------------------------------------------------------------------
HRESULT CSound::FillBufferWithSound(LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger)
{
    HRESULT hr;
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file

    if (m_pWaveFile == NULL)
    {
        return CO_E_NOTINITIALIZED;
    }

    if (pDSB == NULL)
        return CO_E_NOTINITIALIZED;

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    if (FAILED(hr = RestoreBuffer(pDSB, NULL)))
        return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

    // Lock the buffer down
    if (FAILED(hr = pDSB->Lock(0, m_dwDSBufferSize,
                               &pDSLockedBuffer, &dwDSLockedBufferSize,
                               NULL, NULL, 0L)))
        return DXTRACE_ERR(TEXT("Lock"), hr);

    int hr1 = S_OK;
    int hr2 = S_OK;
    WAVEFORMATEX *m_pwfx=0;

    dwWavDataRead = 0;
    m_pWaveFile->Reset();
    dwWavDataRead = m_pWaveFile->Read(pDSLockedBuffer, dwDSLockedBufferSize);
    m_pwfx = m_pWaveFile->GetFormat();

    if (dwWavDataRead == 0)
    {
        // Wav is blank, so just fill with silence
        FillMemory((BYTE*)pDSLockedBuffer,
                   dwDSLockedBufferSize,
                   (BYTE)(m_pwfx->wBitsPerSample == 8 ? 128 : 0));
    }
    else if (dwWavDataRead < dwDSLockedBufferSize)
    {
        // If the wav file was smaller than the DirectSound buffer,
        // we need to fill the remainder of the buffer with data
        if (bRepeatWavIfBufferLarger)
        {
            // Reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwWavDataRead;    // From previous call above.
            while (dwReadSoFar < dwDSLockedBufferSize)
            {
                // This will keep reading in until the buffer is full
                // for very short files
                m_pWaveFile->Reset();
                dwWavDataRead = m_pWaveFile->Read((char*)pDSLockedBuffer + dwReadSoFar,
                                                  dwDSLockedBufferSize - dwReadSoFar);

                dwReadSoFar += dwWavDataRead;
            }
        }
        else
        {
            // Don't repeat the file, just fill in silence
            FillMemory((BYTE*) pDSLockedBuffer + dwWavDataRead,
                        dwDSLockedBufferSize - dwWavDataRead,
                        (BYTE)(m_pwfx->wBitsPerSample == 8 ? 128 : 0));
        }
    }

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSound::RestoreBuffer()
// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was
//       restored.  It can also NULL if the information is not needed.
//-----------------------------------------------------------------------------
HRESULT CSound::RestoreBuffer(LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored)
{
    HRESULT hr;

    if (pDSB == NULL)
        return CO_E_NOTINITIALIZED;
    if (pbWasRestored)
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if (FAILED(hr = pDSB->GetStatus(&dwStatus)))
        return DXTRACE_ERR(TEXT("GetStatus"), hr);

    if (dwStatus & DSBSTATUS_BUFFERLOST)
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so
        // the restoring the buffer may fail.
        // If it does, sleep until DirectSound gives us control.
        do
        {
            hr = pDSB->Restore();
            if (hr == DSERR_BUFFERLOST)
                Sleep(10);
        }
        while ((hr = pDSB->Restore()) == DSERR_BUFFERLOST);

        if (pbWasRestored != NULL)
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}




//-----------------------------------------------------------------------------
// Name: CSound::GetFreeBuffer()
// Desc: Finding the first buffer that is not playing and return a pointer to
//       it, or if all are playing return a pointer to a randomly selected buffer.
//
//  - 17-May-04     floh    additionally returns index of buffer
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetFreeBuffer(DWORD& index)
{
    index = 0;
    if (m_apDSBuffer == NULL)
    {
        return FALSE;
    }

    for (index = 0; index < m_dwNumBuffers; index++)
    {
        if (m_apDSBuffer[index])
        {
            DWORD dwStatus = 0;
            m_apDSBuffer[index]->GetStatus(&dwStatus);
            if ((dwStatus & DSBSTATUS_PLAYING) == 0)
            {
                break;
            }
        }
    }

    if (index != m_dwNumBuffers)
    {
        return m_apDSBuffer[index];
    }
    else
    {
        index = rand() % m_dwNumBuffers;
        return m_apDSBuffer[index];
    }
}




//-----------------------------------------------------------------------------
// Name: CSound::GetBuffer()
// Desc:
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetBuffer(DWORD dwIndex)
{
    if (m_apDSBuffer == NULL)
        return NULL;
    if (dwIndex >= m_dwNumBuffers)
        return NULL;

    return m_apDSBuffer[dwIndex];
}




//-----------------------------------------------------------------------------
// Name: CSound::Get3DBufferInterface()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSound::Get3DBufferInterface(DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer)
{
    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;
    if (dwIndex >= m_dwNumBuffers)
        return E_INVALIDARG;

    *ppDS3DBuffer = NULL;

    return m_apDSBuffer[dwIndex]->QueryInterface(IID_IDirectSound3DBuffer,
                                                 (VOID**)ppDS3DBuffer);
}


//-----------------------------------------------------------------------------
// Name: CSound::Play()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CSound::Play(DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, LONG lPan, DWORD& outIndex)
{
    HRESULT hr;
    BOOL bRestored;

    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;

    LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer(outIndex);

    if (pDSB == NULL)
        return DXTRACE_ERR(TEXT("GetFreeBuffer"), E_FAIL);

    // Restore the buffer if it was lost
    if (FAILED(hr = RestoreBuffer(pDSB, &bRestored)))
        return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

    if (bRestored)
    {
        // The buffer was restored, so we need to fill it with new data
        if (FAILED(hr = FillBufferWithSound(pDSB, FALSE)))
            return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);
    }

    if (m_dwCreationFlags & DSBCAPS_CTRLVOLUME)
    {
        pDSB->SetVolume(lVolume);
    }

    if (lFrequency != -1 &&
        (m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY))
    {
        pDSB->SetFrequency(lFrequency);
    }

    if (m_dwCreationFlags & DSBCAPS_CTRLPAN)
    {
        pDSB->SetPan(lPan);
    }

    hr = pDSB->SetCurrentPosition(0);
    return pDSB->Play(0, dwPriority, dwFlags);
}




//-----------------------------------------------------------------------------
// Name: CSound::Play3D()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CSound::Play3D(LPDS3DBUFFER p3DBuffer, DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, DWORD& outIndex)
{
    HRESULT hr;
    BOOL    bRestored;
    DWORD   dwBaseFrequency;

    if (m_apDSBuffer == NULL)
    {
        return CO_E_NOTINITIALIZED;
    }

    LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer(outIndex);
    if (pDSB == NULL)
    {
        return DXTRACE_ERR(TEXT("GetFreeBuffer"), E_FAIL);
    }

    // Restore the buffer if it was lost
    if (FAILED(hr = RestoreBuffer(pDSB, &bRestored)))
    {
        return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);
    }

    if (bRestored)
    {
        // The buffer was restored, so we need to fill it with new data
        if (FAILED(hr = FillBufferWithSound(pDSB, FALSE)))
        {
            return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);
        }

    }

    if (m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY)
    {
        pDSB->GetFrequency(&dwBaseFrequency);
        pDSB->SetFrequency(dwBaseFrequency + lFrequency);
    }
    if (m_dwCreationFlags & DSBCAPS_CTRLVOLUME)
    {
        pDSB->SetVolume(lVolume);
    }

    // QI for the 3D buffer
    LPDIRECTSOUND3DBUFFER pDS3DBuffer;
    hr = pDSB->QueryInterface(IID_IDirectSound3DBuffer, (VOID**) &pDS3DBuffer);
    if (SUCCEEDED(hr))
    {
        hr = pDS3DBuffer->SetAllParameters(p3DBuffer, DS3D_IMMEDIATE);
        if (SUCCEEDED(hr))
        {
            hr = pDSB->SetCurrentPosition(0);
            hr = pDSB->Play(0, dwPriority, dwFlags);
        }
        pDS3DBuffer->Release();
    }

    return hr;
}



//-----------------------------------------------------------------------------
// Name: CSound::Stop()
// Desc: Stops all sound from playing
//-----------------------------------------------------------------------------
HRESULT CSound::Stop()
{
    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for (DWORD i=0; i<m_dwNumBuffers; i++)
        hr |= m_apDSBuffer[i]->Stop();

    return hr;
}

//-----------------------------------------------------------------------------
// Name: CSound::Stop(index)
// Desc: Stops only the indexed sound from playing
//-----------------------------------------------------------------------------
HRESULT CSound::Stop(DWORD index)
{
    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;
    if ((index < 0) || (index >= m_dwNumBuffers))
        return DSERR_INVALIDPARAM;

    HRESULT hr = m_apDSBuffer[index]->Stop();
    return hr;
}

//-----------------------------------------------------------------------------
// Name: CSound::Reset()
// Desc: Reset all of the sound buffers
//-----------------------------------------------------------------------------
HRESULT CSound::Reset()
{
    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for (DWORD i=0; i<m_dwNumBuffers; i++)
        hr |= m_apDSBuffer[i]->SetCurrentPosition(0);

    return hr;
}

//-----------------------------------------------------------------------------
// Name: CSound::Reset(index)
// Desc: Reset only the indexed sound buffer
//-----------------------------------------------------------------------------
HRESULT CSound::Reset(DWORD index)
{
    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;
    if ((index < 0) || (index >= m_dwNumBuffers))
        return DSERR_INVALIDPARAM;

    HRESULT hr = m_apDSBuffer[index]->SetCurrentPosition(0);
    return hr;
}

//-----------------------------------------------------------------------------
// Name: CSound::IsSoundPlaying()
// Desc: Checks to see if any buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
BOOL CSound::IsSoundPlaying()
{
    BOOL bIsPlaying = FALSE;

    if (m_apDSBuffer == NULL)
        return FALSE;

    for (DWORD i=0; i<m_dwNumBuffers; i++)
    {
        if (m_apDSBuffer[i])
        {
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus(&dwStatus);
            bIsPlaying |= ((dwStatus & DSBSTATUS_PLAYING) != 0);
        }
    }

    return bIsPlaying;
}

//-----------------------------------------------------------------------------
// Name: CSound::IsSoundPlaying(soundIndex)
// Desc: Checks to see if a specific buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
BOOL CSound::IsSoundPlaying(DWORD soundIndex)
{
    BOOL bIsPlaying = FALSE;

    if (m_apDSBuffer == NULL)
        return FALSE;
    if ((soundIndex < 0) || (soundIndex >= m_dwNumBuffers))
        return FALSE;   // buffer overflow!

    if (this->m_apDSBuffer[soundIndex])
    {
        DWORD dwStatus = 0;
        m_apDSBuffer[soundIndex]->GetStatus(&dwStatus);
        bIsPlaying |= ((dwStatus & DSBSTATUS_PLAYING) != 0);
    }
    return bIsPlaying;
}

//-----------------------------------------------------------------------------
// Name: CStreamingSound::CStreamingSound()
// Desc: Setups up a buffer so data can be streamed from the wave file into
//       a buffer.  This is very useful for large wav files that would take a
//       while to load.  The buffer is initially filled with data, then
//       as sound is played the notification events are signaled and more data
//       is written into the buffer by calling HandleWaveStreamNotification()
//-----------------------------------------------------------------------------
CStreamingSound::CStreamingSound(LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize,
                                 nAudioFile* pWaveFile, DWORD dwNotifySize, DWORD dwCreationFlags)
                : CSound(&pDSBuffer, dwDSBufferSize, 1, pWaveFile, dwCreationFlags)
{
    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNotifySize      = dwNotifySize;
    m_dwTriggerWriteOffset = 0;
    m_dwNextWriteOffset = 0;
    m_bFillNextNotificationWithSilence = FALSE;
    this->stopCount = 0;
}

//-----------------------------------------------------------------------------
// Name: CStreamingSound::~CStreamingSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CStreamingSound::~CStreamingSound()
{
    SAFE_DELETE(m_pWaveFile);
}

//-----------------------------------------------------------------------------
// Name: CStreamingSound::IsSoundPlaying()
// Desc: Streaming sounds need different behavior
//-----------------------------------------------------------------------------
BOOL
CStreamingSound::IsSoundPlaying()
{
    return CSound::IsSoundPlaying() && (this->stopCount <= 1);
}

//-----------------------------------------------------------------------------
// Name: CStreamingSound::HandleWaveStreamNotification()
// Desc: Handle the notification that tells us to put more wav data in the
//       circular buffer
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::HandleWaveStreamNotification(BOOL bLoopedPlay)
{
    HRESULT hr;
    DWORD   dwCurrentPlayPos;
    DWORD   dwPlayDelta;
    DWORD   dwBytesWrittenToBuffer;
    VOID*   pDSLockedBuffer = NULL;
    VOID*   pDSLockedBuffer2 = NULL;
    DWORD   dwDSLockedBufferSize;
    DWORD   dwDSLockedBufferSize2;

    if (m_pWaveFile == NULL)// Wav
        return CO_E_NOTINITIALIZED;

    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if (FAILED(hr = RestoreBuffer(m_apDSBuffer[0], &bRestored)))
        return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

    if (bRestored)
    {
        // The buffer was restored, so we need to fill it with new data
        if (FAILED(hr = FillBufferWithSound(m_apDSBuffer[0], FALSE)))
            return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);
        return S_OK;
    }

    // Lock the DirectSound buffer
    if (FAILED(hr = m_apDSBuffer[0]->Lock(m_dwNextWriteOffset, m_dwNotifySize,
                                          &pDSLockedBuffer, &dwDSLockedBufferSize,
                                          &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L)))
        return DXTRACE_ERR(TEXT("Lock"), hr);

    // m_dwDSBufferSize and m_dwNextWriteOffset are both multiples of m_dwNotifySize,
    // it should the second buffer, so it should never be valid
    if (pDSLockedBuffer2 != NULL)
        return E_UNEXPECTED;

    WAVEFORMATEX *m_pwfx = m_pWaveFile->GetFormat();
    if (!m_bFillNextNotificationWithSilence)
    {
        dwBytesWrittenToBuffer = m_pWaveFile->Read((char*) pDSLockedBuffer,
                                                   dwDSLockedBufferSize);
    }
    else
    {
        // Fill the DirectSound buffer with silence
        this->stopCount++;
        FillMemory(pDSLockedBuffer, dwDSLockedBufferSize,
                   (BYTE)(m_pwfx->wBitsPerSample == 8 ? 128 : 0));
        dwBytesWrittenToBuffer = dwDSLockedBufferSize;
    }

    // If the number of bytes written is less than the
    // amount we requested, we have a short file.
    if (dwBytesWrittenToBuffer < dwDSLockedBufferSize)
    {
        if (!bLoopedPlay)
        {
            // Fill in silence for the rest of the buffer.
            FillMemory((BYTE*) pDSLockedBuffer + dwBytesWrittenToBuffer,
                       dwDSLockedBufferSize - dwBytesWrittenToBuffer,
                       (BYTE)(m_pwfx->wBitsPerSample == 8 ? 128 : 0));

            // Any future notifications should just fill the buffer with silence
            m_bFillNextNotificationWithSilence = TRUE;
        }
        else
        {
            // We are looping, so reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwBytesWrittenToBuffer;    // From previous call above.
            while (dwReadSoFar < dwDSLockedBufferSize)
            {
                // This will keep reading in until the buffer is full (for very short files).
                m_pWaveFile->Reset();
                dwBytesWrittenToBuffer = m_pWaveFile->Read((char*)pDSLockedBuffer + dwReadSoFar,
                                                           dwDSLockedBufferSize - dwReadSoFar);
                // 0 Byte Wav-File
                n_assert(dwBytesWrittenToBuffer!=0);

                dwReadSoFar += dwBytesWrittenToBuffer;
            }
        }
    }

    // Unlock the DirectSound buffer
    m_apDSBuffer[0]->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);

    // Figure out how much data has been played so far.  When we have played
    // past the end of the file, we will either need to start filling the
    // buffer with silence or starting reading from the beginning of the file,
    // depending if the user wants to loop the sound
    if (FAILED(hr = m_apDSBuffer[0]->GetCurrentPosition(&dwCurrentPlayPos, NULL)))
        return DXTRACE_ERR(TEXT("GetCurrentPosition"), hr);

    // Check to see if the position counter looped
    if (dwCurrentPlayPos < m_dwLastPlayPos)
        dwPlayDelta = (m_dwDSBufferSize - m_dwLastPlayPos) + dwCurrentPlayPos;
    else
        dwPlayDelta = dwCurrentPlayPos - m_dwLastPlayPos;

    m_dwPlayProgress += dwPlayDelta;
    m_dwLastPlayPos = dwCurrentPlayPos;

    // If we are now filling the buffer with silence, then we have found the end so
    // check to see if the entire sound has played, if it has then stop the buffer.
    if (m_bFillNextNotificationWithSilence)
    {
        // We don't want to cut off the sound before it's done playing.
        if (m_dwPlayProgress >= (DWORD)m_pWaveFile->GetSize())
        {
            m_apDSBuffer[0]->Stop();
        }
    }

    // Update where the buffer will lock (for next time)
    m_dwNextWriteOffset += dwDSLockedBufferSize;
    m_dwTriggerWriteOffset = m_dwNextWriteOffset - m_dwNotifySize;
    m_dwNextWriteOffset %= m_dwDSBufferSize; // Circular buffer
    m_dwTriggerWriteOffset %= m_dwDSBufferSize;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CStreamingSound::Inside()
// Desc: Returns true when the provided buffer offset is within the
//       start and end position (with wraparound)
//-----------------------------------------------------------------------------
bool
CStreamingSound::Inside(DWORD pos, DWORD start, DWORD end)
{
    if (start < end)
    {
        if ((pos >= start) && (pos < end)) return true;
        else                               return false;
    }
    else
    {
        if ((pos >= start) || (pos < end)) return true;
        else                               return false;
    }
}

//-----------------------------------------------------------------------------
// Name: CStreamingSound::CheckStreamUpdate()
// Desc: Returns true when the HandleWaveStreamNotification() method
//       must be called.
//-----------------------------------------------------------------------------
bool
CStreamingSound::CheckStreamUpdate()
{
    // get current write cursor position
    DWORD playCursor;
    DWORD writeCursor;
    HRESULT hr = m_apDSBuffer[0]->GetCurrentPosition(&playCursor, &writeCursor);

    if (this->Inside(writeCursor, this->m_dwTriggerWriteOffset, this->m_dwNextWriteOffset))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Name: CStreamingSound::Reset()
// Desc: Resets the sound so it will begin playing at the beginning
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::Reset()
{
    HRESULT hr;

    if (m_apDSBuffer[0] == NULL || m_pWaveFile == NULL)
        return CO_E_NOTINITIALIZED;

    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNextWriteOffset = 0;
    m_dwTriggerWriteOffset = m_dwDSBufferSize - m_dwNotifySize;
    m_bFillNextNotificationWithSilence = FALSE;
    this->stopCount = 0;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if (FAILED(hr = RestoreBuffer(m_apDSBuffer[0], &bRestored)))
        return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

    if (bRestored)
    {
        // The buffer was restored, so we need to fill it with new data
        if (FAILED(hr = FillBufferWithSound(m_apDSBuffer[0], FALSE)))
            return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);
    }

    m_pWaveFile->Reset();
    return m_apDSBuffer[0]->SetCurrentPosition(0L);
}
