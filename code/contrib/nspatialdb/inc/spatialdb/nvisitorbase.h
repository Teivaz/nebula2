#ifndef N_VISITORBASE_H
#define N_VISITORBASE_H

// header file for the basic visibility and spatial visitor classes

#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "mathlib/bbox.h"
#include "mathlib/sphere.h"

#include "util/narray.h"

class nSpatialElement;
class nGfxServer2;
class nPlaneClipper;

/** VisitorFlags helper struct.  

    This class contains a boolean flag, for specifying that a primitive is
    visible or contained or whatever.  There is also a "antitest" flag that says an object is
    certainly NOT visible or contained, for when you want that information.
    
    Also within the class is a bunch of bits that be used by the visitor
    for transferring state between calls to the visitor. Using the view frustum visitor as an example, if
    you call VisibilityTest() for a bounding box and that bbox is found to be totally on the 'inside' 
    of one of the frustum sides, the bit flags for that side will be turned off in the returned VisitorFlags
    value.  For nested data structures, you can use this updated VisitorFlags when doing VisibilityTest()
    calls on enclosed nodes; because of the bit flags, these child nodes won't require the full set of
    plane tests.

    You can of course ignore all this for simplicity and always pass in the default constructed VisitorFlags.
    The visitor should still work and return the exact same set of elements, it will just be a bit slower
    because it is doing some redundant tests.  You may also want to avoid the whole bitflag thing when using
    simple heirarchy elements or debugging scaffolding elements.

    The exact meaning of the activeflags varies with the visitor, so don't try to take the VisitorFlags
    result from one visitor and use it in another visitor...
*/
struct VisitorFlags {

    /// by default test=true, antitest=false, such that objects are rejected only if they are known not to be visible
    VisitorFlags(bool c=true, bool d=false, int initflags=~0) : m_test(c), m_antitest(d), m_activeflags(initflags) {}
    VisitorFlags(const VisitorFlags &copyme) 
        : m_test(copyme.m_test), m_antitest(copyme.m_antitest), m_activeflags(copyme.m_activeflags) {}

    bool TestResult() { return m_test; }
    bool AntiTestResult() { return m_antitest; }

    bool m_test, m_antitest;
    unsigned short m_activeflags;

    enum { MAXVISITORFLAGS = 15 };
};

/**
    @class nVisibilityVisitor
    @ingroup NSpatialDBContribModule
    @brief Base class for visitors that collect all elements in a given region and visible from a given viewpoint.

    Various subclasses of this abstract class are used for specific types
    of visibility culling.  To add a new type of visibility visitor:

    1. Subclass this class.  Add in stacks for any data that depends on
    the coordinate system.

    2. Override Visit() to process elements.  This will typically add the element to a list of visible
    objects, draw the visible element, or something similar.

    3. To make hierarchy objects work, override the VisibilityTest(...) methods to
    perform a proper test for the given element.

    4. To handle sectors with different coordinate systems, override
    the Enter/LeaveLocalSpace methods, such that you transform into
    the new sector space and push the new state onto your stacks.

*/
class nVisibilityVisitor {
public:
    typedef nArray<nSpatialElement *> VisibleElements;

    nVisibilityVisitor(const vector3 &viewpoint);
    virtual ~nVisibilityVisitor();

    /** Reset any state use during a visibility sweep.
        If you use a visitor multiple times, reset it between each visibility collection */
    virtual void Reset();
    virtual void Reset(const vector3 &newviewpoint);

    /// Test and possibly process an element.
    virtual void Visit(nSpatialElement *visitee)=0;

    /** Check if an element should be culled.  If the returned VisitorFlags produces
    a TestResult() of true, the element is visible.  The returned VisitorFlags can be used
    for more efficient visibility tests of primitives enclosed in this bounding box */
    virtual VisitorFlags VisibilityTest(const bbox3 &testbox, VisitorFlags flags)=0;
    virtual VisitorFlags VisibilityTest(const sphere &testsphere, VisitorFlags flags)=0;

    /** @brief get the current viewpoint in local sector space
        Mainly used by hierarchy elements; needed for occlusion or LOD testing */
    vector3 &GetViewPoint() const;

    /** @brief Get the detail ratio of invisible Details.
     *  For a given object of size S and distance D from the viewpoint, if (S/D) is less than
     *  the DetailRatio the object or detail is effectively invisible.  This will typically correspond to
     *  a few on-screen pixels. Use this value to throw away small objects, or to determine what LOD level
     *  to use.  The detail ratio lets you ignore sub-pixel or small details that don't show up during
     *  rendering.  It can also be used to represent a creature with bad visionif it is especially large.
     *
     *  The returned detail ratio can be 0 if there is no minimal detail level.
     */
    float GetMinimumDetailRatio() const;
    
    /** @brief Start debug visualization for this frame.
        Comes in handy for debugging wierd occlusion artifacts. You can turn off
        debug visualization by setting this value to NULL (the default) */
    virtual void StartVisualizeDebug(nGfxServer2 *gfx);

    virtual void EndVisualizeDebug();

    /**
       entering a new local space; the matrix given will transform from
       the current local system into the new space local coordinate system.
       This is used to possibly update a transform matrix or to transform
       the spatial region to a new coordinate system.
    */
    virtual void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    virtual void LeaveLocalSpace();

protected:
    /// we have a stack of viewpoints to handle sector-local coordinate systems
    nArray<vector3> m_viewpointstack;

