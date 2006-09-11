#ifndef ATTR_FLOAT_ATTRIBUTE_ID_H
#define ATTR_FLOAT_ATTRIBUTE_ID_H
//------------------------------------------------------------------------------
/**
    @class FloatAttributeID

    @brief see class AttributeID

    (C) 2005 Radon Labs GmbH

*/
#include "attr/attributeid.h"

namespace Attr
{

class FloatAttributeID : public AttributeID
{
public:
    FloatAttributeID();
    FloatAttributeID(const _attridTyped<attr::FloatT>*);
    FloatAttributeID(const nString& name); // must exist!
    virtual ~FloatAttributeID();

    /// equality operator
    friend bool operator==(const FloatAttributeID& lhs, const FloatAttributeID& rhs);
    /// inequality operator
    friend bool operator!=(const FloatAttributeID& lhs, const FloatAttributeID& rhs);

    static FloatAttributeID FindFloatAttributeID(const nString& name);
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator==(const FloatAttributeID& lhs, const FloatAttributeID& rhs)
{
    return lhs.attridPtr == rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator!=(const FloatAttributeID& lhs, const FloatAttributeID& rhs)
{
    return lhs.attridPtr != rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
FloatAttributeID::FloatAttributeID(const _attridTyped<attr::FloatT>* aip) :
    AttributeID(aip)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
FloatAttributeID::FloatAttributeID() :
    AttributeID()
{
}

//------------------------------------------------------------------------------
/**
    Gives the AttributeID "name".
    Will fail hard if AttributeID doesn't exist.
*/
inline
FloatAttributeID::FloatAttributeID(const nString& name)
{
    const FloatAttributeID& existingID = FloatAttributeID::FindFloatAttributeID(name);

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
FloatAttributeID::~FloatAttributeID()
{
}

} // namespace Attr

//------------------------------------------------------------------------------
#endif
