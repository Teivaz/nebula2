//------------------------------------------------------------------------------
//  naudiofile.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "audio3/naudiofile.h"

//------------------------------------------------------------------------------
/**
*/
nAudioFile::nAudioFile() :
    isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAudioFile::~nAudioFile()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
    Open the file for reading. The method should open the file and read
    the file's header data.
*/
bool
nAudioFile::Open(const nString& filename)
{
    n_assert(!this->IsOpen());
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the file.
*/
void
nAudioFile::Close()
{
    n_assert(this->IsOpen());
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Read data from file. Returns the number of bytes actually read. If decoding
    happens inside Read() the number of bytes to read and number of bytes actually
    read means "decoded data". The method should wrap around if the end of
    the data is encountered.
*/
uint
nAudioFile::Read(void* buffer, uint bytesToRead)
{
    n_assert(this->IsOpen());
    return 0;
}

//------------------------------------------------------------------------------
/**
    Reset the object to the beginning of the audio data.
*/
bool
nAudioFile::Reset()
{
    n_assert(this->IsOpen());
    return true;
}

//------------------------------------------------------------------------------
/**
    This method returns the (decoded) size of audio data in the file in bytes.
*/
int
nAudioFile::GetSize() const
{
    n_assert(this->IsOpen());
    return 0;
}

//------------------------------------------------------------------------------
/**
    This method returns a pointer to a WAVEFORMATEX structure which describes
    the format of the audio data in the file.
*/
WAVEFORMATEX*
nAudioFile::GetFormat() const
{
    n_error("nAudioFile::GetFormat(): not implemented!");
    return 0;
}
