#ifndef N_NDSOUND_WAVFMT_H
#define N_NDSOUND_WAVFMT_H
//------------------------------------------------------------------------------
/**
    @class nDSound_WavFmt
    @ingroup NebulaDSAudioSystem

    Wav Format Control

    (C) 2004 Bang, Chang Kyu
*/
#include "audio3/naudioserver3.h"

#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

//------------------------------------------------------------------------------
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

//------------------------------------------------------------------------------
/**
    Encapsulates reading or writing sound data to or from a wave file
*/
class nDSound_WavFmt
{
public:
    WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO         m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO      m_ck;          // Multimedia RIFF chunk
    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
    DWORD         m_dwSize;      // The size of the wave file
    MMIOINFO      m_mmioinfoOut;
    DWORD         m_dwFlags;
    BOOL          m_bIsReadingFromMemory;
    BYTE*         m_pbData;
    BYTE*         m_pbDataCur;
    ULONG         m_ulDataSize;
    CHAR*         m_pResourceBuffer;

protected:
    bool ReadMMIO();
    bool WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
    nDSound_WavFmt();
    ~nDSound_WavFmt();

    bool Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
    bool OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
    bool Close();

    bool Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    bool Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );

    DWORD   GetSize();
    bool ResetFile();
    WAVEFORMATEX* GetFormat() { return m_pwfx; };
};

//------------------------------------------------------------------------------
#endif
