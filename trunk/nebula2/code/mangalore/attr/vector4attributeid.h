#ifndef ATTR_VECTOR4_ATTRIBUTE_ID_H
#define ATTR_VECTOR4_ATTRIBUTE_ID_H
//------------------------------------------------------------------------------
/**
    @class Vector4AttributeID

    @brief see class AttributeID

    (C) 2005 Radon Labs GmbH

*/
#include "attr/attributeid.h"

namespace Attr
{
    
class Vector4AttributeID : public AttributeID
{
public:
    Vector4AttributeID();
    Vector4AttributeID( const _attridTyped<attr::Vector4T>* );
    Vector4AttributeID( const nString& name ); // must exist!
    virtual ~Vector4AttributeID();

    /// equality operator
    friend bool operator==(const Vector4AttributeID& lhs, const Vector4AttributeID& rhs );
    /// inequality operator
    friend bool operator!=(const Vector4AttributeID& lhs, const Vector4AttributeID& rhs );

    static Vector4AttributeID FindVector4AttributeID( const nString& name );
};

//------------------------------------------------------------------------------
/**
*/
inline bool operator==(const Vector4AttributeID& lhs, const Vector4AttributeID& rhs )
{
    return lhs.attridPtr == rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline bool operator!=(const Vector4AttributeID& lhs, const Vector4AttributeID& rhs )
{
    return lhs.attridPtr != rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
Vector4AttributeID::Vector4AttributeID( const _attridTyped<attr::Vector4T>* aip ) :
    AttributeID( aip )
{
}

//------------------------------------------------------------------------------
/**
*/
inline 
Vector4AttributeID::Vector4AttributeID() :
    AttributeID()
{
}

//------------------------------------------------------------------------------
/**
    gives the AttributeID "name".
    will fail hard if AttributeID doesnt exist.
*/
inline 
Vector4AttributeID::Vector4AttributeID( const nString& name )
{
    const Vector4AttributeID& existingID = Vector4AttributeID::FindVector4AttributeID( name );
    
    if ( !existingID.IsValid() )
    {
        n_error( "Error: Attribute ID of name \"%s\" not found!", name.Get() );
    }

    this->attridPtr = existingID.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
Vector4AttributeID::~Vector4AttributeID()
{
}

} // namespace
//typedef const _attridTyped<attr::Vector4T>* Vector4AttributeID;
//
//namespace attr
//{
//    /// find Vector4 attribute id by name
//    Vector4AttributeID FindVector4AttributeID( const nString& name );
//}

//------------------------------------------------------------------------------
#endif




    