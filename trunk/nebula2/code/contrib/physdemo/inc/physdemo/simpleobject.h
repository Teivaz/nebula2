#ifndef SIMPLEOBJECT_H
#define SIMPLEOBJECT_H
//------------------------------------------------------------------------------
/**
    @class SimpleObject

    @brief A simple object designed to hold data relevant to a rigid body

    This is a simple object container designed to hold all data relevant to a rigid body existing
    in the PhysDemo world.  It also serves as a (very) basic example of how entities might function in
    user-made applications, and how they would integrate with Nebula 2.

    (C)	2003	Megan Fox
*/
#include "kernel/nroot.h"
#include "scene/nrendercontext.h"
#include "scene/nshapenode.h"

#include "opende/nopendebody.h"
#include "opende/nopendegeom.h"

//------------------------------------------------------------------------------
class SimpleObject : public nRoot
{
public:
    /// constructor
    SimpleObject();
    /// destructor
    virtual ~SimpleObject();
    
    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

    /// release method (to release all the referenced nRef's)
    bool Release();

    /// variable handles for the render context
    nVariable::Handle timeHandle;

    // The object's render context
    nRenderContext renderContext;
    // The root node of the object's nShapeNode hierarchy (usually only 1 node)
    nRef<nShapeNode> refRootShapeNode;
    // The object's universal ID.
    int uID;

    // The object's physical representation
    nRef<nOpendeBody> refPhysBody;
    // The object's collision representation
    nRef<nOpendeGeom> refPhysGeom;
};
//------------------------------------------------------------------------------
#endif

