//------------------------------------------------------------------------------
//  nparticleshapenode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nparticleshapenode.h"
#include "variable/nvariableserver.h"
#include "scene/nrendercontext.h"
#include "scene/nsceneserver.h"
#include "kernel/ntimeserver.h"

nNebulaScriptClass(nParticleShapeNode, "nshapenode");

//------------------------------------------------------------------------------
/**
*/
nParticleShapeNode::nParticleShapeNode() :
    refParticleServer("/sys/servers/particle"),
    refVariableServer("/sys/servers/variable"),
    emissionDuration(10.0),
    loop(true),
    activityDistance(10.0f),
    spreadAngle(0.0f),
    birthDelay(0.0f),
    emitterVarIndex(-1)
{
    int i;
    for (i=0; i<4; i++)
        this->curves[nParticleEmitter::ParticleVelocityFactor].keyFrameValues[i] = 1.0;
    SetMeshUsage(nMesh2::ReadOnly | nMesh2::PointSprite | nMesh2::NeedsVertexShader);
}

//------------------------------------------------------------------------------
/**
*/
nParticleShapeNode::~nParticleShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Compute the resulting modelview matrix and set it in the scene
    server as current modelview matrix.
*/
bool
nParticleShapeNode::RenderTransform(nSceneServer* sceneServer, 
                                nRenderContext* renderContext, 
                                const matrix44& parentMatrix)
{
    n_assert(sceneServer);
    n_assert(renderContext);
    this->InvokeTransformAnimators(renderContext);
    this->UpdateMatrix();

    // get emitter from render context
    nVariableServer* varServer = this->refVariableServer.get();
    nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
    int emitterKey = varEmitter.GetInt();
    nParticleEmitter* emitter = this->refParticleServer->GetParticleEmitter(emitterKey);

    // keep emitter alive
    if (0 != emitter)
    {
        emitter->KeepAlive();
    }
    else
    {
        // need new emitter
        emitter = this->refParticleServer.get()->NewParticleEmitter();
        n_assert(0 != emitter);

        varEmitter.SetInt(emitter->GetKey());
    }

    // setup emitter
    emitter->SetMeshGroupIndex(this->groupIndex);
    emitter->SetEmitterMesh(this->refMesh.get());
    emitter->SetTransform(this->matrix * parentMatrix);
    int windVarHandle = this->refVariableServer->GetVariableHandleByName("wind");
    nVariable* windVar = renderContext->GetVariable(windVarHandle);
    emitter->SetWind(windVar->GetFloat4());

    // set emitter settings
    emitter->SetEmissionDuration(this->emissionDuration);
    emitter->SetLoop(this->loop);
    emitter->SetActivityDistance(this->activityDistance);
    emitter->SetSpreadAngle(this->spreadAngle);
    emitter->SetBirthDelay(this->birthDelay);
    emitter->SetStartRotation(this->startRotation);
    int curveType;
    for (curveType = 0; curveType < nParticleEmitter::CurveTypeCount; curveType++)
    {
        emitter->SetCurve((nParticleEmitter::CurveType) curveType, this->curves[curveType]);
    }
    emitter->SetRGBCurve(this->rgbCurve);

    sceneServer->SetModelTransform(matrix44());
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode::RenderContextCreated(nRenderContext* renderContext)
{
    nShapeNode::RenderContextCreated(renderContext);

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    nParticleEmitter* emitter = this->refParticleServer.get()->NewParticleEmitter();
    n_assert(0 != emitter);

    // put emitter key in render context
    this->emitterVarIndex = renderContext->AddLocalVar(nVariable(0, emitter->GetKey()));
}


//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode::Attach(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    nShapeNode::Attach(sceneServer, renderContext);

    
}

//------------------------------------------------------------------------------
/**
*/
bool
nParticleShapeNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);
	nGfxServer2* gfx = this->refGfxServer.get();

    nTime curTime = kernelServer->GetTimeServer()->GetTime();

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    nVariableServer* varServer = this->refVariableServer.get();
    const nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
    int emitterKey = varEmitter.GetInt();

    nParticleEmitter* emitter = this->refParticleServer->GetParticleEmitter(emitterKey);
    n_assert(0 != emitter);
    emitter->Render();

    return true;
}
