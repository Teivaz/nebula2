#define N_IMPLEMENTS nD3D9Shader
//------------------------------------------------------------------------------
//  nd3d9shader_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9shader.h"
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9texture.h"
#include "kernel/nfileserver2.h"
#include "variable/nvariableserver.h"

nNebulaClass(nD3D9Shader, "nshader2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Shader::nD3D9Shader() :
    refGfxServer(kernelServer, "/sys/servers/gfx"),
    effect(0),
    hasBeenValidated(false),
    didNotValidate(false)
{
    // empty
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
nD3D9Shader::Unload()
{
    if (this->valid)
    {
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

        // clear the embedded variable context
        this->varContext.Clear();

        this->valid = false;
    }
    else
    {
        n_assert(0 == this->effect);
    }
}

//------------------------------------------------------------------------------
/**
    Load D3DX effects file.
*/
bool
nD3D9Shader::Load()
{
    n_assert(this->GetFilename());
    n_assert(!this->valid);
    n_assert(0 == this->effect);

    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);

    // mangle path name
    char mangledPath[N_MAXPATH];
    this->refFileServer->ManglePath(this->GetFilename(), mangledPath, sizeof(mangledPath));

    // load fx file...
    ID3DXBuffer* errorBuffer = 0;
    #if N_D3D9_DEBUG
        DWORD compileFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
    #else
        DWORD compileFlags = 0;
    #endif
    hr = D3DXCreateEffectFromFile(d3d9Dev,          // pDevice
                                  mangledPath,      // pSrcFile
                                  NULL,             // pDefines
                                  NULL,             // pInclude
                                  compileFlags,     // Flags
                                  NULL,             // pPool
                                  &(this->effect),  // ppEffect
                                  &errorBuffer);    // ppCompilationErrors
    if (FAILED(hr))
    {
        n_error("nD3D9Shader: failed to load fx file '%s' with:\n\n%s\n", 
                this->GetFilename(),
                errorBuffer ? errorBuffer->GetBufferPointer() : "No D3DX error message (no shader file?)");
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
    this->valid = true;

    // for each shader parameter add a variable containing the
    // parameter handle to the internal variable context
    n_assert(this->varContext.GetNumVariables() == 0);

    nVariableServer* varServer = this->refVariableServer.get();
    D3DXEFFECT_DESC fxDesc;
    hr = this->effect->GetDesc(&fxDesc);
    n_assert(SUCCEEDED(hr));
    uint i;
    for (i = 0; i < fxDesc.Parameters; i++)
    {
        D3DXHANDLE paramHandle = this->effect->GetParameter(NULL, i);
        D3DXPARAMETER_DESC paramDesc;
        hr = this->effect->GetParameterDesc(paramHandle, &paramDesc);
        n_assert(SUCCEEDED(hr));

        // get a variable handle for the parameter and add a variable
        // to the internal variable context
        if ((D3DXPT_BOOL == paramDesc.Type) ||
            (D3DXPT_INT  == paramDesc.Type) ||
            (D3DXPT_FLOAT == paramDesc.Type) ||
            (D3DXPT_TEXTURE == paramDesc.Type) ||
            (D3DXPT_TEXTURE2D == paramDesc.Type) ||
            (D3DXPT_TEXTURE3D == paramDesc.Type) ||
            (D3DXPT_TEXTURECUBE == paramDesc.Type))
        {
            nVariable::Handle varHandle = varServer->GetVariableHandleByName(paramDesc.Name);
            nVariable newVar(varHandle, int(paramHandle));
            this->varContext.AddVariable(newVar);
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetInt(nVariable::Handle h, int val)
{   
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetInt((D3DXHANDLE)var->GetInt(), val);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetIntArray(nVariable::Handle h, const int* array, int count)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetIntArray((D3DXHANDLE)var->GetInt(), array, count);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetFloat(nVariable::Handle h, float val)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetFloat((D3DXHANDLE)var->GetInt(), val);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void 
nD3D9Shader::SetFloatArray(nVariable::Handle h, const float* array, int count)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetFloatArray((D3DXHANDLE)var->GetInt(), array, count);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetVector(nVariable::Handle h, const float4& val)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetVector((D3DXHANDLE)var->GetInt(), (CONST D3DXVECTOR4*) &val);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void 
nD3D9Shader::SetVectorArray(nVariable::Handle h, const float4* array, int count)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetVectorArray((D3DXHANDLE)var->GetInt(), (CONST D3DXVECTOR4*) array, count);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetMatrix(nVariable::Handle h, const matrix44& val)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetMatrix((D3DXHANDLE)var->GetInt(), (CONST D3DXMATRIX*) &val);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void 
nD3D9Shader::SetMatrixArray(nVariable::Handle h, const matrix44* array, int count)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetMatrixArray((D3DXHANDLE)var->GetInt(), (CONST D3DXMATRIX*) array, count);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void 
nD3D9Shader::SetMatrixPointerArray(nVariable::Handle h, const matrix44** array, int count)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetMatrixPointerArray((D3DXHANDLE)var->GetInt(), (CONST D3DXMATRIX**) array, count);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Shader::SetTexture(nVariable::Handle h, nTexture2* tex)
{
    n_assert(tex);
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    n_assert(var);
    HRESULT hr = this->effect->SetTexture((D3DXHANDLE)var->GetInt(), ((nD3D9Texture*)tex)->GetTexture());
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Shader::IsParameterUsed(nVariable::Handle h)
{
    n_assert(this->effect);
    nVariable* var = this->varContext.GetVariable(h);
    return (var != 0);
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

    // set on first technique that validates correctly
    D3DXHANDLE technique = NULL;
    HRESULT hr = this->effect->FindNextValidTechnique(NULL, &technique);
    if (SUCCEEDED(hr) && (technique != NULL)) 
    {
        // valid technique found
        this->effect->SetTechnique(technique);
        this->hasBeenValidated = true;
        this->didNotValidate = false;
    }
    else
    {
        // no valid technique found
        this->hasBeenValidated = true;
        this->didNotValidate = true;
        n_printf("nD3D9Shader() warning: shader '%s' did not validate!\n", this->GetFilename());
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
