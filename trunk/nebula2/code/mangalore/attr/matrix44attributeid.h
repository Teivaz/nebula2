#ifndef ATTR_MATRIX44_ATTRIBUTE_ID_H
#define ATTR_MATRIX44_ATTRIBUTE_ID_H
//------------------------------------------------------------------------------
/**
    @class Matrix44AttributeID

    @brief see class AttributeID

    (C) 2005 Radon Labs GmbH

*/
#include "attr/attributeid.h"

namespace Attr
{
    
class Matrix44AttributeID : public AttributeID
{
public:
    Matrix44AttributeID();
    Matrix44AttributeID(const _attridTyped<attr::Matrix44T>*);
    Matrix44AttributeID(const nString& name); // must exist!
    virtual ~Matrix44AttributeID();

    /// equality operator
    friend bool operator==(const Matrix44AttributeID& lhs, const Matrix44AttributeID& rhs);
    /// inequality operator
    friend bool operator!=(const Matrix44AttributeID& lhs, const Matrix44AttributeID& rhs);

    static Matrix44AttributeID FindMatrix44AttributeID(const nString& name);
};

//------------------------------------------------------------------------------
/**
*/
inline 
bool 
operator==(const Matrix44AttributeID& lhs, const Matrix44AttributeID& rhs)
{
    return lhs.attridPtr == rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool 
operator!=(const Matrix44AttributeID& lhs, const Matrix44AttributeID& rhs)
{
    return lhs.attridPtr != rhs.attridPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline 
Matrix44AttributeID::Matrix44AttributeID(const _attridTyped<attr::Matrix44T>* aip) :
    AttributeID(aip)
{
}

//------------------------------------------------------------------------------
/**
*/
inline 
Matrix44AttributeID::Matrix44AttributeID() :
    AttributeID()
{
}

//------------------------------------------------------------------------------
/**
    Gives the AttributeID "name".
    Will fail hard if AttributeID doesn't exist.
*/
inline 
Matrix44AttributeID::Matrix44AttributeID(const nString& name)
{
    const Matrix44AttributeID& existingID = Matrix44AttributeID::FindMatrix44AttributeID(name);
    
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
Matrix44AttributeID::~Matrix44AttributeID()
{
}

} // namespace

//------------------------------------------------------------------------------
#endif
