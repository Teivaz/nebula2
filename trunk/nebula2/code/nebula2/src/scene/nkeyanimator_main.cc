//------------------------------------------------------------------------------
//  nkeyanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nkeyanimator.h"

nNebulaClass(nKeyAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nKeyAnimator::nKeyAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Compute an interpolated key value from a time and a key array.

    @param  sampleTime  [in] time position at which to sample
    @param  keyArray    [in] reference of a key array containing the keys
    @param  result      [out] the result
    @return             true if the result is valid, false otherwise
*/
bool
nKeyAnimator::SampleKey(float sampleTime, const nArray<Key>& keyArray, vector4& result)
{
    if (keyArray.Size() > 1)
    {
        float minTime = keyArray.Front().time;
        float maxTime = keyArray.Back().time;
        if (maxTime > 0.0f)
        {
            if (this->GetLoopType() == Loop)
            {
                // in loop mode, wrap time into loop time
                sampleTime = sampleTime - (float(floor(sampleTime / maxTime)) * maxTime);
            }

            // clamp time to range
            if (sampleTime < minTime)       sampleTime = minTime;
            else if (sampleTime >= maxTime) sampleTime = maxTime - 0.001f;

            // find the surrounding keys
            n_assert(keyArray.Front().time == 0.0f);
            int i = 0;;
            while (keyArray[i].time <= sampleTime)
            {
                i++;
            }
            n_assert((i > 0) && (i < keyArray.Size()));

            const Key& key0 = keyArray[i - 1];
            const Key& key1 = keyArray[i];
            float time0 = key0.time;
            float time1 = key1.time;

            // compute the actual interpolated values
            float lerp;
            if (time1 > time0) lerp = (float) ((sampleTime - time0) / (time1 - time0));
            else               lerp = 1.0f;
            result = key0.value + ((key1.value - key0.value) * lerp);
            return true;
        }
    }
    return false;
}

