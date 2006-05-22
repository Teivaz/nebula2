#ifndef ATTR_BOOL_ATTRIBUTE_ID_H
#define ATTR_BOOL_ATTRIBUTE_ID_H
//------------------------------------------------------------------------------
/**
    @class BoolAttributeID

    @brief see class AttributeID

    (C) 2005 Radon Labs GmbH

*/
#include "attr/attributeid.h"

namespace Attr
{
    
class BoolAttributeID : public AttributeID
{
public:
    BoolAttributeID();
    BoolAttributeID( const _attridTyped<attr::BoolT>* );
    BoolAttributeID( const nString& name ); // must exist!
    virtual ~BoolAttributeID();

    /// equality operator
    friend bool operator==(const BoolAttributeID& lhs, const BoolAttributeID& rhs );
    /// inequality operator
    friend bool operator!=(const BoolAttributeID& lhs, const BoolAttributeID& rhs );

    static BoolAttributeID FindBoolAttributeID( const nString& name );
};

//------------------------------------------------------------------------------
/**
*/
inline bool operator==(const BoolAttributeID& lhs, const BoolAttributeID& rhs )
{
    return lhs.attridPtr == rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline bool operator!=(const BoolAttributeID& lhs, const BoolAttributeID& rhs )
{
    return lhs.attridPtr != rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
BoolAttributeID::BoolAttributeID( const _attridTyped<attr::BoolT>* aip ) :
    AttributeID( aip )
{
}

//------------------------------------------------------------------------------
/**
*/
inline 
BoolAttributeID::BoolAttributeID() :
    AttributeID()
{
}

//------------------------------------------------------------------------------
/**
    gives the AttributeID "name".
    will fail hard if AttributeID doesnt exist.
*/
inline 
BoolAttributeID::BoolAttributeID( const nString& name )
{
    const BoolAttributeID& existingID = BoolAttributeID::FindBoolAttributeID( name );
    
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
BoolAttributeID::~BoolAttributeID()
{
}

} // namespace

//typedef const _attridTyped<attr::BoolT>* BoolAttributeID;
//
//namespace attr
//{
//    /// find Bool attribute id by name
//    BoolAttributeID FindBoolAttributeID( const nString& name );
//}

//------------------------------------------------------------------------------
#endif




    
