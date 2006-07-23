#ifndef N_AUDIOFILE_H
#define N_AUDIOFILE_H
//------------------------------------------------------------------------------
/**
    @class nAudioFile
    @ingroup Audio3
    @brief A generic base class for read access to audio files. Derive subclass
    for specific file formats/codecs.

    (C) 2005 Radon Labs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>

//------------------------------------------------------------------------------
class nAudioFile
{
public:
    /// constructor
    nAudioFile();
    /// destructor
    virtual ~nAudioFile();
    /// open the file
    virtual bool Open(const nString& filename);
    /// close the file
    virtual void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// read bytes from file
    virtual uint Read(void* buffer, uint bytesToRead);
    /// reset to beginning of audio data
    virtual bool Reset();
    /// return size of audio data in file in bytes
    virtual int GetSize() const;
    /// return pointer to format description
    virtual WAVEFORMATEX* GetFormat() const;

protected:
    bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAudioFile::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
#endif
