//------------------------------------------------------------------------------
//  nd3d9shader_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9shader.h"
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9texture.h"
#include "gfx2/nd3d9shaderinclude.h"
#include "gfx2/nshaderparams.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

nNebulaClass(nD3D9Shader, "nshader2");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nd3d9shader

    @cppclass
    nD3D9Shader
    
    @superclass
    nshader2
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nD3D9Shader::nD3D9Shader() :
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    inBeginPass(false),
#endif
    refGfxServer("/sys/servers/gfx"),
    effect(0),
    hasBeenValidated(false),
    didNotValidate(false)
{
    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));
}

//------------------------------------------------------------------------------
/**
*/
nD3D9Shader::~nD3D9Shader()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::UnloadResource()
{
    n_assert(this->IsValid());
    n_assert(this->effect);

    nD3D9Server* gfxServer = this->refGfxServer.get();
    n_assert(gfxServer->d3d9Device);

    // if this is the currently set shader, unlink from gfx server
    if (gfxServer->GetShader() == this)
    {
        gfxServer->SetShader(0);
    }

    // release d3dx resources
    this->effect->Release();
    this->effect = 0;

    // reset current shader params
    this->curParams.Clear();

    this->SetValid(false);
}

//------------------------------------------------------------------------------
/**
    Load D3DX effects file.
*/
bool
nD3D9Shader::LoadResource()
{
    n_assert(!this->IsValid());
    n_assert(0 == this->effect);

    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);

    // mangle path name
    nString filename = this->GetFilename();
    nString mangledPath = nFileServer2::Instance()->ManglePath(filename.Get());

    // initialize shader index
    this->shaderIndex = this->refGfxServer->GetShaderIndex(filename.Get());
    
    //load fx file...
    nFile* file = this->refFileServer->NewFileObject();

    // open the file
    if (!file->Open(mangledPath.Get(), "r"))
    {
        n_error("nD3D9Shader: could not load shader file '%s'!", mangledPath.Get());
        return false;
    }

    // get size of file
    int fileSize = file->GetSize();

    // allocate data for file and read it
    void* buffer = n_malloc(fileSize);
    n_assert(buffer);
    file->Read(buffer, fileSize);
    file->Close();
    file->Release();

    ID3DXBuffer* errorBuffer = 0;
    #if N_D3D9_DEBUG
        DWORD compileFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
    #else
        DWORD compileFlags = 0;
    #endif

    // create include file handler
    nPathString shaderPath(mangledPath.Get());
    nD3D9ShaderInclude includeHandler(shaderPath.ExtractDirName());

    // get global effect pool from gfx server
    ID3DXEffectPool* effectPool = this->refGfxServer->GetEffectPool();
    n_assert(effectPool);

    // create effect
    hr = D3DXCreateEffect(
            d3d9Dev,            // pDevice
            buffer,             // pFileData
            fileSize,           // DataSize
            NULL,               // pDefines
            &includeHandler,    // pInclude
            compileFlags,       // Flags
            effectPool,         // pPool
            &(this->effect),    // ppEffect
            &errorBuffer);      // ppCompilationErrors
    n_free(buffer);

    if (FAILED(hr))
    {
        n_error("nD3D9Shader: failed to load fx file '%s' with:\n\n%s\n",
                mangledPath.Get(),
                errorBuffer ? errorBuffer->GetBufferPointer() : "No D3DX error message.");
        if (errorBuffer)
        {
            errorBuffer->Release();
        }
        return false;
    }
    n_assert(this->effect);

    // success
    this->hasBeenValidated = false;
    this->didNotValidate = false;
    this->SetValid(true);

    // validate the effect
    this->ValidateEffect();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetBool(nShaderState::Param p, bool val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetBool(this->parameterHandles[p], val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetInt() on shader failed!");

#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetBoolArray(nShaderState::Param p, const bool* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetBoolArray(this->parameterHandles[p], (const BOOL*)array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetIntArray() on shader failed!");    
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetInt(nShaderState::Param p, int val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetInt(this->parameterHandles[p], val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetInt() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetIntArray(nShaderState::Param p, const int* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetIntArray(this->parameterHandles[p], array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetIntArray() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat(nShaderState::Param p, float val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetFloat(this->parameterHandles[p], val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetFloat() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloatArray(nShaderState::Param p, const float* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetFloatArray(this->parameterHandles[p], array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetFloatArray() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector4(nShaderState::Param p, const vector4& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&val));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetVector() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector3(nShaderState::Param p, const vector3& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    static vector4 v;
    v.set(val.x, val.y, val.z, 1.0f);
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&v));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &v);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetVector() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat4(nShaderState::Param p, const nFloat4& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetVector() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat4Array(nShaderState::Param p, const nFloat4* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetVectorArray(this->parameterHandles[p], (CONST D3DXVECTOR4*) array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetVectorArray() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector4Array(nShaderState::Param p, const vector4* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetVectorArray(this->parameterHandles[p], (CONST D3DXVECTOR4*) array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetVectorArray() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrix(nShaderState::Param p, const matrix44& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(&val));
    HRESULT hr = this->effect->SetMatrix(this->parameterHandles[p], (CONST D3DXMATRIX*) &val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetMatrix() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrixArray(nShaderState::Param p, const matrix44* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetMatrixArray(this->parameterHandles[p], (CONST D3DXMATRIX*) array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetMatrixArray() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrixPointerArray(nShaderState::Param p, const matrix44** array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetMatrixPointerArray(this->parameterHandles[p], (CONST D3DXMATRIX**) array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_dxtrace(hr, "SetMatrixPointerArray() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetTexture(nShaderState::Param p, nTexture2* tex)
{
    n_assert(tex);
    n_assert(this->effect && (p < nShaderState::NumParameters));
    if ((!this->curParams.IsParameterValid(p)) ||
        (this->curParams.IsParameterValid(p) && (this->curParams.GetArg(p).GetTexture() != tex)))
    {
        this->curParams.SetArg(p, nShaderArg(tex));
        HRESULT hr = this->effect->SetTexture(this->parameterHandles[p], ((nD3D9Texture*)tex)->GetBaseTexture());
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumTextureChanges++;
        #endif
        n_dxtrace(hr, "SetTexture() on shader failed!");
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
        if (this->inBeginPass)
        {
            hr = this->effect->CommitChanges();
            n_dxtrace(hr, "CommitChanges() on shader failed!");
        }
#endif
    }
}

//------------------------------------------------------------------------------
/**
    Set a whole shader parameter block at once. This is slightly faster
    (and more convenient) then setting single parameters.
*/
void
nD3D9Shader::SetParams(const nShaderParams& params)
{
    #ifdef __NEBULA_STATS__
    nD3D9Server* gfxServer = this->refGfxServer.get();
    #endif
    int i;
    HRESULT hr;

    int numValidParams = params.GetNumValidParams();
    for (i = 0; i < numValidParams; i++)
    {
        nShaderState::Param curParam = params.GetParamByIndex(i);

        // parameter used in shader?
        D3DXHANDLE handle = this->parameterHandles[curParam];
        if (handle != 0)
        {
            // avoid redundant state switches
            const nShaderArg& curArg = params.GetArgByIndex(i);
            if ((!this->curParams.IsParameterValid(curParam)) ||
                (!(curArg == this->curParams.GetArg(curParam))))
            {
                this->curParams.SetArg(curParam, curArg);
                switch (curArg.GetType())
                {
                    case nShaderState::Void:
                        hr = S_OK;
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Int:
                        hr = this->effect->SetInt(handle, curArg.GetInt());
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Float:
                        hr = this->effect->SetFloat(handle, curArg.GetFloat());
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Float4:
                        hr = this->effect->SetVector(handle, (CONST D3DXVECTOR4*) &(curArg.GetFloat4()));
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Matrix44:
                        hr = this->effect->SetMatrix(handle, (CONST D3DXMATRIX*) curArg.GetMatrix44());
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Texture:
                        hr = this->effect->SetTexture(handle, ((nD3D9Texture*)curArg.GetTexture())->GetBaseTexture());
                        #ifdef __NEBULA_STATS__
                        gfxServer->statsNumTextureChanges++;
                        #endif
                        break;
                }
                n_dxtrace(hr, "Failed to set shader parameter in nD3D9Shader::SetParams");
            }
        }
    }
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    if (this->inBeginPass)
    {
        hr = this->effect->CommitChanges();
        n_dxtrace(hr, "CommitChanges() on shader failed!");
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Update the parameter handles table which maps nShader2 parameters to
    D3DXEffect parameter handles.

    - 19-Feb-04 floh    Now also recognized parameters which are not used
                        by the shader's current technique.
*/
void
nD3D9Shader::UpdateParameterHandles()
{
    n_assert(this->effect);
    HRESULT hr;

    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));

    // for each parameter in the effect...
    D3DXEFFECT_DESC effectDesc = { 0 };
    hr = this->effect->GetDesc(&effectDesc);
    n_dxtrace(hr, "GetDesc() failed in UpdateParameterHandles()");
    uint curParamIndex;
    for (curParamIndex = 0; curParamIndex < effectDesc.Parameters; curParamIndex++)
    {
        D3DXHANDLE curParamHandle = this->effect->GetParameter(NULL, curParamIndex);
        n_assert(NULL != curParamHandle);

        // get the associated Nebula2 parameter index
        D3DXPARAMETER_DESC paramDesc = { 0 };
        hr = this->effect->GetParameterDesc(curParamHandle, &paramDesc);
        n_dxtrace(hr, "GetParameterDesc() failed in UpdateParameterHandles()");
        nShaderState::Param nebParam = nShaderState::StringToParam(paramDesc.Name);
        if (nebParam != nShaderState::InvalidParameter)
        {
            this->parameterHandles[nebParam] = curParamHandle;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Return true if parameter is used by effect.
*/
bool
nD3D9Shader::IsParameterUsed(nShaderState::Param p)
{
    n_assert(p < nShaderState::NumParameters);
    return (0 != this->parameterHandles[p]);
}

//------------------------------------------------------------------------------
/**
    Find the first valid technique and set it as current.
    This sets the hasBeenValidated and didNotValidate members
*/
void
nD3D9Shader::ValidateEffect()
{
    n_assert(!this->hasBeenValidated);
    n_assert(this->effect);
    n_assert(this->refGfxServer->d3d9Device);

    // set on first technique that validates correctly
    D3DXHANDLE technique = this->effect->GetTechnique(0);
    n_assert(NULL != technique);

    HRESULT hr = this->effect->ValidateTechnique(technique);
    if (SUCCEEDED(hr))
    {
        // technique could be validated
        this->effect->SetTechnique(technique);
        this->hasBeenValidated = true;
        this->didNotValidate = false;
        this->UpdateParameterHandles();
    }
    else
    {
        // remember old state
        BOOL oldSoftwareVertexProcessing = this->refGfxServer->d3d9Device->GetSoftwareVertexProcessing( );

        // if not DX9, give it another chance with software vertex processing
    if ((this->refGfxServer->GetFeatureSet() < nGfxServer2::DX9) && (!oldSoftwareVertexProcessing))
        {
            this->refGfxServer->d3d9Device->SetSoftwareVertexProcessing( TRUE );
            hr = this->effect->ValidateTechnique(technique);
            this->refGfxServer->d3d9Device->SetSoftwareVertexProcessing(oldSoftwareVertexProcessing);

            this->hasBeenValidated = true;
            if (SUCCEEDED(hr))
            {
                n_printf("nD3D9Shader() info: shader '%s' needs software vertex processing\n",  this->GetFilename());
                // technique could be validated
                this->effect->SetTechnique(technique);
                this->didNotValidate = false;
                this->UpdateParameterHandles();
            }
            else
            {
                this->didNotValidate = true;
                n_printf("nD3D9Shader() warning: shader '%s' did not validate!\n", this->GetFilename());
            }
        }
        else
        {
            // no valid technique found
            this->hasBeenValidated = true;
            this->didNotValidate = true;
            n_printf("nD3D9Shader() warning: shader '%s' did not validate!\n", this->GetFilename());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nD3D9Shader::Begin(bool saveState)
{
    n_assert(this->effect);

    // check if we already have been validated, if not, find the first
    // valid technique and set it as current
    if (!this->hasBeenValidated)
    {
        this->ValidateEffect();
    }

    if (this->didNotValidate)
    {
        return 0;
    }
    else
    {
        // start rendering the effect
        UINT numPasses;
        DWORD flags;
        if (saveState) flags = 0;
        else           flags = D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE;

        HRESULT hr = this->effect->Begin(&numPasses, flags);
        n_dxtrace(hr, "Begin() failed on effect");
        return numPasses;
    }
}

//------------------------------------------------------------------------------
/**
*/
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
void
nD3D9Shader::BeginPass(int pass)
{
    HRESULT hr;
    n_assert(this->effect);
    n_assert(!this->inBeginPass);
    hr = this->effect->BeginPass(pass);
    n_dxtrace(hr, "BeginPass() failed on effect");
    this->inBeginPass = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::EndPass()
{
    HRESULT hr;
    n_assert(this->effect);
    n_assert(this->inBeginPass);
    hr = this->effect->EndPass();
    n_dxtrace(hr, "EndPass() failed on effect");
    this->inBeginPass = false;
}
#else
//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::Pass(int pass)
{
    HRESULT hr;
    n_assert(this->effect);
    #if (D3D_SDK_VERSION >= 32)
    hr = this->effect->BeginPass(pass);
    #else
    hr = this->effect->Pass(pass);
    #endif
    n_dxtrace(hr, "Pass() failed on effect");
}
#endif

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::End()
{
    HRESULT hr;
    n_assert(this->effect);
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    n_assert(!this->inBeginPass);
#endif
    if (!this->didNotValidate)
    {
        hr = this->effect->End();
        n_dxtrace(hr, "End() failed on effect");
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Shader::SetTechnique(const char* t)
{
    n_assert(t);
    n_assert(this->effect);
    HRESULT hr = this->effect->SetTechnique(t);
    return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
/**
*/
const char*
nD3D9Shader::GetTechnique() const
{
    n_assert(this->effect);
    return this->effect->GetCurrentTechnique();
}
    
//------------------------------------------------------------------------------
/**
    This converts a D3DX parameter handle to a nShaderState::Param.
*/
nShaderState::Param
nD3D9Shader::D3DXParamToShaderStateParam(D3DXHANDLE h)
{
    int i;
    for (i = 0; i < nShaderState::NumParameters; i++)
    {
        if (this->parameterHandles[i] == h)
        {
            return (nShaderState::Param) i;
        }
    }
    // fallthrough: invalid handle
    return nShaderState::InvalidParameter;;
}

//------------------------------------------------------------------------------
/**
    Create or update the instance stream declaration for this shader.
    Stream components will be appended, unless they already exist in the
    declaration. Returns the number of components appended.
*/
int
nD3D9Shader::UpdateInstanceStreamDecl(nInstanceStream::Declaration& decl)
{
    n_assert(this->effect);

    int numAppended = 0;

    HRESULT hr;
    D3DXEFFECT_DESC fxDesc;
    hr = this->effect->GetDesc(&fxDesc);
    n_dxtrace(hr, "GetDesc() failed on effect");
    
    // for each parameter...
    uint paramIndex;
    for (paramIndex = 0; paramIndex < fxDesc.Parameters; paramIndex++)
    {
        D3DXHANDLE paramHandle = this->effect->GetParameter(NULL, paramIndex);
        n_assert(0 != paramHandle);

        D3DXHANDLE annHandle = this->effect->GetAnnotationByName(paramHandle, "Instance");
        if (annHandle)
        {
            BOOL b;
            hr = this->effect->GetBool(annHandle, &b);
            n_dxtrace(hr, 0);
            if (b)
            {
                // add parameter to stream declaration (if not already exists)
                nShaderState::Param param = this->D3DXParamToShaderStateParam(paramHandle);
                n_assert(nShaderState::InvalidParameter != param);

                // get parameter type
                D3DXPARAMETER_DESC paramDesc;
                hr = this->effect->GetParameterDesc(paramHandle, &paramDesc);
                n_dxtrace(hr, 0);
                nShaderState::Type type = nShaderState::Void;
                if (paramDesc.Type == D3DXPT_FLOAT)
                {
                    switch (paramDesc.Class)
                    {
                        case D3DXPC_SCALAR:         
                            type = nShaderState::Float; 
                            break;

                        case D3DXPC_VECTOR:         
                            type = nShaderState::Float4; 
                            break;

                        case D3DXPC_MATRIX_ROWS:
                        case D3DXPC_MATRIX_COLUMNS:
                            type = nShaderState::Matrix44;
                            break;
                    }
                }
                if (nShaderState::Void == type)
                {
                    n_error("nShader2: Invalid data type for instance parameter '%s' in shader '%s'!",
                        paramDesc.Name, this->GetFilename());
                    return 0;
                }
                
                // append instance stream component (if not exists yet)
                int i;
                bool paramExists = false;
                for (i = 0; i < decl.Size(); i++)
                {
                    if (decl[i].GetParam() == param)
                    {
                        paramExists = true;
                        break;
                    }
                }
                if (!paramExists)
                {
                    nInstanceStream::Component streamComponent(type, param);
                    decl.Append(streamComponent);
                    numAppended++;
                }
            }
        }
    }
    return numAppended;
}

