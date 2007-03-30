//-----------------------------------------------------------------------------
//  nmaxcontrol.cc
//
//  (C)2005 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxtransform.h"
#include "export2/nmaxutil.h"
#include "export2/nmaxoptions.h"

#include "kernel/ntypes.h"
#include "mathlib/vector.h"


const float nMaxControl::key_tolerance = TINY * 5;
//-----------------------------------------------------------------------------
/**
*/
nMaxControl::nMaxControl()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxControl::~nMaxControl()
{
}

//-----------------------------------------------------------------------------
/**
    Samples the node TM controllers and specifies retrieved keys to the given 
    sampleKeyArray.

    - 21-Feb-05 kims Fixed transform scale.
*/
void nMaxControl::GetSampledKey(INode* inode, nArray<nMaxSampleKey> & sampleKeyArray, 
                                    int sampleRate, nMaxControlType type, bool optimize)
{
    if (type != nMaxTM  && 
        type != nMaxPos && 
        type != nMaxRot && 
        type != nMaxScale)
    {
        return;
    }

    TimeValue t;
    TimeValue start = nMaxInterface::Instance()->GetAnimStartTime();
    TimeValue end   = nMaxInterface::Instance()->GetAnimEndTime();

    int  delta = GetTicksPerFrame() * sampleRate;

    int numKeys = 0;

    for (t=start; t<end; t+=delta, numKeys++)
    {
        nMaxSampleKey sampleKey;

        sampleKey.tm = nMaxTransform::GetLocalTM(inode, t);

        // transform scale
        float scale = nMaxOptions::Instance()->GetGeomScaleValue();
        if (scale != 0.0f)
        {
            Point3 scaleVal(scale, scale, scale);

            Matrix3 scaleTM;
            scaleTM.IdentityMatrix();
            scaleTM.Scale(scaleVal);

            sampleKey.tm = sampleKey.tm * scaleTM;
        }

        AffineParts ap;

        decomp_affine(sampleKey.tm, &ap );

        if (type == nMaxPos || type == nMaxTM)
            sampleKey.pos   = ap.t;

        if (type == nMaxRot || type == nMaxTM)
            sampleKey.rot   = ap.q;

        if (type == nMaxScale || type == nMaxTM)
            sampleKey.scale = ap.k;

        sampleKey.time = (t - start) * SECONDSPERTICK;
      
        sampleKeyArray.Append(sampleKey);
    }

    // sample last key for exact looping.
    // end key should be inserted to get correct animation.
    {
        t = end;

        nMaxSampleKey sampleKey;
  
        sampleKey.tm = nMaxTransform::GetLocalTM(inode, t);

        // transform scale
        float scale = nMaxOptions::Instance()->GetGeomScaleValue();
        if (scale != 0.0f)
        {
            Point3 scaleVal(scale, scale, scale);

            Matrix3 scaleTM;
            scaleTM.IdentityMatrix();
            scaleTM.Scale(scaleVal);

            sampleKey.tm = sampleKey.tm * scaleTM;
        }

        AffineParts ap;

        decomp_affine(sampleKey.tm, &ap );

        if (type == nMaxPos || type == nMaxTM)
            sampleKey.pos   = ap.t;

        if (type == nMaxRot || type == nMaxTM)
            sampleKey.rot   = ap.q;

        if (type == nMaxScale || type == nMaxTM)
            sampleKey.scale = ap.k;

        sampleKey.time  = (t - start) * SECONDSPERTICK;

        sampleKeyArray.Append(sampleKey);
    }

    if (optimize)
    {
        // remove redundant keys
        nArray<nMaxSampleKey> tmpKeyArray;

        for(int i = 0; i < sampleKeyArray.Size(); ++i)
        {
            if((i == 0) || (i == sampleKeyArray.Size() - 1))
            {
                // first and last keys are important
                tmpKeyArray.Append(sampleKeyArray[i]);
            }
            else
            {
                // current key is important if keys on either side are different to it.
                nMaxSampleKey previousKey = sampleKeyArray[i - 1];
                nMaxSampleKey currentKey = sampleKeyArray[i];
                nMaxSampleKey nextKey = sampleKeyArray[i + 1];
                if(/*!AreKeysEqual(currentKey, previousKey, type) || 
                    !AreKeysEqual(currentKey, nextKey, type) ||*/
                    !AreKeysLerp(currentKey, previousKey, nextKey, type))
                {
                    tmpKeyArray.Append(currentKey);
                }
            }
        }

        sampleKeyArray.Clear();
        sampleKeyArray = tmpKeyArray;
    }
}

