//------------------------------------------------------------------------------
//  nmrtsceneserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmrtsceneserver.h"
#include "scene/nscenenode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/ntexture2.h"
#include "scene/nmaterialnode.h"
#include "gfx2/nshader2.h"
#include "gfx2/ndisplaymode2.h"
#include "gfx2/nmesh2.h"
#include "scene/nshadownode.h"
#include "scene/nlightnode.h"

nNebulaScriptClass(nMRTSceneServer, "nstdsceneserver");

//------------------------------------------------------------------------------
/**
*/
nMRTSceneServer::nMRTSceneServer() :
    dbgNumInstanceGroups("sceneInstanceGroups", nArg::Int),
    dbgNumInstances("sceneInstances", nArg::Int),
    refShadowServer("/sys/servers/shadow"),
    compositingEnabled(true),
    saturation(1.0f),
    balance(1.0f, 1.0f, 1.0f, 1.0f),
    luminance(0.299f, 0.587f, 0.114f, 0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMRTSceneServer::~nMRTSceneServer()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
nMRTSceneServer::AreResourcesValid()
{
    // only require extra resources if feature set is DX9 or better
    // and compositing is enabled
    if (this->compositingEnabled)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        bool valid = true;
        if (gfxServer->GetFeatureSet() >= nGfxServer2::DX9)
        {
            if (!(this->renderBuffer.isvalid() && this->renderBuffer->IsValid()))
            {
                valid = false;
            }
            if (!(this->compShader.isvalid() && this->compShader->IsValid()))
            {
                valid = false;
            }
            if ((!(this->quadMesh.isvalid() && this->quadMesh->IsValid()) ||
                (this->quadMesh->GetRefillBuffersMode() == nMesh2::NeededNow)))
            {
                valid = false;
            }
        }
        return valid;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nMRTSceneServer::UnloadResources()
{
    if (this->renderBuffer.isvalid())
    {
        this->renderBuffer->Release();
        this->renderBuffer.invalidate();
    }
    if (this->compShader.isvalid())
    {
        this->compShader->Release();
        this->compShader.invalidate();
    }
    if (this->quadMesh.isvalid())
    {
        this->quadMesh->Release();
        this->quadMesh.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nMRTSceneServer::LoadResources()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    n_assert(gfxServer->GetFeatureSet() >= nGfxServer2::DX9);
    const nDisplayMode2& mode = gfxServer->GetDisplayMode();

    // create an offscreen render buffer
    if (!this->renderBuffer.isvalid())
    {
        this->renderBuffer = gfxServer->NewRenderTarget("mrtRenderBuffer",
                                                        mode.GetWidth(),
                                                        mode.GetHeight(),
                                                        nTexture2::X8R8G8B8,
                                                        nTexture2::RenderTargetColor);
        n_assert(this->renderBuffer.isvalid());
    }
    if (!this->renderBuffer->IsValid())
    {
        this->renderBuffer->Load();
        n_printf("nMRTSceneServer: renderBuffer->Load() called!\n");
    }

    // load compositing shader
    nShader2* shd;
    if (!this->compShader.isvalid())
    {
        const char* shdFilename = "shaders:mrtcompose.fx";
        shd = gfxServer->NewShader(shdFilename);
        shd->SetFilename(shdFilename);
        this->compShader = shd;
    }
    else
    {
        shd = this->compShader;
    }
    if (!shd->IsValid())
    {
        bool shaderLoaded = shd->Load();
        n_assert(shaderLoaded);
    }

    // create a fullscreen quad mesh
    nMesh2* mesh = 0;
    if (!this->quadMesh.isvalid())
    {
        mesh = gfxServer->NewMesh(0);
        mesh->SetUsage(nMesh2::WriteOnce);
        mesh->SetRefillBuffersMode(nMesh2::Enabled);
        mesh->SetNumVertices(4);
        mesh->SetNumIndices(6);
        mesh->SetVertexComponents(nMesh2::Coord | nMesh2::Uv0);
        this->quadMesh = mesh;
    }
    else
    {
        mesh = this->quadMesh.get();
    }

    // fill mesh
    if (!mesh->IsValid() || (nMesh2::NeededNow == mesh->GetRefillBuffersMode()))
    {
        bool success = mesh->Load();
        n_assert(success);

        // fill rectangle vertices
        float* vPtr = mesh->LockVertices();
        n_assert(vPtr);
        *vPtr++ = -1.0f; *vPtr++ =  1.0f; *vPtr++ = 0.0f; *vPtr++ = 0.0f; *vPtr++ = 0.0f;
        *vPtr++ = -1.0f; *vPtr++ = -1.0f; *vPtr++ = 0.0f; *vPtr++ = 0.0f; *vPtr++ = 1.0f;
        *vPtr++ = 1.0f;  *vPtr++ =  1.0f; *vPtr++ = 0.0f; *vPtr++ = 1.0f; *vPtr++ = 0.0f;
        *vPtr++ = 1.0f;  *vPtr++ = -1.0f; *vPtr++ = 0.0f; *vPtr++ = 1.0f; *vPtr++ = 1.0f;
        mesh->UnlockVertices();

        // fill rectangle indices
        ushort* iPtr = mesh->LockIndices();
        *iPtr++ = 0; *iPtr++ = 1; *iPtr++ = 2;
        *iPtr++ = 1; *iPtr++ = 3; *iPtr++ = 2;
        mesh->UnlockIndices();

        mesh->SetRefillBuffersMode(nMesh2::Enabled);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Prepare rendering & check if it can be done
*/
bool
nMRTSceneServer::BeginScene(const matrix44& invView)
{
    if (nSceneServer::BeginScene(invView))
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();

        // make sure all required resources are ok
        if (!this->AreResourcesValid())
        {
            bool success = this->LoadResources();
            n_assert(success);
        }

        // select render target
        if (this->compositingEnabled && (gfxServer->GetFeatureSet() >= nGfxServer2::DX9))
        {
            gfxServer->SetRenderTarget(this->renderBuffer);
        }
        else
        {
            gfxServer->SetRenderTarget(0);
        }
        this->inBeginScene = nGfxServer2::Instance()->BeginScene();

        this->dbgNumInstanceGroups->SetI(0);
        this->dbgNumInstances->SetI(0);
    }
    return this->inBeginScene;
}

//------------------------------------------------------------------------------
/**
    Render the scene. Rendering happens in multiple passes:

    (1) clear the screen
    (2) render the unlit scene into the offscreen color buffer
    (3) render lighting using the color buffer as texture
    (4) render shadow
*/
void
nMRTSceneServer::RenderScene()
{
    nStdSceneServer::RenderScene();

    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // if compositing enabled, compose-blit render buffer to back buffer
    if (this->compositingEnabled && (gfxServer->GetFeatureSet() >= nGfxServer2::DX9))
    {
        gfxServer->EndScene();
        this->DoCompositing();
    }
}

//------------------------------------------------------------------------------
/**
    Perform the frame compositing. This blits the offscreen render buffer
    to the back buffer using a compositing pixel shader.
*/
void
nMRTSceneServer::DoCompositing()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->SetRenderTarget(0);
    if (gfxServer->BeginScene())
    {
        nShader2* shader = this->compShader;
        if (shader->IsParameterUsed(nShaderState::DiffMap0))
        {
            shader->SetTexture(nShaderState::DiffMap0, this->renderBuffer);
        }
        if (shader->IsParameterUsed(nShaderState::MatDiffuse))
        {
            shader->SetVector4(nShaderState::MatDiffuse, this->balance);
        }
        if (shader->IsParameterUsed(nShaderState::Intensity0))
        {
            shader->SetFloat(nShaderState::Intensity0, this->saturation);
        }
        if (shader->IsParameterUsed(nShaderState::MatAmbient))
        {
            shader->SetVector4(nShaderState::MatAmbient, this->luminance);
        }
        gfxServer->SetShader(shader);
        gfxServer->SetMesh(this->quadMesh);
        gfxServer->SetVertexRange(0, 4);
        gfxServer->SetIndexRange(0, 6);
        gfxServer->DrawIndexed(nGfxServer2::TriangleList);
    }
}
