#include "export2/nmax.h"
#include "export2/nmaxcontroller.h"
#include "kernel/ntypes.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxController::nMaxController()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxController::~nMaxController()
{
}

//-----------------------------------------------------------------------------
/**
*/
//void nMaxController::Export(INode* inode)
//{
//    Control* control = inode->GetTMController();
//
//    Export(control);
//}

//-----------------------------------------------------------------------------
/**
    Get Controller type with given Control.

	@param control pointer to the Control which we want to know its type.
	@return type of control.
*/
nMaxController::Type nMaxController::GetType(Control *control)
{
    n_assert(control);

	ulong partA = control->ClassID().PartA();

	switch(partA)
	{
	case TCBINTERP_FLOAT_CLASS_ID:       return TCBFloat;
	case TCBINTERP_POSITION_CLASS_ID:    return TCBPosition;
	case TCBINTERP_ROTATION_CLASS_ID:    return TCBRotation;
	case TCBINTERP_SCALE_CLASS_ID:       return TCBScale;
    case TCBINTERP_POINT3_CLASS_ID:      return TCBPoint3;
	case HYBRIDINTERP_FLOAT_CLASS_ID:    return HybridFloat;
	case HYBRIDINTERP_POSITION_CLASS_ID: return HybridPosition;
	case HYBRIDINTERP_ROTATION_CLASS_ID: return HybridRotation;
	case HYBRIDINTERP_SCALE_CLASS_ID:    return HybridScale;
    case HYBRIDINTERP_POINT3_CLASS_ID:   return HybridPoint3;
	case LININTERP_FLOAT_CLASS_ID:       return LinearFloat;
	case LININTERP_POSITION_CLASS_ID:    return LinearPosition;
	case LININTERP_ROTATION_CLASS_ID:    return LinearRotation;
	case LININTERP_SCALE_CLASS_ID:       return LinearScale;
	default:
		return Unknown;
	}
}