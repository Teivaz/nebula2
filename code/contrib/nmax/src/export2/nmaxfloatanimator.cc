//-----------------------------------------------------------------------------
//  nmaxfloatanimator.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxfloatanimator.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxoptions.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "gfx2/nshaderstate.h"
#include "scene/nfloatanimator.h"
#include "util/nanimlooptype.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxFloatAnimator::nMaxFloatAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxFloatAnimator::~nMaxFloatAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
nAnimator* nMaxFloatAnimator::Export(const char* paramName, Control* control)
{
    nFloatAnimator* createdAnimator = 0;

    // the number of the keys in the control should be checked cause some of 
    // the node has control even it has no animations.
    if (control->NumKeys())
    {
        nShaderState::Param param = nShaderState::StringToParam(paramName);
        if (param == nShaderState::InvalidParameter)
        {
            n_maxlog(Error, "Invalid shader parameter for vector animator.");
            return NULL;
        }

        // e.g. convert 'AlphaRef' to 'alpharefanimator'
        nString animatorName;
        animatorName += paramName;
        animatorName += "animator";
        animatorName.ToLower();

        createdAnimator = static_cast<nFloatAnimator*>(CreateNebulaObject("nfloatanimator", 
                                                                        animatorName.Get()));

        if (createdAnimator)
        {
            int numFrames = nMaxInterface::Instance()->GetNumFrames();;

            // retrieves sampled keys from the control.
            nArray<nMaxSampleKey> sampleKeyArray;

            int sampleRate;
            sampleRate = nMaxOptions::Instance()->GetSampleRate();

            nMaxControl::GetSampledKey(control, sampleKeyArray, sampleRate, nMaxFloat, true);

            // add key values to the animator.
            for (int i=0; i<sampleKeyArray.Size(); i++)
            {
                nMaxSampleKey key = sampleKeyArray[i];

                float val = (float)key.fval;

                createdAnimator->AddKey(key.time, val);
            }

            createdAnimator->SetParamName(paramName);
            createdAnimator->SetChannel("time");

            //FIXME: 'oneshot' loop type should be available too.
            createdAnimator->SetLoopType(nAnimLoopType::Loop);
        }
    }

    return createdAnimator;
}
