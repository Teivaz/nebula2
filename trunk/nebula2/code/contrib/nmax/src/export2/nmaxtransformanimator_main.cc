//-----------------------------------------------------------------------------
//  nmaxtransformanimator_main.cc
//
//  (c)2005 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontrol.h"
#include "scene/ntransformanimator.h"
#include "export2/nMaxtransformanimator.h"

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
void nMaxTransformAnimator::Export(INode* inode, Control *control)
{
    n_assert(inode);
    n_assert(control);

    this->maxNode = inode;

    Control *posControl, *rotControl, *scaleControl;
    posControl   = control->GetPositionController();
    rotControl   = control->GetRotationController();
    scaleControl = control->GetScaleController();

    // create nTransformAnimator.
    nTransformAnimator* animator;
    animator = static_cast<nTransformAnimator*>(CreateNebulaObject("ntransformanimator", "animator"));

	if (control->GetPositionController())
	{
		ExportPosition(posControl, animator);
	}

	if (control->GetRotationController())
	{
		ExportRotation(rotControl, animator);
	}

	if (control->GetScaleController())
	{
		ExportScale(scaleControl, animator);
	}

    //
    //animator->SetChannel("time");
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

