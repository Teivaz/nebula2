#ifndef N_XMLTABLECELL_H
#define N_XMLTABLECELL_H
//------------------------------------------------------------------------------
/**
    @class nXmlTableCell
    @ingroup Xml

    @brief A single cell in an XML table.

    The primary representation is as string, but bool, int and float values
    can be returned (converted values are cached for efficiency). Conversion
    happens during Set().
    
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
    void Set(const char* s);
    /// get value
    const char* Get() const;
    /// get value as nString
    const nString& AsString() const;
    /// get value as int
    int AsInt() const;
    /// get value as float
    float AsFloat() const;
    /// get value as bool
    bool AsBool() const;
    /// get value as vector3
    const vector3& AsVector3() const;
    /// get value as vector4
    const vector4& AsVector4() const;

private:
    nString valueAsString;
    int valueAsInt;
    float valueAsFloat;
    bool valueAsBool;
    float valueAsVector[4];
};

//------------------------------------------------------------------------------
/**
*/
inline
nXmlTableCell::nXmlTableCell() :
    valueAsString("<invalid>"),
    valueAsInt(0),
    valueAsFloat(0),
    valueAsBool(false)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        this->valueAsVector[i] = 0.0f;
    }
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
    01-Jun-04   added UTF-8 to ANSI-conversion
*/
inline
void
nXmlTableCell::Set(const char* str)
{
    n_assert(str);

    this->valueAsString = str;
    this->valueAsString.UTF8toANSI();

    this->valueAsFloat  = float(atof(str));
    this->valueAsInt    = atoi(str);
    this->valueAsBool   = false;

    nArray<nString> tokens;
    this->valueAsString.Tokenize(" ,", tokens);
    for (int i = 0; (i < 4) && (i < tokens.Size()); i++)
    {
        this->valueAsVector[i] = float(atof(tokens[i].Get()));
    }

    // NOTE: odd indices mean "false"
    // FIXME: this could be a real performance killer
    static const char* boolOptions[] = {
        "ja", "nein", "yes", "no", "ein", "aus", "on", "off", "wahr", "falsch", "true", "false", 0
    };
    const char* cur;
    int index = -1;
    int boIdx = 0;
    while ((cur = boolOptions[boIdx]) != 0)
    {
        if (0 == n_stricmp(cur, str))
        {
            index = boIdx;
            break;
        }
        boIdx++;
    }
    if (index != -1)
    {
        this->valueAsBool = ((index & 1) == 0);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nXmlTableCell::Get() const
{
    return this->valueAsString.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nXmlTableCell::AsString() const
{
    return this->valueAsString;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nXmlTableCell::AsInt() const
{
    return this->valueAsInt;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nXmlTableCell::AsFloat() const
{
    return this->valueAsFloat;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlTableCell::AsBool() const
{
    return this->valueAsBool;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nXmlTableCell::AsVector3() const
{
    return *(const vector3*)this->valueAsVector;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nXmlTableCell::AsVector4() const
{
    return *(const vector4*)this->valueAsVector;
}

//------------------------------------------------------------------------------
#endif

