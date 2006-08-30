#ifndef ATTR_INT_ATTRIBUTE_ID_H
#define ATTR_INT_ATTRIBUTE_ID_H
//------------------------------------------------------------------------------
/**
    @class IntAttributeID

    @brief see class AttributeID

    (C) 2005 Radon Labs GmbH

*/
#include "attr/attributeid.h"

namespace Attr
{

class IntAttributeID : public AttributeID
{
public:
    IntAttributeID();
    IntAttributeID(const _attridTyped<attr::IntT>*);
    IntAttributeID(const nString& name); // must exist!
    virtual ~IntAttributeID();

    /// equality operator
    friend bool operator==(const IntAttributeID& lhs, const IntAttributeID& rhs);
    /// inequality operator
    friend bool operator!=(const IntAttributeID& lhs, const IntAttributeID& rhs);

    static IntAttributeID FindIntAttributeID(const nString& name);
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator==(const IntAttributeID& lhs, const IntAttributeID& rhs)
{
    return lhs.attridPtr == rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator!=(const IntAttributeID& lhs, const IntAttributeID& rhs)
{
    return lhs.attridPtr != rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
IntAttributeID::IntAttributeID(const _attridTyped<attr::IntT>* aip) :
    AttributeID(aip)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
IntAttributeID::IntAttributeID() :
    AttributeID()
{
}

//------------------------------------------------------------------------------
/**
    Gives the AttributeID "name".
    Will fail hard if AttributeID doesn't exist.
*/
inline
IntAttributeID::IntAttributeID(const nString& name)
{
    const IntAttributeID& existingID = IntAttributeID::FindIntAttributeID(name);

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
IntAttributeID::~IntAttributeID()
{
}

} // namespace

//------------------------------------------------------------------------------
#endif
