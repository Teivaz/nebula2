//------------------------------------------------------------------------------
//  nswingshapenode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nature/nswingshapenode.h"
#include "variable/nvariableserver.h"
#include "scene/nrendercontext.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nSwingShapeNode, "nshapenode");

//------------------------------------------------------------------------------
/**
*/
nSwingShapeNode::nSwingShapeNode() :
    timeVarHandle(nVariable::InvalidHandle),
    windVarHandle(nVariable::InvalidHandle),
    swingAngle(45.0f),
    swingTime(5.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSwingShapeNode::~nSwingShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method must return the mesh usage flag combination required by
    this shape node class. Subclasses should override this method
    based on their requirements.

    @return     a combination on nMesh2::Usage flags
*/
int
nSwingShapeNode::GetMeshUsage() const
{
    return nMesh2::WriteOnce | nMesh2::NeedsVertexShader;
}

//------------------------------------------------------------------------------
/**
    This validates the variable handles for time and wind.
*/
bool
nSwingShapeNode::LoadResources()
{
    if (nShapeNode::LoadResources())
    {
        this->timeVarHandle = this->refVariableServer->GetVariableHandleByName("time");
        this->windVarHandle = this->refVariableServer->GetVariableHandleByName("wind");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This invalidates the variable handles for time and wind.
*/
void
nSwingShapeNode::UnloadResources()
{
    this->timeVarHandle = nVariable::InvalidHandle;
    this->windVarHandle = nVariable::InvalidHandle;
    nShapeNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Permute the provided static angle by time and world space position. Lots
    of constants here, but the result is quite reasonable for a swinging tree.
    The swinging geometry should not move around in world space, as that
    would break the position offset and lead to stuttering in the
    geometry.

    @param  pos             position in world space
    @param  time            current time
*/
float
nSwingShapeNode::ComputeAngle(const vector3& pos, nTime time) const
{
    // add position offset to time to prevent that all trees swing in sync
    time += pos.x + pos.y + pos.z;

    // sinus wave swing value (between +1 and -1)
    float swing = (float) n_sin((time * n_deg2rad(360.0f)) / this->swingTime);

    // get a wind strength "swinging" angle, we want no swinging at
    // min and max wind strength, and max swinging at 0.5 wind strength
    return this->swingAngle * 0.3f + (this->swingAngle * swing * 0.7f);
}

//------------------------------------------------------------------------------
/**
    This computes the Swing rotation matrix and bounding box parameters needed by
    the shaders which implement swinging geometry.
*/
bool
nSwingShapeNode::RenderShader(uint fourcc, nSceneServer* sceneServer, nRenderContext* renderContext)
{
    if (nMaterialNode::RenderShader(fourcc, sceneServer, renderContext))
    {
        // get current wind dir and wind strength
        nVariable* timeVar = renderContext->GetVariable(this->timeVarHandle);
        nVariable* windVar = renderContext->GetVariable(this->windVarHandle);
        n_assert(timeVar && windVar);
        nTime time = (nTime) timeVar->GetFloat();
        const nFloat4& wind = windVar->GetFloat4();

        // build horizontal wind vector
        vector3 windVec(wind.x, wind.y, wind.z);

        // get current position in world space
        const matrix44& model = this->refGfxServer->GetTransform(nGfxServer2::Model);

        // implement swinging by permuting angle by time and position
        float permutedAngle = this->ComputeAngle(model.pos_component(), time);

        // build a rotation matrix from the permuted angle
        static const vector3 up(0.0f, 1.0f, 0.0f);
        matrix44 rotMatrix;
        rotMatrix.rotate(windVec * up, permutedAngle);

        // set shader parameter
        nShader2* shader = this->refGfxServer->GetShader();
        n_assert(shader);
        if (shader->IsParameterUsed(nShader2::Swing))
        {
            shader->SetMatrix(nShader2::Swing, rotMatrix);
        }

        // set bounding box parameters
        if (shader->IsParameterUsed(nShader2::BoxMinPos))
        {
            shader->SetVector3(nShader2::BoxMinPos, this->localBox.vmin);
        }
        if (shader->IsParameterUsed(nShader2::BoxMaxPos))
        {
            shader->SetVector3(nShader2::BoxMaxPos, this->localBox.vmax);
        }
        if (shader->IsParameterUsed(nShader2::BoxCenter))
        {
            shader->SetVector3(nShader2::BoxCenter, this->localBox.center());
        }

        // set wind shader parameter
        if (shader->IsParameterUsed(nShader2::Wind))
        {
            shader->SetFloat4(nShader2::Wind, windVar->GetFloat4());
        }

        return true;
    }
    return false;
}
