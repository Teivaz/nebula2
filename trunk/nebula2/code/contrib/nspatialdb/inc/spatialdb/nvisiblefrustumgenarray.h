#ifndef N_VISIBLEFRUSTUMGENARRAY_H
#define N_VISIBLEFRUSTUMGENARRAY_H

/**
    @class nVisibleFrustumGenList
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view frustum and populates
    an nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "spatialdb/nvisiblefrustumvisitor.h"

class nVisibleFrustumGenArray : public nVisibleFrustumVisitor {
public:
    nVisibleFrustumGenArray(const nCamera2 &cam, const matrix44 &camxform, VisibleElements &foundarray);
    ~nVisibleFrustumGenArray();

    void Reset();

    using nVisibleFrustumVisitor::Visit;

    void Visit(nSpatialElement *visitee, int recursedepth); 


protected:
    void ClearArray();

    VisibleElements &m_visarray;
};

#endif

