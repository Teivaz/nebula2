//-----------------------------------------------------------------------------
//  nmaxtransformcurveanimator.h
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXPATHCONTROLLER_H
#define N_MAXPATHCONTROLLER_H
//-----------------------------------------------------------------------------
/**
    @class nMaxTransformCurveAnimator
    @ingroup

    @brief A class for handling path in 3dsmax.
           (This handles procedure controllers of 3dsmax)
*/
#include "export2/nmaxnode.h"

//-----------------------------------------------------------------------------
class nMaxTransformCurveAnimator : public nMaxNode
{
public:
    nMaxTransformCurveAnimator();
    virtual ~nMaxTransformCurveAnimator();

    virtual void Export(Control *control);
protected:
    

};
//-----------------------------------------------------------------------------
#endif
