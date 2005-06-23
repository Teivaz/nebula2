//------------------------------------------------------------------------------
//  ndsound3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/ndsound3.h"
#include "audio3/ndsoundserver3.h"
#include "audio3/ndsoundresource.h"

nNebulaClass(nDSound3, "nsound3");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    ndsound3

    @cppclass
    nDSound3
    
    @superclass
    nsound3
    
    @classinfo
    Docs needed.
*/


//------------------------------------------------------------------------------
/**
*/
nDSound3::nDSound3() :
    refSoundServer("/sys/servers/audio"),
    m_dwDS3DPROPSMode(DS3DMODE_HEADRELATIVE),
    dsVolume(0),
    m_apDSBuffer(0)
{
    memset(&(this->ds3DProps), 0, sizeof(this->ds3DProps));
    this->ds3DProps.dwSize = sizeof(this->ds3DProps);
}

//------------------------------------------------------------------------------
/**
*/
nDSound3::~nDSound3()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
    n_delete_array(this->m_apDSBuffer);
    n_delete(this->m_pWaveFile);
}

//------------------------------------------------------------------------------
/**
    Creates a shared sound resource, and initializes it.
*/
bool
nDSound3::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(!this->refSoundResource.isvalid());

    // create a sound resource object
    nDSoundResource* rsrc = (nDSoundResource*) this->refSoundServer->NewSoundResource(this->GetName());
    n_assert(rsrc);

    // if sound resource not opened yet, do it
    if (!rsrc->IsLoaded())
    {
        rsrc->SetFilename(this->GetFilename());
        rsrc->SetNumTracks(this->GetNumTracks());
        rsrc->SetAmbient(this->GetAmbient());
        rsrc->SetStreaming(this->GetStreaming());
        rsrc->SetLooping(this->GetLooping());
        rsrc->setSound3(this);
        bool success = rsrc->Load();
        if (!success)
        {
            rsrc->Release();
            return false;
        }
    }

    this->refSoundResource = rsrc;
    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unloads the sound resource object.
