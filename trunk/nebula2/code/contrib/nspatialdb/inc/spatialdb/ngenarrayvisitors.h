#ifndef N_GENARRAYVISITORS_H
#define N_GENARRAYVISITORS_H

// this file contains subclasses of the various visitors that take the found
// nSpatialElements and put them into an array container that is specified
// by the user.  Note that all of these "GenArray" classes are designed such
// that you do a Reset() and Visit() to get a new set of found objects in
// the array.  In particular, the array is cleared when a GenArray-style
// visitor is destroyed!  For other behavior such as appending found objects
// to a list non-destructively, you can construct your own subclasses based
// upon these examples


/**
    @class nVisibleFrustumGenArray
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view frustum and populates
    an nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "spatialdb/nvisiblefrustumvisitor.h"

class nVisibleFrustumGenArray : public nVisibleFrustumVisitor {
public:
    nVisibleFrustumGenArray(const nCamera2 &cam, const matrix44 &camxform, VisibleElements &foundarray)
		: nVisibleFrustumVisitor(cam, camxform), m_visarray(foundarray)	{ }

    ~nVisibleFrustumGenArray() { }

	void Reset() { ClearArray(); nVisibleFrustumVisitor::Reset(); }

    using nVisibleFrustumVisitor::Visit;

	void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }


protected:
	void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};


/**
    @class nOccludingFrustumGenArray
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a view frustum and populates
    an nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "spatialdb/noccludingfrustumvisitor.h"

class nOccludingFrustumGenArray : public nOccludingFrustumVisitor {
public:
    nOccludingFrustumGenArray(const nCamera2 &cam, const matrix44 &camxform, VisibleElements &foundarray)
		: nOccludingFrustumVisitor(cam, camxform), m_visarray(foundarray) { }

	~nOccludingFrustumGenArray() { }

    virtual void Reset() { ClearArray(); }

    virtual void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};


/**
    @class nVisibleSphereGenArray
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a sphere and populates an
    nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "spatialdb/nvisiblespherevisitor.h"

class nVisibleSphereGenArray : public nVisibleSphereVisitor {
public:
    nVisibleSphereGenArray(const sphere &viewsphere, VisibleElements &foundarray)
        : nVisibleSphereVisitor(viewsphere), m_visarray(foundarray) { }

    ~nVisibleSphereGenArray() { }

    virtual void Reset() { ClearArray(); }

    virtual void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};

/**
    @class nOccludingSphereGenArray
    @ingroup NSpatialDBContribModule
    @brief Visits all elements visible in a sphere and populates an
    nArray with the visible elements.

    This is a simple example of how to override visibilityvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "spatialdb/noccludingspherevisitor.h"

class nOccludingSphereGenArray : public nOccludingSphereVisitor {
public:
    nOccludingSphereGenArray(const sphere &viewsphere, VisibleElements &foundarray)
        : nOccludingSphereVisitor(viewsphere), m_visarray(foundarray) { }

    ~nOccludingSphereGenArray() { }

    virtual void Reset() { ClearArray(); }

    virtual void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    VisibleElements &m_visarray;
};

/**
    @class nSpatialSphereGenArray
    @ingroup NSpatialDBContribModule
    @brief Visits all elements contained in a sphere and populates an
    nArray with the visible elements.

    This is a simple example of how to override spatialvisitor.  We
    just override Visit() to append the given element to the array
*/

#include "spatialdb/nspatialspherevisitor.h"

class nSpatialSphereGenArray : public nSpatialSphereVisitor {
public:
    nSpatialSphereGenArray(const sphere &viewsphere, SpatialElements &foundarray)
        : nSpatialSphereVisitor(viewsphere), m_visarray(foundarray) { }

    ~nSpatialSphereGenArray() { }

    virtual void Reset() { ClearArray(); }

    virtual void Visit(nSpatialElement *visitee, int recursedepth) { m_visarray.Append(visitee); }

protected:
    void ClearArray() { m_visarray.Clear(); }

    SpatialElements &m_visarray;
};



#endif

