//-----------------------------------------------------------------------------
//  nmaxcontrol.cc
//
//  (C)2005 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxcontroller.h"
#include "export2/nmaxtransform.h"
#include "export2/nmaxutil.h"

#include "kernel/ntypes.h"

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
*/
void nMaxControl::GetSampledKey(INode* inode, nArray<nMaxSampleKey> & sampleKeyArray, 
                                    int sampleRate, nMaxControlType type)
{
    if (type != nMaxTM  && 
        type != nMaxPos && 
        type != nMaxRot && 
        type != nMaxScale)
    {
        return;
    }

    TimeValue t;
    TimeValue start	= nMaxInterface::Instance()->GetAnimStartTime();
    TimeValue end	= nMaxInterface::Instance()->GetAnimEndTime();

    int  delta	= GetTicksPerFrame() * sampleRate;

    int numKeys = 0;

    for (t=start; t<end; t+=delta, numKeys++)
    {
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
}

//-----------------------------------------------------------------------------
/**
    Samples the float or point3 controllers and specifies retrieved keys to 
    the given sampleKeyArray.
*/
void nMaxControl::GetSampledKey(Control* control, nArray<nMaxSampleKey> & sampleKeyArray, 
                                    int sampleRate, nMaxControlType type)
{
    TimeValue t;
    TimeValue start	= nMaxInterface::Instance()->GetAnimStartTime();
    TimeValue end	= nMaxInterface::Instance()->GetAnimEndTime();

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

            sampleKey.fVal = value;
        }

        if (type == nMaxPoint3)
        {
            Point3 value;
            control->GetValue(t, &value, interv);

            sampleKey.pos = value;
        }

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

            sampleKey.fVal = value;
        }

        if (type == nMaxPoint3)
        {
            Point3 value;
            control->GetValue(t, &value, interv);

            sampleKey.pos = value;
        }

        sampleKeyArray.Append(sampleKey);
    }
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
