#ifndef ATTR_VOID_ATTRIBUTE_ID_H
#define ATTR_VOID_ATTRIBUTE_ID_H
//------------------------------------------------------------------------------
/**
    @class VoidAttributeID

    @brief see class AttributeID

    (C) 2005 Radon Labs GmbH

*/
#include "attr/attributeid.h"

namespace Attr
{
    
class VoidAttributeID : public AttributeID
{
public:
    VoidAttributeID();
    VoidAttributeID( const _attridTyped<attr::VoidT>* );
    VoidAttributeID( const nString& name ); // must exist!
    virtual ~VoidAttributeID();

    /// equality operator
    friend bool operator==(const VoidAttributeID& lhs, const VoidAttributeID& rhs );
    /// inequality operator
    friend bool operator!=(const VoidAttributeID& lhs, const VoidAttributeID& rhs );

    static VoidAttributeID FindVoidAttributeID( const nString& name );
};

//------------------------------------------------------------------------------
/**
*/
inline bool operator==(const VoidAttributeID& lhs, const VoidAttributeID& rhs )
{
    return lhs.attridPtr == rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline bool operator!=(const VoidAttributeID& lhs, const VoidAttributeID& rhs )
{
    return lhs.attridPtr != rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
VoidAttributeID::VoidAttributeID( const _attridTyped<attr::VoidT>* aip ) :
    AttributeID( aip )
{
}

//------------------------------------------------------------------------------
/**
*/
inline 
VoidAttributeID::VoidAttributeID() :
    AttributeID()
{
}

//------------------------------------------------------------------------------
/**
    gives the AttributeID "name".
    will fail hard if AttributeID doesnt exist.
*/
inline 
VoidAttributeID::VoidAttributeID( const nString& name )
{
    const VoidAttributeID& existingID = VoidAttributeID::FindVoidAttributeID( name );
    
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
VoidAttributeID::~VoidAttributeID()
{
}

} // namespace
//typedef const _attridTyped<attr::VoidT>* VoidAttributeID;
//
//namespace attr
//{
//    /// find Void attribute id by name
//    VoidAttributeID FindVoidAttributeID( const nString& name );
//}

//------------------------------------------------------------------------------
#endif




    
