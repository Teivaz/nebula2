//-----------------------------------------------------------------------------
//  nmaxtransformanimator.h
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXTRANSFROMANIMATOR_H
#define N_MAXTRANSFROMANIMATOR_H
//-----------------------------------------------------------------------------
/**
    @class nMaxTransformAnimator
    @ingroup NebulaMaxExport2Contrib

    @brief A class for handling Position/Rotation/Scale(PRS) transform animation.

    The most similar controller for nebula's transform animator is <b>Linear
    Controller</b> which interpolates between animation keys by evenly dividing 
    the change from one key value to the next by the amount of time between the 
    keys.
*/
#include "export2/nmaxnode.h"

class nTransformAnimator;

//-----------------------------------------------------------------------------
class nMaxTransformAnimator : public nMaxNode
{
public:
    nMaxTransformAnimator();
    virtual ~nMaxTransformAnimator();

    virtual bool Export(INode* inode, Control *control);

protected:
    // @name position
    // @{
    virtual int ExportPosition(Control*, nTransformAnimator*);

	virtual int ExportTCBPosition(IKeyControl*, int numKeys, nTransformAnimator*);
	virtual int ExportHybridPosition(IKeyControl*, int numKeys, nTransformAnimator*);
	virtual int ExportLinearPosition(IKeyControl*, int numKeys, nTransformAnimator*);
    virtual int ExportSampledPosition(nTransformAnimator*);
    // @}

    // @name rotation
    // @{
    virtual int ExportRotation(Control*, nTransformAnimator*);
    virtual int ExportTCBRotation(IKeyControl*, int numKeys, nTransformAnimator*);
    virtual int ExportHybridRotation(IKeyControl*, int numKeys, nTransformAnimator*);
    virtual int ExportLinearRotation(IKeyControl*, int numKeys, nTransformAnimator*);
    virtual int ExportEulerRotation(Control*, int numKeys, nTransformAnimator*);
    virtual int ExportSampledKeyRotation(nTransformAnimator*);
    // @}

    // @name scale
    // @{
    virtual int ExportScale(Control*, nTransformAnimator*);
    virtual int ExportTCBScale(IKeyControl*, int numKeys, nTransformAnimator*);
    virtual int ExportHybridScale(IKeyControl*, int numKeys, nTransformAnimator*);
    virtual int ExportLinearScale(IKeyControl*, int numKeys, nTransformAnimator*);
    virtual int ExportSampledScale(nTransformAnimator*);
    // @}    
	
    bool HasSampledKeys(Control *control);

protected:
    INode* maxNode;

};
//-----------------------------------------------------------------------------
#endif
