#ifndef N_GENARRAYVISITORS_H
#define N_GENARRAYVISITORS_H

// this file contains subclasses of the various visitors that take the found
// nSpatialElements and put them into an array container that is specified
// by the user.  Note that all of these "GenArray" classes are designed such
// that you use them by performing a Reset(), then an Accept() on the target
// sector/hierarchy to get a new set of found objects in
// the array.  Without the Reset(), doing multiple visits will just append found
// objects. This appending behavior can be used in some situations where, for example, you want to get 
// all the objects found in two sphere regions.


#include "spatialdb/nvisiblefrustumvisitor.h"
#include "spatialdb/noccludedfrustumvisitor.h"
#include "spatialdb/nvisiblespherevisitor.h"

/**
    @class nVisibleFrustumGenArray
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view frustum and populates
    an nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

class nVisibleFrustumGenArray : public nVisibleFrustumVisitor {
public:
    nVisibleFrustumGenArray(const nCamera2 &cam, const matrix44 &camxform, VisibleElements &foundarray)
		: nVisibleFrustumVisitor(cam, camxform), m_visarray(foundarray)	{ }

    ~nVisibleFrustumGenArray() { }

	void Reset() { ClearArray(); nVisibleFrustumVisitor::Reset(); }

	void Visit(nSpatialElement *visitee) { m_visarray.Append(visitee); }


protected:
	void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};

class nOccludedFrustumGenArray : public nOccludedFrustumVisitor {
public:
    nOccludedFrustumGenArray(const nCamera2 &cam, 
        const matrix44 &camxform, 
        nOcclusionVisitor &occlusion,
        VisibleElements &foundarray)
        : nOccludedFrustumVisitor(cam, camxform, occlusion), m_visarray(foundarray) { }

    ~nOccludedFrustumGenArray() {}

    void Reset() { ClearArray(); nOccludedFrustumVisitor::Reset(); }
    void Reset(const nCamera2 &newcamera, const matrix44 &newxform)
    { ClearArray(); nOccludedFrustumVisitor::Reset(newcamera, newxform); }


    void Visit(nSpatialElement *visitee) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};

class nVisibleSphereGenArray : public nVisibleSphereVisitor {
public:
    nVisibleSphereGenArray(const sphere &viewsphere, VisibleElements &foundarray)
        : nVisibleSphereVisitor(viewsphere), m_visarray(foundarray) {}
    ~nVisibleSphereGenArray() { }

	void Reset() { ClearArray(); nVisibleSphereVisitor::Reset(); }
    void Reset(const sphere &s) { ClearArray(); nVisibleSphereVisitor::Reset(s); }

	void Visit(nSpatialElement *visitee) { m_visarray.Append(visitee); }

protected:
	void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};

#endif

