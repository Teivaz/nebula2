#ifndef N_OCCLUDINGFRUSTUMGENARRAY_H
#define N_OCCLUDINGFRUSTUMGENARRAY_H

/**
    @class nOccludingFrustumGenList
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view frustum and populates
    an nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "spatialdb/noccludingfrustumvisitor.h"

class nOccludingFrustumGenArray : public nOccludingFrustumVisitor {
public:
    typedef nArray<nSpatialElement *> VisibleElements;

    nOccludingFrustumGenArray(nCamera2 &cam, matrix44 &camxform, VisibleElements &foundarray);
    ~nOccludingFrustumGenArray();

    virtual void Reset();

    virtual void Visit(nSpatialElement *visitee, int recursedepth); 

protected:
    void ClearArray();

    VisibleElements &m_visarray;
};

#endif

