#ifndef N_SPATIALELEMENT_H
#define N_SPATIALELEMENT_H


#include "nvisitorbase.h"
#include "kernel/nref.h"


class nSpatialSector;

//--------------------------------------------------------------------
/**
    @class nSpatialElement
    @ingroup NSpatialDBContribModule
    @brief The interface for elements contained within sector.

    A spatial element represents some sort of object within a sector.  This
    could be a simple object, a portal, or something else entirely.

    The bounding box data member is stuck here instead of some subclass for speed
    since it is used by many spatial tree data structures.  The AABB stored is a
    conservative bounding box, guaranteed to enclose the entire element.  Since
    the actual element may be much smaller for occlusion or visibility purposes, you
    have the ability to write some more sophisticated code in the Accept() methods,
    setting the AABB value for use with octree/kd-tree/bsp processors.  
    The BBoxSpatialElement simply uses the stored AABB value for visibility purposes.
 
*/
class nSpatialElement
{
public:
    enum {
        // extend with your own element types in a subclass
        N_SPATIAL_PORTAL     = (1<<0), /// points into another sector
        N_SPATIAL_HIERARCHY  = (1<<1), /// hold sub-objects, has special processing via virtual calls
        N_SPATIAL_OCCLUDER   = (1<<2), /// contains occlusion information
        N_SPATIAL_AABB       = (1<<3), /// contains AABB information
    };
    typedef int nSpatialElementType;

    nSpatialElement(nSpatialElementType starttype=0) : m_spatialtype(starttype), m_AABB(), m_ptr(NULL) {};
    nSpatialElement(nSpatialElementType starttype, const bbox3 &initbbox) 
        : m_spatialtype(starttype | N_SPATIAL_AABB), m_AABB(initbbox), m_ptr(NULL) { };
    virtual ~nSpatialElement() { }

    nSpatialElementType GetSpatialType() const { return m_spatialtype; }

    /// If this returns true, then there is occlusion info in this element
    bool HasOccluder() const { return (m_spatialtype & N_SPATIAL_OCCLUDER) != 0; }

    /// If this returns true, then there is valid AABB info in this element
    bool HasAABB() const  { return (m_spatialtype & N_SPATIAL_AABB)     != 0; }
    bbox3 GetAABB() const { return m_AABB; }
    void SetAABB(const bbox3 &newbbox) { m_AABB=newbbox; }

    void *GetPtr() const { return m_ptr; }
    void SetPtr(void *ptr) { m_ptr = ptr; }

    /** get processed by a visitor.  The element can do some tests, call nested objects, etc.
        and then will call Visit() on the visitor if this element should be considered
        "visible" or "contained" as the case may be. the Visitor then processes the element in
        some visitor-dependent manner.
        */
    virtual void Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags);
    virtual void Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags);
    virtual void Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags);

protected:
    int m_spatialtype;
    bbox3 m_AABB;
    void *m_ptr;
};

/**
   @class nBBoxSpatialElement
   @ingroup NSpatialDBContribModule
   @brief A class that adds bounding-box testing to a given spatial element

    This templated class adds extra stuff to a given spatial element, specifically
    it stores a bounding box and uses that to do visibility and containment tests on
    visitors, such that this element is processed only if the bounding box is visible
    or contained in the spatial region.  Stick this onto a generic (spatially-undefined)
    spatial element to make the spatial element a bounding-box based element

    The BBoxSpatialElement simply uses the stored AABB value for visibility and occlusion
    purposes.  If
    your object is not well represented by its AABB you should implement your own
    Accept() methods with different visibility and occlusion code.  The AABB should still be set 
    properly so that spatial data structures have something to work with.

    Example:
    @code
    typedef BBoxSpatialElement<nSpatialPortal> nBoxPortalElement;

    nBoxPortalElement portal(bboxlocation); // a bbox-based portal
    portal.Accept(viewfrustumvisitor);
    @endcode

*/
class nBBoxSpatialElement : public nSpatialElement
{
public:
    nBBoxSpatialElement(nSpatialElementType t, const bbox3 &initialspace) : nSpatialElement(t | N_SPATIAL_AABB, initialspace) { }
    nBBoxSpatialElement(const bbox3 &initialspace) : nSpatialElement(N_SPATIAL_AABB, initialspace) { }
    ~nBBoxSpatialElement();