//-----------------------------------------------------------------------------
/**
    Samples the float or point3 controllers and specifies retrieved keys to 
    the given sampleKeyArray.

    - 21-Feb-05 kims added optimize routine which remove a key if the key has
                     same value except time value to previous key.
*/
void nMaxControl::GetSampledKey(Control* control, nArray<nMaxSampleKey> & sampleKeyArray, 
                                    int sampleRate, nMaxControlType type, bool optimize)
{
    n_assert(control);
    nArray<TimeValue> sampleTimes;
    TimeValue t;
    TimeValue start = nMaxInterface::Instance()->GetAnimStartTime();
    TimeValue end   = nMaxInterface::Instance()->GetAnimEndTime();
    TimeValue delta = GetTicksPerFrame() * sampleRate;
    int i;

    // create sample times array
    if (control->IsKeyable())
    {
        // Control *pKPC = control->GetPositionController();
        IKeyControl *keyControl = GetKeyControlInterface(control);
        n_assert(keyControl);
        IKey *key = (IKey *)n_malloc(keyControl->GetKeySize());
        int numKeys = keyControl->GetNumKeys();
        size_t lastOutTanType = size_t(-1);
        TimeValue lastKeyTime = start;
        for (i = 0; i < numKeys; i++)
        {
            Interval interv;
            keyControl->GetKey(i, key);

            if( key->time < start)
                continue;

            size_t inTanType = GetInTanType(key->flags);
            switch(lastOutTanType)
            {
            case BEZKEY_SMOOTH:
                sampleTimes.AppendArray(SampleTime(lastKeyTime, key->time, delta));
                break;
            case BEZKEY_LINEAR:
                if (inTanType == BEZKEY_LINEAR) // both linear tangent type, could ignore them
                    break;
                else
                    sampleTimes.AppendArray(SampleTime(lastKeyTime, key->time, delta));
                break;
            case BEZKEY_STEP:
                // last out tangent type is step, so just ignore the current in tangent type
                sampleTimes.Append(n_max(lastKeyTime, key->time - 1));
                break;
            case BEZKEY_FAST:
            case BEZKEY_SLOW:
            case BEZKEY_USER:
            case BEZKEY_FLAT:
                sampleTimes.AppendArray(SampleTime(lastKeyTime, key->time, delta));
                break;
            case size_t(-1):
                break;
            default:
                n_assert2(0, "Invalid tangent type.");
                break;
            }
            
            // append the key time
            sampleTimes.Append(key->time);

            lastOutTanType = GetOutTanType(key->flags);
            lastKeyTime = key->time;
        }
        n_free(key);
    }
    else
    {
        // generate fixed period sample times
        sampleTimes.AppendArray(SampleTime(start, end, delta));
    }

    // insert start frame
    if( sampleTimes[0] > start )
    {
        TimeValue t = start;
        sampleTimes.Insert(0, t);
    }

    // sample last key for exact looping.
    sampleTimes.Append(end);

    // get samples
    int numKeys = sampleTimes.Size();
    TimeValue lastKeyTime = start - 1;
    for (i = 0; i < numKeys; i++)
    {
        t = sampleTimes[i];
        if (t == lastKeyTime) // ignore same time
            continue;

        lastKeyTime = t;

        nMaxSampleKey sampleKey;
        Interval interv;
        switch(type)
        {
        case nMaxFloat:
            {
                float value;
                control->GetValue(t, &value, interv, CTRL_ABSOLUTE);

                sampleKey.fval = value;
            }
            break;
        case nMaxPoint3:
            {
                Point3 value;
                control->GetValue(t, &value, interv);

                sampleKey.pos = value;
            }
            break;
        case nMaxPoint4:
            {
                Point4 value;
                control->GetValue(t, &value, interv);

                sampleKey.pt4 = value;
            }
            break;
        }

        sampleKey.time = (t - start) * SECONDSPERTICK;
        sampleKeyArray.Append(sampleKey);
    }

    if (optimize && sampleKeyArray.Size() > 2)
    {
        // remove redundant keys
        nArray<nMaxSampleKey> tmpKeyArray;

        // first and last keys are important
        tmpKeyArray.Append(sampleKeyArray.Front());

        for(i = 1; i < sampleKeyArray.Size() - 1; ++i)
        {
            // current key is important if keys on either side are different to it.
            const nMaxSampleKey &previousKey = sampleKeyArray[i - 1];
            const nMaxSampleKey &currentKey = sampleKeyArray[i];
            const nMaxSampleKey &nextKey = sampleKeyArray[i + 1];
            if(/*!AreKeysEqual(currentKey, previousKey, type) || 
                !AreKeysEqual(currentKey, nextKey, type) ||*/
                !AreKeysLerp(currentKey, previousKey, nextKey, type))
            {
                tmpKeyArray.Append(currentKey);
            }
        }
        // first and last keys are important
        tmpKeyArray.Append(sampleKeyArray.Back());

        // sampleKeyArray.Clear(); // seems no need to clear it
        sampleKeyArray = tmpKeyArray;
    }
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxControl::AreKeysEqual(const nMaxSampleKey& key1, const nMaxSampleKey& key2, nMaxControlType type)
{
    switch (type)
    {
    case nMaxPoint4:
        // 3dsmax sdk, Point4 has no const Equals method
        return const_cast<Point4&>(key1.pt4).Equals(key2.pt4) != 0;
    case nMaxPoint3:
        return key1.pos.Equals(key2.pos) != 0;
    case nMaxFloat:
        return key1.fval == key2.fval;
    case nMaxPos:
        return key1.pos.Equals(key2.pos) != 0;
    case nMaxScale:
        return key1.scale.Equals(key2.scale) != 0;
    case nMaxRot:
        return key1.rot.Equals(key2.rot) != 0;
    case nMaxTM:
        return key1.pos.Equals(key2.pos) != 0 && key1.scale.Equals(key2.scale) != 0 && key1.rot.Equals(key2.rot) != 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nMaxControl::AreKeysLerp(const vector4& p, const vector4& p1, const vector4& p2, float lerpVal)
{
    vector4 ptmp;
    ptmp.lerp(p1, p2, lerpVal);

    // use proportion tolerance instead of neubla2 vector's difference tolerance
    vector4 tolerance(p * key_tolerance);

    return fabs(p.x - ptmp.x) <= tolerance.x && fabs(p.y - ptmp.y) <= tolerance.y &&
        fabs(p.z - ptmp.z) <= tolerance.z && fabs(p.w - ptmp.w) <= tolerance.w;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nMaxControl::AreKeysLerp(const vector3& p, const vector3& p1, const vector3& p2, float lerpVal)
{
    vector3 ptmp;
    ptmp.lerp(p1, p2, lerpVal);

    // use proportion tolerance instead of neubla2 vector's difference tolerance
    vector3 tolerance(p * key_tolerance);
    return fabs(p.x - ptmp.x) <= tolerance.x && fabs(p.y - ptmp.y) <= tolerance.y &&
        fabs(p.z - ptmp.z) <= tolerance.z;
}

//-----------------------------------------------------------------------------
/** 
    since nebula2 use lerp interpolation, keys that matchs the lerp will be redundant
*/
bool nMaxControl::AreKeysLerp(const nMaxSampleKey& key, const nMaxSampleKey& key1, const nMaxSampleKey& key2, nMaxControlType type)
{
    float lerpVal = (key.time - key1.time) / (key2.time - key1.time);
    switch (type)
    {
    case nMaxPoint4:
        {
            const vector4 p(key.pt4.x, key.pt4.y, key.pt4.z, key.pt4.w);
            const vector4 p1(key1.pt4.x, key1.pt4.y, key1.pt4.z, key1.pt4.w);
            const vector4 p2(key2.pt4.x, key2.pt4.y, key2.pt4.z, key2.pt4.w);
            return AreKeysLerp(p, p1, p2, lerpVal);
        }
        break;
    case nMaxRot:
        {
            const vector4 p(key.rot.x, key.rot.y, key.rot.z, key.rot.w);
            const vector4 p1(key1.rot.x, key1.rot.y, key1.rot.z, key1.rot.w);
            const vector4 p2(key2.rot.x, key2.rot.y, key2.rot.z, key2.rot.w);
            return AreKeysLerp(p, p1, p2, lerpVal);
        }
        break;
    case nMaxScale:
        {
            const vector3 p(key.scale.x, key.scale.y, key.scale.z);
            const vector3 p1(key1.scale.x, key1.scale.y, key1.scale.z);
            const vector3 p2(key2.scale.x, key2.scale.y, key2.scale.z);
            return AreKeysLerp(p, p1, p2, lerpVal);
        }
        break;
    case nMaxPos:
        // nMaxPos is acctually the same with nMaxPoint3
    case nMaxPoint3:
        {
            const vector3 p(key.pos.x, key.pos.y, key.pos.z);
            const vector3 p1(key1.pos.x, key1.pos.y, key1.pos.z);
            const vector3 p2(key2.pos.x, key2.pos.y, key2.pos.z);
            return AreKeysLerp(p, p1, p2, lerpVal);
        }
        break;
    case nMaxFloat:
        {
            float p = key.fval;
            float p1 = key1.fval;
            float p2 = key2.fval;
            float ptmp = n_lerp(p1, p2, lerpVal);
            // use proportion tolerance instead of neubla2 vector's difference tolerance
            return fabs(p - ptmp) <= key_tolerance;
        }
        break;
    case nMaxTM:
        {
            return AreKeysLerp(key, key1, key2, nMaxPos) &&
                AreKeysLerp(key, key1, key2, nMaxRot) &&
                AreKeysLerp(key, key1, key2, nMaxScale);
        }
        break;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Get Controller type with given Control.

	@param control pointer to the Control which we want to know its type.
	@return type of control.
*/
nMaxControl::Type nMaxControl::GetType(Control *control)
{
    n_assert(control);

    ulong partA = control->ClassID().PartA();

    switch(partA)
    {
    case TCBINTERP_FLOAT_CLASS_ID:       return TCBFloat;
    case TCBINTERP_POSITION_CLASS_ID:    return TCBPosition;
    case TCBINTERP_ROTATION_CLASS_ID:    return TCBRotation;
    case TCBINTERP_SCALE_CLASS_ID:       return TCBScale;
    case TCBINTERP_POINT3_CLASS_ID:      return TCBPoint3;
    case HYBRIDINTERP_FLOAT_CLASS_ID:    return HybridFloat;
    case HYBRIDINTERP_POSITION_CLASS_ID: return HybridPosition;
    case HYBRIDINTERP_ROTATION_CLASS_ID: return HybridRotation;
    case HYBRIDINTERP_SCALE_CLASS_ID:    return HybridScale;
    case HYBRIDINTERP_POINT3_CLASS_ID:   return HybridPoint3;
    case LININTERP_FLOAT_CLASS_ID:       return LinearFloat;
    case LININTERP_POSITION_CLASS_ID:    return LinearPosition;
    case LININTERP_ROTATION_CLASS_ID:    return LinearRotation;
    case LININTERP_SCALE_CLASS_ID:       return LinearScale;
    case EULER_CONTROL_CLASS_ID:
    case LOCAL_EULER_CONTROL_CLASS_ID:   return EulerRotation;
    default:
        return Unknown;
    }
}

//-----------------------------------------------------------------------------
/**
*/
nArray<TimeValue> nMaxControl::SampleTime(TimeValue start, TimeValue end, TimeValue delta)
{
    n_assert(start < end && delta > 0);
    nArray<TimeValue> array;
    for (TimeValue t = start + delta / 2; t < end; t += delta)
        array.Append(t);

    // delta is too big, but we will generate at least one time
    if (array.Empty())
    {
        array.Append((start + end) / 2);
    }
    return array;
}