*/
void
nDSound3::UnloadResource()
{
    n_assert(this->IsLoaded());
    n_assert(this->refSoundResource.isvalid());
    this->refSoundResource->Release();
    this->refSoundResource.invalidate();
    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Start()
{
    HRESULT hr; 
    LONG dsVolume = this->GetDSVolume();
    DS3DBUFFER* ds3DProps = this->GetDS3DProps();

    // a streaming sound?
    if (this->GetStreaming())
    {
        // reset buffer and fill with new data
        this->Reset();
        LPDIRECTSOUNDBUFFER dsBuffer = this->GetBuffer(0);
        this->FillBufferWithSound(dsBuffer, this->GetLooping());
    }
    else
    {
        // a oneshot sound, just reset the buffer
        this->Reset();
    }

    // playback flags
    int flags = 0;//DSBPLAY_TERMINATEBY_PRIORITY;
    if (this->GetLooping() || this->GetStreaming())
    {
        flags |= DSBPLAY_LOOPING;
    }
    if (this->GetAmbient())
    {
        // play as 2D sound
        hr = this->Play(this->GetPriority(), flags, dsVolume, 0);
        if (FAILED(hr))
        {
            n_printf("nDSoundServer3: failed to start 2D sound '%s'\n", this->GetFilename());
        }
    }
    else
    {
        // play as 3D sound (FIXME: hmm, no volume???

        // DSBPLAY_TERMINATEBY_PRIORITY
        // Select the buffer that has the lowest priority of candidate buffers, as set in the call to Play.
        // If this is combined with one of the other two flags, the other flag is used only to resolve ties.
        flags |= DSBPLAY_TERMINATEBY_PRIORITY;

        hr = this->Play3D(ds3DProps, this->GetPriority(), flags, 0);
        if (FAILED(hr))
        {
            n_printf("nDSoundServer3: failed to start 3D sound '%s'\n", this->GetFilename());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Stop()
{
    if( m_apDSBuffer == NULL )
        return;

    HRESULT hr = 0;
    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_apDSBuffer[i]->Stop();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Update()
{
    // NOTE: for now updating works only for streaming sound, because
    // they are garanteed to have only one voice, for static sounds
    // we need to track which directsound buffer we are associated with!
    if (this->GetStreaming())
    {
        if (this->IsSoundPlaying())
        {
            LPDIRECTSOUNDBUFFER dsBuffer = this->GetBuffer(0);

            // update the volume
            dsBuffer->SetVolume(this->GetDSVolume());
        }
    }
}

//------------------------------------------------------------------------------
/**
    create direct sound buffer
*/
void
nDSound3::createDSBuffer(LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, 
                DWORD dwNumBuffers, nDSound_WavFmt* pWaveFile, DWORD dwCreationFlags )
{
    DWORD i;
    n_delete_array( m_apDSBuffer );
    m_apDSBuffer = n_new_array(LPDIRECTSOUNDBUFFER,dwNumBuffers);
    if( NULL != m_apDSBuffer )
    {
        for( i=0; i<dwNumBuffers; i++ )
            m_apDSBuffer[i] = apDSBuffer[i];
    
        m_dwDSBufferSize = dwDSBufferSize;
        m_dwNumBuffers   = dwNumBuffers;
        m_pWaveFile      = pWaveFile;
        m_dwCreationFlags = dwCreationFlags;
        
        FillBufferWithSound( m_apDSBuffer[0], FALSE );
    }
}

//------------------------------------------------------------------------------
/**
    create direct sound buffer
*/
void
nDSound3::createDSBufferStreaming(LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, 
                                  nDSound_WavFmt* pWaveFile, DWORD dwNotifySize )
{
    this->createDSBuffer( &pDSBuffer, dwDSBufferSize, 1, pWaveFile, 0 );

    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNotifySize      = dwNotifySize;
    m_dwTriggerWriteOffset = 0;
    m_dwNextWriteOffset = 0;
    m_bFillNextNotificationWithSilence = FALSE;
}

//------------------------------------------------------------------------------
/**
    Fills a DirectSound buffer with a sound file 
*/
bool nDSound3::FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger )
{
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

    n_assert(0 != pDSB);

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    hr = RestoreBuffer( pDSB, NULL );
    n_assert(hr);

    // Lock the buffer down
    if( FAILED( hr = pDSB->Lock( 0, m_dwDSBufferSize, &pDSLockedBuffer, &dwDSLockedBufferSize, NULL, NULL, 0L ) ) )
        n_assert(hr);

    // Reset the wave file to the beginning 
    m_pWaveFile->ResetFile();

    hr = m_pWaveFile->Read( (BYTE*) pDSLockedBuffer,
                            dwDSLockedBufferSize, 
                            &dwWavDataRead );
    n_assert(hr);

    if( dwWavDataRead == 0 )
    {
        // Wav is blank, so just fill with silence
        FillMemory( (BYTE*) pDSLockedBuffer, 
                    dwDSLockedBufferSize, 
                    (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
    }
    else if( dwWavDataRead < dwDSLockedBufferSize )
    {
        // If the wav file was smaller than the DirectSound buffer, 
        // we need to fill the remainder of the buffer with data 
        if( bRepeatWavIfBufferLarger )
        {       
            // Reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwWavDataRead;    // From previous call above.
            while( dwReadSoFar < dwDSLockedBufferSize )
            {  
                // This will keep reading in until the buffer is full 
                // for very short files
                hr = m_pWaveFile->ResetFile();
                n_assert(hr);

                hr = m_pWaveFile->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar,
                                        dwDSLockedBufferSize - dwReadSoFar,
                                        &dwWavDataRead );
                n_assert(hr);

                dwReadSoFar += dwWavDataRead;
            } 
        }
        else
        {
            // Don't repeat the wav file, just fill in silence 
            FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
                        dwDSLockedBufferSize - dwWavDataRead, 
                        (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
        }
    }

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    return true;
}

//------------------------------------------------------------------------------
/**
    Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
    restored.  It can also NULL if the information is not needed.
*/
bool nDSound3::RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored )
{
    HRESULT hr;

    n_assert(0 != pDSB);
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = pDSB->GetStatus( &dwStatus ) ) )
        return false;

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = pDSB->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( ( hr = pDSB->Restore() ) == DSERR_BUFFERLOST );

        if( pbWasRestored != NULL )
            *pbWasRestored = TRUE;

        return true;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
*/
long nDSound3::Get3DBufferInterface( DWORD dwIndex,
                                     LPDIRECTSOUND3DBUFFER* ppDS3DBuffer )
{
    if( m_apDSBuffer == NULL )
        return false;
    if( dwIndex >= m_dwNumBuffers )
        return false;

    *ppDS3DBuffer = NULL;

    return m_apDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)ppDS3DBuffer );
}

//------------------------------------------------------------------------------
/**
*/
LPDIRECTSOUNDBUFFER nDSound3::GetFreeBuffer()
{
    if( m_apDSBuffer == NULL )
        return false; 

    DWORD i;
    for( i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
                break;
        }
    }

    if( i != m_dwNumBuffers )
        return m_apDSBuffer[ i ];
    else
        return m_apDSBuffer[ rand() % m_dwNumBuffers ];
}

//------------------------------------------------------------------------------
/**
*/
LPDIRECTSOUNDBUFFER nDSound3::GetBuffer( DWORD dwIndex )
{
    if( m_apDSBuffer == NULL )
        return NULL;
    if( dwIndex >= m_dwNumBuffers )
        return NULL;

    return m_apDSBuffer[dwIndex];
}

//------------------------------------------------------------------------------
/**
*/
long nDSound3::Play( DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, LONG lPan )
{
    HRESULT hr;
    BOOL    bRestored;

    if( m_apDSBuffer == NULL )
        this->LoadResource();   // if sound resource not opened yet, do it

    LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();

    if( pDSB == NULL )
        return false;

    // Restore the buffer if it was lost
    if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
        return false;

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
            return false;
    }

    if( m_dwCreationFlags & DSBCAPS_CTRLVOLUME )
    {
        pDSB->SetVolume( lVolume );
    }

    if( lFrequency != -1 && 
        (m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY) )
    {
        pDSB->SetFrequency( lFrequency );
    }
    
    if( m_dwCreationFlags & DSBCAPS_CTRLPAN )
    {
        pDSB->SetPan( lPan );
    }
    
    return pDSB->Play( 0, dwPriority, dwFlags );
}

