//------------------------------------------------------------------------------
//  nrpxmlparser.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "renderpath/nrpxmlparser.h"
#include "kernel/nfileserver2.h"
#include "renderpath/nrenderpath2.h"
#include "renderpath/nrprendertarget.h"
#include "renderpath/nrppass.h"
#include "renderpath/nrpphase.h"
#include "renderpath/nrpsequence.h"
#include "gfx2/ngfxserver2.h"
#include "util/nstream.h"

//------------------------------------------------------------------------------
/**
*/
nRpXmlParser::nRpXmlParser() :
    xmlDocument(0),
    renderPath(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRpXmlParser::~nRpXmlParser()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nRpXmlParser::OpenXml()
{
    n_assert(this->renderPath);
    n_assert(0 == this->xmlDocument);

    this->mangledPath = nFileServer2::Instance()->ManglePath(this->renderPath->GetFilename().Get());
    this->xmlDocument = n_new(TiXmlDocument);
    if (this->xmlDocument->LoadFile(mangledPath.Get()))
    {
        TiXmlHandle docHandle(this->xmlDocument);
        TiXmlElement* elmRenderPath = docHandle.FirstChildElement("RenderPath").Element();
        n_assert(elmRenderPath);
        this->renderPath->SetName(elmRenderPath->Attribute("name"));
        this->renderPath->SetShaderPath(elmRenderPath->Attribute("shaderPath"));
        return true;
    }
    else
    {
        n_delete(this->xmlDocument);
        this->xmlDocument = 0;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nRpXmlParser::CloseXml()
{
    n_assert(this->xmlDocument);
    n_delete(this->xmlDocument);
    this->xmlDocument = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nRpXmlParser::ParseXml()
{
    n_assert(this->renderPath);
    n_assert(this->xmlDocument);

    TiXmlHandle docHandle(this->xmlDocument);
    TiXmlElement* elmRenderPath = docHandle.FirstChildElement("RenderPath").Element();
    n_assert(elmRenderPath);

    // parse child elements
    TiXmlElement* child;
    for (child = elmRenderPath->FirstChildElement(); child; child = child->NextSiblingElement())
    {
        if (child->Value() == nString("Shaders"))
        {
            this->ParseShaders();
        }
        else if (child->Value() == nString("RenderTarget"))
        {
            this->ParseRenderTarget(child, renderPath);
        }
        else if (child->Value() == nString("Float"))
        {
            this->ParseGlobalVariable(nVariable::Float, child, renderPath);
        }
        else if (child->Value() == nString("Float4"))
        {
            this->ParseGlobalVariable(nVariable::Vector4, child, renderPath);
        }
        else if (child->Value() == nString("Int"))
        {
            this->ParseGlobalVariable(nVariable::Int, child, renderPath);
        }
        else if (child->Value() == nString("Texture"))
        {
            this->ParseGlobalVariable(nVariable::Object, child, renderPath);
        }
        else if (child->Value() == nString("Section"))
        {
            this->ParseSection(child, renderPath);
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse Shader xml element.
*/
void
nRpXmlParser::ParseShader(TiXmlElement* elm, nRenderPath2* renderPath)
{
    n_assert(elm && renderPath);
    nRpShader newShader;
    newShader.SetName(elm->Attribute("name"));
    newShader.SetFilename(elm->Attribute("file"));
    renderPath->AddShader(newShader);
}

//------------------------------------------------------------------------------
/**
*/
void
nRpXmlParser::ParseShaders()
{
    // setup the xml stream object
    nStream newXmlStream;
	n_assert(this->mangledPath.Get());
    newXmlStream.SetFilename(this->mangledPath);
	if(!newXmlStream.Open(nStream::Read))
    {
        n_error(nString("Failed to open " + this->mangledPath + "\n").Get());
    }
	else
	{
		newXmlStream.SetToNode("/RenderPath/Shaders");
	}

	newXmlStream.SetToFirstChild();
    do
    {
		if(newXmlStream.HasAttr("name") && newXmlStream.HasAttr("file"))
		{
			nRpShader newShader;
			nString name = newXmlStream.GetString("name");
			nString file = newXmlStream.GetString("file");

			newShader.SetName(name);
			newShader.SetFilename(file);
			renderPath->AddShader(newShader);
		}
		else
		{
			break;
		}
    }
    while(newXmlStream.SetToNextChild());
}

//------------------------------------------------------------------------------
/**
    Parse a RenderTarget xml element.
*/
void
nRpXmlParser::ParseRenderTarget(TiXmlElement* elm, nRenderPath2* renderPath)
{
    n_assert(elm && renderPath);
    nRpRenderTarget newRenderTarget;
    newRenderTarget.SetName(elm->Attribute("name"));
    newRenderTarget.SetFormat(nTexture2::StringToFormat(elm->Attribute("format")));
    newRenderTarget.SetRelSize(this->GetFloatAttr(elm, "relSize", 1.0f));
    if (this->HasAttr(elm, "width"))
    {
        newRenderTarget.SetWidth(this->GetIntAttr(elm, "width", 0));
    }
    if (this->HasAttr(elm, "height"))
    {
        newRenderTarget.SetHeight(this->GetIntAttr(elm, "height", 0));
    }
    renderPath->AddRenderTarget(newRenderTarget);
}

//------------------------------------------------------------------------------
/**
    Create a nVariable from XML element attributes "name" and "value".
*/
nVariable
nRpXmlParser::ParseVariable(nVariable::Type dataType, TiXmlElement* elm)
{
    n_assert(elm);
    const char* varName = elm->Attribute("name");
    n_assert(varName);
    nVariable::Handle varHandle = nVariableServer::Instance()->GetVariableHandleByName(varName);
    nVariable newVariable(dataType, varHandle);
    switch (dataType)
    {
        case nVariable::Int:
            newVariable.SetInt(this->GetIntAttr(elm, "value", 0));
            break;

        case nVariable::Float:
            newVariable.SetFloat(this->GetFloatAttr(elm, "value", 0.0f));
            break;

        case nVariable::Vector4:
            {
                vector4 v4(0.0f, 0.0f, 0.0f, 0.0f);
                newVariable.SetVector4(this->GetVector4Attr(elm, "value", v4));
            }
            break;

        case nVariable::Object:
            {
                // initialize a texture object
                const char* filename = elm->Attribute("value");
                n_assert(filename);
                nTexture2* tex = nGfxServer2::Instance()->NewTexture(filename);
                if (!tex->IsLoaded())
                {
                    tex->SetFilename(filename);
                    if (!tex->Load())
                    {
                        n_error("nRpXmlParser::ParseGlobalVariable(): could not load texture '%s'!", filename);
                    }
                }
                newVariable.SetObj(tex);
            }
            break;

        default:
            n_error("nRpXmlParser::ParseGlobalVariable(): invalid datatype for variable '%s'!", varName);
            break;
    }
    return newVariable;
}

//------------------------------------------------------------------------------
/**
    Parse a Float, Vector4, Int or Bool element inside a RenderPath
    element (these are global variable definitions.
*/
void
nRpXmlParser::ParseGlobalVariable(nVariable::Type dataType, TiXmlElement* elm, nRenderPath2* renderPath)
{
    n_assert(elm && renderPath);
    nVariable var = this->ParseVariable(dataType, elm);
    renderPath->AddVariable(var);
}

//------------------------------------------------------------------------------
/**
    Parse a Section element inside a RenderPath element.
*/
void
nRpXmlParser::ParseSection(TiXmlElement* elm, nRenderPath2* renderPath)
{
    n_assert(elm && renderPath);

    // parse attributes
    nRpSection newSection;
    newSection.SetName(elm->Attribute("name"));

    // parse children
    TiXmlElement* child;
    for (child = elm->FirstChildElement(); child; child = child->NextSiblingElement())
    {
        if (child->Value() == nString("Pass"))
        {
            this->ParsePass(child, &newSection);
        }
    }
    renderPath->AddSection(newSection);
}

//------------------------------------------------------------------------------
/**
    Parse a Pass element inside a Section element.
*/
void
nRpXmlParser::ParsePass(TiXmlElement* elm, nRpSection* section)
{
    n_assert(elm && section);

    // parse attributes
    nRpPass newPass;
    newPass.SetName(elm->Attribute("name"));
    newPass.SetShaderAlias(elm->Attribute("shader"));
    nString renderTargetName("renderTarget");
    int i = 0;
    while (this->HasAttr(elm, renderTargetName.Get()))
    {
        newPass.SetRenderTargetName(i, elm->Attribute(renderTargetName.Get()));
        renderTargetName.Set("renderTarget");
        renderTargetName.AppendInt(++i);
    }

    if (this->HasAttr(elm, "stats"))
    {
        newPass.SetStatsEnabled(this->GetBoolAttr(elm, "stats", true));
    }
    int clearFlags = 0;
    if (this->HasAttr(elm, "clearColor"))
    {
        clearFlags |= nGfxServer2::ColorBuffer;
        newPass.SetClearColor(this->GetVector4Attr(elm, "clearColor", vector4(0.0f, 0.0f, 0.0f, 1.0f)));
    }
    if (this->HasAttr(elm, "clearDepth"))
    {
        clearFlags |= nGfxServer2::DepthBuffer;
        newPass.SetClearDepth(this->GetFloatAttr(elm, "clearDepth", 1.0f));
    }
    if (this->HasAttr(elm, "clearStencil"))
    {
        clearFlags |= nGfxServer2::StencilBuffer;
        newPass.SetClearStencil(this->GetIntAttr(elm, "clearStencil", 0));
    }
    newPass.SetClearFlags(clearFlags);
    if (this->HasAttr(elm, "drawQuad"))
    {
        newPass.SetDrawFullscreenQuad(this->GetBoolAttr(elm, "drawQuad", false));
    }
    if (this->HasAttr(elm, "drawShadows"))
    {
        newPass.SetDrawShadows(nRpPass::StringToShadowTechnique(elm->Attribute("drawShadows")));
    }
    if (this->HasAttr(elm, "occlusionQuery"))
    {
        newPass.SetOcclusionQuery(this->GetBoolAttr(elm, "occlusionQuery", false));
    }
    if (this->HasAttr(elm, "drawGui"))
    {
        newPass.SetDrawGui(this->GetBoolAttr(elm, "drawGui", false));
    }
    if (this->HasAttr(elm, "technique"))
    {
        newPass.SetTechnique(elm->Attribute("technique"));
    }
    if (this->HasAttr(elm, "shadowEnabledCondition"))
    {
        newPass.SetShadowEnabledCondition(this->GetBoolAttr(elm, "shadowEnabledCondition", false));
    }

    // parse children
    TiXmlElement* child;
    for (child = elm->FirstChildElement(); child; child = child->NextSiblingElement())
    {
        if (child->Value() == nString("Float"))
        {
            this->ParseShaderState(nShaderState::Float, child, &newPass);
        }
        else if (child->Value() == nString("Float4"))
        {
            this->ParseShaderState(nShaderState::Float4, child, &newPass);
        }
        else if (child->Value() == nString("Int"))
        {
            this->ParseShaderState(nShaderState::Int, child, &newPass);
        }
        else if (child->Value() == nString("Texture"))
        {
            this->ParseShaderState(nShaderState::Texture, child, &newPass);
        }
        else if (child->Value() == nString("Phase"))
        {
            this->ParsePhase(child, &newPass);
        }
    }
    section->AddPass(newPass);
}

//------------------------------------------------------------------------------
/**
    Parse a shader state element inside a Pass XML element.
*/
void
nRpXmlParser::ParseShaderState(nShaderState::Type type, TiXmlElement* elm, nRpPass* pass)
{
    n_assert(elm && pass);

    nShaderState::Param p = nShaderState::StringToParam(elm->Attribute("name"));
    nShaderArg arg(type);
    if (this->HasAttr(elm, "value"))
    {
        // this is a constant shader parameter
        switch (type)
        {
            case nShaderState::Int:
                arg.SetInt(this->GetIntAttr(elm, "value", 0));
                break;

            case nShaderState::Float:
                arg.SetFloat(this->GetFloatAttr(elm, "value", 0.0f));
                break;

            case nShaderState::Float4:
                {
                    nFloat4 f4 = { 0.0f, 0.0f, 0.0f, 0.0f };
                    arg.SetFloat4(this->GetFloat4Attr(elm, "value", f4));
                }
                break;

            case nShaderState::Texture:
                {
                    // initialize a texture object
                    const char* filename = elm->Attribute("value");
                    n_assert(filename);
                    nTexture2* tex = nGfxServer2::Instance()->NewTexture(filename);
                    if (!tex->IsLoaded())
                    {
                        tex->SetFilename(filename);
                        if (!tex->Load())
                        {
                            n_error("nRpXmlParser::ParseGlobalVariable(): could not load texture '%s'!", filename);
                        }
                    }
                    arg.SetTexture(tex);
                }
                break;

            default:
                n_error("nRpXmlParser::ParseShaderState(): invalid datatype '%s'!", elm->Attribute("name"));
                break;
        }
        pass->AddConstantShaderParam(p, arg);
    }
    else if (this->HasAttr(elm, "variable"))
    {
        const char* varName = elm->Attribute("variable");
        pass->AddVariableShaderParam(varName, p, arg);
    }
}

//------------------------------------------------------------------------------
/**
    Parse a shader state element inside a Pass XML element.
*/
void
nRpXmlParser::ParseShaderState(nShaderState::Type type, TiXmlElement* elm, nRpSequence* seq)
{
    n_assert(elm && seq);

    nShaderState::Param p = nShaderState::StringToParam(elm->Attribute("name"));
    nShaderArg arg(type);
    if (this->HasAttr(elm, "value"))
    {
        // this is a constant shader parameter
        switch (type)
        {
            case nShaderState::Int:
                arg.SetInt(this->GetIntAttr(elm, "value", 0));
                break;

            case nShaderState::Float:
                arg.SetFloat(this->GetFloatAttr(elm, "value", 0.0f));
                break;

            case nShaderState::Float4:
                {
                    nFloat4 f4 = { 0.0f, 0.0f, 0.0f, 0.0f };
                    arg.SetFloat4(this->GetFloat4Attr(elm, "value", f4));
                }
                break;

            case nShaderState::Texture:
                {
                    // initialize a texture object
                    const char* filename = elm->Attribute("value");
                    n_assert(filename);
                    nTexture2* tex = nGfxServer2::Instance()->NewTexture(filename);
                    if (!tex->IsLoaded())
                    {
                        tex->SetFilename(filename);
                        if (!tex->Load())
                        {
                            n_error("nRpXmlParser::ParseGlobalVariable(): could not load texture '%s'!", filename);
                        }
                    }
                    arg.SetTexture(tex);
                }
                break;

            default:
                n_error("nRpXmlParser::ParseShaderState(): invalid datatype '%s'!", elm->Attribute("name"));
                break;
        }
        seq->AddConstantShaderParam(p, arg);
    }
    else if (this->HasAttr(elm, "variable"))
    {
        const char* varName = elm->Attribute("variable");
        seq->AddVariableShaderParam(varName, p, arg);
    }
}

//------------------------------------------------------------------------------
/**
    Parse a Phase XML element.
*/
void
nRpXmlParser::ParsePhase(TiXmlElement* elm, nRpPass* pass)
{
    n_assert(elm && pass);

    nRpPhase newPhase;

    // read attributes
    newPhase.SetName(elm->Attribute("name"));
    newPhase.SetShaderAlias(elm->Attribute("shader"));
    newPhase.SetSortingOrder(nRpPhase::StringToSortingOrder(elm->Attribute("sort")));
    newPhase.SetLightMode(nRpPhase::StringToLightMode(elm->Attribute("lightMode")));
    if (this->HasAttr(elm, "technique"))
    {
        newPhase.SetTechnique(elm->Attribute("technique"));
    }

    // read Sequence elements
    TiXmlElement* child;
    for (child = elm->FirstChildElement("Sequence"); child; child = child->NextSiblingElement("Sequence"))
    {
        this->ParseSequence(child, &newPhase);
    }
    pass->AddPhase(newPhase);
}

//------------------------------------------------------------------------------
/**
    Parse a Sequence XML element.
*/
void
nRpXmlParser::ParseSequence(TiXmlElement* elm, nRpPhase* phase)
{
    n_assert(elm && phase);
    nRpSequence newSequence;
    newSequence.SetShaderAlias(elm->Attribute("shader"));
    if (this->HasAttr(elm, "technique"))
    {
        newSequence.SetTechnique(elm->Attribute("technique"));
    }
    newSequence.SetShaderUpdatesEnabled(this->GetBoolAttr(elm, "shaderUpdates", true));
    newSequence.SetFirstLightAlphaEnabled(this->GetBoolAttr(elm, "firstLightAlpha", false));
    newSequence.SetMvpOnlyHint(this->GetBoolAttr(elm, "mvpOnly", false));

    // parse children
    TiXmlElement* child;
    for (child = elm->FirstChildElement(); child; child = child->NextSiblingElement())
    {
        if (child->Value() == nString("Float"))
        {
            this->ParseShaderState(nShaderState::Float, child, &newSequence);
        }
        else if (child->Value() == nString("Float4"))
        {
            this->ParseShaderState(nShaderState::Float4, child, &newSequence);
        }
        else if (child->Value() == nString("Int"))
        {
            this->ParseShaderState(nShaderState::Int, child, &newSequence);
        }
        else if (child->Value() == nString("Texture"))
        {
            this->ParseShaderState(nShaderState::Texture, child, &newSequence);
        }
    }
    phase->AddSequence(newSequence);
}









