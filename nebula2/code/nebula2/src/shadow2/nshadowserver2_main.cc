//------------------------------------------------------------------------------
//  nshadowserver2_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "shadow2/nshadowserver2.h"
#include "gfx2/ngfxserver2.h"
#include "resource/nresourceserver.h"
#include "gfx2/nshader2.h"
#include "shadow2/nshadowcaster2.h"

nNebulaScriptClass(nShadowServer2, "nroot");
nShadowServer2* nShadowServer2::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nShadowServer2::nShadowServer2() :
    isOpen(false),
    inBeginScene(false),
    inBeginLight(false),
    useZFail(true),
    shadowsEnabled(true),
    numShaderPasses(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nShadowServer2::~nShadowServer2()
{
    if (this->isOpen)
    {
        this->Close();
    }
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shadow caster.
*/
nShadowCaster2*
nShadowServer2::NewShadowCaster(nShadowCaster2::Type t, const char* rsrcName)
{
    nResourceServer* rsrcServer = nResourceServer::Instance();
    nShadowCaster2* newCaster = 0;
    switch (t)
    {
        case nShadowCaster2::Static:
            newCaster = (nShadowCaster2*)rsrcServer->NewResource("nstaticshadowcaster2", rsrcName, nResource::Mesh);
            break;

        case nShadowCaster2::Skinned:
            newCaster = (nShadowCaster2*)rsrcServer->NewResource("nskinnedshadowcaster2", rsrcName, nResource::Mesh);
            break;
    }
    n_assert(newCaster);
    return newCaster;
}

//------------------------------------------------------------------------------
/**
    Open the shadow server.
*/
bool
nShadowServer2::Open()
{
    n_assert(!this->IsOpen());

    // initialize shadow volume shader
    nShader2* shd = nGfxServer2::Instance()->NewShader("shaders:shadow.fx");
    shd->SetFilename("shaders:shadow.fx");
    if (!shd->IsLoaded())
    {
        bool shadowShaderLoaded = shd->Load();
        n_assert(shadowShaderLoaded);
    }
    this->refShader = shd;

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the shadow server.
*/
void
nShadowServer2::Close()
{
    n_assert(this->IsOpen());

    if (this->refShader.isvalid())
    {
        this->refShader->Unload();
        this->refShader->Release();
        this->refShader.invalidate();
    }
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Begin rendering shadow volumes.
*/
bool
nShadowServer2::BeginScene()
{
    n_assert(!this->inBeginScene);
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    if (this->shadowsEnabled && (gfxServer->GetNumStencilBits() > 0))
    {
        // catch evil hardware
        if (nGfxServer2::Intel_82865G == gfxServer->GetDeviceIdentifier())
        {
            return false;
        }

        // setup the shader
        nShader2* shd = this->refShader;
        if (this->useZFail)
        {
            // shader setup for zfail
            shd->SetInt(nShaderState::StencilFrontZFailOp, nShaderState::DECR);
            shd->SetInt(nShaderState::StencilFrontPassOp,  nShaderState::KEEP);
            shd->SetInt(nShaderState::StencilBackZFailOp,  nShaderState::INCR);
            shd->SetInt(nShaderState::StencilBackPassOp,   nShaderState::KEEP);
        }
        else
        {
            // shader setup for zpass
            shd->SetInt(nShaderState::StencilFrontZFailOp, nShaderState::KEEP);
            shd->SetInt(nShaderState::StencilFrontPassOp,  nShaderState::INCR);
            shd->SetInt(nShaderState::StencilBackZFailOp,  nShaderState::KEEP);
            shd->SetInt(nShaderState::StencilBackPassOp,   nShaderState::DECR);
        }

        this->inBeginScene = true;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    End rendering shadow volumes.
*/
void
nShadowServer2::EndScene()
{
    n_assert(this->inBeginScene);
    n_assert(!this->inBeginLight);
    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
/**
    Begin rendering with a new light.
*/
void
nShadowServer2::BeginLight(const nLight& l)
{
    n_assert(this->inBeginScene);
    n_assert(!this->inBeginLight);

    // setup shadow volume shader
    nShader2* shd = this->refShader;
    shd->SetInt(nShaderState::LightType, l.GetType());
    shd->SetFloat(nShaderState::LightRange, l.GetRange());
    nGfxServer2::Instance()->SetShader(shd);
    this->numShaderPasses = shd->Begin(true);
    if (1 == this->numShaderPasses)
    {
        // specific optimization for 1-pass shadow volume renderer
        shd->BeginPass(0);
    }

    this->curLight = l;
    this->inBeginLight = true;
}

//------------------------------------------------------------------------------
/**
    Finish rendering with the current light.
*/
void
nShadowServer2::EndLight()
{
    n_assert(this->inBeginScene);
    n_assert(this->inBeginLight);
    nShader2* shd = this->refShader;
    if (this->numShaderPasses == 1)
    {
        shd->EndPass();
    }
    shd->End();
    this->inBeginLight = false;
}

//------------------------------------------------------------------------------
/**
    Render a shadow caster with the current light.
*/
void
nShadowServer2::RenderShadowCaster(nShadowCaster2* caster, const matrix44& modelMatrix)
{
    n_assert(caster);
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nShader2* shd = this->refShader;

    // setup some shader state
    gfxServer->SetTransform(nGfxServer2::Model, modelMatrix);
    const matrix44& invModel = gfxServer->GetTransform(nGfxServer2::InvModel);
    matrix44 invModelLight = this->curLight.GetTransform() * invModel;
    if (this->curLight.GetType() == nLight::Directional)
    {
        shd->SetVector3(nShaderState::ModelLightPos, invModelLight.z_component());
    }
    else
    {
        shd->SetVector3(nShaderState::ModelLightPos, invModelLight.pos_component());
    }

    // create the shadow volume
    caster->SetupShadowVolume(this->curLight, invModelLight);

    // render the shadow volume
    if (1 == this->numShaderPasses)
    {
        caster->RenderShadowVolume();
    }
    else
    {
        int pass;
        for (pass = 0; pass < this->numShaderPasses; pass++)
        {
            shd->BeginPass(pass);
            caster->RenderShadowVolume();
            shd->EndPass();
        }
    }
}
