#ifndef N_AUDIOSERVER3_H
#define N_AUDIOSERVER3_H
//------------------------------------------------------------------------------
/**
    @class nAudioServer3

    Audio subsystem 3 server.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nSound3;
class nSoundResource;
class nListener3;

class nAudioServer3 : public nRoot
{
public:
    /// constructor
    nAudioServer3();
    /// destructor
    virtual ~nAudioServer3();

    /// open the audio device
    virtual bool Open();
    /// close the audio device
    virtual void Close();
    /// begin an audio frame
    virtual bool BeginScene();
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

protected:
    bool isOpen;
    bool inBeginScene;
};

//------------------------------------------------------------------------------
#endif    

