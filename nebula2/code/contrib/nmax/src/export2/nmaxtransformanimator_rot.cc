//-----------------------------------------------------------------------------
//  nmaxtransformanimator_rot.cc
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxtransformanimator.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/ntypes.h"
#include "scene/ntransformanimator.h"

//-----------------------------------------------------------------------------
/**
*/
void nMaxTransformAnimator::ExportRotation(Control *control, nTransformAnimator* animator)
{
    IKeyControl* iKeyControl = GetKeyControlInterface(control);

    int numKeys = iKeyControl->GetNumKeys();

    if (iKeyControl)
    {
        if (numKeys > 0)
        {
            nMaxControl::Type type = nMaxControl::GetType(control);

            switch(type)
            {
            case nMaxControl::TCBRotation:
                ExportTCBRotation(iKeyControl, numKeys);
                break;

            case nMaxControl::HybridRotation:
                ExportHybridRotation(iKeyControl, numKeys);
                break;

            case nMaxControl::LinearRotation:
                ExportLinearRotation(iKeyControl, numKeys);
                break;

            case nMaxControl::EulerRotation:
                ExportEulerRotation(control, numKeys);
                break;

            default:
                ExportSampledKeyRotation(numKeys, animator);
                break;
            }
        }
    }

}

//-----------------------------------------------------------------------------
/**
*/
void nMaxTransformAnimator::ExportTCBRotation(IKeyControl* ikc, int numKeys)
{
    //for (i=0; i<numKeys; i++)
    //{
    //    ITCBRotKey key;
    //    ikc->GetKey(i, &key);
    //}

    n_maxlog(Warning, "The 'TCBRotation' type of control is not supported.");
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxTransformAnimator::ExportHybridRotation(IKeyControl* ikc, int numKeys)
{
    //for (i=0; i<numKeys; i++) 
    //{
    //    IBezQuatKey key;
    //    ikc->GetKey(i, &key);
    //}

    n_maxlog(Warning, "The 'HybridRotation' type of control is not supported.");
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxTransformAnimator::ExportLinearRotation(IKeyControl* ikc, int numKeys)
{
    //for (i=0; i<numKeys; i++)
    //{
    //    ILinRotKey key;
    //    ikc->GetKey(i, &key);
    //}

    n_maxlog(Warning, "The 'LinearRotation' type of control is not supported.");
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxTransformAnimator::ExportEulerRotation(Control* control, int numKeys)
{
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxTransformAnimator::ExportSampledKeyRotation(int numKeys, nTransformAnimator* animator)
{
    nArray<nMaxSampleKey> sampleKeyArray;
    sampleKeyArray.SetFixedSize(numKeys + 1);

    nMaxControl::GetSampledKey(this->maxNode, sampleKeyArray, 1, nMaxRot);

    // assign sample keys to animator.
    for (int i=0; i<sampleKeyArray.Size(); i++)
    {
        nMaxSampleKey sampleKey = sampleKeyArray[i];

        TimeValue time = sampleKey.time;

        quaternion rot;
        rot.x = -(sampleKey.rot.x);
        rot.y = sampleKey.rot.z;
        rot.z = sampleKey.rot.y;
        rot.w = -(sampleKey.rot.w);

        animator->AddQuatKey(time, rot);
    }
}
