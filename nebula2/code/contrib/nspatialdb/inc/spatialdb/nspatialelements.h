#ifndef N_SPATIALELEMENT_H
#define N_SPATIALELEMENT_H

#include "octree/noctree.h"

class nVisibilityVisitor;
class nSpatialVisitor;
class nSpatialSector;

//--------------------------------------------------------------------
/**
    @class nSpatialElement
    @ingroup NSpatialDBContribModule
    @brief The interface for elements contained within sector.

    A spatial element represents some sort of object within a sector.  This
    could be a simple object, a portal, or something else entirely.
*/
class nSpatialElement : public nOctElement {
public:
    enum {
        N_SPATIAL_PORTAL     = (1<<0), /// points into another sector
        N_SPATIAL_HIERARCHY  = (1<<1), /// hold sub-objects, has special processing via virtual calls
        N_SPATIAL_OCCLUDER   = (1<<2), /// acts as an occluder
    };
    typedef int nSpatialElementType;

    nSpatialElement() : nOctElement(), m_spatialtype(0) {
    };
    virtual ~nSpatialElement() {
    }
    nSpatialElementType GetSpatialType() const { return m_spatialtype; }

    virtual void Accept(nVisibilityVisitor &visitor, int recursiondepth);
    virtual void Accept(nSpatialVisitor &visitor, int recursiondepth);

protected:
    int m_spatialtype;
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
class nSpatialPortal : public nSpatialElement {
    nSpatialPortal(nSpatialSector *parentsector, nSpatialSector *connecttosector) : 
        nSpatialElement(), m_parentsector(parentsector), m_otherside(connecttosector)
    {
        m_spatialtype |= N_SPATIAL_PORTAL;
    }
    ~nSpatialPortal() {
    }

    virtual void Accept(nVisibilityVisitor &visitor, int recursiondepth);
    virtual void Accept(nSpatialVisitor &visitor, int recursiondepth);

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
    nSpatialHierarchyElement() : nSpatialElement() {
        m_spatialtype |= N_SPATIAL_HIERARCHY;
    }
    ~nSpatialHierarchyElement() {
    }

    // note you will need to subclass and override the Visit() methods!

protected:
};

//----------------------------------------------------------------------
/**
    @class nSpatialOccluderElement
    @ingroup NSpatialDBContribModule
    @brief The interface for elements that function as occluders.

    This is for objects that represent occluders of some sort.  An
    occluder element renders object behind it (from the point-of-view
    of the viewer) non-visible, so it is used by occluding visibility
    visitors.  The occluder is pretty much ignored the rest of the time.
*/
class nSpatialOccluderElement : public nSpatialElement {

public:
    nSpatialOccluderElement() : nSpatialElement() {
        m_spatialtype |= N_SPATIAL_OCCLUDER;
    }
    ~nSpatialOccluderElement() {
    }
};

#endif
