//-----------------------------------------------------------------------------
// nmaxtransformanimator_pos.cc
//
// (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxtransformanimator.h"
#include "export2/nmaxtransform.h"
#include "export2/nmaxoptions.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/ntypes.h"
#include "util/narray.h"
#include "scene/ntransformanimator.h"

//-----------------------------------------------------------------------------
/**
    @return the number of the keys which to be used for the actual aniamtion.
*/
int nMaxTransformAnimator::ExportPosition(Control *control, nTransformAnimator* animator)
{
    if (control && control->NumKeys())
    {
    
        IKeyControl* iKeyControl = GetKeyControlInterface(control);

        Control* xControl = control->GetXController();
        Control* yControl = control->GetYController();
        Control* zControl = control->GetZController();

        if (iKeyControl)
        {
            int numKeys = iKeyControl->GetNumKeys();
            if (numKeys > 0)
            {
                nMaxControl::Type type = nMaxControl::GetType(control);

                switch(type)
                {
                case nMaxControl::TCBPosition:
                    return ExportTCBPosition(iKeyControl, numKeys, animator);

                case nMaxControl::HybridPosition:
                    return ExportHybridPosition(iKeyControl, numKeys, animator);

                case nMaxControl::LinearPosition:
                    return ExportLinearPosition(iKeyControl, numKeys, animator);

                default:
                    return ExportSampledPosition(animator);
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
        if (xControl || yControl || zControl)
        {
            if (HasSampledKeys(control))
            {
                return ExportSampledPosition(animator);
            }
            else
            {
                // got one of the xControl, yControl or zControl but any control of those has no keys.
                //n_maxlog(Warning, "The node '%s' has Control but no keys.", this->maxNode->GetName());
                return 0;
            }
        }
        else
        {
            // export sampled key position animation.
            return ExportSampledPosition(animator);
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
int nMaxTransformAnimator::ExportTCBPosition(IKeyControl* ikc, int numKeys, 
                                             nTransformAnimator* animator)
{
    //for (int i=0; i<numKeys; i++)
    //{
    //    ITCBPoint3Key key;
    //    ikc->GetKey(i, &key);
    //}

    n_maxlog(Warning, "The 'TCB Position' type of control is not supported.");

    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
int nMaxTransformAnimator::ExportHybridPosition(IKeyControl* ikc, int numKeys, 
                                                nTransformAnimator* animator)
{
    //for (int i=0; i<numKeys; i++)
    //{
    //    IBezPoint3Key key;
    //    ikc->GetKey(i, &key);
    //}

    n_maxlog(Warning, "The 'Hybrid Position' type of control is not supported.");

    return 0;
}

//-----------------------------------------------------------------------------
/**
    -03-Jun-06  kims  Fixed to use second key value if there is no first one 
                      when its start time.
                      Thanks Kim, Seung Hoon for this fix.

    @return the number of the keys which to be used for the actual aniamtion.
*/
int nMaxTransformAnimator::ExportLinearPosition(IKeyControl* ikc, int numKeys, 
                                                 nTransformAnimator* animator)
{
    for (int i=0; i<numKeys; i++)
    {
        ILinPoint3Key key;
        ikc->GetKey(i, &key);

        vector3 pos;
        pos.x = -(key.val.x);
        pos.y = key.val.z;
        pos.z = key.val.y;

        float time = key.time * SECONDSPERTICK;

        // There should be any value at 0.0 sec, the start time.
        // If the value is not exist the second key value can be used for it 
        // because 3dsmax uses the second key instead of the first 
        // if the first one is not exist.
        if (time > 0.0f && animator->GetNumPosKeys() == 0)
        {
            animator->AddPosKey(0.0f, pos);
        }

        animator->AddPosKey(time, pos);
    }

    return numKeys;
}

//-----------------------------------------------------------------------------
/**
    -03-Jun-06  kims  Fixed to use second key value if there is no first one 
                      when its start time.
                      Thanks Kim, Seung Hoon for this fix.

    @return the number of the keys which to be used for the actual aniamtion.
*/
int nMaxTransformAnimator::ExportSampledPosition(nTransformAnimator* animator)
{
    int sampleRate = nMaxOptions::Instance()->GetSampleRate();

    nArray<nMaxSampleKey> sampleKeyArray;

    nMaxControl::GetSampledKey(this->maxNode, sampleKeyArray, sampleRate, nMaxPos, true);

    // assign sample keys to animator.
    for (int i=0; i<sampleKeyArray.Size(); i++)
    {
        nMaxSampleKey sampleKey = sampleKeyArray[i];

        float time = sampleKey.time;

        vector3 pos;
        pos.x = -(sampleKey.pos.x);
        pos.y = sampleKey.pos.z;
        pos.z = sampleKey.pos.y;

        // There should be any value at 0.0 sec, the start time.
        // If the value is not exist the second key value can be used for it 
        // because 3dsmax uses the second key instead of the first 
        // if the first one is not exist.
        if (time > 0.0f && animator->GetNumPosKeys() == 0)
        {
            animator->AddPosKey(0.0f, pos);
        }

        animator->AddPosKey(time, pos);
    }

    return sampleKeyArray.Size();
}

//-----------------------------------------------------------------------------
/**
    Check the given controller has any one of the xyz sub controller.

    @note:
    Controllers that have XYZ sub-controllers are the Euler angle controller 
    or the Position XYZ controller.
*/
bool nMaxTransformAnimator::HasSampledKeys(Control *control)
{
    bool result = false;

    Control* xControl = control->GetXController();
    Control* yControl = control->GetYController();
    Control* zControl = control->GetZController();

    IKeyControl* ikeyControl;

    if (xControl)
    {
        ikeyControl = GetKeyControlInterface(xControl);
        if (ikeyControl && ikeyControl->GetNumKeys() > 0)
            result = true;
    }

    if (yControl)
    {
        ikeyControl = GetKeyControlInterface(yControl);
        if (yControl && ikeyControl->GetNumKeys() > 0)
            result = true;
    }

    if (zControl)
    {
        ikeyControl = GetKeyControlInterface(zControl);
        if (yControl && ikeyControl->GetNumKeys() > 0)
            result = true;
    }

    return result;
}
