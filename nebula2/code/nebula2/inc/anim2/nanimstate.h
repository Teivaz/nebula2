#ifndef N_ANIMSTATE_H
#define N_ANIMSTATE_H
//------------------------------------------------------------------------------
/**
    @class nAnimState
    @ingroup NebulaAnimationSystem

    An animation state contains any number of nAnimClip objects of identical
    size (number of anim curves contained in the clip) and can sample a 
    weight-blended result from them. Each anim clip is associated with a weight 
    value between 0 and 1 which defines how much that anim clip influences 
    the resulting animation.
    
    An nAnimation object connected to the nAnimState object delivers
    the actual animation data.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "anim2/nanimclip.h"
#include "variable/nvariablecontext.h"
#include "mathlib/quaternion.h"

//------------------------------------------------------------------------------
class nAnimState
{
public:
    /// constructor
    nAnimState();
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

private:
    nArray<nAnimClip> clipArray;
    int animGroupIndex;
    float fadeInTime;
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
/**
    Sample the complete animation state at a given time into the
    provided float4 array. The size of the provided key array must be
    equal to the number of curves in any anim clip in the state (all clips
    must have the same number of curves).

    @param  time            the time at which to sample
    @param  animSource      the nAnimation object which provides the anim data
    @param  varContext      a variable context containing the clip weights
    @param  keyArray        pointer to a float4 array which will be filled with 
                            the sampled values (one per curve)
    @param  keyArraySize    number of elements in the key array, must be identical 
                            to the number of curves in any anim clip
    @return                 true, if the returned keys are valid (false if all
                            clip weights are zero)
*/
inline
bool
nAnimState::Sample(float time, 
                   nAnimation* animSource, 
                   nVariableContext* varContext, 
                   vector4* keyArray, 
                   vector4* scratchKeyArray,
                   int keyArraySize)
{
    n_assert(animSource);
    n_assert(keyArray);
    n_assert(keyArraySize >= this->GetClipAt(0).GetNumCurves());
    n_assert(varContext);

    // first, read the clip weights from the variable context and
    // sum them up, so that we can normalize the clip weights later
    int clipIndex;
    int numClips = this->GetNumClips();
    float weightSum = 0.0f;
    for (clipIndex = 0; clipIndex < numClips; clipIndex++)
    {
        nAnimClip& clip = this->GetClipAt(clipIndex);
        float weight = 0.0f;
        nVariable* var = varContext->GetVariable(clip.GetWeightChannelHandle());
        if (var)
        {
            weight = var->GetFloat();
        }
        clip.SetWeight(weight);
        weightSum += weight;
    }
    if (weightSum > 0.0f)
    {
        // some static helper objects
        static quaternion quatCurrent;
        static quaternion quatAccum;
        static quaternion quatSlerp;

        // for each clip...
        bool firstIter = true;
        float weightAccum = 0.0f;
        for (clipIndex = 0; clipIndex < numClips; clipIndex++)
        {
            nAnimClip& clip  = this->GetClipAt(clipIndex);
            float clipWeight = clip.GetWeight() / weightSum;

            // only process clip if its weight is greater 0
            if (clipWeight > 0.0f)
            {
                // for each curve in the clip...
                int numCurves = clip.GetNumCurves();
                int firstCurveIndex = clip.GetFirstCurveIndex();

                // obtain sampled curve value for the clip's anim curve range
                animSource->SampleCurves(time, this->animGroupIndex, firstCurveIndex, numCurves, scratchKeyArray);

                int curveIndex;
                for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
                {
                    int absCurveIndex = firstCurveIndex + curveIndex;
                    vector4& curArrayKey  = keyArray[curveIndex];
                    const vector4& curSampleKey = scratchKeyArray[curveIndex];

                    // perform weighted blending
                    nAnimation::Curve& animCurve = animSource->GetGroupAt(this->animGroupIndex).GetCurveAt(absCurveIndex);
                    if (animCurve.GetIpolType() == nAnimation::Curve::Quat)
                    {
                        // for quaternions, blending is a bit tricky...
                        if (firstIter)
                        {
                            // first time init of accumulators
                            curArrayKey = curSampleKey;
                        }
                        else
                        {
                            // FIXME: all those vector4/quaternion transfers are inefficient
                            quatCurrent.set(curSampleKey.x, curSampleKey.y, curSampleKey.z, curSampleKey.w);
                            quatAccum.set(curArrayKey.x, curArrayKey.y, curArrayKey.z, curArrayKey.w);

                            // scale weightAccum so that 1 == (weightAccum + weight)
                            float scaledWeight = clipWeight / (weightAccum + clipWeight);

                            quatSlerp.slerp(quatAccum, quatCurrent, scaledWeight);
                            curArrayKey.set(quatSlerp.x, quatSlerp.y, quatSlerp.z, quatSlerp.w);
                        }
                    }
                    else
                    {
                        // do normal linear blending
                        if (firstIter)
                        {
                            curArrayKey = curSampleKey * clipWeight;
                        }
                        else
                        {
                            curArrayKey += curSampleKey * clipWeight;
                        }
                    }
                }
                firstIter = false;
                weightAccum += clipWeight;
            }
        }
        return true;
    }
    else
    {
        // sum of weight is zero
        return false;
    }
}

//------------------------------------------------------------------------------
#endif
