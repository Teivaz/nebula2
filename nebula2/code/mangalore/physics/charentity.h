#ifndef PHYSICS_CHARENTITY_H
#define PHYSICS_CHARENTITY_H
//------------------------------------------------------------------------------
/**
    @class Physics::CharEntity

    A specialized Physics entity for characters and NPCs. In the
    physics subsystem, a character is just a sphere rolling on the
    ground with a very high friction. An angular motor is attached
    to the sphere.
    
    (C) 2003 RadonLabs GmbH
*/
#include "foundation/ptr.h"
#include "physics/entity.h"
#include "physics/materialtable.h"
#include "physics/filterset.h"
#include "character/ncharacter2.h"
#include "physics/areaimpulse.h"

//------------------------------------------------------------------------------
namespace Physics
{
class RigidBody;
class BoxShape;
class AMotor;
class Ragdoll;
class Composite;

class CharEntity : public Entity
{
    DeclareRtti;
	DeclareFactory(CharEntity);

public:
    /// constructor
    CharEntity();
    /// destructor
    virtual ~CharEntity();
    /// called when attached to game entity
    virtual void OnActivate();
    /// called when removed from game entity
    virtual void OnDeactivate();
    /// invoked before stepping the simulation
    virtual void OnStepBefore();
    /// called on collision; decide if it is valid
    virtual bool OnCollide(Shape* collidee);
    /// set the current world space transformation
    virtual void SetTransform(const matrix44& m);
    /// get the current world space transformation
    virtual matrix44 GetTransform() const;
    /// get the current world space velocity
    virtual vector3 GetVelocity() const;
    /// set the intended velocity vector
    void SetDesiredVelocity(const vector3& v);
    /// get the intended 2D velocity vector
    const vector3& GetDesiredVelocity() const;
    /// set the desired lookat vector
    void SetDesiredLookat(const vector3& v);
    /// get the desired lookat vector
    const vector3& GetDesiredLookat() const;
    /// set radius
    void SetRadius(float r);
    /// get radius
    float GetRadius() const;
    /// set height
    void SetHeight(float h);
    /// get height
    float GetHeight() const;
    /// set hover of capsule
    void SetHover(float h);
    /// get hover of capsule
    float GetHover() const;
    /// set pointer to Nebula2 character object
    void SetCharacter(nCharacter2* chr);
    /// get pointer to Nebula2 character
    nCharacter2* GetCharacter() const;
    /// set an impulse to apply on ragdoll when it is activated
    void SetRagdollImpulse(AreaImpulse* imp);
    /// activate the ragdoll
    void ActivateRagdoll();
    /// deactivate the ragdoll
    void DeactivateRagdoll();
    /// return true if ragdoll currently active
    bool IsRagdollActive() const;

    /// return the material of the ground the character stands currently on
    MaterialType GetGroundMaterial() const;
    /// get ground exclude set
    FilterSet& GetGroundExcludeSet();
    
protected:
    /// return distance to ground (only within ball radius)
    bool CheckGround(float& dist);
    /// create the default composite, used when character is alive
    void CreateDefaultComposite();
    /// create optional ragdoll composite
    void CreateRagdollComposite();

    vector3 desiredVelocity;
    vector3 lookatDirection;    // looks into last valid desiredVelocity direction
    vector3 angularVelocity;
    bool angularVelocityDirty;
    Ptr<RigidBody> baseBody;
    Ptr<AMotor> aMotor;
    Ptr<Composite> defaultComposite;
    Ptr<Ragdoll> ragdollComposite;
    float radius;
    float height;
    float hover;
    nCharacter2* nebCharacter;
    bool ragdollActive;
    MaterialType groundMaterial;
    FilterSet groundExcludeSet;

    Ptr<AreaImpulse> ragdollImpulse;
};

RegisterFactory(CharEntity);

//------------------------------------------------------------------------------
/**
*/
inline
void
CharEntity::SetRagdollImpulse(AreaImpulse* imp)
{
    this->ragdollImpulse = imp;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
CharEntity::IsRagdollActive() const
{
    return this->ragdollActive;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CharEntity::SetCharacter(nCharacter2* chr)
{
    if (this->nebCharacter)
    {
        this->nebCharacter->Release();
        this->nebCharacter = 0;
    }
    if (chr)
    {
        this->nebCharacter = chr;
        this->nebCharacter->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharacter2*
CharEntity::GetCharacter() const
{
    return this->nebCharacter;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CharEntity::SetDesiredVelocity(const vector3& v)
{
    this->desiredVelocity = v;
    this->angularVelocityDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
CharEntity::GetDesiredVelocity() const
{
    return this->desiredVelocity;
}

//------------------------------------------------------------------------------
/**
    Sets the desired lookat direction of the char physics entity. Vector
    must have length greater 0, otherwise this lookatDirection will not
    be changed.
*/
inline
void
CharEntity::SetDesiredLookat(const vector3& v)
{
    if (v.len() > 0)
    {
        this->lookatDirection = v;
        this->lookatDirection.norm();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
CharEntity::GetDesiredLookat() const
{
    return this->lookatDirection;
}

//------------------------------------------------------------------------------
/**
    Set the radius of the character on the x/z plane.
*/
inline
void
CharEntity::SetRadius(float r)
{
    this->radius = r;
}

//------------------------------------------------------------------------------
/**
    Get the radius of the character.
*/
inline
float
CharEntity::GetRadius() const
{
    return this->radius;
}

//------------------------------------------------------------------------------
/**
    Set the height of the character. Must be at least 2x radius.
*/
inline
void
CharEntity::SetHeight(float h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CharEntity::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CharEntity::SetHover(float h)
{
    this->hover = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CharEntity::GetHover() const
{
    return this->hover;
}

//------------------------------------------------------------------------------
/**
*/
inline
MaterialType
CharEntity::GetGroundMaterial() const
{
    return this->groundMaterial;
}

//------------------------------------------------------------------------------
/**
*/
inline
FilterSet&
CharEntity::GetGroundExcludeSet()
{
    return this->groundExcludeSet;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
    
