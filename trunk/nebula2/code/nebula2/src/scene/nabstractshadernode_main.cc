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
	        nTexture2* tex = this->refGfxServer->NewTexture(texNode.texName.Get());
	        n_assert(tex);
	        if (!tex->IsValid())
	        {
	            tex->SetFilename(texNode.texName.Get());
	            if (!tex->Load())
	            {
						n_printf("nAbstractShaderNode: Error loading texture '%s'\n", texNode.texName.Get());
	                return false;
	            }
	        }
	        texNode.refTexture = tex;
            this->shaderParams.SetTexture(texNode.shaderParameter, tex);
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
nAbstractShaderNode::SetTexture(nShader2::Parameter param, const char* texName)
{
    n_assert(texName);

    // silently ignore invalid parameters
    if (nShader2::InvalidParameter == param)
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
}

//------------------------------------------------------------------------------
/**
*/
const char*
nAbstractShaderNode::GetTexture(nShader2::Parameter param) const
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

