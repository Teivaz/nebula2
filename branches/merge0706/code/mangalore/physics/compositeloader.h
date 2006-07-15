#ifndef PHYSICS_COMPOSITELOADER_H
#define PHYSICS_COMPOSITELOADER_H
//------------------------------------------------------------------------------
/**
    @class Physics::CompositeLoader

    Creates a Physics::Composite from an XML stream.

    (C) 2005 RadonLabs GmbH
*/
#include "physics/composite.h"
#include "util/nstream.h"

//------------------------------------------------------------------------------
namespace Physics
{
class CompositeLoader
{
public:
    /// constructor
    CompositeLoader();
    /// destructor
    ~CompositeLoader();
    /// construct composite from xml file
    Composite* Load(const nString& name);

private:
    /// load from physics file
    bool LoadPhysicsXml(const nString& filename);
    /// load optional from collide mesh
    bool LoadCollideMesh(const nString& filename);
    /// parse rigid body names of a joint and set on joint
    void ParseJointRigidBodies(Composite* composite, const nStream& stream, Joint* joint);
};

} // namespace Physics
//------------------------------------------------------------------------------
#endif
