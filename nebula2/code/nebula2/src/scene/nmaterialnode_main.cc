//------------------------------------------------------------------------------
//  nmaterialnode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmaterialnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ntexture2.h"
#include "kernel/ntimeserver.h"
#include "scene/nrendercontext.h"
#include "scene/nsceneserver.h"
#include "kernel/ndebug.h"

nNebulaScriptClass(nMaterialNode, "nabstractshadernode");

//------------------------------------------------------------------------------
/**
*/
nMaterialNode::nMaterialNode() :
    shaderArray(4, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMaterialNode::~nMaterialNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Unload all shaders.
*/
void
nMaterialNode::UnloadShaders()
{
    int i;
    for (i = 0; i < this->shaderArray.Size(); i++)
    {
        if (this->shaderArray[i].IsShaderValid())
        {
            this->shaderArray[i].GetShader()->Release();
            this->shaderArray[i].Invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Load shader resources.
*/
bool
nMaterialNode::LoadShaders()
{
    // load shaders
    int i;
    for (i = 0; i < this->shaderArray.Size(); i++)
    {
        ShaderEntry& shaderEntry = this->shaderArray[i];
        if ((!shaderEntry.IsShaderValid()) && 
            (shaderEntry.GetName()) &&
            (this->refSceneServer->IsShaderUsed(shaderEntry.GetFourCC())))
        {
            // create a new empty shader object
            nShader2* shd = this->refGfxServer->NewShader(shaderEntry.GetName());
            n_assert(shd);
            if (!shd->IsValid())
            {
                // load shader resource file
                shd->SetFilename(shaderEntry.GetName());
                if (!shd->Load())
                {
                    shd->Release();
                    shd = 0;
                }
            }
            if (shd)
            {
            	shaderEntry.SetShader(shd);
			}
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nMaterialNode::LoadResources()
{
    if (this->LoadShaders())
    {
		if (nAbstractShaderNode::LoadResources())
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nMaterialNode::UnloadResources()
{
    nAbstractShaderNode::UnloadResources();
    this->UnloadShaders();
}

//------------------------------------------------------------------------------
/**
    Find shader object associated with fourcc code.
*/
nMaterialNode::ShaderEntry*
nMaterialNode::FindShaderEntry(uint fourcc) const
{
    int i;
    int numShaders = this->shaderArray.Size();
    for (i = 0; i < numShaders; i++)
    {
        ShaderEntry& shaderEntry = this->shaderArray[i];
        if (shaderEntry.GetFourCC() == fourcc)
        {
            return &shaderEntry;
        }
    }
    // fallthrough: no loaded shader matches this fourcc code
    return 0;
}

//------------------------------------------------------------------------------
/**
    Indicate to scene server that we provide a shader.
*/
bool
nMaterialNode::HasShader(uint fourcc) const
{
    return (this->FindShaderEntry(fourcc) != 0);
}

//------------------------------------------------------------------------------
/**
    Update shader and set as current shader in the gfx server.

    - 15-Jan-04     floh    AreResourcesValid()/LoadResources() moved to scene server
*/
bool
nMaterialNode::RenderShader(uint fourcc, nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // find shader matching fourcc code, do nothing if shader not exists
    ShaderEntry* shaderEntry = this->FindShaderEntry(fourcc);
    if (0 == shaderEntry)
    {
        #ifdef _DEBUG
            n_printf("WARNING:" __FILE__ "Shader '%i' not found!", fourcc);
        #endif
        return false;
    }

    // do nothing if shader could not be loaded
    if (!shaderEntry->IsShaderValid())
    {
        return false;
    }
    nShader2* shader = shaderEntry->GetShader();

    // invoke shader manipulators
    this->InvokeShaderAnimators(renderContext);

    // set texture transforms
    n_assert(nGfxServer2::MaxTextureStages >= 4);
    static matrix44 m;
    this->textureTransform[0].getmatrix44(m);
    gfxServer->SetTransform(nGfxServer2::Texture0, m);

    this->textureTransform[1].getmatrix44(m);
    gfxServer->SetTransform(nGfxServer2::Texture1, m);

    this->textureTransform[2].getmatrix44(m);
    gfxServer->SetTransform(nGfxServer2::Texture2, m);

    this->textureTransform[3].getmatrix44(m);
    gfxServer->SetTransform(nGfxServer2::Texture3, m);

    // transfer shader parameters en block
    shader->SetParams(this->shaderParams);

    // make shader current
    this->refGfxServer->SetShader(shader);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nMaterialNode::SetShader(uint fourcc, const char* name)
{
    n_assert(name);
    ShaderEntry* shaderEntry = this->FindShaderEntry(fourcc);
    if (shaderEntry)
    {
        shaderEntry->Invalidate();
        shaderEntry->SetName(name);
    }
    else
    {
        ShaderEntry newShaderEntry(fourcc, name);
        this->shaderArray.Append(newShaderEntry);
    }
}

//------------------------------------------------------------------------------
/**
*/
const char*
nMaterialNode::GetShader(uint fourcc) const
{
    ShaderEntry* shaderEntry = this->FindShaderEntry(fourcc);
    if (shaderEntry)
    {
        return shaderEntry->GetName();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialNode::IsTextureUsed(nShaderState::Param param)
{
	// check in all shaders if anywhere the texture specified by param is used
    int	i;
    int numShaders = this->shaderArray.Size();
    for (i = 0; i < numShaders; i++)
    {
		const ShaderEntry& shaderEntry = this->shaderArray[i];

		// first be sure that the shader entry could be loaded
		if (shaderEntry.IsShaderValid())
		{		
			nShader2* shader = shaderEntry.GetShader();
			if (shader->IsParameterUsed(param))
            {
                return true;
            }
		}
	}
    // fallthrough: texture not used by any shader
	return false;
}
