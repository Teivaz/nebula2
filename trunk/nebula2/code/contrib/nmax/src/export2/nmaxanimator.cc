//-----------------------------------------------------------------------------
//  nmaxanimator.cc
//
//  (C)2004 Kim, Hyoiun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxtransformanimator.h"
#include "export2/nmaxikanimator.h"
#include "export2/nmaxtransformcurveanimator.h"
#include "export2/nMaxAnimator.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/ntypes.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxAnimator::nMaxAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxAnimator::~nMaxAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxAnimator::Export(INode* inode)
{
    nMaxNode* createdNode = CreateAnimator(inode);
    if (createdNode)
    {
        //TODO: pop created node.

        //controller->Export(inode);
        n_delete(createdNode);
    }
    else
    {
        n_maxlog(High, "%s node does not have any controller.", inode->GetName());
    }
}

//-----------------------------------------------------------------------------
/**
*/
nMaxNode* nMaxAnimator::CreateAnimator(INode* inode)
{
    n_assert(inode);

    Control* control = inode->GetTMController();
    if (!control)
    {
        return NULL;
    }

    nMaxNode* createdNode = 0;

    Control *posControl, *rotControl, *scaleControl;

    posControl   = control->GetPositionController();
    rotControl   = control->GetRotationController();
    scaleControl = control->GetScaleController();

    if (posControl || rotControl || scaleControl)
    {
        // we have one of the animation controller at least.

        if (control->ClassID() == IKSLAVE_CLASSID || 
            posControl->ClassID() == IKSLAVE_CLASSID ||
            rotControl->ClassID() == IKSLAVE_CLASSID)
        {
            // the control is IK control.
            nMaxIKAnimator* ikAnimator = n_new(nMaxIKAnimator);
            ikAnimator->Export(inode);
            createdNode = ikAnimator;
        }
        else
        if (posControl->ClassID() == Class_ID(PATH_CONTROL_CLASS_ID, 0))
        {
            // the control is path control.
            nMaxTransformCurveAnimator* tmCurveAnimator = n_new(nMaxTransformCurveAnimator);
            tmCurveAnimator->Export(control);
            createdNode = tmCurveAnimator;

        }
        else
        {
            nMaxTransformAnimator* prsController = n_new(nMaxTransformAnimator);
            prsController->Export(inode, control);
            createdNode = prsController;
        }
    }
    else
    {
        n_maxlog(High, "%s node has Control but it does not contain have any PRS controller.", 
                 inode->GetName());
    }

    return createdNode;
}

