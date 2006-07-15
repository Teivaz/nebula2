#ifndef N_AUDIOOBJECT_H
#define N_AUDIOOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nAudioObject
    @ingroup Application

    @brief A wrapper object for Nebula2 sounds.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

class nSound3;

//------------------------------------------------------------------------------
class nAudioObject
{
public:
    /// constructor
    nAudioObject();
    /// destructor
    ~nAudioObject();
    /// set resource name
    void SetName(const nString& n);
    /// get resource name
    const nString& GetName() const;
    /// load associated resources
    bool Load();
    /// unload associated resources
    void Unload();
    /// return true if currently valid
    bool IsValid();
    /// set current position
    void SetPosition(const vector3& p);
    /// get current position
    const vector3& GetPosition() const;
    /// start playback
    void Start();
    /// stop playback
    void Stop();
    /// return true if currently playing
    bool IsPlaying() const;

private:
    /// validate the sound object
    void Validate();

    nString name;
    vector3 position;
    bool posDirty;
    nRef<nSound3> refSound;
};

//------------------------------------------------------------------------------
#endif

