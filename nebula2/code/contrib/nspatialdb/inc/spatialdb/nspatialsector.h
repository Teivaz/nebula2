#ifndef N_SPATIALSECTOR_H
#define N_SPATIALSECTOR_H

#include "octree/noctree.h"

//--------------------------------------------------------------------
/**
    @class nSpatialSector
    @ingroup NSpatialDBContribModule
    @brief A collection of elements in a specific region of space.

    A sector represents a section of space with a bunch of objects
    in it.  Note that the sector could be infinite in size, which is
    typically used for top or root-level sectors.  Having finite-size
    sectors is encouraged where possible since it allows the spatial
    querying to work more efficiently.

    You can query a sector with a @ref nVisibilityVisitor or
    @ref nSpatialVisitor, of which there are various types in order
    to perform different kinds of queries.

    Sectors are typically connected to each other by portals.  You
    can also nest sectors within each other using the NOH; a sector
    contains all the sectors that are child nodes of this sector.
*/
class nSpatialSector : public nOctree {
public:
    nSpatialSector();
    ~nSpatialSector();

    void Configure(const matrix44 &transformtoglobalspace);
protected:
};


#endif
