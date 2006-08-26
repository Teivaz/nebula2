#ifndef N_ATTR_H
#define N_ATTR_H
//------------------------------------------------------------------------------
/**
    @class nAttr
    @ingroup Util

    An nAttr object extends an nVariant object by a name.

    (C) 2005 Radon Labs GmbH
*/
#include "util/nvariant.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nAttr : public nVariant
{
public:
    /// default constructor
    nAttr();
    /// copy constructor
    nAttr(const nAttr& rhs);
    /// int constructor
    nAttr(const nString& name, int val);
    /// float constructor
    nAttr(const nString& name, float rhs);
    /// bool constructor
    nAttr(const nString& name, bool rhs);
    /// string constructor
    nAttr(const nString& name, const char* val);
    /// vector3 constructor
    nAttr(const nString& name, const vector3& v);
    /// vector4 constructor
    nAttr(const nString& name, const vector4& v);
    /// matrix44 constructor
    nAttr(const nString& name, const matrix44& m);
    /// assignment operator
    void operator=(const nAttr& rhs);
    /// set attribute name
    void SetName(const nString& name);
    /// get attribute name
    const nString& GetName() const;

private:
    nString name;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr(const nAttr& rhs) :
    nVariant(rhs),
    name(rhs.name)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr(const nString& n, int val) :
    nVariant(val),
    name(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr(const nString& n, float val) :
    nVariant(val),
    name(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr(const nString& n, bool val) :
    nVariant(val),
    name(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr(const nString& n, const char* val) :
    nVariant(val),
    name(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr(const nString& n, const vector3& val) :
    nVariant(val),
    name(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr(const nString& n, const vector4& val) :
    nVariant(val),
    name(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAttr::nAttr(const nString& n, const matrix44& val) :
    nVariant(val),
    name(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAttr::operator=(const nAttr& rhs)
{
    nVariant::operator=(rhs);
    this->name = rhs.name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAttr::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nAttr::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
#endif
