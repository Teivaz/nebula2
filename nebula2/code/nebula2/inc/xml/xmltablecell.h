#ifndef XML_XMLTABLECELL_H
#define XML_XMLTABLECELL_H
//------------------------------------------------------------------------------
/**
    @class XmlTableCell
    @ingroup Obsolete

    Cell inside an table read from an (Excel exported) xml file.
    
    (C) 2003-2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class XmlTableCell
{
public:
    /// Construct
    XmlTableCell();
    /// Destruct
    ~XmlTableCell();

    /// set value as string
    void SetString(const nString& v);

    /// get cell value as bool
    bool GetBool() const;
    /// get cell value as float
    float GetFloat() const;
    /// get cell value as int
    int GetInt() const;
    /// get cell value as string
    const nString& GetString() const;

private:
    bool valueAsBool;
    float valueAsFloat;
    int valueAsInt;
    nString valueAsString;
};

//------------------------------------------------------------------------------
/**
*/
inline
XmlTableCell::XmlTableCell() :
    valueAsBool(false),
    valueAsFloat(0.0f),
    valueAsInt(0),
    valueAsString("")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
XmlTableCell::~XmlTableCell()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    - 01-Jun-04     floh    UTF-8 to ANSI conversion
*/
inline
void
XmlTableCell::SetString(const nString& v)
{
    this->valueAsString = v;
    this->valueAsString.UTF8toANSI();

    // convert string to other representations
    this->valueAsFloat = float(atof(v.Get()));
    this->valueAsInt   = atoi(v.Get());
    this->valueAsBool  = false;

    // note: odd indices mean "false"
    static const char* boolOptions[] = {
        "ja", "nein", "yes", "no", "ein", "aus", "on", "off", "wahr", "falsch", "true", "false", 0
    };
    const char* cur;
    int index = -1;
    int i = 0;
    while ((cur = boolOptions[i]) != 0)
    {
        if (0 == n_stricmp(cur, v.Get()))
        {
            index = i;
            break;
        }
        i++;
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
bool
XmlTableCell::GetBool() const
{
    return this->valueAsBool;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
XmlTableCell::GetFloat() const
{
    return this->valueAsFloat;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
XmlTableCell::GetInt() const
{
    return this->valueAsInt;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
XmlTableCell::GetString() const
{
    return this->valueAsString;
}

//------------------------------------------------------------------------------
#endif