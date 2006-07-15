#ifndef ATTR_STRING_ATTRIBUTE_ID_H
#define ATTR_STRING_ATTRIBUTE_ID_H
//------------------------------------------------------------------------------
/**
    @class StringAttributeID

    @brief see class AttributeID

    (C) 2005 Radon Labs GmbH

*/
#include "attr/attributeid.h"

namespace Attr
{
    
class StringAttributeID : public AttributeID
{
public:
    StringAttributeID();
    StringAttributeID(const _attridTyped<attr::StringT>*);
    StringAttributeID(const nString& name); // must exist!
    virtual ~StringAttributeID();

    /// equality operator
    friend bool operator==(const StringAttributeID& lhs, const StringAttributeID& rhs);
    /// inequality operator
    friend bool operator!=(const StringAttributeID& lhs, const StringAttributeID& rhs);

    static StringAttributeID FindStringAttributeID(const nString& name);
};

//------------------------------------------------------------------------------
/**
*/
inline 
bool 
operator==(const StringAttributeID& lhs, const StringAttributeID& rhs)
{
    return lhs.attridPtr == rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool 
operator!=(const StringAttributeID& lhs, const StringAttributeID& rhs)
{
    return lhs.attridPtr != rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
StringAttributeID::StringAttributeID(const _attridTyped<attr::StringT>* aip) :
    AttributeID(aip)
{
}

//------------------------------------------------------------------------------
/**
*/
inline 
StringAttributeID::StringAttributeID() :
    AttributeID()
{
}

//------------------------------------------------------------------------------
/**
    Gives the AttributeID "name".
    Will fail hard if AttributeID doesn't exist.
*/
inline 
StringAttributeID::StringAttributeID(const nString& name)
{
    const StringAttributeID& existingID = StringAttributeID::FindStringAttributeID(name);
    
    if (!existingID.IsValid())
    {
        n_error("Error: Attribute ID of name \"%s\" not found!", name.Get());
    }

    this->attridPtr = existingID.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
StringAttributeID::~StringAttributeID()
{
}

} // namespace

//------------------------------------------------------------------------------
#endif
