//-----------------------------------------------------------------------------
//  nmaxvectoranimator.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXVECTORANIMATOR_H
#define N_MAXVECTORANIMATOR_H
//-----------------------------------------------------------------------------
/**
    @class nMaxVectorAnimator
    @ingroup NebulaMaxExport2Contrib

    @brief A class for exporting nVectorAnimator.
*/
#include "export2/nmaxnode.h"

class nAnimator;

//-----------------------------------------------------------------------------
class nMaxVectorAnimator : public nMaxNode
{
public:
    nMaxVectorAnimator();
    virtual ~nMaxVectorAnimator();

    nAnimator* Export(const char* paramName, Control* control);

};
//-----------------------------------------------------------------------------
#endif
