#ifndef ATTR_ATTRIBUTE_ID_H
#define ATTR_ATTRIBUTE_ID_H
//------------------------------------------------------------------------------
/**
    @class AttributeID

    The AttributeID class is used to add new attribute ID to Your
    application. Those can then be used to add attributes to Your Entities.
    Also see db::Atrribute for usage of Attributes.

    (C) 2005 Radon Labs GmbH

    usage:

    - to declare(add) a new attribute ID use:
      
      DeclareBool( attributeTypeName );
      DeclareString( attributeTypeName );
      ..

      ( possible types are: Void, Bool, Int, Float, String, Vector3, Vector4, Matrix44 )

    - you then have to define the attribute ID with:
      
      DefineBool( attributeTypeName );
      DefineString( attributeTypeName );

      or 

      DefineBoolStorable( attributeTypeName );
      DefineStringStorable( attributeTypeName );

    - e.g.:
      
      DeclareIntAttribute( Hitpoints );
      DefineIntAttribute( Hitpoints );
*/
#include "attr/_attridtyped.h"
#include "attr/attributeclasses.h"

namespace Attr
{
    /// data types
    enum Type
    {
        Void    = nVariant::Void,
        Int     = nVariant::Int,
        Float   = nVariant::Float,
        Bool    = nVariant::Bool,
        Vector3 = nVariant::Vector3,
        Vector4 = nVariant::Vector4,
        String  = nVariant::String,
        Matrix44= nVariant::Matrix44,
        None
    };

class AttributeID
{
public:
    AttributeID();
    AttributeID(const AttributeID&);
    AttributeID(const _attrid*);
    AttributeID(const nString& name); // must exist!
    virtual ~AttributeID();

    /// assignment operator
    void operator=(const AttributeID& rhs);
    /// equality operator
    friend bool operator==(const AttributeID& lhs, const AttributeID& rhs);
    /// inequality operator
    friend bool operator!=(const AttributeID& lhs, const AttributeID& rhs);
    /// check for valid ID
    bool IsValid() const;


    /// get id name
    const nString& GetName() const;
    /// get id data type
    Type GetType() const;
    /// return true if storable
    bool IsStorable() const;
    /// return true if writable
    bool IsWritable() const;


    /// find untyped attribute id by name
    static AttributeID FindAttributeID(const nString& name);

protected:
    const _attrid* attridPtr;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool operator==(const AttributeID& lhs, const AttributeID& rhs)
{
    return lhs.attridPtr == rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool operator!=(const AttributeID& lhs, const AttributeID& rhs)
{
    return lhs.attridPtr != rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
AttributeID::IsValid() const
{
    return 0 != this->attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
AttributeID::operator=(const AttributeID& rhs)
{
    this->attridPtr = rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
AttributeID::~AttributeID()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
AttributeID::AttributeID() : 
    attridPtr(0)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
AttributeID::AttributeID(const _attrid* aip) :
    attridPtr(aip)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
AttributeID::AttributeID(const AttributeID& aId)
{
    this->attridPtr = aId.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
AttributeID::AttributeID(const nString& name)
{
    const AttributeID& existingID = AttributeID::FindAttributeID(name);
    
    if ( !existingID.IsValid() )
    {
        n_error("Error: Attribute ID of name \"%s\" not found!", name.Get());
    }

    this->attridPtr = existingID.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
AttributeID::GetName() const
{
    return this->attridPtr->GetName();
}

//------------------------------------------------------------------------------
/**
*/
inline
Type 
AttributeID::GetType() const
{
    return (Type) this->attridPtr->GetType();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
AttributeID::IsStorable() const
{
    return this->attridPtr->IsStorable();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
AttributeID::IsWritable() const
{
    return this->attridPtr->IsWritable();
}

} // namespace Attr

//typedef const _attrid* AttributeID;

//------------------------------------------------------------------------------
#endif
