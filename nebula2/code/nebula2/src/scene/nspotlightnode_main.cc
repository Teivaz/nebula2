//------------------------------------------------------------------------------
//  nspotlightnode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nspotlightnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "scene/nrendercontext.h"

nNebulaScriptClass(nSpotLightNode, "nlightnode");

//------------------------------------------------------------------------------
/**
*/
nSpotLightNode::nSpotLightNode() :
    camera(45.0f, 1.0f, 0.1f, 10.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSpotLightNode::~nSpotLightNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Define the cone parameters. Since the spot light essentially defines
    a projection transformation, an nCamera2 object is used for the cone
    definition.

    @param  c   an nCamera2 object defining the cone
*/
void
nSpotLightNode::SetCamera(const nCamera2& c)
{
    this->camera = c;
}

//------------------------------------------------------------------------------
/**
    Returns the cone definition.

    @return     an nCamera2 object holding the cone definition
*/
const nCamera2&
nSpotLightNode::GetCamera() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
    Set perspective projection parameters.

    @param  aov     angle of view in degree
    @param  aspect  aspect ration
    @param  nearp   near plane distance
    @param  farp    far plane distance
*/
void
nSpotLightNode::SetPerspective(float aov, float aspect, float nearp, float farp)
{
    this->camera.SetPerspective(aov, aspect, nearp, farp);
}

//------------------------------------------------------------------------------
/**
    Set orthogonal projection parameters.

    @param  w       view volume width
    @param  h       view volume height
    @param  nearp   near plane distance
    @param  farp    far plane distance
*/
void
nSpotLightNode::SetOrthogonal(float w, float h, float nearp, float farp)
{
    this->camera.SetOrthogonal(w, h, nearp, farp);
}

//------------------------------------------------------------------------------
/**
    "Render" the spot light node.

    @param  sceneServer         pointer to the scene server object rendering this node
    @param  renderContext       pointer to the render context for this node
    @param  lightTransform      this node's model matrix
*/
bool
nSpotLightNode::RenderLight(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& lightTransform)
{
    nGfxServer2* gfxServer = this->refGfxServer.get();
    nShader2* shader = this->refGfxServer->GetShader();
    n_assert(shader);

    // apply user-defined shader parameters
    if (!nLightNode::RenderLight(sceneServer, renderContext, lightTransform))
    {
        return false;
    }

    // compute the modelLightProjection matrix
    if (shader->IsParameterUsed(nShader2::ModelLightProjection))
    {
        matrix44 scaleAndBias(0.5f, 0.0f,  0.0f, 0.0f,
                              0.0f, -0.5f, 0.0f, 0.0f,
                              0.0f, 0.0f,  1.0f, 0.0f,
                              0.5f, 0.5f,  0.0f, 1.0f);

        matrix44 invLightTransform = lightTransform;
        invLightTransform.invert_simple();
        const matrix44& model = gfxServer->GetTransform(nGfxServer2::Model);
        const matrix44& projection = this->camera.GetProjection();

        matrix44 modelLight = model * invLightTransform;
        matrix44 modelLightProjection = modelLight * projection;
        matrix44 mvpScaledAndBiased = modelLightProjection * scaleAndBias;
        shader->SetMatrix(nShader2::ModelLightProjection, mvpScaledAndBiased);
    }

    // compute the lightPos in model space
    vector3 lightPos = lightTransform.pos_component();
    if (shader->IsParameterUsed(nShader2::ModelLightPos))
    {
        vector3 p = gfxServer->GetTransform(nGfxServer2::InvModel) * lightPos;
        nFloat4 f4;
        f4.x = p.x;
        f4.y = p.y;
        f4.z = p.z;
        f4.w = 1.0f;
        shader->SetFloat4(nShader2::ModelLightPos, f4);
    }

    // ...or if world space position is needed...
    if (shader->IsParameterUsed(nShader2::LightPos))
    {
        nFloat4 f4;
        f4.x = lightPos.x;
        f4.y = lightPos.y;
        f4.z = lightPos.z;
        f4.w = 1.0f;
        shader->SetFloat4(nShader2::LightPos, f4);
    }
    return true;
}
