//-----------------------------------------------------------------------------
//  nmaxtransformanimator_main.cc
//
//  (c)2005 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontrol.h"
#include "scene/ntransformanimator.h"
#include "export2/nMaxtransformanimator.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/ntypes.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxTransformAnimator::nMaxTransformAnimator() :
    maxNode(0)
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxTransformAnimator::~nMaxTransformAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxTransformAnimator::Export(INode* inode, Control *control)
{
    n_assert(inode);
    n_assert(control);

    this->maxNode = inode;

    Control *posControl, *rotControl, *scaleControl;
    posControl   = control->GetPositionController();
    rotControl   = control->GetRotationController();
    scaleControl = control->GetScaleController();

    nString animatorName;
    animatorName += inode->GetName();
    animatorName += "animator";
    animatorName.ToLower();

    // create nTransformAnimator.
    nTransformAnimator* animator;
    animator = static_cast<nTransformAnimator*>(CreateNebulaObject("ntransformanimator", 
                                                                   animatorName.Get()));
    if (animator)
    {
        int numKeys = 0;

        if (control->GetPositionController())
        {
            numKeys += ExportPosition(posControl, animator);
        }

        if (control->GetRotationController())
        {
            numKeys += ExportRotation(rotControl, animator);
        }

        if (control->GetScaleController())
        {
            numKeys += ExportScale(scaleControl, animator);
        }

        if (numKeys)
        {
            // specifies the created animator to the parent node.
            nSceneNode* parent = static_cast<nSceneNode*>(animator->GetParent());
            parent->AddAnimator(animator->GetName());
        }
        else
        {
            // no keys are actually exported, so release created transform animator.
            animator->Release();
        }
    }
    else
    {
        animator->Release();
        n_maxlog(Error, "Failed to create Nebula object for the node '%s'.", inode->GetName());
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------
/**
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
