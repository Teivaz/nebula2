#ifndef N_WAVFILE_H
#define N_WAVFILE_H
//------------------------------------------------------------------------------
/**
    @class nWavFile
    @ingroup Audio3
    @brief Provide read-access to a wav file.

    NOTE: this is basically a cleaned-up rippoff of DSound-Framework's
    CWaveFile class.
    
    (C) 2005 Radon Labs GmbH
*/
#include "audio3/naudiofile.h"

//------------------------------------------------------------------------------
class nWavFile : public nAudioFile
{
public:
    /// constructor
    nWavFile();
    /// destructor
    virtual ~nWavFile();
    /// open the file
    virtual bool Open(const nString& filename);
    /// close the file
    virtual void Close();
    /// read bytes from file
    virtual uint Read(void* buffer, uint bytesToRead);
    /// reset to beginning of audio data
    virtual bool Reset();
    /// return size of audio data in file in bytes
    virtual int GetSize() const;
    /// return pointer to format description
    virtual WAVEFORMATEX* GetFormat() const;

private:
    /// read wav header data
    bool ReadMMIO();
    /// install MMIO custom file procedures
    void InstallMMIOProc();
    /// uninstall MMIO custom file procedures
    void UninstallMMIOProc();

    static bool mmioProcInstalled;    

    WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO         m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO      m_ck;          // Multimedia RIFF chunk
    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
    DWORD         m_dwSize;      // The size of the wave file
    MMIOINFO      m_mmioinfoOut;
    ULONG         m_ulDataSize;
};
//------------------------------------------------------------------------------
#endif
    
