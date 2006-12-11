//------------------------------------------------------------------------------
//  nabstractshadernode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nabstractshadernode.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nAbstractShaderNode, "ntransformnode");

//------------------------------------------------------------------------------
/**
*/
nAbstractShaderNode::nAbstractShaderNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAbstractShaderNode::~nAbstractShaderNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Unload a texture resource.
*/
void
nAbstractShaderNode::UnloadTexture(int index)
{
    TexNode& texNode = this->texNodeArray[index];
    if (texNode.refTexture.isvalid())
    {
        texNode.refTexture->Release();
        texNode.refTexture.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Load a texture resource.
*/
bool
nAbstractShaderNode::LoadTexture(int index)
{
    TexNode& texNode = this->texNodeArray[index];
    if ((!texNode.refTexture.isvalid()) && (!texNode.texName.IsEmpty()))
    {
        // load only if the texture is used in the shader
        if (this->IsTextureUsed(texNode.shaderParameter))
        {
            nTexture2* tex = nGfxServer2::Instance()->NewTexture(texNode.texName);
            n_assert(tex);
            if (!tex->IsLoaded())
            {
                tex->SetFilename(texNode.texName);
                if (!tex->Load())
                {
                    n_printf("nAbstractShaderNode: Error loading texture '%s'\n", texNode.texName.Get());
                    return false;
                }
            }
            texNode.refTexture = tex;
            this->shaderParams.SetArg(texNode.shaderParameter, nShaderArg(tex));
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nAbstractShaderNode::LoadResources()
{
    if (nSceneNode::LoadResources())
    {
        int i;
        int num = this->texNodeArray.Size();
        for (i = 0; i < num; i++)
        {
            this->LoadTexture(i);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nAbstractShaderNode::UnloadResources()
{
    nSceneNode::UnloadResources();
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        this->UnloadTexture(i);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nAbstractShaderNode::SetTexture(nShaderState::Param param, const char* texName)
{
    n_assert(texName);

    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }

    // see if texture variable already exists
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->texNodeArray[i].shaderParameter == param)
        {
            break;
        }
    }
    if (i == num)
    {
        // add new texnode to array
        TexNode newTexNode(param, texName);
        this->texNodeArray.Append(newTexNode);
    }
    else
    {
        // invalidate existing texture
        this->UnloadTexture(i);
        this->texNodeArray[i].texName = texName;
    }
    // flag to load resources
    this->resourcesValid = false;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nAbstractShaderNode::GetTexture(nShaderState::Param param) const
{
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->texNodeArray[i].shaderParameter == param)
        {
            return texNodeArray[i].texName.Get();
        }
    }
    // fallthrough: invalid variable name
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAbstractShaderNode::IsTextureUsed(nShaderState::Param /*param*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
   Returns true, if param is valid
*/
bool
nAbstractShaderNode::HasParam(nShaderState::Param param)
{
    return this->shaderParams.IsParameterValid(param);
}

//------------------------------------------------------------------------------
/**
   get nShaderParams from node
*/
nShaderParams&
nAbstractShaderNode::GetShaderParams()
{
    return this->shaderParams;
}

