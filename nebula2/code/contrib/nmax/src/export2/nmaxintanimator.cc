//-----------------------------------------------------------------------------
//  nmaxintanimator.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxintanimator.h"
#include "export2/nmaxcontrol.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "gfx2/nshaderstate.h"
#include "scene/nintanimator.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxIntAnimator::nMaxIntAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxIntAnimator::~nMaxIntAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
nAnimator* nMaxIntAnimator::Export(const char* paramName, Control* control)
{
    nIntAnimator* createdAnimator = 0;

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

    createdAnimator = static_cast<nIntAnimator*>(CreateNebulaObject("nintanimator", animatorName.Get()));

    if (createdAnimator)
    {
        int numFrames = nMaxInterface::Instance()->GetNumFrames();;

        // retrieves sampled keys from the control.
        nArray<nMaxSampleKey> sampleKeyArray;
        sampleKeyArray.SetFixedSize(numFrames + 1);

        //HACK: 3dsmax does not support int type of control, so we use float and cast it.
        nMaxControl::GetSampledKey(control, sampleKeyArray, 1, nMaxFloat);

        // add key values to the animator.
        for (int i=0; i<numFrames; i++)
        {
            nMaxSampleKey key = sampleKeyArray[i];

            int val = (int)key.fval;

            createdAnimator->AddKey(key.time, val);
        }

        createdAnimator->SetVectorName(paramName);
        createdAnimator->SetChannel("time");

        //FIXME: 'oneshot' loop type should be available too.
        createdAnimator->SetLoopType(nAnimator::Loop);
    }

    return createdAnimator;
}
