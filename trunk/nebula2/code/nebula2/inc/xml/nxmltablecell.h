#ifndef N_XMLTABLECELL_H
#define N_XMLTABLECELL_H
//------------------------------------------------------------------------------
/**
    @class nXmlTableCell
    @ingroup Xml

    A single cell in an XML table. The primary representation is as string,
    but converted data types can be returned as well (such as int, float, etc...).

    If the XML table cell is empty, the method IsEmpty() returns true, and
    all IsValidXXX() method return false. This differs from the previous
    behavior, where the string "<invalid>" was returned.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class nXmlTableCell
{
public:
    /// constructor
    nXmlTableCell();
    /// destructor
    ~nXmlTableCell();
    /// set value
    void Set(const nString& s);
    /// return true if the content of the cell is empty
    bool IsEmpty() const;
    /// return true if the cell is not empty
    bool IsValid() const;
    /// return true if the content is a valid int
    bool IsValidInt() const;
    /// return true if the content is a valid float
    bool IsValidFloat() const;
    /// return true if the content is a valid bool
    bool IsValidBool() const;
    /// return true if the content is a valid vector3
    bool IsValidVector3() const;
    /// return true if the content is a valid vector4
    bool IsValidVector4() const;
    /// get value as nString
    const nString& AsString() const;
    /// get value as int
    int AsInt() const;
    /// get value as float
    float AsFloat() const;
    /// get value as bool
    bool AsBool() const;
    /// get value as vector3
    vector3 AsVector3() const;
    /// get value as vector4
    vector4 AsVector4() const;

private:
    nString value;
};

//------------------------------------------------------------------------------
/**
*/
inline
nXmlTableCell::nXmlTableCell()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nXmlTableCell::~nXmlTableCell()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Sets the value of the cell. NOTE: an automatic UTF8-conversion happens
    automatically on the new content of the cell.
*/
inline
void
nXmlTableCell::Set(const nString& str)
{
    this->value = str;
    this->value.UTF8toANSI();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::IsEmpty() const
{
    return this->value.IsEmpty();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::IsValid() const
{
    return this->value.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::IsValidInt() const
{
    return this->value.IsValidInt();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::IsValidFloat() const
{
    return this->value.IsValidFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::IsValidBool() const
{
    return this->value.IsValidBool();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::IsValidVector3() const
{
    return this->value.IsValidVector3();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::IsValidVector4() const
{
    return this->value.IsValidVector4();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nXmlTableCell::AsString() const
{
    return this->value;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nXmlTableCell::AsInt() const
{
    return this->value.AsInt();
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nXmlTableCell::AsFloat() const
{
    return this->value.AsFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::AsBool() const
{
    return this->value.AsBool();
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3
nXmlTableCell::AsVector3() const
{
    return this->value.AsVector3();
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nXmlTableCell::AsVector4() const
{
    return this->value.AsVector4();
}

//------------------------------------------------------------------------------
#endif
