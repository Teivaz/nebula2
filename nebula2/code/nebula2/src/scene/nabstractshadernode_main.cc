#define N_IMPLEMENTS nAbstractShaderNode
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
nAbstractShaderNode::nAbstractShaderNode() :
    refGfxServer("/sys/servers/gfx"),
    refVariableServer("/sys/servers/variable")
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
        nTexture2* tex = this->refGfxServer->NewTexture(texNode.texName.Get());
        n_assert(tex);
        if (!tex->IsValid())
        {
            tex->SetFilename(texNode.texName.Get());
            if (!tex->Load())
            {
                n_printf("nMaterialNode::Error loading texture '%s'\n", texNode.texName.Get());
                return false;
            }
        }
        texNode.refTexture = tex;
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
    Return false if invalid resources exist.
*/
bool
nAbstractShaderNode::AreResourcesValid() const
{
    if (nSceneNode::AreResourcesValid())
    {
        int i;
        int num = this->texNodeArray.Size();
        for (i = 0; i < num; i++)
        {
            if (!this->texNodeArray[i].refTexture.isvalid())
            {
                return false;
            }
        }
        // fallthrough: all textures valid
        return true;
    }
    else
    {
        return false;
    }
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
nAbstractShaderNode::SetTexture(nVariable::Handle varHandle, const char* texName)
{
    n_assert(nVariable::INVALID_HANDLE != varHandle);
    n_assert(texName);

    // see if texture variable already exists
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->texNodeArray[i].varHandle == varHandle)
        {
            break;
        }
    }
    if (i == num)
    {
        // add new texnode to array
        TexNode newTexNode(varHandle, texName);
        this->texNodeArray.PushBack(newTexNode);
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
nAbstractShaderNode::GetTexture(nVariable::Handle varHandle) const
{
    n_assert(nVariable::INVALID_HANDLE != varHandle);

    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->texNodeArray[i].varHandle == varHandle)
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
void
nAbstractShaderNode::SetInt(nVariable::Handle varHandle, int val)
{
    n_assert(varHandle != nVariable::INVALID_HANDLE);
    nVariable* var = this->varContext.GetVariable(varHandle);
    if (var)
    {
        var->SetInt(val);
    }
    else
    {
        this->varContext.AddVariable(nVariable(varHandle, val));
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nAbstractShaderNode::GetInt(nVariable::Handle varHandle) const
{
    n_assert(varHandle != nVariable::INVALID_HANDLE);
    nVariable* var = this->varContext.GetVariable(varHandle);
    n_assert(var);
    return var->GetInt();
}

//------------------------------------------------------------------------------
/**
*/
void
nAbstractShaderNode::SetFloat(nVariable::Handle varHandle, float val)
{
    n_assert(varHandle != nVariable::INVALID_HANDLE);
    nVariable* var = this->varContext.GetVariable(varHandle);
    if (var)
    {
        var->SetFloat(val);
    }
    else
    {
        nVariable newVar(varHandle, val);
        this->varContext.AddVariable(nVariable(varHandle, val));
    }
}

//------------------------------------------------------------------------------
/**
*/
float
nAbstractShaderNode::GetFloat(nVariable::Handle varHandle) const
{
    n_assert(varHandle != nVariable::INVALID_HANDLE);
    nVariable* var = this->varContext.GetVariable(varHandle);
    n_assert(var);
    return var->GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
void
nAbstractShaderNode::SetVector(nVariable::Handle varHandle, const vector4& val)
{
    n_assert(varHandle != nVariable::INVALID_HANDLE);
    nVariable* var = this->varContext.GetVariable(varHandle);
    float4 float4Val = 
    {
        val.x, val.y, val.z, val.w
    };
    if (var)
    {
        var->SetVector(float4Val);
    }
    else
    {
        this->varContext.AddVariable(nVariable(varHandle, float4Val));
    }
}

//------------------------------------------------------------------------------
/**
*/
vector4
nAbstractShaderNode::GetVector(nVariable::Handle varHandle) const
{
    n_assert(varHandle != nVariable::INVALID_HANDLE);
    nVariable* var = this->varContext.GetVariable(varHandle);
    n_assert(var);
    const float4& float4Val = var->GetVector();
    return vector4(float4Val[0], float4Val[1], float4Val[2], float4Val[3]);
}