//------------------------------------------------------------------------------
/**
*/
bool nDSound3::Play3D( LPDS3DBUFFER p3DBuffer, DWORD dwPriority, DWORD dwFlags, LONG lFrequency )
{
    HRESULT hr;
    BOOL    bRestored;
    DWORD   dwBaseFrequency;

    if( m_apDSBuffer == NULL )
        this->LoadResource();   // if sound resource not opened yet, do it

    LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();
    if( pDSB == NULL )
        return false;

    // Restore the buffer if it was lost
    if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
        return false;

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
            return false;
    }

    if( m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY )
    {
        pDSB->GetFrequency( &dwBaseFrequency );
        pDSB->SetFrequency( dwBaseFrequency + lFrequency );
    }

    // QI for the 3D buffer 
    LPDIRECTSOUND3DBUFFER pDS3DBuffer;
    hr = pDSB->QueryInterface( IID_IDirectSound3DBuffer, (VOID**) &pDS3DBuffer );
    if( SUCCEEDED( hr ) )
    {
        hr = pDS3DBuffer->SetAllParameters( p3DBuffer, DS3D_IMMEDIATE );
        if( SUCCEEDED( hr ) )
        {
            hr = pDSB->Play( 0, dwPriority, dwFlags );
        }

        pDS3DBuffer->Release();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
long nDSound3::Reset()
{
    if( this->GetStreaming() )
    {
        HRESULT hr;

        if( m_apDSBuffer[0] == NULL || m_pWaveFile == NULL )
            return 0;

        m_dwLastPlayPos     = 0;
        m_dwPlayProgress    = 0;
        m_dwNextWriteOffset = 0;
        m_bFillNextNotificationWithSilence = FALSE;

        // Restore the buffer if it was lost
        BOOL bRestored;
        if( FAILED( hr = RestoreBuffer( m_apDSBuffer[0], &bRestored ) ) )
            return 0;

        if( bRestored )
        {
            // The buffer was restored, so we need to fill it with new data
            if( FAILED( hr = FillBufferWithSound( m_apDSBuffer[0], FALSE ) ) )
                return 0;
        }

        m_pWaveFile->ResetFile();

        return m_apDSBuffer[0]->SetCurrentPosition( 0L );  
    }
    else
    {
        if( m_apDSBuffer == NULL )
            return 0;

        HRESULT hr = 0;

        for( DWORD i=0; i<m_dwNumBuffers; i++ )
            hr |= m_apDSBuffer[i]->SetCurrentPosition( 0 );
    }

    return 1;
}

//------------------------------------------------------------------------------
/**
*/
BOOL nDSound3::IsSoundPlaying()
{
    BOOL bIsPlaying = FALSE;

    if( m_apDSBuffer == NULL )
        return FALSE; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
        }
    }

    return bIsPlaying;
}

