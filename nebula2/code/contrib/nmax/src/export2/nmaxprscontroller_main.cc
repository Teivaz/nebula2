//-----------------------------------------------------------------------------
//
//
//  (c)2005 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontroller.h"
#include "scene/ntransformanimator.h"
#include "export2/nmaxprscontroller.h"
#include "kernel/ntypes.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxPRSController::nMaxPRSController()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxPRSController::~nMaxPRSController()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPRSController::Export(INode* inode)
{
    n_assert(inode);
    this->maxNode = inode;
    Control* control = inode->GetTMController();

	n_assert(control);

    // create nTransformAnimator.
    nTransformAnimator* animator = 0;

	if (control->GetPositionController())
	{
		ExportPosition(control, animator);
	}

	if (control->GetRotationController())
	{
		ExportRotation(control, animator);
	}

	if (control->GetScaleController())
	{
		ExportScale(control, animator);
	}

    //
    //animator->SetChannel("time");
}


//-----------------------------------------------------------------------------
/**
*/
bool nMaxPRSController::HasSampledKeys(Control *control)
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