    void UpdateBBox(const bbox3 &newbox) { m_AABB = newbox; }

    virtual void Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags);
    virtual void Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags);
    virtual void Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags);

protected:
};

//---------------------------------------------------------------------
/**
    @class nSpatialPortal
    @ingroup NSpatialDBContribModule
    @brief A visibility interface between two sectors.

    A sector conveys a spatial relationship between two sectors; that they
    are connected spatially through the portal, and that one sector is
    visible to another sector through the portal.  This means especially
    the visibility routine to completely ignore a sector if none of the
    portals connecting to that sector are visible.

    The portal is not considered a spatial hierarchy element because
    it obeys special rules; more notably, spatial queries ignore portals
    altogether and use sector nesting and bounding boxes instead.
    Visibility queries cull away non-visible portals and use the visible
    portals to determine what other sectors are visible.
*/

class nSpatialPortalElement : public nBBoxSpatialElement {
public:
    nSpatialPortalElement(const bbox3 &initbbox, nSpatialSector *parentsector, nSpatialSector *connecttosector) : 
        nBBoxSpatialElement(N_SPATIAL_PORTAL, initbbox), m_parentsector(parentsector), m_otherside(connecttosector)
    {
    }

    ~nSpatialPortalElement() {
    }

    virtual void Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags);
    virtual void Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags);
protected:
    nRef<nSpatialSector> m_parentsector, m_otherside;
};


//--------------------------------------------------------------------
/**
    @class nSpatialHierarchyElement
    @ingroup NSpatialDBContribModule
    @brief The interface for elements that contain sub-elements in some
    special manner.

    This is for custom element containment methods.  A hierarchy element
    contains sub-elements as well as some custom code to specially process
    those elements and decide which ones exist and which don't.  

    The query visitor will call one of the given query functions, depending
    on whether or not it is a spatial or a visibility query.  When
    subclassing you should override the two query functions to properly
    decide one what elements to process.  You can get data from the visitor
    (such as the viewpoint, for visibility visitors) to help decide.
    Then for each element, invoke Visit() on the visitor.

    You can of course nest hierarchy elements inside other hierarchy
    elements, allowing you to produce an octree or quadtree type of
    structure.  For a visibility query, typically the hierarchy will
    terminate at varying levels depending on the LOD required.
*/
class nSpatialHierarchyElement : public nSpatialElement {
public:

    nSpatialHierarchyElement() : nSpatialElement(nSpatialElement::N_SPATIAL_HIERARCHY)
    {
    }
    ~nSpatialHierarchyElement() {
    }

    // note you will need to subclass and override the Accept() methods!

protected:
};

//----------------------------------------------------------------------
/**
    @class nSpatialOccluder
    @ingroup NSpatialDBContribModule
    @brief The interface for occluder information.

    This is for objects that represent occluders of some sort.  An
    occluder element renders object behind it (from the point-of-view
    of the viewer) non-visible, so it is used by occluding visibility
    visitors.  The occluder is pretty much ignored the rest of the time.

    The occluder itself can then talk to an OcclusionStrategy to describe the occluder
    primitive(s) represented by this occluder.
*/

class nSpatialOccluderElement : public nBBoxSpatialElement
{

public:
    nSpatialOccluderElement(const bbox3 &box)
        : nBBoxSpatialElement(nSpatialElement::N_SPATIAL_OCCLUDER, box)  { }

    virtual ~nSpatialOccluderElement() {
    }

    void Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags);

protected:
};

#endif
