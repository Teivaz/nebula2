#ifndef PHYSICS_FILTERSET_H
#define PHYSICS_FILTERSET_H
//------------------------------------------------------------------------------
/**
    @class Physics::FilterSet

    A filter set object allows to define conditions which define a set of
    physics entities. It is used to include or exclude physics entities
    from various tests (like stabbing checks).

    (C) 2004 RadonLabs GmbH
*/
#include "util/narray.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Shape;

class FilterSet
{
public:
    /// constructor
    FilterSet();
    /// destructor
    virtual ~FilterSet();
    /// clear the filter set
    void Clear();
    /// add an entity unique id to the set
    void AddEntityId(uint id);
    /// access to entity id's
    const nArray<uint>& GetEntityIds() const;
    /// add a rigid body unique id to the set
    void AddRigidBodyId(uint id);
    /// access to rigid body id's
    const nArray<uint>& GetRigidBodyIds() const;
    /// add a material type to the set
    void AddMaterialType(uint t);
    /// access to material id's
    const nArray<uint>& GetMaterialTypes() const;
    /// return true if entity id is in set
    bool CheckEntityId(uint id) const;
    /// return true if rigid body id is in set
    bool CheckRigidBodyId(uint id) const;
    /// return true if material type is in set
    bool CheckMaterialType(uint t) const;
    /// check a shape for exlusion
    bool CheckShape(Shape* shape) const;

private:
    nArray<uint> entityIds;
    nArray<uint> rigidBodyIds;
    nArray<uint> materialTypes;
};

//------------------------------------------------------------------------------
/**
*/
inline
FilterSet::FilterSet()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
FilterSet::~FilterSet()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FilterSet::Clear()
{
    this->entityIds.Clear();
    this->rigidBodyIds.Clear();
    this->materialTypes.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FilterSet::AddEntityId(uint id)
{
    this->entityIds.Append(id);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<uint>&
FilterSet::GetEntityIds() const
{
    return this->entityIds;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FilterSet::AddRigidBodyId(uint id)
{
    this->rigidBodyIds.Append(id);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<uint>&
FilterSet::GetRigidBodyIds() const
{
    return this->rigidBodyIds;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FilterSet::AddMaterialType(uint t)
{
    this->materialTypes.Append(t);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<uint>&
FilterSet::GetMaterialTypes() const
{
    return this->materialTypes;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
FilterSet::CheckEntityId(uint id) const
{
    return (this->entityIds.Find(id) != 0);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
FilterSet::CheckRigidBodyId(uint id) const
{
    return (this->rigidBodyIds.Find(id) != 0);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
FilterSet::CheckMaterialType(uint id) const
{
    return (this->materialTypes.Find(id) != 0);
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
