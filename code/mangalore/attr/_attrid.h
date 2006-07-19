#ifndef ATTR__ATTRID_H
#define ATTR__ATTRID_H
//------------------------------------------------------------------------------
/**
    @brief private helper class for the Mangalore attribute system

    Private helper class for the Mangalore attribute system. This adds
    a data type to the generic _id class. The class is used as a base class
    for _attridTyped which adds compile time type information to the attribute 
    ids.
    
    (C) 2005 Radon Labs GmbH
*/
#include "util/narray.h"
#include "util/nstring.h"
#include "util/nvariant.h"
#include "attr/attributeclasses.h"

/// size of the id tables, i.e. max. number of ids of each type
const int ID_REGISTRY_SIZE = 256;

class _attrid
{
public:
    /// data types
    enum Type
    {
        Void     = nVariant::Void,
        Int      = nVariant::Int,
        Float    = nVariant::Float,
        Bool     = nVariant::Bool,
        Vector3  = nVariant::Vector3,
        Vector4  = nVariant::Vector4,
        String   = nVariant::String,
        Matrix44 = nVariant::Matrix44,
    };
    /// writable/storable flags
    enum
    {
        Read  = (1<<0),
        Write = (1<<1),
        Store = (1<<2),
    };

    /// constructor
    _attrid(const char* name, Type t, uchar flags);
    /// destructor
    //~_attrid();

    /// get id name
    const nString& GetName() const;
    /// get id data type
    Type GetType() const;
    /// return true if storable
    bool IsStorable() const;
    /// return true if writable
    bool IsWritable() const;

protected:
    /// default constructor is protected
    _attrid() = 0;

    nString name;
    Type type;
    uchar flags;
};

//------------------------------------------------------------------------------
/**
*/
inline
_attrid::_attrid(const char* n, Type t, uchar f) :
    name(n),
    type(t),
    flags(f)
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
inline
_attrid::Type
_attrid::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
_attrid::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
_attrid::IsStorable() const
{
    return 0 != (this->flags & Store);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
_attrid::IsWritable() const
{
    return 0 != (this->flags & Write);
}

//------------------------------------------------------------------------------
#endif
