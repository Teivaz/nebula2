//-----------------------------------------------------------------------------
//  nmaxtransformanimator.h
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXTRANSFROMANIMATOR_H
#define N_MAXTRANSFROMANIMATOR_H
//-----------------------------------------------------------------------------
/**
    @class nMaxPRSController
    @ingroup NebulaMaxExport2Contrib

    @brief A class for handling Position/Rotation/Scale(PRS) transform animation.

*/
#include "export2/nmaxnode.h"

class nTransformAnimator;

//-----------------------------------------------------------------------------
class nMaxTransformAnimator : public nMaxNode
{
public:
    nMaxTransformAnimator();
    virtual ~nMaxTransformAnimator();

    virtual void Export(INode* inode, Control *control);

protected:
    // @name position
    // @{
    virtual void ExportPosition(Control *control, nTransformAnimator* animator);

	virtual void ExportTCBPosition(IKeyControl* ikc, int numKeys, nTransformAnimator* animator);
	virtual void ExportHybridPosition(IKeyControl* ikc, int numKeys, nTransformAnimator* animator);
	virtual void ExportLinearPosition(IKeyControl* ikc, int numKeys, nTransformAnimator* animator);
    virtual void ExportSampledKeyPosition(int numKeys, nTransformAnimator* animator);
    // @}

    // @name rotation
    // @{
    virtual void ExportRotation(Control *control, nTransformAnimator* animator);

    virtual void ExportTCBRotation(IKeyControl* ikc, int numKeys);
    virtual void ExportHybridRotation(IKeyControl* ikc, int numKeys);
    virtual void ExportLinearRotation(IKeyControl* ikc, int numKeys);
    virtual void ExportEulerRotation(Control* control, int numKeys);
    virtual void ExportSampledKeyRotation(int numKeys, nTransformAnimator* animator);
    // @}

    // @name scale
    // @{
    virtual void ExportScale(Control *control, nTransformAnimator* animator);
    // @}    
	
    bool HasSampledKeys(Control *control);

protected:
    INode* maxNode;

};
//-----------------------------------------------------------------------------
#endif
