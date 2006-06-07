//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwshaderpanel.h"
#include "tinyxml/tinyxml.h"
#include "lwexporter/nlwshaderpanelfactory.h"
#include "lwexporter/nlwshaderexportsettings.h"
#include "lwexporter/nlwshaderpanelparam.h"

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelTemplate::nLWShaderPanelTemplate(const nString& shaderName, 
                                               const nString& shaderFile) :
    shaderName(shaderName),
    shaderFile(shaderFile)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelTemplate::~nLWShaderPanelTemplate()
{
    for (int i = 0; i < this->shaderParamArray.Size(); i++)
    {
        n_delete(this->shaderParamArray[i]);
    }
    this->shaderParamArray.Reset();
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelTemplate::AddShaderParam(const TiXmlElement* paramElem)
{
    // ignore params marked as non-exportable
    int export = 0;
    paramElem->Attribute("export", &export);
    if (!export)
        return;

    nString paramType = paramElem->Attribute("type");
    if (paramType.IsEmpty())
        return;

    paramType.ToLower();

    nLWShaderPanelParam* param = 0;

    if ("enum" == paramType)
    {
        param = n_new(nLWShaderPanelEnumParam(this->shaderName));
    }
    else if ("int" == paramType)
    {
        param = n_new(nLWShaderPanelIntParam(this->shaderName));
    }
    else if ("float" == paramType)
    {
        param = n_new(nLWShaderPanelFloatParam(this->shaderName));
    }
    else if ("bool" == paramType)
    {
        param = n_new(nLWShaderPanelBoolParam(this->shaderName));
    }
    else if ("color" == paramType)
    {
        param = n_new(nLWShaderPanelColorParam(this->shaderName));
    }
    else if ("texture" == paramType)
    {
        param = n_new(nLWShaderPanelTextureParam(this->shaderName));
    }
    else if ("bumptexture" == paramType)
    {
        param = n_new(nLWShaderPanelTextureParam(this->shaderName));
    }
    else if ("cubetexture" == paramType)
    {
        param = n_new(nLWShaderPanelTextureParam(this->shaderName));
    }
    else
    {
        param = n_new(nLWShaderPanelPlaceHolderParam(this->shaderName));
    }

    if (param)
    {
        param->Load(paramElem);
        this->shaderParamArray.PushBack(param);
    }
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWShaderPanelTemplate::LoadLayout(TiXmlElement* shaderElem)
{
    assert(shaderElem);
    if (shaderElem)
    {
        int display = 0;
        TiXmlHandle xmlhShader(shaderElem);
        TiXmlElement* paramElem = xmlhShader.FirstChild("param").Element();
        for ( ; paramElem; paramElem = paramElem->NextSiblingElement())
        {
            this->AddShaderParam(paramElem);
        }
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
