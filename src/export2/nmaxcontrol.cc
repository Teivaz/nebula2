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

        sampleKey.time = t * SECONDSPERTICK;
      
        sampleKeyArray.Append(sampleKey);
    }

    // sample last key for exact looping.
    //if (t != end)
    {
        t = end;

        nMaxSampleKey sampleKey;
  
        sampleKey.tm = nMaxTransform::GetLocalTM(inode, t);

        AffineParts ap;

        decomp_affine(sampleKey.tm, &ap );

        if (type == nMaxPos || type == nMaxTM)
            sampleKey.pos   = ap.t;

        if (type == nMaxRot || type == nMaxTM)
            sampleKey.rot   = ap.q;

        if (type == nMaxScale || type == nMaxTM)
            sampleKey.scale = ap.k;

        sampleKey.time  = t * SECONDSPERTICK;

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

    TimeValue t;
    TimeValue start = nMaxInterface::Instance()->GetAnimStartTime();
    TimeValue end   = nMaxInterface::Instance()->GetAnimEndTime();

    int  delta	= GetTicksPerFrame() * sampleRate;

    int numKeys = 0;

    for (t=start; t<end; t+=delta, numKeys++)
    {
        nMaxSampleKey sampleKey;

        Interval interv;
        if (type == nMaxFloat)
        {
            float value;
            control->GetValue(t, &value, interv, CTRL_ABSOLUTE);

            sampleKey.fval = value;
        }

        if (type == nMaxPoint3)
        {
            Point3 value;
            control->GetValue(t, &value, interv);

            sampleKey.pos = value;
        }

        if (type == nMaxPoint4)
        {
            Point4 value;
            control->GetValue(t, &value, interv);

            sampleKey.pt4 = value;
        }

        sampleKey.time = t * SECONDSPERTICK;

        sampleKeyArray.Append(sampleKey);
    }

    // sample last key for exact looping.
    //if (t != end)
    {
        t = end;

        nMaxSampleKey sampleKey;

        Interval interv;
        if (type == nMaxFloat)
        {
            float value;
            control->GetValue(t, &value, interv, CTRL_ABSOLUTE);

            sampleKey.fval = value;
        }

        if (type == nMaxPoint3)
        {
            Point3 value;
            control->GetValue(t, &value, interv);

            sampleKey.pos = value;
        }

        if (type == nMaxPoint4)
        {
            Point4 value;
            control->GetValue(t, &value, interv);

            sampleKey.pt4 = value;
        }

        sampleKey.time = t * SECONDSPERTICK;

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
            vector4 p(key.pt4.x, key.pt4.y, key.pt4.z, key.pt4.w);
            vector4 p1(key1.pt4.x, key1.pt4.y, key1.pt4.z, key1.pt4.w);
            vector4 p2(key2.pt4.x, key2.pt4.y, key2.pt4.z, key2.pt4.w);
            return AreKeysLerp(p, p1, p2, lerpVal);
        }
        break;
    case nMaxRot:
        {
            vector4 p(key.rot.x, key.rot.y, key.rot.z, key.rot.w);
            vector4 p1(key1.rot.x, key1.rot.y, key1.rot.z, key1.rot.w);
            vector4 p2(key2.rot.x, key2.rot.y, key2.rot.z, key2.rot.w);
            return AreKeysLerp(p, p1, p2, lerpVal);
        }
        break;
    case nMaxScale:
        {
            vector3 p(key.scale.x, key.scale.y, key.scale.z);
            vector3 p1(key1.scale.x, key1.scale.y, key1.scale.z);
            vector3 p2(key2.scale.x, key2.scale.y, key2.scale.z);
            return AreKeysLerp(p, p1, p2, lerpVal);
        }
        break;
    case nMaxPos:
        // nMaxPos is acctually the same with nMaxPoint3
    case nMaxPoint3:
        {
            vector3 p(key.pos.x, key.pos.y, key.pos.z);
            vector3 p1(key1.pos.x, key1.pos.y, key1.pos.z);
            vector3 p2(key2.pos.x, key2.pos.y, key2.pos.z);
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
