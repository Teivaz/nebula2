//-----------------------------------------------------------------------------
//  nmaxtransformanimator_rot.cc
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxoptions.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxtransformanimator.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/ntypes.h"
#include "scene/ntransformanimator.h"

//-----------------------------------------------------------------------------
/**
    @return the number of the keys which to be used for the actual animation.
*/
int nMaxTransformAnimator::ExportRotation(Control *control, nTransformAnimator* animator)
{
    if (control->NumKeys())
    {
        IKeyControl* iKeyControl = GetKeyControlInterface(control);

        if (iKeyControl)
        {
            int numKeys = iKeyControl->GetNumKeys();
            if (numKeys)
            {
                nMaxControl::Type type = nMaxControl::GetType(control);

                switch(type)
                {
                case nMaxControl::TCBRotation:
                    return ExportTCBRotation(iKeyControl, numKeys, animator);

                case nMaxControl::HybridRotation:
                    return ExportHybridRotation(iKeyControl, numKeys, animator);

                case nMaxControl::LinearRotation:
                    return ExportLinearRotation(iKeyControl, numKeys, animator);

                case nMaxControl::EulerRotation:
                    return ExportEulerRotation(control, numKeys, animator);

                default:
                    return ExportSampledKeyRotation(animator);
                }
            }
            else
            {
                // we have Controller but it has no keys.
                n_maxlog(Warning, "The node '%s' has Control but no keys.", this->maxNode->GetName());
                return 0;
            }
        }
        else
        {
            return ExportSampledKeyRotation(animator);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
int nMaxTransformAnimator::ExportTCBRotation(IKeyControl* ikc, int numKeys,
                                             nTransformAnimator* animator)
{
    //for (int i=0; i<numKeys; i++)
    //{
    //    ITCBRotKey key;
    //    ikc->GetKey(i, &key);
    //}
    n_maxlog(Warning, "The 'TCBRotation' type of control is not supported.");

    return 0;
}

//-----------------------------------------------------------------------------
/**
    -03-Jun-06  kims  Fixed to use second key value if there is no first one
                      when its start time.
                      Thanks Kim, Seung Hoon for this fix.

*/
int nMaxTransformAnimator::ExportHybridRotation(IKeyControl* ikc, int numKeys,
                                                nTransformAnimator* animator)
{
    n_maxlog(Warning, "The bezier controller is used for rotation. \
                      Only rotation value will be exported.");

    for (int i=0; i<numKeys; i++)
    {
        IBezQuatKey key;
        ikc->GetKey(i, &key);

        quaternion rot;
        rot.x = -key.val.x;
        rot.y = key.val.z;
        rot.z = key.val.y;
        rot.w = -key.val.w;

        float time = key.time * SECONDSPERTICK;

        // There should be any value at 0.0 sec, the start time.
        // If the value is not exist the second key value can be used for it
        // because 3dsmax uses the second key instead of the first
        // if the first one is not exist.
        if (time > 0.0f &&
            animator->GetNumQuatKeys() == 0)
        {
            animator->AddQuatKey(0.0f, rot);
        }

        animator->AddQuatKey(time, rot);
    }

    return numKeys;
}

//-----------------------------------------------------------------------------
/**
    -03-Jun-06  kims  Fixed to use second key value if there is no first one
                      when its start time.
                      Thanks Kim, Seung Hoon for this fix.

    @return the number of the keys which to be used for the actual animation.
*/
int nMaxTransformAnimator::ExportLinearRotation(IKeyControl* ikc, int numKeys,
                                                nTransformAnimator* animator)
{
    for (int i=0; i<numKeys; i++)
    {
        ILinRotKey key; //quaternion value
        ikc->GetKey(i, &key);

        quaternion rot;
        rot.x = -key.val.x;
        rot.y = key.val.z;
        rot.z = key.val.y;
        rot.w = -key.val.w;

        float time = key.time * SECONDSPERTICK;

        // There should be any value at 0.0 sec, the start time.
        // If the value is not exist the second key value can be used for it
        // because 3dsmax uses the second key instead of the first
        // if the first one is not exist.
        if (time > 0.0f &&
            animator->GetNumQuatKeys() == 0)
        {
            animator->AddQuatKey(0.0f, rot);
        }

        animator->AddQuatKey(time, rot);
    }

    return numKeys;
}

//-----------------------------------------------------------------------------
/**
    @return the number of the keys which to be used for the actual animation.
*/
int nMaxTransformAnimator::ExportEulerRotation(Control* control, int numKeys,
                                               nTransformAnimator* animator)
{
    n_maxlog(Warning, "The 'Euler Rotation' type of control is not supported.");

    return 0;
}

//-----------------------------------------------------------------------------
/**
    -03-Jun-06  kims  Fixed to use second key value if there is no first one
                      when its start time.
                      Thanks Kim, Seung Hoon for this fix.

    @return the number of the keys which to be used for the actual animation.
*/
int nMaxTransformAnimator::ExportSampledKeyRotation(nTransformAnimator* animator)
{
    int sampleRate = nMaxOptions::Instance()->GetSampleRate();

    nArray<nMaxSampleKey> sampleKeyArray;

    nMaxControl::GetSampledKey(this->maxNode, sampleKeyArray, sampleRate, nMaxRot);

    // assign sample keys to animator.
    for (int i=0; i<sampleKeyArray.Size(); i++)
    {
        nMaxSampleKey sampleKey = sampleKeyArray[i];

        quaternion rot;
        rot.x = -(sampleKey.rot.x);
        rot.y = sampleKey.rot.z;
        rot.z = sampleKey.rot.y;
        rot.w = -(sampleKey.rot.w);

        float time = sampleKey.time;

        // There should be any value at 0.0 sec, the start time.
        // If the value is not exist the second key value can be used for it
        // because 3dsmax uses the second key instead of the first
        // if the first one is not exist.
        if (time > 0.0f &&
            animator->GetNumQuatKeys() == 0)
        {
            animator->AddQuatKey(0.0f, rot);
        }

        animator->AddQuatKey(time, rot);
    }

    return sampleKeyArray.Size();
}
