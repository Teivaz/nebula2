//------------------------------------------------------------------------------
//  nwavfile.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "audio3/nwavfile.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

bool nWavFile::mmioProcInstalled = false;

//------------------------------------------------------------------------------
/**
    This is a custom file access hook for the Win32 MMIO subsystem. It
    redirects all file accesses to the Nebula2 file server, and thus
    makes audio files inside an NPK archive accessible for the MMIO subsystem
*/

LRESULT CALLBACK
mmioProc(LPSTR lpstr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
    MMIOINFO* lpMMIOInfo = (MMIOINFO*)lpstr;

    n_assert(0 != lpMMIOInfo);
    n_assert(mmioFOURCC('N','E','B','2') == lpMMIOInfo->fccIOProc);
    n_assert(mmioProc == lpMMIOInfo->pIOProc);

    switch (uMsg)
    {
        case MMIOM_OPEN:
            {
                if (0 == (lpMMIOInfo->dwFlags & (MMIO_WRITE | MMIO_READWRITE)))
                {
                    // strip the "X.NEB2+" prefix from the file
                    nString rawName((const char*) lParam1);
                    nString filename = rawName.ExtractRange(7, rawName.Length() - 7);

                    // open a Nebula2 file for reading
                    nFile* file = nFileServer2::Instance()->NewFileObject();
                    if (file->Open(filename.Get(), "rb"))
                    {
                        // store nebula file object in mmio struct
                        lpMMIOInfo->adwInfo[0] = (DWORD) file;
                        return MMSYSERR_NOERROR;
                    }
                    else
                    {
                        file->Release();
                    }
                }

                // fallthrough: some error occured
                return MMIOERR_CANNOTOPEN;
            }

        case MMIOM_CLOSE:
            {
                nFile* file = (nFile*) lpMMIOInfo->adwInfo[0];
                n_assert(0 != file);

                file->Close();
                file->Release();
                lpMMIOInfo->adwInfo[0] = 0;

                return 0;
            }

        case MMIOM_READ:
            {
                nFile* file = (nFile*) lpMMIOInfo->adwInfo[0];
                n_assert(0 != file && file->IsOpen());

                int bytesRead = file->Read((HPSTR)lParam1, (LONG)lParam2);
                lpMMIOInfo->lDiskOffset += bytesRead;

                return bytesRead;
            }

        case MMIOM_SEEK:
            {
                nFile* file = (nFile*) lpMMIOInfo->adwInfo[0];
                n_assert(0 != file && file->IsOpen());

                nFile::nSeekType seekType;
                switch ((int)lParam2)
                {
                    case SEEK_CUR: seekType = nFile::CURRENT; break;
                    case SEEK_END: seekType = nFile::END; break;
                    default:       seekType = nFile::START; break;
                }

                if (file->Seek((LONG)lParam1, seekType))
                {
                    lpMMIOInfo->lDiskOffset = file->Tell();
                    return lpMMIOInfo->lDiskOffset;
                }
                else
                {
                    return -1;
                }
            }

        case MMIOM_WRITE:
        case MMIOM_WRITEFLUSH:
            // not supported
            return -1;
    }

    // message not recognized
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nWavFile::nWavFile() :
    m_pwfx(0),
    m_hmmio(NULL),
    m_dwSize(0)
{
    // install MMIO custom file routines?
    if (!mmioProcInstalled)
    {
        LPMMIOPROC p = mmioInstallIOProc(mmioFOURCC('N','E','B','2'), mmioProc, MMIO_INSTALLPROC);
        n_assert(p);
        mmioProcInstalled = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
nWavFile::~nWavFile()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nWavFile::Open(const nString& filename)
{
    n_assert(!this->IsOpen());
    n_assert(filename.IsValid());

    // modify filename so that MMIO will invoke our custom file function
    nString mangledPath = nFileServer2::Instance()->ManglePath(filename);
    nString mmioFilename("X.NEB2+");
    mmioFilename.Append(mangledPath);

    // open the file
    this->m_hmmio = mmioOpen((LPSTR)mmioFilename.Get(), NULL, MMIO_ALLOCBUF | MMIO_READ);
    if (NULL == this->m_hmmio)
    {
        n_error("nWavFile::Open(): failed to open file '%s'!", filename.Get());
        return false;
    }

    // check if it's a wav file
    if (!this->ReadMMIO())
    {
        mmioClose(this->m_hmmio, 0);
        n_error("nWavFile::Open(): not a wav file (%s)!", filename.Get());
        return false;
    }

    // reset the file to the beginning of data
    this->Reset();

    // store size of file (valid after ResetFile())
    this->m_dwSize = this->m_ck.cksize;

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nWavFile::Close()
{
    n_assert(this->IsOpen());
    n_assert(0 != this->m_hmmio);

    n_delete(this->m_pwfx);
    this->m_pwfx = 0;

    mmioClose(this->m_hmmio, 0);
    this->m_hmmio = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
int
nWavFile::GetSize() const
{
    n_assert(this->IsOpen());
    return this->m_dwSize;
}

//------------------------------------------------------------------------------
/**
*/
WAVEFORMATEX*
nWavFile::GetFormat() const
{
    return this->m_pwfx;
}

//------------------------------------------------------------------------------
/**
    This private method reads the WAV file header data and other wav specific
    stuff.
*/
bool
nWavFile::ReadMMIO()
{
    n_assert(this->m_hmmio);

    MMCKINFO ckIn;
    PCMWAVEFORMAT pcmWaveFormat;

    this->m_pwfx = NULL;

    // read next chunk
    if (0 != mmioDescend(this->m_hmmio, &this->m_ckRiff, NULL, 0))
    {
        return false;
    }

    // check for valid wav file
    if ((this->m_ckRiff.ckid != FOURCC_RIFF) ||
        (this->m_ckRiff.fccType != mmioFOURCC('W','A','V','E')))
    {
        return false;
    }

    // search the fmt chunk
    ckIn.ckid = mmioFOURCC('f','m','t',' ');
    if (0 != mmioDescend(this->m_hmmio, &ckIn, &this->m_ckRiff, MMIO_FINDCHUNK))
    {
        return false;
    }

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
    if (ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT))
    {
        return false;
    }

    // read fmt chunk into pcmWaveFormat
    if (mmioRead(this->m_hmmio, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
    {
        return false;
    }

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
    {
        this->m_pwfx = (WAVEFORMATEX*) n_new(char[sizeof(WAVEFORMATEX)]);
        n_assert(this->m_pwfx);

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy(this->m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
        this->m_pwfx->cbSize = 0;
    }
    else
    {
        // read in length of extra bytes
        WORD cbExtraBytes = 0L;
        if (mmioRead(m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD))
        {
            return false;
        }

        this->m_pwfx = (WAVEFORMATEX*) n_new(char[sizeof(WAVEFORMATEX) + cbExtraBytes]);
        n_assert(this->m_pwfx);

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy(this->m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
        this->m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if (mmioRead(this->m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize))+sizeof(WORD)), cbExtraBytes) != cbExtraBytes)
        {
            n_delete(this->m_pwfx);
            this->m_pwfx = 0;
            return false;
        }
    }

    // ascend the input file out of the 'fmt ' chunk
    if (0 != mmioAscend(this->m_hmmio, &ckIn, 0))
    {
        n_delete(this->m_pwfx);
        this->m_pwfx = 0;
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Resets the file pointer to the beginning of the audio data.
*/
bool
nWavFile::Reset()
{
    n_assert(this->m_hmmio);

    // seek to the data
    if (-1 == mmioSeek(this->m_hmmio, this->m_ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET))
    {
        return false;
    }

    // search for 'data' chunk
    this->m_ck.ckid = mmioFOURCC('d','a','t','a');
    if (0 != mmioDescend(this->m_hmmio, &this->m_ck, &this->m_ckRiff, MMIO_FINDCHUNK))
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Reads section of data from a wave file into pBuffer and returns
    how much read in pdwSizeRead, reading not more than dwSizeToRead.
    This uses m_ck to determine where to start reading from.  So
    subsequent calls will be continue where the last left off unless
    Reset() is called.
*/
uint
nWavFile::Read(void* buffer, uint bytesToRead)
{
    n_assert(this->IsOpen());
    n_assert(this->m_hmmio);
    n_assert(buffer);
    n_assert(bytesToRead > 0);

    MMIOINFO mmioInfoIn;    // current status of m_hmmio
    mmioGetInfo(this->m_hmmio, &mmioInfoIn, 0);

    uint cbDataIn = bytesToRead;
    if (cbDataIn > this->m_ck.cksize)
    {
        cbDataIn = this->m_ck.cksize;
    }
    this->m_ck.cksize -= cbDataIn;

    DWORD cT;
    for (cT = 0; cT < cbDataIn; cT++)
    {
        // read next chunk if necessary
        if (mmioInfoIn.pchNext == mmioInfoIn.pchEndRead)
        {
            mmioAdvance(this->m_hmmio, &mmioInfoIn, MMIO_READ);
        }

        // copy to buffer
        *((uchar*)buffer+cT) = *((uchar*)mmioInfoIn.pchNext);
        mmioInfoIn.pchNext++;
    }
    mmioSetInfo(this->m_hmmio, &mmioInfoIn, 0);
    return cbDataIn;
}






