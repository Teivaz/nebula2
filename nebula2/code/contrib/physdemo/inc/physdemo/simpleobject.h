#ifndef SIMPLEOBJECT_H
#define SIMPLEOBJECT_H
//------------------------------------------------------------------------------
/**
    @class SimpleObject
    @ingroup PhysDemoContribModule
    @brief A simple object designed to hold data relevant to a rigid body

    This is a simple object container designed to hold all data relevant to
    a rigid body existing in the PhysDemo world.  It also serves as a (very)
    basic example of how entities might function in user-made applications,
    and how they would integrate with Nebula 2.

    (C) 2003    Megan Fox
*/
#include "kernel/nroot.h"
#include "mathlib/transform44.h"
#include "scene/nrendercontext.h"

#include "gui/nguitextlabel.h"

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

    /// used to manipulate the object's transform (position, rotation, etc.)
    transform44 Transform;

    // The object's render context
    nRenderContext renderContext;

    // The object's physical representation
    nRef<nOpendeBody> refPhysBody;
    // The object's collision representation
    nRef<nOpendeGeom> refPhysGeom;

    // Initialize this with the proper nGuiTextLabel pointer if the object is to have floaty text
    nRef<nGuiTextLabel> refFloatyText;
    float textWidth, textHeight;
    int disableTimeout;
};
//------------------------------------------------------------------------------
#endif