    /// gee,I guess we need a stack of minimal detail values as well.
    nArray<float>   m_minimumdetailratiostack;

    nGfxServer2 *m_gfxdebug;

};

/**
    @class nSpatialVisitor
    @ingroup NSpatialDBContribModule
    @brief Visitor to collect all elements in a given region of space.

*/
class nSpatialVisitor {
public:
    typedef nArray<nSpatialElement *> SpatialElements;

    nSpatialVisitor() : m_gfxdebug(NULL) {
    }
    virtual ~nSpatialVisitor() {
    }

    virtual void Reset();

    /// Process an element.  This adds the element to a list, draws it on the screen, etc.
    virtual void Visit(nSpatialElement *visitee)=0;

    /** Check if an element is in the region represented by this visitor.
    If the returned VisitorFlags produces
    a TestResult() of true, the element is in the region.  The returned VisitorFlags can be used
    for more efficient visibility tests of primitives enclosed in this bounding box
    Mainly used by hierarchy elements.
 
    Note that elements partially in the region may be rejected or
	accepted, depending on the visitor.  Check the docs for that visitor!
    */
    virtual VisitorFlags ContainmentTest(const bbox3 &testbox, VisitorFlags flags) = 0;
    virtual VisitorFlags ContainmentTest(const sphere &testsphere, VisitorFlags flags) = 0;

    /** @brief Specify gfxserver to use for debug visualization.
        Comes in handy for debugging wierd culling artifacts. You can turn off
        debug visualization by setting this value to NULL (the default) */
    virtual void StartVisualizeDebug(nGfxServer2 *gfx);

    virtual void EndVisualizeDebug();

    /**
       entering a new local space; the matrix given will transform from
       the current local system into the new space local coordinate
       system.  This is used to possibly update a transform matrix or
       to transform the spatial region to a new coordinate system.
    */
    virtual void EnterLocalSpace(matrix44 &warp) = 0;

    /// leave a local space
    virtual void LeaveLocalSpace() = 0;

protected:
    nGfxServer2 *m_gfxdebug;

};

/**
    @class nOcclusionVisitor
    @ingroup NSpatialDBContribModule
    @brief Visitor to collect occluders and provide visibility testing using those occluders.

    The OcclusionVisitor is a general visitor class that processes occluders and construct
    some sort of occlusiong representation so that it can determine if other objects
    are occluder or not.  This may be acheived by simply recording all the occluding objects
    and doing a geometrical inspection of potentially visible objects, or it may be acheive
    with occlusion maps or some other method.  The keys attributes of the occlusion visitor are:

    1. Non-compiled.  You can add an occluder, test to see if an object is occluded so far, and
    then add another occluder.  If the occlusion determination needs to compile some sort of internal
    data, it must be able to do so multiple times; there is no requirement to add all the
    occluders and then start doing occlusion tests.  This means you may need to have a flag that
    forces a recompilation of internal data structures whenever a new occluder is added.

    2. Conservative.  If the occlusion determination is not sure a given object is
    totally occluded, it should consider an object visible.  A specific occlusion method can
    do "aggressive culling" and remove objects that are possibly or partially visible, but this
    should be noted in the documentation.  Also, the degree of aggressive culling should be
    controllable through some user parameters.

    3. Deterministic.  If I perform an occlusion test on and object, and then do another
    occlusion test on that same object, it should return the same result unless I have modified
    the occlusion state (for instance, by adding another occluder).  However, if I reset the occlusion
    state and add in all the same occluders again, you can return a different result.  This allows
    the use of temporal coherence if you really want it.

 * This visitor doesn't put any spatial bounds on the occluders it will use.  If you
 * only want occluders in a specific region--such as a view frustum--you will have
 * to provide a properly configured nVisibilityVisitor using SetVisibilityVisitor.
 * Then potential occluding objects are checked against the visibilitytest() of 
 * the visibilityvisitor.  The occlusion visitor will tell it's restricting visibility
 * visitor about local space enter/leave events.
*/
class nOcclusionVisitor : public nVisibilityVisitor {
public:
    nOcclusionVisitor(const vector3 &viewpoint);
    virtual ~nOcclusionVisitor();

    virtual void StartVisualizeDebug(nGfxServer2 *gfx2);

    virtual void EndVisualizeDebug();

    void SetRestrictingVisitor(nVisibilityVisitor *vv) { m_restrictingvisitor = vv; }

    /// Add an occluder shape like an AABB to the occluding set
    virtual void AddBBoxOccluder(const bbox3 &boxoccluder)=0;
    /// Add an occluder shaped like a sphere to the occluding set
    virtual void AddSphereOccluder(const sphere &sphereoccluder)=0;
    /// Add an occluder shaped like a convex hull (represented by a set of halfspaces) to the occluding set
    virtual void AddHullOccluder(const nPlaneClipper &hulloccluder)=0;

    /// Test of a given object AABB is visible or occluded
    virtual VisitorFlags VisibilityTest(const bbox3 &testbox, VisitorFlags flags)=0;
    /// Test of a given object sphere is visible or occluded
    virtual VisitorFlags VisibilityTest(const sphere &testsphere, VisitorFlags flags)=0;

protected:
    // optional visibility visitor used to restrict our occlusion gathering
    nVisibilityVisitor *m_restrictingvisitor;
};

#endif

