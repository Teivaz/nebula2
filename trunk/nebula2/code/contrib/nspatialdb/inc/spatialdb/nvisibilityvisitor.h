#ifndef N_VISIBILITYVISITOR_H
#define N_VISIBILITYVISITOR_H

#include "mathlib/vector.h"
#include "mathlib/matrix.h"

#include "util/narray.h"

class nSpatialSector;
class nSpatialElement;
class nGfxServer2;

/**
    @class nVisibilityVisitor
    @ingroup NSpatialDBContribModule
    @brief Base class for visitors that collect all elements in a given region and visible from a given viewpoint.

    Various subclasses of this abstract class are used for specific types
    of visibility culling.  To add a new type of visibility visitor:

    1. Subclass this class.  Add in stacks for any data that depends on
    the coordinate system.

    2. override the Accept(nSpatialSector *visitee, int recursedepth)
    method to correctly cull the octree for that sector.  Invoke Visit()
    for each element you find visible.

    3. To make hierarchy objects work, override VisibilityTest(...) to
    perform a proper test for the given element.

    4. To handle sectors with different coordinate systems, override
    the Enter/LeaveLocalSpace methods, such that you transform into
    the sectors space and push the new state onto your stacks.

    5. In your class or a subclass, override Accept(nSpatialElement *, int)
    to process each visible element, either by accumulating the visible
    elements in a list, sending them to a render system, or whatever.
*/

class nVisibilityVisitor {
public:
    nVisibilityVisitor(const vector3 &viewpoint);
    ~nVisibilityVisitor();

    /** Reset any state use during a visibility sweep.
        If you use a visitor multiple times, reset it between each visibility collection */
    virtual void Reset();

    /** process a sector-override this to do your special visibility culling
        This function will Visit() for each spatial element or sector it decides is visible in the sector */
    virtual void Visit(nSpatialSector *visitee, int recursedepth)=0;

    /// process a spatial element after it has been determined to be visible.
    virtual void Visit(nSpatialElement *visitee, int recursedepth);

    /** Check if an element should be culled.

        Returns true if the element is (possibly) visible, false if it is not
	visible.

        Mainly used by hierarchy elements. */
    virtual bool VisibilityTest(nSpatialElement *visitee)=0;

    /** @brief get the current viewpoint in local sector space
        Mainly used by hierarchy elements; needed for occlusion or LOD testing */
    vector3 &GetViewPoint() const;

    /** @brief Render some debug visualization.
        Comes in handy for debugging wierd occlusion artifacts. */
    virtual void VisualizeDebug(nGfxServer2 *gfx2);

protected:
    /// we have a stack of viewpoints to handle sector-local coordinate systems
    nArray<vector3> m_viewpointstack;

    /**
       entering a new local space; the matrix given will transform from
       the current local system into the new space local coordinate system.
       This is used to possibly update a transform matrix or to transform
       the spatial region to a new coordinate system.
    */
    virtual void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    virtual void LeaveLocalSpace();
};
#endif
