//------------------------------------------------------------------------------
//  nshadowserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "shadow/nshadowserver.h"
#include "gfx2/nshader2.h"

nNebulaScriptClass(nShadowServer, "nroot");
nShadowServer* nShadowServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nShadowServer::nShadowServer() :
    showShadow(true),
    inBeginScene(false),
    inBeginLight(false),
    shadowColor(0.0f, 0.0f, 0.1f, 0.6f),
    debugShowGeometry(false),
    useZFail(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nShadowServer::~nShadowServer()
{
    this->UnloadResources();
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Create a new shadow caster resource of the specified type.
    @param  casterType  shadowcaster type
    @param  rsrcName    resource name
*/
nShadowCaster*
nShadowServer::NewShadowCaster(CasterType casterType, const char* rsrcName)
{
    nShadowCaster* ret = 0;
    switch (casterType)
    {
        case Static:
            ret = (nShadowCaster*) nResourceServer::Instance()->NewResource("nstaticshadowcaster", rsrcName, nResource::Mesh);
            break;
        case Skin:
            ret = (nShadowCaster*) nResourceServer::Instance()->NewResource("nskinshadowcaster", rsrcName, nResource::Mesh);
            break;
    }        
    
    n_assert(ret);
    return ret;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowServer::AreResourcesValid()
{
    bool valid = true;
    
    int i;
    for (i = 0; valid && i < NumDrawTypes; i++)
    {
        valid &= this->refStencilShader[i].isvalid() && this->refStencilShader[i]->IsValid();
        valid &= this->dynMesh[i].IsValid();
    }

    valid &= this->refDebugShader.isvalid() && this->refDebugShader->IsValid();
    valid &= this->refPlaneMesh.isvalid() && this->refPlaneMesh->IsValid() && (this->refPlaneMesh->GetRefillBuffersMode() != nMesh2::NeededNow);
    
    return valid;
}

//------------------------------------------------------------------------------
/**
    TODO: make shader filenames accessible by script
*/
bool
nShadowServer::LoadResources()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    int type;
    for (type = 0; type < NumDrawTypes; type++)
    {
        //load stencil shaders
        switch (type)
        {
            case zPass:
            case zFail:
                if ( ! this->refStencilShader[type].isvalid() )
                {
                    nString resName("shaders:shadow_volume.fx");
                    resName.AppendInt(type);
                    nShader2* shader = gfxServer->NewShader(resName.Get());
                    shader->SetFilename("shaders:shadow_volume.fx");
                    this->refStencilShader[type] = shader;
                }
            break;
        }

        // init
        if (! this->refStencilShader[type]->IsValid())
        {
            nShader2* shader = this->refStencilShader[type];
            shader->Load();

            if (shader->IsParameterUsed(nShaderState::StencilFrontZFailOp)
                && shader->IsParameterUsed(nShaderState::StencilFrontPassOp)
                && shader->IsParameterUsed(nShaderState::StencilBackZFailOp)
                && shader->IsParameterUsed(nShaderState::StencilBackPassOp))
            {
                switch (type)
                {
                    case zPass:
                    {
                        // setup stencil operation
                        shader->SetInt(nShaderState::StencilFrontZFailOp, nShaderState::KEEP);
                        shader->SetInt(nShaderState::StencilFrontPassOp,  nShaderState::INCR);
                        shader->SetInt(nShaderState::StencilBackZFailOp,  nShaderState::KEEP);
                        shader->SetInt(nShaderState::StencilBackPassOp,   nShaderState::DECR);
                    }
                    break;
                    case zFail:
                    {
                        // setup stencil operation
                        shader->SetInt(nShaderState::StencilFrontZFailOp, nShaderState::DECR);
                        shader->SetInt(nShaderState::StencilFrontPassOp,  nShaderState::KEEP);
                        shader->SetInt(nShaderState::StencilBackZFailOp,  nShaderState::INCR);
                        shader->SetInt(nShaderState::StencilBackPassOp,   nShaderState::KEEP);
                    }
                    break;
                    default:
                        n_assert(false); // should not happen.
                }
            }
        }

        // setup dyn mesh
        if (!this->dynMesh[type].IsValid())
        {
            this->dynMesh[type].Initialize(nGfxServer2::TriangleList, nMesh2::Coord, 0, false, false);
            n_assert(this->refStencilShader[type].isvalid() && this->refStencilShader[type]->IsValid());
            this->dynMesh[type].SetShader(this->refStencilShader[type]);
        }
    }
    
    // load debug shader
    if ( ! this->refDebugShader.isvalid() )
    {
        nShader2* shader = gfxServer->NewShader("shaders:shadow_debug.fx");
        shader->SetFilename("shaders:shadow_debug.fx");
        this->refDebugShader = shader;
    }

    // init
    if (! this->refDebugShader->IsValid())
    {
        this->refDebugShader->Load();
    }

    // load stencil shadow plane shader
    if ( ! this->refPlaneShader.isvalid() )
    {
        nShader2* shader = gfxServer->NewShader("shaders:stencil_plane.fx");
        shader->SetFilename("shaders:stencil_plane.fx");
        this->refPlaneShader = shader;
    }

    // init
    if ( ! this->refPlaneShader->IsValid() )
    {
        this->refPlaneShader->Load();
    }

    // create plane mesh
    if ( ! this->refPlaneMesh.isvalid() )     
    {
        nMesh2* plane = gfxServer->NewMesh("shadow_plane");
        plane->SetUsage(nMesh2::WriteOnce);
        plane->SetVertexComponents(nMesh2::Coord);
        plane->SetNumVertices(4);
        plane->SetNumIndices(6);

        plane->SetRefillBuffersMode(nMesh2::Enabled);
        this->refPlaneMesh = plane;
    }

    // init plane mesh
    if (! this->refPlaneMesh->IsValid() || nMesh2::NeededNow == this->refPlaneMesh->GetRefillBuffersMode() )
    {
        bool success = this->refPlaneMesh->Load();
        n_assert(success);
        
        // verticies
        float* vPtr = this->refPlaneMesh->LockVertices();
        n_assert(vPtr);

        *vPtr++ = -1.0f; *vPtr++ = 1.0f; *vPtr++ = 0.0f; 
        *vPtr++ = -1.0f; *vPtr++ = -1.0f; *vPtr++ = 0.0f; 
        *vPtr++ = 1.0f;  *vPtr++ =  1.0f; *vPtr++ = 0.0f; 
        *vPtr++ = 1.0f;  *vPtr++ = -1.0f; *vPtr++ = 0.0f; 
        this->refPlaneMesh->UnlockVertices();

        // indicies
        ushort* iPtr = this->refPlaneMesh->LockIndices();
        n_assert(iPtr);
        *iPtr++ = 0; *iPtr++ = 1; *iPtr++ = 2;
        *iPtr++ = 1; *iPtr++ = 3; *iPtr++ = 2;
        this->refPlaneMesh->UnlockIndices();

        this->refPlaneMesh->SetRefillBuffersMode(nMesh2::Enabled);
    }

    return true;
}
        
        
//------------------------------------------------------------------------------
/**
*/
void
nShadowServer::UnloadResources()
{
    n_assert(!this->inBeginScene);

    if (this->refPlaneShader.isvalid())
    {
        this->refPlaneShader->Release();
        this->refPlaneShader.invalidate();
    }

    if (this->refPlaneMesh.isvalid())
    {
        this->refPlaneMesh->Release();
        this->refPlaneMesh.invalidate();
    }
    
    int type;
    for (type = 0; type < NumDrawTypes; type++)
    {
        if (this->refStencilShader[type].isvalid())
        {
            this->refStencilShader[type]->Release();
            this->refStencilShader[type].invalidate();
        }
    }

    if (this->refDebugShader.isvalid())
    {
        this->refDebugShader->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowServer::BeginScene()
{
    if (this->showShadow && (nGfxServer2::Instance()->GetNumStencilBits() > 0) )
    {
        if (nGfxServer2::Intel_82865G == nGfxServer2::Instance()->GetDeviceIdentifier())
        {
            n_printf("nShadowServer::BeginScene(): Intel 82865G Graphics detected! Because of bugs shadow will be disabled!");
            this->showShadow = false;
            return false;
        }
        
        n_assert(!this->inBeginScene);
        
        if (!this->AreResourcesValid())
        {
            this->UnloadResources();
            this->LoadResources();
        }

        // FIXME: test if the shader has not validated
        if (! this->refStencilShader[0]->IsParameterUsed(nShaderState::StencilFrontZFailOp))
        {
            return false;
        }
        
        this->inBeginScene = true;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowServer::BeginLight(nLight::Type lType, const vector3& lightPos)
{
    n_assert(this->inBeginScene);
    n_assert(!this->inBeginLight);

    this->lightPosition = lightPos;
    this->lightType = lType;

    // init dynamic meshes
    int type;
    for (type = 0; type < NumDrawTypes; type++)
    {
        n_assert(this->dynMesh[type].IsValid());
        
        // access buffers
        float* vtxPtr;
        this->dynMesh[type].Begin(vtxPtr, this->maxNumCoord[type]);
        n_assert(vtxPtr);
        this->coordPtr[type] = (vector3*) vtxPtr;
        n_assert(this->maxNumCoord[type] > 0);
        this->numCoord[type] = 0;
    }

    this->inBeginLight = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowServer::BeginDrawBuffer(DrawType type, vector3*& dstCoords, int& numCoords)
{
    n_assert(this->inBeginScene);
    n_assert(this->inBeginLight);
    n_assert(type >= 0 && type < NumDrawTypes);
    n_assert(this->dynMesh[type].IsValid());

    dstCoords = this->coordPtr[type];
    numCoords = this->numCoord[type];
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowServer::SwapDrawBuffer(DrawType type, vector3*& dstCoords, int& numCoords)
{
    n_assert(this->inBeginScene);
    n_assert(this->inBeginLight);
    n_assert(type >= 0 && type < NumDrawTypes);
    n_assert(this->dynMesh[type].IsValid());

    // draw data
    float* vtxPtr = 0;
    this->dynMesh[type].Swap(numCoords, vtxPtr);
    n_assert(vtxPtr);
    this->coordPtr[type] = (vector3*) vtxPtr;
    this->numCoord[type] = 0;
    
    dstCoords = this->coordPtr[type];
    numCoords = this->numCoord[type];
    
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowServer::EndDrawBuffer(DrawType type, vector3* dstCoords, int numCoords)
{
    n_assert(this->inBeginScene);
    n_assert(this->inBeginLight);
    n_assert(type >= 0 && type < NumDrawTypes);
    n_assert(this->dynMesh[type].IsValid());
    
    this->coordPtr[type] = dstCoords;
    this->numCoord[type] = numCoords;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowServer::DrawDebugShadows(const vector4& matDiffuse)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nShader2* shader = this->refDebugShader;
    shader->SetVector4(nShaderState::MatDiffuse, matDiffuse);
    gfxServer->SetShader(shader);
    
    int numPass = shader->Begin(true);
    int i; 
    for (i = 0; i < numPass; i++)
    {
        shader->BeginPass(i);
        gfxServer->DrawNS(nGfxServer2::TriangleList);
        shader->EndPass();
    }
    shader->End();
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowServer::DrawDebugEdges(const vector4& matDiffuse)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nShader2* shader = this->refDebugShader;
    shader->SetVector4(nShaderState::MatDiffuse, matDiffuse);
    gfxServer->SetShader(shader);
    
    int numPass = shader->Begin(true);
    int i; 
    for (i = 0; i < numPass; i++)
    {
        shader->BeginPass(i);
        gfxServer->DrawNS(nGfxServer2::LineList);
        shader->EndPass();
    }
    shader->End();
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowServer::EndLight()
{
    n_assert(this->inBeginScene);
    n_assert(this->inBeginLight);

    int type;
    for (type = 0; type < NumDrawTypes; type++)
    {
        //DEBUG
        this->dynMesh[type].End(this->numCoord[type]);
        this->numCoord[type] = 0;
    }

    this->inBeginLight = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowServer::EndScene()
{
    n_assert(this->inBeginScene);
    n_assert(!this->inBeginLight);
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    
    // rendner shadow plane    
    gfxServer->PushTransform(nGfxServer2::Model, matrix44());
    gfxServer->PushTransform(nGfxServer2::View, matrix44());
    gfxServer->PushTransform(nGfxServer2::Projection, matrix44());
    
    this->refPlaneShader->SetVector4(nShaderState::MatDiffuse, shadowColor);
    gfxServer->SetShader(this->refPlaneShader);

    gfxServer->SetMesh(this->refPlaneMesh);
    gfxServer->SetIndexRange(0, 6);
    gfxServer->SetVertexRange(0, 4);
    
    gfxServer->DrawIndexed(nGfxServer2::TriangleList);

    gfxServer->PopTransform(nGfxServer2::Projection);
    gfxServer->PopTransform(nGfxServer2::View);
    gfxServer->PopTransform(nGfxServer2::Model);

    this->inBeginScene = false;
}
