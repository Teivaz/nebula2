#ifndef N_RPXMLPARSER_H
#define N_RPXMLPARSER_H
//------------------------------------------------------------------------------
/**
    @class nRpXmlParser
    @ingroup Scene
    @brief Configure a nRenderPath2 object from an XML file.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "tinyxml/tinyxml.h"
#include "mathlib/vector.h"
#include "variable/nvariable.h"
#include "gfx2/nshaderstate.h"
#include "util/nstring.h"
#include "util/narray.h"

class nRenderPath2;
class nRpSection;
class nRpPass;
class nRpPhase;
class nRpSequence;

//------------------------------------------------------------------------------
class nRpXmlParser
{
public:
    /// constructor
    nRpXmlParser();
    /// destructor
    ~nRpXmlParser();
    /// set pointer to render path object to initialize
    void SetRenderPath(nRenderPath2* rp);
    /// get pointer to render path object
    nRenderPath2* GetRenderPath() const;
    /// open the XML document
    bool OpenXml();
    /// close the XML document
    void CloseXml();
    /// parse the XML data and initialize the render path object
    bool ParseXml();
    /// get the shader path, valid after OpenXml()
    const nString& GetShaderPath() const;

private:
    /// return true if XML attribute exists
    bool HasAttr(TiXmlElement* elm, const char* name);
    /// get an integer attribute from an xml element
    int GetIntAttr(TiXmlElement* elm, const char* name, int defaultValue);
    /// get a float attribute from an xml element
    float GetFloatAttr(TiXmlElement* elm, const char* name, float defaultValue);
    /// get a vector4 attribute from an xml element
    vector4 GetVector4Attr(TiXmlElement* elm, const char* name, const vector4& defaultValue);
    /// get a float4 attribute from an xml element
    nFloat4 GetFloat4Attr(TiXmlElement* elm, const char* name, const nFloat4& defaultValue);
    /// get a bool attribute from an xml element
    bool GetBoolAttr(TiXmlElement* elm, const char* name, bool defaultValue);
    /// parse a shader XML element
    void ParseShader(TiXmlElement* elm, nRenderPath2* renderPath);
	/// 
	void ParseShaders();
    /// parse RenderTarget XML element
    void ParseRenderTarget(TiXmlElement* elm, nRenderPath2* renderPath);
    /// create a variable from an XML element
    nVariable ParseVariable(nVariable::Type dataType, TiXmlElement* elm);
    /// parse a global variable under RenderPath
    void ParseGlobalVariable(nVariable::Type dataType, TiXmlElement* elm, nRenderPath2* renderPath);
    /// parse a Secion XML element
    void ParseSection(TiXmlElement* elm, nRenderPath2* renderPath);
    /// parse a Pass XML element
    void ParsePass(TiXmlElement* elm, nRpSection* section);
    /// parse a shader state inside a pass
    void ParseShaderState(nShaderState::Type type, TiXmlElement* elm, nRpPass* pass);
    /// parse a shader state inside a pass
    void ParseShaderState(nShaderState::Type type, TiXmlElement* elm, nRpSequence* seq);
    /// parse a phase
    void ParsePhase(TiXmlElement* elm, nRpPass* pass);
    /// parse a sequence
    void ParseSequence(TiXmlElement* elm, nRpPhase* phase);

    TiXmlDocument* xmlDocument;
    nRenderPath2* renderPath;
    nString shaderPath;
	nString mangledPath;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpXmlParser::SetRenderPath(nRenderPath2* rp)
{
    n_assert(rp);
    this->renderPath = rp;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath2*
nRpXmlParser::GetRenderPath() const
{
    return this->renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpXmlParser::HasAttr(TiXmlElement* elm, const char* name)
{
    n_assert(elm && name);
    return (elm->Attribute(name) != 0);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpXmlParser::GetIntAttr(TiXmlElement* elm, const char* name, int defaultValue)
{
    n_assert(elm && name);
    int value;
    int ret = elm->QueryIntAttribute(name, &value);
    if (TIXML_SUCCESS == ret)
    {
        return value;
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nRpXmlParser::GetFloatAttr(TiXmlElement* elm, const char* name, float defaultValue)
{
    n_assert(elm && name);
    double value;
    int ret = elm->QueryDoubleAttribute(name, &value);
    if (TIXML_SUCCESS == ret)
    {
        return float(value);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nRpXmlParser::GetVector4Attr(TiXmlElement* elm, const char* name, const vector4& defaultValue)
{
    n_assert(elm && name);
    nString valStr = elm->Attribute(name);
    if (!valStr.IsEmpty())
    {
        nArray<nString> tokens;
        valStr.Tokenize(" ", tokens);
        vector4 value;
        value.x = tokens[0].AsFloat();
        value.y = tokens[1].AsFloat();
        value.z = tokens[2].AsFloat();
        value.w = tokens[3].AsFloat();
        return value;
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nFloat4
nRpXmlParser::GetFloat4Attr(TiXmlElement* elm, const char* name, const nFloat4& defaultValue)
{
    n_assert(elm && name);
    nString valStr = elm->Attribute(name);
    if (!valStr.IsEmpty())
    {
        nArray<nString> tokens;
        valStr.Tokenize(" ", tokens);
        nFloat4 value;
        value.x = tokens[0].AsFloat();
        value.y = tokens[1].AsFloat();
        value.z = tokens[2].AsFloat();
        value.w = tokens[3].AsFloat();
        return value;
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpXmlParser::GetBoolAttr(TiXmlElement* elm, const char* name, bool defaultValue)
{
    n_assert(elm && name);
    nString valStr = elm->Attribute(name);
    if (!valStr.IsEmpty())
    {
        if ((valStr == "true") || (valStr == "True") || 
            (valStr == "on") || (valStr == "On") ||
            (valStr == "yes") || (valStr == "Yes"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
#endif
