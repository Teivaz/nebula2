#ifndef N_DSOUNDSERVER3_H
#define N_DSOUNDSERVER3_H
//------------------------------------------------------------------------------
/**
    @class nDSoundServer3
    @ingroup Audio3
    @brief nAudioServer3 implemention using DirectSound.

    (C) 2003 RadonLabs GmbH
*/
#include "audio3/naudioserver3.h"
#include "audio3/dsutil.h"
#include "kernel/nautoref.h"

class nResourceServer;
class nEnv;

//------------------------------------------------------------------------------
class nDSoundServer3 : public nAudioServer3
{
public:
    /// constructor
    nDSoundServer3();
    /// destructor
    virtual ~nDSoundServer3();
    /// open the audio device
    virtual bool Open();
    /// close the audio device
    virtual void Close();
    /// begin an audio frame
    virtual bool BeginScene(nTime t);
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
    /// create a non-shared sound object
    virtual nSound3* NewSound();
    /// create a shared sound resource object
    virtual nSoundResource* NewSoundResource(const char* rsrcName);
    /// return a pointer to the CSoundManager object
    CSoundManager* GetSoundManager();

protected:
    CSoundManager* soundManager;
    nAutoRef<nEnv> refHwnd;
    IDirectSound3DListener8* dsListener;
    DS3DLISTENER dsListenerProps;
    nTime lastStreamUpdateCheck;
    bool noSoundDevice;
};

//------------------------------------------------------------------------------
#endif
