#ifndef N_SPATIALVISITOR_H
#define N_SPATIALVISITOR_H

/**
    @class nSpatialVisitor
    @ingroup NSpatialDBContribModule
    @brief Visitor to collect all elements in a given region of space

*/
class nSpatialVisitor {
public:
    nSpatialVisitor() {
    }
    ~nSpatialVisitor() {
    }

    /** process a sector-override this to do your special spatial culling
        This function will Visit() for each spatial element or sector it decides
        is in the specified region */
    virtual void Visit(nSpatialSector *visitee,int recursiondepth=0);

    /** process an element that is known to be in the region (i.e., don't
        do a clip check)
    */
    virtual void Visit(nSpatialElement *visitee, int recursiondepth=0);

    /** Check if an element is in the region.
        Returns true if the element is in the region, false if the element
	is not in the region.

        Note that elements partially in the region may be rejected or
	accepted, depending on the visitor.  Check the docs for that visitor!

        Mainly used by hierarchy elements. */
    virtual bool ContainmentTest(nSpatialElement *visitee) = 0;

protected:
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

#endif
