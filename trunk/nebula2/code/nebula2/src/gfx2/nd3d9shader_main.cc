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

//------------------------------------------------------------------------------
/**
*/
nD3D9Shader::nD3D9Shader() :
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

    // clear current parameter settings
    this->curParams.Reset();

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
    char mangledPath[N_MAXPATH];
    this->refFileServer->ManglePath(filename.Get(), mangledPath, sizeof(mangledPath));

    // check if the shader file actually exist, a non-existing shader file is
    // not a fatal error (result is that no rendering will be done)
    if (!this->refFileServer->FileExists(mangledPath))
    {
        n_printf("WARNING: shader file '%s' does not exist!\n", mangledPath);
        return false;
    }

    //load fx file...
    nFile* file = this->refFileServer->NewFileObject();

    // open the file
    if (!file->Open(mangledPath, "r"))
    {
        n_error("WARNING: could not load shader file '%s'!", mangledPath);
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
    nPathString shaderPath(mangledPath);
    nD3D9ShaderInclude includeHandler(shaderPath.ExtractDirName());

    // create effect
    hr = D3DXCreateEffect(
            d3d9Dev,            // pDevice
            buffer,             // pFileData
            fileSize,           // DataSize
            NULL,               // pDefines
            &includeHandler,    // pInclude
            compileFlags,       // Flags
            NULL,               // pPool
            &(this->effect),    // ppEffect
            &errorBuffer);      // ppCompilationErrors


    n_free(buffer);

    if (FAILED(hr))
    {
        n_error("nD3D9Shader: failed to load fx file '%s' with:\n\n%s\n",
                mangledPath,
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
nD3D9Shader::SetBool(Parameter p, bool val)
{
    n_assert(this->effect && (p < NumParameters));
    if (this->curParams.GetArg(p).GetBool() != val)
    {
        this->curParams.SetBool(p, val);
        HRESULT hr = this->effect->SetBool(this->parameterHandles[p], val);
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
        n_assert(SUCCEEDED(hr));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetBoolArray(Parameter p, const bool* array, int count)
{
    n_assert(this->effect && (p < NumParameters));
    HRESULT hr = this->effect->SetBoolArray(this->parameterHandles[p], (const BOOL*)array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetInt(Parameter p, int val)
{
    n_assert(this->effect && (p < NumParameters));
    if (this->curParams.GetArg(p).GetInt() != val)
    {
        this->curParams.SetInt(p, val);
        HRESULT hr = this->effect->SetInt(this->parameterHandles[p], val);
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
        n_assert(SUCCEEDED(hr));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetIntArray(Parameter p, const int* array, int count)
{
    n_assert(this->effect && (p < NumParameters));
    HRESULT hr = this->effect->SetIntArray(this->parameterHandles[p], array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat(Parameter p, float val)
{
    n_assert(this->effect && (p < NumParameters));
    if (this->curParams.GetArg(p).GetFloat() != val)
    {
        this->curParams.SetFloat(p, val);
        HRESULT hr = this->effect->SetFloat(this->parameterHandles[p], val);
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumRenderStateChanges++;
        #endif
        n_assert(SUCCEEDED(hr));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloatArray(Parameter p, const float* array, int count)
{
    n_assert(this->effect && (p < NumParameters));
    HRESULT hr = this->effect->SetFloatArray(this->parameterHandles[p], array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector4(Parameter p, const vector4& val)
{
    n_assert(this->effect && (p < NumParameters));
    this->curParams.SetFloat4(p, *((nFloat4*)&val));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector3(Parameter p, const vector3& val)
{
    n_assert(this->effect && (p < NumParameters));
    static vector4 v;
    v.set(val.x, val.y, val.z, 1.0f);
    this->curParams.SetFloat4(p, *((nFloat4*)&v));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &v);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat4(Parameter p, const nFloat4& val)
{
    n_assert(this->effect && (p < NumParameters));
    this->curParams.SetFloat4(p, val);
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (CONST D3DXVECTOR4*) &val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat4Array(Parameter p, const nFloat4* array, int count)
{
    n_assert(this->effect && (p < NumParameters));
    HRESULT hr = this->effect->SetVectorArray(this->parameterHandles[p], (CONST D3DXVECTOR4*) array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector4Array(Parameter p, const vector4* array, int count)
{
    n_assert(this->effect && (p < NumParameters));
    HRESULT hr = this->effect->SetVectorArray(this->parameterHandles[p], (CONST D3DXVECTOR4*) array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrix(Parameter p, const matrix44& val)
{
    n_assert(this->effect && (p < NumParameters));
    this->curParams.SetMatrix44(p, &val);
    HRESULT hr = this->effect->SetMatrix(this->parameterHandles[p], (CONST D3DXMATRIX*) &val);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrixArray(Parameter p, const matrix44* array, int count)
{
    n_assert(this->effect && (p < NumParameters));
    HRESULT hr = this->effect->SetMatrixArray(this->parameterHandles[p], (CONST D3DXMATRIX*) array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrixPointerArray(Parameter p, const matrix44** array, int count)
{
    n_assert(this->effect && (p < NumParameters));
    HRESULT hr = this->effect->SetMatrixPointerArray(this->parameterHandles[p], (CONST D3DXMATRIX**) array, count);
    #ifdef __NEBULA_STATS__
    this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetTexture(Parameter p, nTexture2* tex)
{
    n_assert(tex);
    n_assert(this->effect && (p < NumParameters));
    if (this->curParams.GetArg(p).GetTexture() != tex)
    {
        this->curParams.SetTexture(p, tex);
        HRESULT hr = this->effect->SetTexture(this->parameterHandles[p], ((nD3D9Texture*)tex)->GetBaseTexture());
        #ifdef __NEBULA_STATS__
        this->refGfxServer->statsNumTextureChanges++;
        #endif
        n_assert(SUCCEEDED(hr));
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
    for (i = 0; i < NumParameters; i++)
    {
        Parameter p = (Parameter) i;

        // source parameter valid?
        if (params.IsParameterValid(p))
        {
            // parameter used in shader?
            D3DXHANDLE handle = this->parameterHandles[p];
            if (handle != 0)
            {
                // avoid redundant state switches
                const nShaderArg& curArg = params.GetArg(p);
                if (!(curArg == this->curParams.GetArg(p)))
                {
                    this->curParams.SetArg(p, curArg);
                    switch (curArg.GetType())
                    {
                        case nShaderArg::Void:
                            hr = S_OK;
                            #ifdef __NEBULA_STATS__
                            gfxServer->statsNumRenderStateChanges++;
                            #endif
                            break;

                        case nShaderArg::Int:
                            hr = this->effect->SetInt(handle, curArg.GetInt());
                            #ifdef __NEBULA_STATS__
                            gfxServer->statsNumRenderStateChanges++;
                            #endif
                            break;

                        case nShaderArg::Float:
                            hr = this->effect->SetFloat(handle, curArg.GetFloat());
                            #ifdef __NEBULA_STATS__
                            gfxServer->statsNumRenderStateChanges++;
                            #endif
                            break;

                        case nShaderArg::Float4:
                            hr = this->effect->SetVector(handle, (CONST D3DXVECTOR4*) &(curArg.GetFloat4()));
                            #ifdef __NEBULA_STATS__
                            gfxServer->statsNumRenderStateChanges++;
                            #endif
                            break;

                        case nShaderArg::Matrix44:
                            hr = this->effect->SetMatrix(handle, (CONST D3DXMATRIX*) curArg.GetMatrix44());
                            #ifdef __NEBULA_STATS__
                            gfxServer->statsNumRenderStateChanges++;
                            #endif
                            break;

                        case nShaderArg::Texture:
                            hr = this->effect->SetTexture(handle, ((nD3D9Texture*)curArg.GetTexture())->GetBaseTexture());
                            #ifdef __NEBULA_STATS__
                            gfxServer->statsNumTextureChanges++;
                            #endif
                            break;
                    }
                    n_assert(SUCCEEDED(hr));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Update the parameter handles table which maps nShader2 parameters to
    D3DXEffect parameter handles. Will only check the current valid technique.
*/
void
nD3D9Shader::UpdateParameterHandles()
{
    n_assert(this->effect);

    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));

    D3DXHANDLE curTechnique = this->effect->GetCurrentTechnique();
    int param;
    for (param = 0; param < NumParameters; param++)
    {
        const char* paramName = this->ParameterToString((Parameter)param);
        n_assert(paramName);
        if (this->effect->IsParameterUsed(paramName, curTechnique))
        {
            this->parameterHandles[param] = this->effect->GetParameterByName(0, paramName);
            n_assert(0 != this->parameterHandles[param]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Return true if parameter is used by effect.
*/
bool
nD3D9Shader::IsParameterUsed(Parameter p)
{
    n_assert(p < NumParameters);
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
        if ((nGfxServer2::DX9 != this->refGfxServer->GetFeatureSet()) && (!oldSoftwareVertexProcessing))
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
nD3D9Shader::Begin()
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
        HRESULT hr = this->effect->Begin(&numPasses, D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE);
        n_assert(SUCCEEDED(hr));
        return numPasses;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::Pass(int pass)
{
    HRESULT hr;
    n_assert(this->effect);
    hr = this->effect->Pass(pass);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::End()
{
    HRESULT hr;
    n_assert(this->effect);
    if (!this->didNotValidate)
    {
        hr = this->effect->End();
        n_assert(SUCCEEDED(hr));
    }
}
