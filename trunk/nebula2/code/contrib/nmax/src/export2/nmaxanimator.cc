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

#include "kernel/nkernelserver.h"

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
        nKernelServer::Instance()->PopCwd();

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
        Class_ID classID = control->ClassID();

        if (classID == BIPBODY_CONTROL_CLASS_ID ||
            classID == BIPSLAVE_CONTROL_CLASS_ID ||
            classID == FOOTPRINT_CLASS_ID )
        {
            // check any biped controls, if any of it exists skip it.
            // we don't need to check any other type of controls, so just return.
            return NULL;
        }

        if (classID == IKCONTROL_CLASS_ID || classID == IKCHAINCONTROL_CLASS_ID)
        {
            n_maxlog(Warning, "Lookat control %s is not supported.", inode->GetName());
            // we don't need to check any other type of controls, so just return.
            return NULL;
        }

        if (classID == IKSLAVE_CLASSID || 
            (posControl && posControl->ClassID() == IKSLAVE_CLASSID) ||
            (rotControl && rotControl->ClassID() == IKSLAVE_CLASSID))
        {
            // the control is IK control.
            nMaxIKAnimator* ikAnimator = n_new(nMaxIKAnimator);
            ikAnimator->Export(inode);
            createdNode = ikAnimator;
        }

        if (classID == Class_ID(LOOKAT_CONTROL_CLASS_ID, 0))
        {
            n_maxlog(Warning, "Lookat control %s is not supported.", inode->GetName());
        }

        if (posControl && posControl->ClassID() == Class_ID(PATH_CONTROL_CLASS_ID, 0))
        {
            // the control is path control.
            nMaxTransformCurveAnimator* tmCurveAnimator = n_new(nMaxTransformCurveAnimator);
            tmCurveAnimator->Export(control);
            createdNode = tmCurveAnimator;

        }
        else
        if (posControl || rotControl || scaleControl)
        {
            nMaxTransformAnimator* prsController = n_new(nMaxTransformAnimator);
            if (prsController->Export(inode, control))
            {
                createdNode = prsController;
            }
            else
            {
                n_maxlog(Warning, "The control has animations but no animation was exported \
                                  for the node %s.", inode->GetName());
                return NULL;
            }
        }
    }
    else
    {
        n_maxlog(High, "%s node has Control but it does not contain have any PRS controller.", 
                 inode->GetName());
        return NULL;
    }

    return createdNode;
}

