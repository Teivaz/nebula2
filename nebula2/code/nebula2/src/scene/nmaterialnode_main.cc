#define N_IMPLEMENTS nMaterialNode
//------------------------------------------------------------------------------
//  nmaterialnode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmaterialnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ntexture2.h"

nNebulaScriptClass(nMaterialNode, "nabstractshadernode");

//------------------------------------------------------------------------------
/**
*/
nMaterialNode::nMaterialNode() :
    modelViewVarHandle(nVariable::INVALID_HANDLE),
    modelViewProjectionVarHandle(nVariable::INVALID_HANDLE),
    modelEyePosVarHandle(nVariable::INVALID_HANDLE),
    modelVarHandle(nVariable::INVALID_HANDLE)
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
    Compute the "shader variable mask". Each variable which is used by
    this shader gets a 1-bit in the mask.
    NOTE: this implementation is limited to at most 32 shader and texture
    variables.
*/
void
nMaterialNode::UpdateShaderVariableMasks()
{
    int shaderIndex;
    for (shaderIndex = 0; shaderIndex < this->shaderArray.Size(); shaderIndex++)
    {
        if (!this->shaderArray[shaderIndex].IsVariableMaskValid())
        {
            nShader2* shd = this->shaderArray[shaderIndex].GetShader();
            n_assert(shd);

            nVariableServer* varServer = this->refVariableServer.get();
            uint mask = 0;
            int bitMask = 1;

            // handle shader variables
            int numVariables = this->varContext.GetNumVariables();
            int varIndex;
            for (varIndex = 0; varIndex < numVariables; varIndex++, bitMask <<= 1)
            {
                const nVariable& var = this->varContext.GetVariableAt(varIndex);
                if (shd->IsParameterUsed(var.GetHandle()))
                {
                    mask |= bitMask;
                }
            }

            // handle texture variables
            int numTextures = this->texNodeArray.Size();
            int texIndex;
            for (texIndex = 0; texIndex < numTextures; texIndex++, bitMask <<= 1)
            {
                if (shd->IsParameterUsed(this->texNodeArray[texIndex].varHandle))
                {
                    mask |= bitMask;
                }
            }
            
            this->shaderArray[shaderIndex].SetVariableMask(mask);
            this->shaderArray[shaderIndex].SetVariableMaskValid(true);
        }
    }
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
        if ((!shaderEntry.IsShaderValid()) && (shaderEntry.GetName()))
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
                    n_printf("nMaterialNode: Error loading shader '%s'\n", shaderEntry.GetName());
                    return false;
                }
            }
            shaderEntry.SetShader(shd);
            shaderEntry.SetVariableMaskValid(false);
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
    if (nAbstractShaderNode::LoadResources())
    {
        if (this->LoadShaders())
        {
            // validate internal variable handles
            if (nVariable::INVALID_HANDLE == this->modelEyePosVarHandle)
            {
                this->modelEyePosVarHandle = this->refVariableServer->GetVariableHandleByName("modelEyePos");
                n_assert(nVariable::INVALID_HANDLE != this->modelEyePosVarHandle);
            }
            if (nVariable::INVALID_HANDLE == this->modelViewVarHandle)
            {
                this->modelViewVarHandle = this->refVariableServer->GetVariableHandleByName("modelView");
                n_assert(nVariable::INVALID_HANDLE != this->modelViewVarHandle);
            }
            if (nVariable::INVALID_HANDLE == this->modelViewProjectionVarHandle)
            {
                this->modelViewProjectionVarHandle = this->refVariableServer->GetVariableHandleByName("modelViewProjection");
                n_assert(nVariable::INVALID_HANDLE != this->modelViewProjectionVarHandle);
            }
            if (nVariable::INVALID_HANDLE == this->modelVarHandle)
            {
                this->modelVarHandle = this->refVariableServer->GetVariableHandleByName("model");
                n_assert(nVariable::INVALID_HANDLE != this->modelVarHandle);
            }

            // validate shader variable masks
            this->UpdateShaderVariableMasks();
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
    Check if resources are valid.
*/
bool
nMaterialNode::AreResourcesValid() const
{
    if (nAbstractShaderNode::AreResourcesValid())
    {
        bool valid = true;

        // check shaders
        int i;
        for (i = 0; i < this->shaderArray.Size(); i++)
        {
            valid &= this->shaderArray[i].IsShaderValid();
            valid &= this->shaderArray[i].IsVariableMaskValid();
        }

        // check internal variable handles
        valid &= (this->modelEyePosVarHandle != nVariable::INVALID_HANDLE);
        valid &= (this->modelViewVarHandle != nVariable::INVALID_HANDLE);
        valid &= (this->modelViewProjectionVarHandle != nVariable::INVALID_HANDLE);
        valid &= (this->modelVarHandle != nVariable::INVALID_HANDLE);

        return valid;
    }
    return false;
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
        if (this->shaderArray[i].GetFourCC() == fourcc)
        {
            return &(this->shaderArray[i]);
        }
    }
    // fallthrough: no shader matches this fourcc code
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
*/
void
nMaterialNode::RenderShader(uint fourcc, nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);
    nVariableServer* varServer = this->refVariableServer.get();
    nGfxServer2* gfxServer = this->refGfxServer.get();

    // find shader matching fourcc code, do nothing if shader not exists
    ShaderEntry* shaderEntry = this->FindShaderEntry(fourcc);
    if (0 == shaderEntry)
    {
        return;
    }

    // see if any resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
    nShader2* shader = shaderEntry->GetShader();

    // invoke shader manipulators
    this->InvokeShaderAnimators(renderContext);

    // apply int, float and vector variables to the shader
    uint usedBit = 1;
    uint usedMask = shaderEntry->GetVariableMask();
    int numShaderVariables = this->varContext.GetNumVariables();
    int i;
    for (i = 0; i < numShaderVariables; i++, usedBit <<= 1)
    {
        // check if the variable is actually used by the shader
        if (usedMask & usedBit)
        {
            const nVariable& var = this->varContext.GetVariableAt(i);
            nVariable::Handle varHandle = var.GetHandle();
            switch (var.GetType())
            {
                case nVariable::INT:
                    shader->SetInt(varHandle, var.GetInt());
                    break;

                case nVariable::FLOAT:
                    shader->SetFloat(varHandle, var.GetFloat());
                    break;

                case nVariable::VECTOR:
                    shader->SetVector(varHandle, var.GetVector());
                    break;

                default:
                    n_assert(false);
                    break;
            }
        }
    }

    // apply texture shader variables to the shader
    int numTextureVariables = this->texNodeArray.Size();
    for (i = 0; i < numTextureVariables; i++, usedBit <<= 1)
    {
        // check if the texture variable is actually used by the shader
        if (usedMask & usedBit)
        {
            TexNode& texNode = this->texNodeArray[i];
            shader->SetTexture(texNode.varHandle, texNode.refTexture.get());
        }
    }

    // transfer matrices to the shader
    if (shader->IsParameterUsed(this->modelViewVarHandle))
    {
        const matrix44& modelView = gfxServer->GetTransform(nGfxServer2::MODELVIEW);
        shader->SetMatrix(this->modelViewVarHandle, modelView);
    }
    if (shader->IsParameterUsed(this->modelViewProjectionVarHandle))
    {
        const matrix44& modelViewProj = gfxServer->GetTransform(nGfxServer2::MODELVIEWPROJECTION);
        shader->SetMatrix(this->modelViewProjectionVarHandle, modelViewProj);
    }
    if (shader->IsParameterUsed(this->modelVarHandle))
    {
        const matrix44& model = gfxServer->GetTransform(nGfxServer2::MODEL);
        shader->SetMatrix(this->modelVarHandle, model);
    }
    if (shader->IsParameterUsed(this->modelEyePosVarHandle))
    {
        const matrix44& invModelView  = refGfxServer->GetTransform(nGfxServer2::INVMODELVIEW);
        float4 modelEyePos = { invModelView.M41, invModelView.M42, invModelView.M43, 1.0f };
        shader->SetVector(this->modelEyePosVarHandle, modelEyePos);
    }

    // render the shader
    this->refGfxServer->SetShader(shader);
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
        this->shaderArray.PushBack(newShaderEntry);
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

