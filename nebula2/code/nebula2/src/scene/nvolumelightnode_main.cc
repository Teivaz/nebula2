#define N_IMPLEMENTS nVolumeLightNode
//------------------------------------------------------------------------------
//  nvolumelightnode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nvolumelightnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "scene/nsceneserver.h"

nNebulaClass(nVolumeLightNode, "nabstractshadernode");

//------------------------------------------------------------------------------
/**
*/
nVolumeLightNode::nVolumeLightNode() :
    modelLightPosVarHandle(nVariable::INVALID_HANDLE)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nVolumeLightNode::~nVolumeLightNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nVolumeLightNode::HasLightVolume() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    This simply validates the variable handles used by this object.
*/
bool
nVolumeLightNode::LoadResources()
{
    if (nVariable::INVALID_HANDLE == this->modelLightPosVarHandle)
    {
        this->modelLightPosVarHandle = this->refVariableServer->GetVariableHandleByName("modelLightPos");
        n_assert(nVariable::INVALID_HANDLE != this->modelLightPosVarHandle);
    }
    return nAbstractShaderNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
    Check whether the variable handles are valid.
*/
bool
nVolumeLightNode::AreResourcesValid() const
{
    if (nAbstractShaderNode::AreResourcesValid())
    {
        return (this->modelLightPosVarHandle != nVariable::INVALID_HANDLE);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This "renders" the light. The scene server calls this method for
    each object which needs to be lighted by this light source. The light
    source should get the current shader object from the graphics server,
    and modify its parameters (since the shader is responsible for 
    lighting this object in the end).

    The only hardcoded lighting parameter is:

    modelLightMatrix    - 4x4 transform matrix of light in model space

    Depending on the lighting system in the shader, any other parameters
    can be set in the nVolumeLightNode, which are then simply transfered
    to the shader.
*/
void
nVolumeLightNode::RenderLightVolume(nSceneServer* sceneServer, 
                                    nRenderContext* renderContext,
                                    const matrix44& lightModelView)
{
    n_assert(sceneServer);
    n_assert(renderContext);

    nGfxServer2* gfxServer = this->refGfxServer.get();
    nVariableServer* varServer = this->refVariableServer.get();

    // invoke shader animators
    this->InvokeShaderAnimators(renderContext);

    // see if any resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
    
    // get the current shader object from the gfx server
    nShader2* shader = this->refGfxServer->GetShader();
    n_assert(shader);

    // apply texture shader variables to the shader
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        TexNode& texNode = this->texNodeArray[i];
        shader->SetTexture(texNode.varHandle, texNode.refTexture.get());
    }

    // apply int, float and vector variables to the shader
    num = this->varContext.GetNumVariables();
    for (i = 0; i < num; i++)
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

    // finally, compute our transformation in light space and transfer 
    // it to the shader
    const matrix44& invObjModelView = refGfxServer->GetTransform(nGfxServer2::INVMODELVIEW);
    matrix44 modelLightMatrix = lightModelView * invObjModelView;
    float4 modelLightPos = {
        modelLightMatrix.M41,
        modelLightMatrix.M42,
        modelLightMatrix.M43,
        1.0f
    };

//    shader->SetMatrix("modelLightMatrix", modelLightMatrix);
    shader->SetVector(this->modelLightPosVarHandle, modelLightPos);
}
