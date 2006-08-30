//------------------------------------------------------------------------------
//
//  attr/attributeid.cc
//  (C) 2005 Radon Labs GmbH
//
//------------------------------------------------------------------------------
#include "attr/attributeid.h"

#include "attr/voidattributeid.h"
#include "attr/boolattributeid.h"
#include "attr/intattributeid.h"
#include "attr/floatattributeid.h"
#include "attr/stringattributeid.h"
#include "attr/vector3attributeid.h"
#include "attr/vector4attributeid.h"
#include "attr/matrix44attributeid.h"

namespace Attr
{

using namespace attr;

//------------------------------------------------------------------------------
/**
    FIXME: [np] slow and ugly for now. will get changed if needed.
*/
AttributeID
AttributeID::FindAttributeID( const nString& name )
{
    AttributeID attributeID;

    attributeID = _attridTyped<VoidT>::Find( name );
    if ( attributeID.IsValid() ) return attributeID;

    attributeID = _attridTyped<BoolT>::Find( name );
    if ( attributeID.IsValid() ) return attributeID;

    attributeID = _attridTyped<IntT>::Find( name );
    if ( attributeID.IsValid() ) return attributeID;

    attributeID = _attridTyped<FloatT>::Find( name );
    if ( attributeID.IsValid() ) return attributeID;

    attributeID = _attridTyped<StringT>::Find( name );
    if ( attributeID.IsValid() ) return attributeID;

    attributeID = _attridTyped<Vector3T>::Find( name );
    if ( attributeID.IsValid() ) return attributeID;

    attributeID = _attridTyped<Vector4T>::Find( name );
    if ( attributeID.IsValid() ) return attributeID;

    attributeID = _attridTyped<Matrix44T>::Find( name );
    if ( attributeID.IsValid() ) return attributeID;

    // not found, return the invalid attributeID
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
VoidAttributeID
VoidAttributeID::FindVoidAttributeID( const nString& name )
{
    VoidAttributeID attributeID;

    attributeID = _attridTyped<VoidT>::Find( name );
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
BoolAttributeID
BoolAttributeID::FindBoolAttributeID( const nString& name )
{
    BoolAttributeID attributeID;

    attributeID = _attridTyped<BoolT>::Find( name );
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
IntAttributeID
IntAttributeID::FindIntAttributeID( const nString& name )
{
    IntAttributeID attributeID;

    attributeID = _attridTyped<IntT>::Find( name );
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
FloatAttributeID
FloatAttributeID::FindFloatAttributeID( const nString& name )
{
    FloatAttributeID attributeID;

    attributeID = _attridTyped<FloatT>::Find( name );
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
StringAttributeID
StringAttributeID::FindStringAttributeID( const nString& name )
{
    StringAttributeID attributeID;

    attributeID = _attridTyped<StringT>::Find( name );
    return attributeID;
}

    //------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
Vector3AttributeID
Vector3AttributeID::FindVector3AttributeID( const nString& name )
{
    Vector3AttributeID attributeID;

    attributeID = _attridTyped<Vector3T>::Find( name );
    return attributeID;
}

    //------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
Vector4AttributeID
Vector4AttributeID::FindVector4AttributeID( const nString& name )
{
    Vector4AttributeID attributeID;

    attributeID = _attridTyped<Vector4T>::Find( name );
    return attributeID;
}

    //------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
Matrix44AttributeID
Matrix44AttributeID::FindMatrix44AttributeID( const nString& name )
{
    Matrix44AttributeID attributeID;

    attributeID = _attridTyped<Matrix44T>::Find( name );
    return attributeID;
}

} // namespace

#if 0

//------------------------------------------------------------------------------
/**
    FIXME: [np] slow and ugly for now. will get changed if needed.
*/
AttributeID
attr::FindAttributeID(const nString& name)
{
    AttributeID attributeID;

    attributeID = _attridTyped<VoidT>::Find(name);
    if (0 != attributeID) return attributeID;

    attributeID = _attridTyped<BoolT>::Find(name);
    if (0 != attributeID) return attributeID;

    attributeID = _attridTyped<IntT>::Find(name);
    if (0 != attributeID) return attributeID;

    attributeID = _attridTyped<FloatT>::Find(name);
    if (0 != attributeID) return attributeID;

    attributeID = _attridTyped<StringT>::Find(name);
    if (0 != attributeID) return attributeID;

    attributeID = _attridTyped<Vector3T>::Find(name);
    if (0 != attributeID) return attributeID;

    attributeID = _attridTyped<Vector4T>::Find(name);
    if (0 != attributeID) return attributeID;

    attributeID = _attridTyped<Matrix44T>::Find(name);
    if (0 != attributeID) return attributeID;

    // not found, let caller decide if this is an error
    return 0;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
VoidAttributeID
attr::FindVoidAttributeID(const nString& name)
{
    VoidAttributeID attributeID;

    attributeID = _attridTyped<VoidT>::Find(name);
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
BoolAttributeID
attr::FindBoolAttributeID(const nString& name)
{
    BoolAttributeID attributeID;

    attributeID = _attridTyped<BoolT>::Find(name);
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
IntAttributeID
attr::FindIntAttributeID(const nString& name)
{
    IntAttributeID attributeID;

    attributeID = _attridTyped<IntT>::Find(name);
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
FloatAttributeID
attr::FindFloatAttributeID(const nString& name)
{
    FloatAttributeID attributeID;

    attributeID = _attridTyped<FloatT>::Find(name);
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
StringAttributeID
attr::FindStringAttributeID(const nString& name)
{
    StringAttributeID attributeID;

    attributeID = _attridTyped<StringT>::Find(name);
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
Vector3AttributeID
attr::FindVector3AttributeID(const nString& name)
{
    Vector3AttributeID attributeID;

    attributeID = _attridTyped<Vector3T>::Find(name);
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
Vector4AttributeID
attr::FindVector4AttributeID(const nString& name)
{
    Vector4AttributeID attributeID;

    attributeID = _attridTyped<Vector4T>::Find(name);
    return attributeID;
}

//------------------------------------------------------------------------------
/**
    @param  name    attribute name as string
    @return         attribute if found, 0 otherwise
*/
Matrix44AttributeID
attr::FindMatrix44AttributeID(const nString& name)
{
    Matrix44AttributeID attributeID;

    attributeID = _attridTyped<Matrix44T>::Find(name);
    return attributeID;
}

#endif

