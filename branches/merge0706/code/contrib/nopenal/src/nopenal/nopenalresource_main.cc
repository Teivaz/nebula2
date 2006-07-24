//------------------------------------------------------------------------------
//  nopenalserver_main.cc
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenal/nopenalserver.h"
#include "kernel/nfileserver2.h"

#include "kernel/nfile.h"

#include "nopenal/nopenalresource.h"
#include "util/narray.h"

//#include <sys/stat.h>

nNebulaClass(nOpenALResource, "nsoundresource");


//-----------------------------------------------------------
//    ov_callback specific functions
//-----------------------------------------------------------

size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    nFile* fp = reinterpret_cast<nFile*>(datasource);
    return fp->Read((char *)ptr, size * nmemb);
}

int SeekOgg(void *datasource, ogg_int64_t offset, int whence)
{
    nFile* fp = reinterpret_cast<nFile*>(datasource);
    nFile::nSeekType dir;
    switch (whence)
	{
    case SEEK_SET:
        dir = nFile::nSeekType::START;
        break;
	case SEEK_CUR:
        dir = nFile::nSeekType::CURRENT;
        break;
	case SEEK_END:
        dir = nFile::nSeekType::END;
        break;
	default: return -1;
	}
    int ret = fp->Seek(offset, dir) ? 0 : -1;
	return ret;
}

long TellOgg(void *datasource)
{
	nFile* fp = reinterpret_cast<nFile*>(datasource);
	return fp->Tell();
}

int CloseOgg(void *datasource)
{
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
nOpenALResource::nOpenALResource() :
        refSoundServer("/sys/servers/audio"),
        m_handle(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nOpenALResource::~nOpenALResource()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Load the sound resource.
*/
bool
nOpenALResource::LoadResource()
{
    n_assert(!this->IsValid());

    nFileServer2* fileServer = kernelServer->GetFileServer();

    // get mangled path name
    nString mangledPath = fileServer->ManglePath(this->GetFilename().Get());

    n_assert(this->refSoundServer->createBufSrc());

    unsigned int nextBuffer = this->refSoundServer->getNextBuffer();
    unsigned int nextSource = this->refSoundServer->getNextSource();
    unsigned int buffer = this->refSoundServer->getBuffer();
    unsigned int source = this->refSoundServer->getSource();

    nString dstFileName (mangledPath.Get());

   // load data into buffer
#ifdef WINDOWS
    int error;

    if( dstFileName.GetExtension() == nString("wav") )
    {
        ALsizei size, freq;
        ALenum format;
        ALvoid *data;
        ALboolean loop;

        // Load xxx.wav
        alutLoadWAVFile((ALbyte*)dstFileName.Get(),&format,&data,&size,&freq,&loop);
        if ((error = alGetError()) != AL_NO_ERROR)
        {
            n_message("alutLoadWAVFile %s : %s\n", dstFileName.Get(), alGetString(error));
            //// Delete Buffers
            //alDeleteBuffers(NUM_BUFFERS, g_Buffers);
            return false;
        }

        // Copy wav data into AL Buffer 0
        alBufferData(buffer,format,data,size,freq);
        if ((error = alGetError()) != AL_NO_ERROR)
        {
            n_message("alBufferData buffer %s : %s\n", dstFileName.Get(), alGetString(error));
            //// Delete Buffers
            //alDeleteBuffers(NUM_BUFFERS, g_Buffers);
            return false;
        }

        // Unload wav
        alutUnloadWAV(format,data,size,freq);
        if ((error = alGetError()) != AL_NO_ERROR)
        {
            n_message("alutUnloadWAV : %s\n", alGetString(error));
            //// Delete Buffers
            //alDeleteBuffers(NUM_BUFFERS, g_Buffers);
            return false;
        }
    }
    else if( dstFileName.GetExtension() == nString("ogg") )
    {
        this->load_ogg_file(dstFileName, buffer);
    }
    else
    {
        n_message( "Unknown format: '%s'\n", dstFileName.GetExtension() );
        return false;
    }
#endif
#if defined(MACOS) | defined(MAC_OS_X)
#endif
#ifdef LINUX
#endif

    // set static source properties
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, this->GetLooping());
    alSourcef(source, AL_REFERENCE_DISTANCE, 1);
    alSourcef(source, AL_GAIN, 1.0f);
    alSourcef(source, AL_PITCH, 1.0f);

    m_handle = this->refSoundServer->getNextBuffer();

    this->refSoundServer->incNextBuffer();
    this->refSoundServer->incNextSource();

    //this->SetValid(true);
    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload everything.
*/
void
nOpenALResource::UnloadResource()
{
    n_assert(this->IsValid());
    //this->SetValid(false);
    this->SetState(Unloaded);
}

void
nOpenALResource::load_ogg_file(const nString& filename, const unsigned int& buffer)
{
    // Open Ogg file
	nFile* fp = nFileServer2::Instance()->NewFileObject();
    if (!fp->Open(filename.Get(), "rb"))
    {
        n_error("nOpenALResource unable to load: %s\n", filename.Get());
    }

    // OggVorbis specific structures
	ov_callbacks  cb;
	cb.close_func = CloseOgg;
	cb.read_func  = ReadOgg;
	cb.seek_func  = SeekOgg;
	cb.tell_func  = TellOgg;

    OggVorbis_File vf;
    nOpenALServer::Instance()->check_ov_error("ov_open_callbacks() said", ov_open_callbacks(fp, &vf, 0, -1, cb));

    read_ogg_block(buffer, vf);

    nOpenALServer::Instance()->check_ov_error("ov_clear() said", ov_clear(&vf));
}

void
nOpenALResource::read_ogg_block(const unsigned int& buffer, OggVorbis_File& vf)
{
    int blockSize = ov_pcm_total(&vf, -1);
    nOpenALServer::Instance()->check_ov_error("ov_pcm_total() said", blockSize);
    blockSize *= 2;

    // vars
	int	  current_section = 0;
	long  TotalRet        = 0;
    long  ret             = 0;
	char* buf             = new char[blockSize];
    const char* pbuf      = buf;

	// Read loop
	while (TotalRet < blockSize)
	{
		ret = ov_read(&vf, buf + TotalRet, blockSize - TotalRet, 0, 2, 1, &current_section);

		// if end of file or read limit exceeded
		if (ret == 0) break;
		else if (ret < 0) 		// Error in bitstream
		{
			//
		}
		else
		{
			TotalRet += ret;
		}
	}

	vorbis_info* pInfo = ov_info(&vf, -1);
    n_assert(pInfo);

    ALenum format;
    if (pInfo->channels == 1)
    {
        format = AL_FORMAT_MONO16;
    }
    else
    {
        format = AL_FORMAT_STEREO16;
    }

    ALsizei freq = pInfo->rate;
    alBufferData(buffer, format, (void*)pbuf, blockSize, freq);
    nOpenALServer::Instance()->check_al_error();

    delete [] buf;
}










