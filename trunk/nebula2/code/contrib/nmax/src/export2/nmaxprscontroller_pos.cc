//-----------------------------------------------------------------------------
// nmaxprscontroller_pos.cc
//
// (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontroller.h"
#include "export2/nmaxprscontroller.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxtransform.h"

#include "kernel/ntypes.h"

//-----------------------------------------------------------------------------
/**
*/
void nMaxPRSController::ExportPosition(Control *control, nTransformAnimator* animator)
{
    IKeyControl* iKeyControl = GetKeyControlInterface(control);

    Control* xControl = control->GetXController();
    Control* yControl = control->GetYController();
    Control* zControl = control->GetZController();

    if (iKeyControl)
    {
        if (iKeyControl->GetNumKeys() > 0)
        {
            nMaxController::Type type = GetType(control);

            switch(type)
            {
            case TCBPosition:
                ExportTCBPosition();
                break;
            case HybridPosition:
                ExportHybridPosition();
                break;
            case LinearPosition:
                ExportLinearPosition();
                break;
            default:
                //ExportDefaultPosition();
                break;
            }
        }
        else
        {
            // we have Controller but it has no keys.
            return;
        }

    }
    else
    if (xControl || yControl || zControl)
    {
        if (HasSampledKeys(control))
        {
            //ExportDefaultPosition();
        }
        else
        {
            // no keys.
            return;
        }
    }
    else
    {
        // export default position animation.
        //ExportDefaultPosition();
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPRSController::ExportDefaultPosition(INode *inode)
{
    //uint i;
    //TimeValue time;

    //TimeValue animStart = nMaxInterface::Instance()->GetAnimStartTime();
    //int numFrames = nMaxInterface::Instance()->GetNumFrames();

    //for (i=0, time = animStart; i<numFrames; i++, time += GetTicksPerFrame())
    //{
    //    Matrix3 m;
    //    AffineParts parts;

    //    m = nMaxTransform::GetLocalTM(inode, time);
    //    decomp_affine(m, &parts);

    //    vector3 pos;
    //    pos.x = -parts.t.x;
    //    pos.y = parts.t.z;
    //    pos.z = parts.t.y;
    //}

    // Get sampled key array

    
    /*
    // for each sampled key.
        vector3 pos();
        animator->AddPosKey(, pos);

    */
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPRSController::ExportTCBPosition()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPRSController::ExportHybridPosition()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPRSController::ExportLinearPosition()
{
}