//------------------------------------------------------------------------------
/**
*/
bool nDSound3::HandleWaveStreamNotification( BOOL bLoopedPlay )
{
    HRESULT hr;
    DWORD   dwCurrentPlayPos;
    DWORD   dwPlayDelta;
    DWORD   dwBytesWrittenToBuffer;
    VOID*   pDSLockedBuffer = NULL;
    VOID*   pDSLockedBuffer2 = NULL;
    DWORD   dwDSLockedBufferSize;
    DWORD   dwDSLockedBufferSize2;

    if( m_apDSBuffer == NULL || m_pWaveFile == NULL )
        return false;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if( FAILED( hr = RestoreBuffer( m_apDSBuffer[0], &bRestored ) ) )
        return false;

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( m_apDSBuffer[0], FALSE ) ) )
            return false;
        return true;
    }

    // Lock the DirectSound buffer
    if( FAILED( hr = m_apDSBuffer[0]->Lock( m_dwNextWriteOffset, m_dwNotifySize, 
                                            &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                            &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L ) ) )
        return false;

    // m_dwDSBufferSize and m_dwNextWriteOffset are both multiples of m_dwNotifySize, 
    // it should the second buffer, so it should never be valid
    if( pDSLockedBuffer2 != NULL )
        return false; 

    if( !m_bFillNextNotificationWithSilence )
    {
        // Fill the DirectSound buffer with wav data
        if( FAILED( hr = m_pWaveFile->Read( (BYTE*) pDSLockedBuffer, 
                                                  dwDSLockedBufferSize, 
                                                  &dwBytesWrittenToBuffer ) ) )           
            return false;
    }
    else
    {
        // Fill the DirectSound buffer with silence
        FillMemory( pDSLockedBuffer, dwDSLockedBufferSize, 
                    (BYTE)( m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
        dwBytesWrittenToBuffer = dwDSLockedBufferSize;
    }

    // If the number of bytes written is less than the 
    // amount we requested, we have a short file.
    if( dwBytesWrittenToBuffer < dwDSLockedBufferSize )
    {
        if( !bLoopedPlay ) 
        {
            // Fill in silence for the rest of the buffer.
            FillMemory( (BYTE*) pDSLockedBuffer + dwBytesWrittenToBuffer, 
                        dwDSLockedBufferSize - dwBytesWrittenToBuffer, 
                        (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );

            // Any future notifications should just fill the buffer with silence
            m_bFillNextNotificationWithSilence = TRUE;
        }
        else
        {
            // We are looping, so reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwBytesWrittenToBuffer;    // From previous call above.
            while( dwReadSoFar < dwDSLockedBufferSize )
            {  
                // This will keep reading in until the buffer is full (for very short files).
                if( FAILED( hr = m_pWaveFile->ResetFile() ) )
                    return false;

                if( FAILED( hr = m_pWaveFile->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar,
                                                          dwDSLockedBufferSize - dwReadSoFar,
                                                          &dwBytesWrittenToBuffer ) ) )
                    return false;

                dwReadSoFar += dwBytesWrittenToBuffer;
            } 
        } 
    }

    // Unlock the DirectSound buffer
    m_apDSBuffer[0]->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    // Figure out how much data has been played so far.  When we have played
    // past the end of the file, we will either need to start filling the
    // buffer with silence or starting reading from the beginning of the file, 
    // depending if the user wants to loop the sound
    if( FAILED( hr = m_apDSBuffer[0]->GetCurrentPosition( &dwCurrentPlayPos, NULL ) ) )
        return false;

    // Check to see if the position counter looped
    if( dwCurrentPlayPos < m_dwLastPlayPos )
        dwPlayDelta = ( m_dwDSBufferSize - m_dwLastPlayPos ) + dwCurrentPlayPos;
    else
        dwPlayDelta = dwCurrentPlayPos - m_dwLastPlayPos;

    m_dwPlayProgress += dwPlayDelta;
    m_dwLastPlayPos = dwCurrentPlayPos;

    // If we are now filling the buffer with silence, then we have found the end so 
    // check to see if the entire sound has played, if it has then stop the buffer.
    if( m_bFillNextNotificationWithSilence )
    {
        // We don't want to cut off the sound before it's done playing.
        if( m_dwPlayProgress >= m_pWaveFile->GetSize() )
        {
            m_apDSBuffer[0]->Stop();
        }
    }

    // Update where the buffer will lock (for next time)
    m_dwNextWriteOffset += dwDSLockedBufferSize; 
    m_dwTriggerWriteOffset = m_dwNextWriteOffset - m_dwNotifySize;
    m_dwNextWriteOffset %= m_dwDSBufferSize; // Circular buffer
    m_dwTriggerWriteOffset %= m_dwDSBufferSize;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nDSound3::Inside(DWORD pos, DWORD start, DWORD end)
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

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nDSound3::CheckStreamUpdate()
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

    return true;
}
