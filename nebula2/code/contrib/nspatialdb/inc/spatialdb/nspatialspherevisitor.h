
#ifndef N_SPATIALSPHEREVISITOR_H
#define N_SPATIALSPHEREVISITOR_H

/**
    @class nSpatialSphereVisitor
    @ingroup NSpatialDBContribModule
    @brief Visits all elements within a given sphere region; useful for finding all objects within distance N of a point.

    This visitor finds all the elements that are fully or partially within a sphere of a given
    position and radius.

    The visitor will call Visit() for every element determined to be fully or partially within the sphere.
    */

#include "spatialdb/nspatialvisitor.h"
#include "spatialdb/nsphereclipper.h"
#include "spatialdb/nspatialelements.h"
#include "spatialdb/nspatialsector.h"

class nSpatialSphereVisitor : public nSpatialVisitor {
public:
    nSpatialSphereVisitor(const sphere &sphereregion);
    ~nSpatialSphereVisitor();

    void Reset();

    void Visit(nSpatialSector *visitee, int recursiondepth);

    bool ContainmentTest(nSpatialElement *visitee);

protected:
    // changes if we enter a sector with different local coordinate system
    nArray<sphere> m_sphereregionstack;
    nArray<nSphereClipper> m_sphereclipperstack;

    /// gets the current sphere clipper used
    nSphereClipper &GetSphereClipper() const;

    // entering a new local space; the matrix given will transform from the current local system into
    // the new space local coordinate system.  This is used to possibly update a transform matrix
    // or to transform the spatial region to a new coordinate system.
    virtual void EnterLocalSpace(matrix44 &warp);

    /// leave a local space
    virtual void LeaveLocalSpace();

    /// recursive descent of the octree embedded inside a sector
    void CheckOctNode(nOctNode *testnode, nSphereClipper &clipper, nSphereClipper::result_info clipstatus, int recursivedepth);
};

inline
nSphereClipper &nSpatialSphereVisitor::GetSphereClipper() const
{
    n_assert(m_sphereclipperstack.Size() > 0);
    return m_sphereclipperstack.Back();
}

#endif

