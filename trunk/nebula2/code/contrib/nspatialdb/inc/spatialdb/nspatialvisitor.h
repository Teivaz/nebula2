#ifndef N_SPATIALVISITOR_H
#define N_SPATIALVISITOR_H

#include "mathlib/vector.h"
#include "mathlib/matrix.h"

#include "util/narray.h"

class nSpatialElement;
class nGfxServer2;

/**
    @class nSpatialVisitor
    @ingroup NSpatialDBContribModule
    @brief Visitor to collect all elements in a given region of space

*/
class nSpatialVisitor {
public:
    typedef nArray<nSpatialElement *> SpatialElements;

    nSpatialVisitor() : m_gfxdebug(NULL) {
    }
    ~nSpatialVisitor() {
    }

    virtual void Reset();

    /** process an element that is known to be in the region (i.e., don't
        do a clip check)
    */
    virtual void Visit(nSpatialElement *visitee, int recursiondepth=0, VisitorFlags flags=EmptyVisitoFlags());

    /** Check if an element is in the region.
        Returns true if the element is in the region, false if the element
	is not in the region.

        Note that elements partially in the region may be rejected or
	accepted, depending on the visitor.  Check the docs for that visitor!

        Mainly used by hierarchy elements. */
    virtual bool ContainmentTest(nSpatialElement *visitee) = 0;

    /** @brief Specify gfxserver to use for debug visualization.
        Comes in handy for debugging wierd culling artifacts. You can turn off
        debug visualization by setting this value to NULL (the default) */
    void VisualizeDebug(nGfxServer2 *gfx);

protected:
    nGfxServer2 *m_gfxdebug;

    /**
       entering a new local space; the matrix given will transform from
       the current local system into the new space local coordinate
       system.  This is used to possibly update a transform matrix or
       to transform the spatial region to a new coordinate system.
    */
    virtual void EnterLocalSpace(matrix44 &warp) = 0;

    /// leave a local space
    virtual void LeaveLocalSpace() = 0;
};

inline void nSpatialVisitor::VisualizeDebug(nGfxServer2 *gfx) { m_gfxdebug = gfx; }


#endif
