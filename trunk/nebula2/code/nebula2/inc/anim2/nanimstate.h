#ifndef N_ANIMSTATE_H
#define N_ANIMSTATE_H
//------------------------------------------------------------------------------
/**
    @class nAnimState
    @ingroup Anim2

    @brief An animation state contains any number of nAnimClip objects of
    identical size (number of anim curves contained in the clip) and can
    sample a weight-blended result from them. Each anim clip is associated
    with a weight value between 0 and 1 which defines how much that anim
    clip influences the resulting animation.

    An nAnimation object connected to the nAnimState object delivers
    the actual animation data.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "anim2/nanimclip.h"
#include "variable/nvariablecontext.h"
#include "mathlib/quaternion.h"
#include "anim2/nanimeventhandler.h"

//------------------------------------------------------------------------------
class nAnimState
{
public:
    /// constructor
    nAnimState();
    /// set state name
    void SetName(const nString& n);
    /// get state name
    const nString& GetName() const;
    /// set index of animation group in animation resource file
    void SetAnimGroupIndex(int index);
    /// get animation group index
    int GetAnimGroupIndex() const;
    /// set the fadein time
    void SetFadeInTime(float t);
    /// get the fadein time
    float GetFadeInTime() const;
    /// begin defining animation clips
    void BeginClips(int num);
    /// set an animation clip
    void SetClip(int index, const nAnimClip& clip);
    /// finish defining animation clips
    void EndClips();
    /// get number of animation clips
    int GetNumClips() const;
    /// get anim clip at index
    nAnimClip& GetClipAt(int index) const;
    /// sample weighted values at a given time from nAnimation object
    bool Sample(float time, nAnimation* animSource, nVariableContext* varContext, vector4* keyArray, vector4* scratchKeyArray, int keyArraySize);
    /// emit animation events for a given time range
    void EmitAnimEvents(float fromTime, float toTime, nAnimation* animSource, nAnimEventHandler* handler);

private:
    /// begin defining blended animation events
    void BeginEmitEvents();
    /// add a blended animation event
    void AddEmitEvent(const nAnimEventTrack& track, const nAnimEvent& event, float weight);
    /// finish defining blended anim events, emit the events
    void EndEmitEvents(nAnimEventHandler* handler);

    nArray<nAnimClip> clipArray;
    nString name;
    int animGroupIndex;
    float fadeInTime;
    nArray<nAnimEventTrack> outAnimEventTracks;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimState::nAnimState() :
    clipArray(0, 0),
    animGroupIndex(0),
    fadeInTime(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nAnimState::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetAnimGroupIndex(int index)
{
    this->animGroupIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimState::GetAnimGroupIndex() const
{
    return this->animGroupIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetFadeInTime(float t)
{
    this->fadeInTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimState::GetFadeInTime() const
{
    return this->fadeInTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::BeginClips(int num)
{
    this->clipArray.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetClip(int index, const nAnimClip& clip)
{
    // ensure identical number of curves in clips
    if (index > 0)
    {
        n_assert(this->clipArray[0].GetNumCurves() == clip.GetNumCurves());
    }
    this->clipArray[index] = clip;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::EndClips()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimState::GetNumClips() const
{
    return this->clipArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimClip&
nAnimState::GetClipAt(int index) const
{
    return this->clipArray[index];
}

//------------------------------------------------------------------------------
#endif
