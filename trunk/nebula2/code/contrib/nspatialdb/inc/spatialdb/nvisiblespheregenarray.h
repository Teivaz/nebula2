#ifndef N_VISIBLESPHEREGENARRAY_H
#define N_VISIBLESPHEREGENARRAY_H

/**
    @class nVisibleSphereGenArray
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a sphere and populates an
    nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "spatialdb/nvisiblespherevisitor.h"

class nVisibleSphereGenArray : public nVisibleSphereVisitor {
public:
    nVisibleSphereGenArray(const sphere &viewsphere, VisibleElements &foundarray);
    ~nVisibleSphereGenArray();

    virtual void Reset();

    virtual void Visit(nSpatialElement *visitee, int recursedepth);

protected:
    void ClearArray();

    VisibleElements &m_visarray;
};

#endif

