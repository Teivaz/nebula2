//-----------------------------------------------------------------------------
//  nmaxvectoranimator.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxvectoranimator.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxoptions.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "gfx2/nshaderstate.h"
#include "scene/nvectoranimator.h"
#include "util/nanimlooptype.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxVectorAnimator::nMaxVectorAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxVectorAnimator::~nMaxVectorAnimator()
{
}

//-----------------------------------------------------------------------------
/**
    Create a nVectorAnimator from the given shader parameter and control.

    FIXME: only full sampled keys are supported at the moment.
           Do we need any other control type supporting via IKeyControl?

    @param paramName A valid shader parameter name. e.g. "MatDiffuse"
    @param control Controller which is animatable.
                   It can be retrieved from parameter block.
*/
nAnimator* nMaxVectorAnimator::Export(const char* paramName, Control* control)
{
    nVectorAnimator* createdAnimator = NULL;

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

        // e.g. convert 'MatDiffuse' to 'matdiffuseanimator'
        nString animatorName;
        animatorName += paramName;
        animatorName += "animator";
        animatorName.ToLower();

        createdAnimator = static_cast<nVectorAnimator*>(CreateNebulaObject("nvectoranimator",
                                                                           animatorName.Get()));

        if (createdAnimator)
        {
            int numFrames = nMaxInterface::Instance()->GetNumFrames();;

            // retrieves sampled keys from the control.
            nArray<nMaxSampleKey> sampleKeyArray;

            int sampleRate;
            sampleRate = nMaxOptions::Instance()->GetSampleRate();

            nMaxControl::GetSampledKey(control, sampleKeyArray, sampleRate, nMaxPoint4, true);

            // add key values to the animator.
            for (int i=0; i<sampleKeyArray.Size(); i++)
            {
                nMaxSampleKey key = sampleKeyArray[i];

                vector4 color;
                color.x = key.pt4.x; // r
                color.y = key.pt4.y; // g
                color.z = key.pt4.z; // b
                color.w = key.pt4.w; // a

                createdAnimator->AddKey(key.time, color);
            }

            createdAnimator->SetParamName(paramName);
            createdAnimator->SetChannel("time");

            //FIXME: 'oneshot' loop type should be available too.
            createdAnimator->SetLoopType(nAnimLoopType::Loop);
        }
    }

    return createdAnimator;
}
