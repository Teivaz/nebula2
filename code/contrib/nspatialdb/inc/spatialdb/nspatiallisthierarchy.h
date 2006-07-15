#ifndef N_SPATIALLIST_H
#define N_SPATIALLIST_H

#include "nspatialelements.h"

/**
 *  @class nSpatialListHierarchy
 *  @ingroup NSpatialDBContribModule
 *  @brief A collection of elements stored as a list
 *
 *  This is pretty much the most basic spatial heirarchy 
 *  you can implement, which is not really a heirarchy at all.
 *  The set of spatial elements are stuck
 *  into a list.  For processing visitors, the spatiallist simply
 *  calls @c Accept() on the all the spatial elements, relying on the
 *  elements themselves to figure out visibility/occlusion and whatnot.
 *
 *  Obviously more efficient spatial heirarchy data structures exist, but
 *  this method is simple and doesn't have the overhead of more complicated
 *  spatial heirarchies, making it useful for compiling and processing small
 *  sets of spatial elements.
 */
class nSpatialListHierarchy : public nSpatialHierarchyElement
{
public:
    nSpatialListHierarchy();
    ~nSpatialListHierarchy();

    /// Clear out spatial element references.  Elements are not deleted!
    void ClearList();

    /// Append an element to the list of elements
    void AddElement(nSpatialElement *appendme);
    /// Remove a given element
    void RemoveElement(nSpatialElement *removeme);

    /// Process the list of elements
    virtual void Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags);
    /// Process the list of elements
    virtual void Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags);
    /// Process the list of elements
    virtual void Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags);

protected:
    nList m_ElementList;
};

#endif
