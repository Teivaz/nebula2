#ifndef N_ANIMCLIP_H
#define N_ANIMCLIP_H
//------------------------------------------------------------------------------
/**
    @class nAnimClip
    @ingroup Anim2

    @brief An animation clip bundles several animation curves into a unit
    and associates them with a weight value.

    (C) 2003 RadonLabs GmbH
*/
#include "anim2/nanimation.h"
#include "variable/nvariable.h"
#include "anim2/nanimeventtrack.h"

//------------------------------------------------------------------------------
class nAnimClip
{
public:
    /// constructor
    nAnimClip();
    /// constructor
    nAnimClip(int firstAnimCurveIndex, int numAnimCurves, nVariable::Handle weightChnHandle);
    /// get number of anim curves in the clip
    int GetNumCurves() const;
    /// get the index of the first anim curve in the animation object's group
    int GetFirstCurveIndex() const;
    /// get the weight channel's variable handle
    nVariable::Handle GetWeightChannelHandle() const;
    /// set current weight value
    void SetWeight(float w);
    /// get current weight value
    float GetWeight() const;
    /// set number of animation event tracks
    void SetNumAnimEventTracks(int num);
    /// get number of animation event tracks
    int GetNumAnimEventTracks() const;
    /// read/write access to animation event track
    nAnimEventTrack& GetAnimEventTrackAt(int index);
    /// access to anim event tracks array
    nFixedArray<nAnimEventTrack>& AnimEventTracks();

private:
    int numCurves;
    int firstCurveIndex;
    float weight;
    nVariable::Handle weightChannelHandle;
    nFixedArray<nAnimEventTrack> animEventTracks;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimClip::nAnimClip() :
    numCurves(0),
    firstCurveIndex(0),
    weight(0.0f),
    weightChannelHandle(nVariable::InvalidHandle)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimClip::nAnimClip(int firstAnimCurveIndex, int numAnimCurves, nVariable::Handle weightChnHandle) :
    numCurves(numAnimCurves),
    firstCurveIndex(firstAnimCurveIndex),
    weight(0.0f),
    weightChannelHandle(weightChnHandle)
{
    n_assert(numAnimCurves >= 0);
    n_assert(firstAnimCurveIndex >= 0);
    n_assert(weightChnHandle != nVariable::InvalidHandle);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimClip::GetNumCurves() const
{
    return this->numCurves;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimClip::GetFirstCurveIndex() const
{
    return this->firstCurveIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::Handle
nAnimClip::GetWeightChannelHandle() const
{
    return this->weightChannelHandle;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimClip::SetWeight(float w)
{
    this->weight = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimClip::GetWeight() const
{
    return this->weight;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimClip::SetNumAnimEventTracks(int num)
{
    this->animEventTracks.SetSize(num);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimClip::GetNumAnimEventTracks() const
{
    return this->animEventTracks.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimEventTrack&
nAnimClip::GetAnimEventTrackAt(int index)
{
    return this->animEventTracks[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
nFixedArray<nAnimEventTrack>&
nAnimClip::AnimEventTracks()
{
    return this->animEventTracks;
}

//------------------------------------------------------------------------------
#endif
