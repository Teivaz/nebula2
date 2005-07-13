//------------------------------------------------------------------------------
//  nopenalserver.h
//  (C) 2004 Bang, Chang Kyu.
//------------------------------------------------------------------------------
#ifndef N_OPENALSERVER_H
#define N_OPENALSERVER_H

#include "audio3/naudioserver3.h"
#include "kernel/nautoref.h"

#include "al\al.h"
#include "al\alc.h"
#include "al\alut.h"
//#include "al\eax.h"

// OS-selection switches
#ifdef _WIN32
# define WINDOWS
#endif
#if defined(__APPLE__) & defined(__MACH__) // check for OS X
#define MAC_OS_X
#else
#ifdef __MACOS__
# define MACOS
#endif
#endif
#ifdef unix
# ifndef LINUX
# define LINUX
# endif /* LINUX */
#endif

class nResourceServer;
class nEnv;
class nSound3;

#define NUM_BUFFERS 256 // Number of buffers to be Generated

//-----------------------------------------------------------------------------
/**
    @class nOpenALServer
    @ingroup NOpenALContribModule
    @brief
    A sound server based on OpenAL.
*/
class nOpenALServer : public nAudioServer3
{
    static nOpenALServer * m_pSelf;
public:
    /// constructor
    nOpenALServer();
    /// destructor
    virtual ~nOpenALServer();
    /// open the audio device
    virtual bool Open();
    /// close the audio device
    virtual void Close();
    /// begin an audio frame
    virtual bool BeginScene(nTime time);
    /// update listener attributes
    virtual void UpdateListener(const nListener3& l);
    /// start a sound
    virtual void StartSound(nSound3* s);
    /// update a sound
    virtual void UpdateSound(nSound3* s);
    /// stop a sound
    virtual void StopSound(nSound3* s);
    /// end the audio frame
    virtual void EndScene();

    static nOpenALServer * Instance();

    /// create a non-shared sound object
    virtual nSound3* NewSound();
    /// create a shared sound resource object
    virtual nSoundResource* NewSoundResource(const char* rsrcName);

    bool nOpenALServer::SaveCmds (nPersistServer* ps);

    unsigned int getNextBuffer() { return nextBuffer; }
    unsigned int getNextSource() { return nextSource; }
    unsigned int getBuffer() { return buffer[nextBuffer]; }
    unsigned int *getBufferPtr() { return &buffer[nextBuffer]; }
    unsigned int getBuffer(int index) { return buffer[index]; }
    unsigned int *getBufferPtr(int index) { return &buffer[index]; }
    unsigned int getSource() { return source[nextSource]; }
    unsigned int *getSourcePtr() { return &source[nextSource]; }
    unsigned int getSource(int index) { return source[index]; }
    unsigned int *getSourcePtr(int index) { return &source[index]; }
    void incNextBuffer() { nextBuffer++; }
    void incNextSource() { nextSource++; }
    bool createBufSrc();

protected:

    friend class nOpenALResource;

    void get_info();
    void check_alc_error();
    void check_al_error();
    void check_ov_error(const nString&, const int&);

    nAutoRef<nResourceServer> refResourceServer;
    nAutoRef<nEnv> refHwnd;
    nTime lastStreamUpdateCheck;

    ALCdevice * m_pDevice;

    ALuint buffer[256];
    unsigned int source[256];
    unsigned int nextBuffer, nextSource;
    //unsigned int EAXlevel;
};

inline
nOpenALServer*
nOpenALServer::Instance()
{
    n_assert(m_pSelf);
    return m_pSelf;
}

#endif /*N_OPENALSERVER_H*/
