//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexportnodetype.h"
#include "tinyxml/tinyxml.h"
#include "tools/nmeshbuilder.h"

//----------------------------------------------------------------------------
/**
*/
namespace Settings
{
    enum Setting
    {
        SceneNodeType = 0,
        ExportTextures,
        ExportAnimation,
        VertexComponents,
        NumSettings,
        InvalidSetting
    };

    const char* const SETTING_NAME_ARRAY[] = {
        "SceneNode",
        "Textures",
        "Animation",
        "VertexComponents",
    };

    const int SETTING_NAME_ARRAY_SIZE = sizeof(SETTING_NAME_ARRAY) / sizeof(const char*);

    //----------------------------------------------------------------------------
    /**
    */
    Setting
    NameToEnum(const nString& settingName)
    {
        for (int i = 0; i < SETTING_NAME_ARRAY_SIZE; i++)
        {
            if (SETTING_NAME_ARRAY[i] == settingName)
            {
                return (Setting)i;
            }
        }
        return InvalidSetting; // setting not found
    }

    //----------------------------------------------------------------------------
    /**
    */
    const char*
    EnumToName(Setting setting)
    {
        if ((int)setting < SETTING_NAME_ARRAY_SIZE)
        {
            return SETTING_NAME_ARRAY[(int)setting];
        }
        return 0; // setting not found
    }

} // namespace Settings

//----------------------------------------------------------------------------
/**
*/
bool
nLWExportNodeType::LoadFromXML(TiXmlElement* elem)
{
    n_assert(elem);
    if (!elem)
    {
        return false;
    }

    this->typeName = elem->Attribute("name");
    if (this->typeName.IsEmpty())
    {
        return false;
    }

    TiXmlHandle xmlhElem(elem);
    TiXmlElement* settingElem = 0;
    settingElem = xmlhElem.FirstChild("setting").Element();
    for ( ; settingElem; settingElem = settingElem->NextSiblingElement())
    {
        Settings::Setting setting = Settings::NameToEnum(settingElem->Attribute("name"));
        nString valStr = settingElem->Attribute("value");
        switch (setting)
        {
            case Settings::SceneNodeType:
                this->sceneNodeType = valStr;
                break;

            case Settings::ExportTextures:
                this->exportTextures = (valStr.AsInt() == 1 ? true : false);
                break;

            case Settings::ExportAnimation:
                this->exportAnimation = (valStr.AsInt() == 1 ? true : false);
                break;

            case Settings::VertexComponents:
            {
                nArray<nString> tokens;
                valStr.Tokenize("|", tokens);
                this->vertexComponentMask = 0;
                for (int i = 0; i < tokens.Size(); i++)
                {
                    const nString& token = tokens[i];
                    if ("coord" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::COORD;
                    }
                    else if ("tangent" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::TANGENT;
                    }
                    else if ("binormal" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::BINORMAL;
                    }
                    else if ("normal" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::NORMAL;
                    }
                    else if ("uv0" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::UV0;
                    }
                    else if ("uv1" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::UV1;
                    }
                    else if ("uv2" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::UV2;
                    }
                    else if ("uv3" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::UV3;
                    }
                    else if ("jointweights" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::JINDICES;
                        this->vertexComponentMask |= nMeshBuilder::Vertex::WEIGHTS;
                    }
                    else if ("color" == token)
                    {
                        this->vertexComponentMask |= nMeshBuilder::Vertex::COLOR;
                    }
                    else
                    {
                        n_assert("Unknown vertex component!");
                    }
                }
                break;
            }

            default:
                n_assert2(false, "Unknown setting!");
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWExportNodeType::SaveToXML(TiXmlElement* elem) const
{
    n_assert(elem);
    if (!elem)
    {
        return false;
    }

    elem->SetAttribute("name", this->typeName.Get());
    TiXmlElement* settingElem = 0;
    
    settingElem = n_new(TiXmlElement("setting"));
    if (settingElem)
    {
        const char* name = Settings::EnumToName(Settings::SceneNodeType);
        if (name)
        {
            settingElem->SetAttribute("name", name);
            settingElem->SetAttribute("type", "String");
            settingElem->SetAttribute("value", this->sceneNodeType.Get());
            elem->LinkEndChild(settingElem);
        }
        else
        {
            n_delete(settingElem);
        }
    }
    
    settingElem = n_new(TiXmlElement("setting"));
    if (settingElem)
    {
        const char* name = Settings::EnumToName(Settings::ExportTextures);
        if (name)
        {
            settingElem->SetAttribute("name", name);
            settingElem->SetAttribute("type", "Bool");
            settingElem->SetAttribute("value", this->exportTextures ? 1 : 0);
            elem->LinkEndChild(settingElem);
        }
        else
        {
            n_delete(settingElem);
        }
    }

    settingElem = n_new(TiXmlElement("setting"));
    if (settingElem)
    {
        const char* name = Settings::EnumToName(Settings::ExportAnimation);
        if (name)
        {
            settingElem->SetAttribute("name", name);
            settingElem->SetAttribute("type", "Bool");
            settingElem->SetAttribute("value", this->exportAnimation ? 1 : 0);
            elem->LinkEndChild(settingElem);
        }
        else
        {
            n_delete(settingElem);
        }
    }

    settingElem = n_new(TiXmlElement("setting"));
    if (settingElem)
    {
        const char* name = Settings::EnumToName(Settings::VertexComponents);
        if (name)
        {
            nString maskStr;
            if (this->vertexComponentMask & nMeshBuilder::Vertex::COORD)
            {
                maskStr += "coord";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::TANGENT)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "tangent";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::BINORMAL)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "binormal";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::NORMAL)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "normal";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::UV0)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "uv0";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::UV1)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "uv1";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::UV2)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "uv2";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::UV3)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "uv3";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::WEIGHTS)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "jointweights";
            }
            if (this->vertexComponentMask & nMeshBuilder::Vertex::COLOR)
            {
                if (!maskStr.IsEmpty())
                {
                    maskStr += "|";
                }
                maskStr += "color";
            }

            settingElem->SetAttribute("name", name);
            settingElem->SetAttribute("type", "String");
            settingElem->SetAttribute("value", maskStr.Get());
            elem->LinkEndChild(settingElem);
        }
        else
        {
            n_delete(settingElem);
        }
    }

    return true;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
