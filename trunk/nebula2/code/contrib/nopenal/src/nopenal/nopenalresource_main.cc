//------------------------------------------------------------------------------
//  nopenalserver_main.cc
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#include "nopenal/nopenalserver.h"
#include "kernel/nfileserver2.h"
#include "nopenal/nopenalresource.h"
#include <sys/stat.h>

nNebulaClass(nOpenALResource, "nsoundresource");

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

    nPathString dstFileName (mangledPath.Get());

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
        alutLoadWAVFile((ALbyte *)dstFileName.Get(),&format,&data,&size,&freq,&loop);
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
        if (alIsExtensionPresent((ALubyte *)"AL_EXT_vorbis") == AL_FALSE)
        {
            n_message( "error No supported vorbis\n" );
            return false;
        }

        void *ovData;   
        unsigned int ovSize;

        FILE *fh;
        fh = fopen(dstFileName.Get(), "rb");
        if (fh != NULL)
        {
            struct stat sbuf;
            if (stat(dstFileName.Get(), &sbuf) != -1)
            {
                ovSize = sbuf.st_size;
                ovData = malloc(ovSize);

                if (ovData != NULL)
                {
                    fread(ovData, 1, ovSize, fh);

                    alBufferData(buffer, AL_FORMAT_VORBIS_EXT, ovData, ovSize, 22050);
                    if ((error = alGetError()) != AL_NO_ERROR)
                    {
                        free(ovData);
                        fclose(fh);
                        n_message("alBufferData buffer 0 : %s\n", alGetString(error));
                        //// Delete Buffers
                        //alDeleteBuffers(NUM_BUFFERS, g_Buffers);
                        return false;
                    }
                    free(ovData);
                }
            }
            fclose(fh);
        }
    }
    else
    {
        n_message( "unsurpported format\n" );
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
    alSourcef(source, AL_REFERENCE_DISTANCE, 10);

    m_handle = this->refSoundServer->getNextBuffer();

    this->refSoundServer->incNextBuffer();
    this->refSoundServer->incNextSource();

    this->SetValid(true);
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
    this->SetValid(false);
}
