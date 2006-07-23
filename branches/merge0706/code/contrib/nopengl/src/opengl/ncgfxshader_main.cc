//------------------------------------------------------------------------------
//  ncgfxshader_main.cc
//  13-Dec-2003 Haron
//------------------------------------------------------------------------------
#include "opengl/ncgfxshader.h"
#include "opengl/ncgfxshaderinclude.h"
#include "opengl/nglserver2.h"
#include "opengl/ngltexture.h"
#include "gfx2/nshaderparams.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

#include "util/nstrlist.h"

nNebulaClass(nCgFXShader, "gfx2::nshader2");

const char* cgFX_modes[] = {"","OpenGL","DirectX8","DirectX9"};

// init nCgFXShader static variables
bool nCgFXShader::deviceInit = false;
CgFXMode nCgFXShader::mode = CgFX_Unknown;

//------------------------------------------------------------------------------
/**
    CgFX error handling.

     - 19-Oct-2004   Haron    created
*/
void
__cdecl
n_cgfxerror(HRESULT hr, const char *msg)
{
    if (FAILED(hr))
    {
        nString message(msg);
        LPCSTR errorString = 0;
        CgFXGetErrors(&errorString);
        message.Append("\n    Message: ");
        if (errorString)
        {
            message.Append(errorString);
        }
        else
        {
            message.Append("There is no CgFX error message.");
        }
        message.Append("\n");
        n_error(message.Get());
    }
}

//------------------------------------------------------------------------------
/**
    CgFX warning handling.

     - 19-Oct-2004   Haron    created
*/
bool
__cdecl
n_cgfxwarning(HRESULT hr, const char *msg)
{
    if (FAILED(hr))
    {
        nString message(msg);
        LPCSTR errorString = 0;
        CgFXGetErrors(&errorString);
        message.Append("\n    Message: ");
        if (errorString)
        {
            message.Append(errorString);
        }
        else
        {
            message.Append("There is no CgFX error message.");
        }
        message.Append("\n");
        n_printf(message.Get());
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Create CgFX device
*/
bool
nCgFXShader::CreateDevice()
{
    if (deviceInit) return true;

    nGfxServer2* gfxServer = nGfxServer2::Instance();

    HRESULT hr;
    LPVOID device;
    const char* gfxname = gfxServer->GetClass()->GetName();

    if (n_stricmp(gfxname,"nglserver2") == 0) mode = CgFX_OpenGL;
    else if (n_stricmp(gfxname,"nd3d8server") == 0) mode = CgFX_Direct3D8;
    else if (n_stricmp(gfxname,"nd3d9server") == 0) mode = CgFX_Direct3D9;
    else
    {
        n_error("nCgFXShader::CreateDevice(): failed to determain gfx server. gfx name: %sn", gfxname);
        return false;
    }

    if (mode == CgFX_OpenGL)
    {
        device = (LPVOID)((nGLServer2*)gfxServer)->context;
    }
    else
    {
        n_error("nCgFXShader::CreateDevice(): CgFX for <%s> not supported yet!",
            cgFX_modes[mode]);
        return false;
    }
    n_gltrace("nCgFXShader::CreateDevice(1).");

    //n_printf("Device %p\n", device);

    // Set device
    hr = CgFXSetDevice(cgFX_modes[mode], device);
    n_cgfxerror(hr, "nCgFXShader::CreateDevice(): failed to set device in CgFXSetDevice.");

    deviceInit = true;
    n_gltrace("nCgFXShader::CreateDevice(2).");
    return true;
}

//------------------------------------------------------------------------------
/**
    Release CgFX device
*/
bool
nCgFXShader::ReleaseDevice()
{
    if (!deviceInit) return true;

    deviceInit = false;

    HRESULT hr;
    LPVOID device;

    if (mode == CgFX_OpenGL)
    {
        device = (LPVOID)((nGLServer2*)nGfxServer2::Instance())->context;
    }
    else
    {
        n_error("nCgFXShader::ReleaseDevice(): CgFX for <%s> not supported yet!",
            cgFX_modes[mode]);
        return false;
    }

    // Destroy CgFX context
    hr = CgFXFreeDevice(cgFX_modes[mode], device);
    n_cgfxerror(hr, "nCgFXShader::ReleaseDevice(): Error in CgFXFreeDevice.");

/*    // there is no such function in CgFX 1.2
    if (FAILED(CgFXRelease()))
    {
        CgFXGetErrors(&errorString);
        n_error("Error (nCgFXShader::ReleaseDevice): Error in CgFXRelease - %s\n", errorString);
        return false;
    }
*/
    n_gltrace("nCgFXShader::ReleaseDevice().");
    return true;
}

//------------------------------------------------------------------------------
/**
*/
nCgFXShader::nCgFXShader() :
    effect(0),
    hasBeenValidated(false),
    didNotValidate(false)
{
    n_assert(this->deviceInit);
    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));
}

//------------------------------------------------------------------------------
/**
*/
nCgFXShader::~nCgFXShader()
{
    if (!deviceInit) return;
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::UnloadResource()
{
    n_assert(this->IsValid());
    n_assert(this->effect);

    // if this is the currently set shader, unlink from gfx server
    if (nGfxServer2::Instance()->GetShader() == this)
    {
        nGfxServer2::Instance()->SetShader(0);
    }
/*
    // Destroy CgFX context
    CgFXFreeDevice(cgFX_modes[mode],(void*)nGfxServer2::Instance()->context);
    CgFXRelease();
*/
    this->effect = 0;

    // clear current parameter settings
    //this->curParams.Reset();

    this->SetValid(false);
}

//------------------------------------------------------------------------------
/**
    Load CgFX-shader file.
*/
bool
nCgFXShader::LoadResource()
{
    n_assert(deviceInit);
    n_assert(!this->IsValid());
    n_assert(0 == this->effect);
    n_assert(CgFX_OpenGL == mode);

    n_printf("Start shader loading...\n");

    nCgFXShaderInclude si;

    si.Begin(this->GetFilename());

    // Load new effect
    const char* errorString;
    HRESULT hr;

    hr = CgFXCreateEffectFromFileA(si.GetFileName().Get(), 0, &this->effect, &errorString);
    nString errStr("nCgFXShader::LoadResource(): Failed to load fx file '");
    errStr += this->GetFilename() + "'\n    in CgFXCreateEffectFromFileA.";
    n_cgfxerror(hr, errStr.Get());
    n_assert(this->effect);

    si.End();

    // success
    this->hasBeenValidated = false;
    this->didNotValidate = false;
    this->SetValid(true);

    // validate the effect
    this->ValidateEffect();

    n_gltrace("nCgFXShader::LoadResource().");
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetBool(nShaderState::Param p, bool val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("SetBool: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetBool(this->parameterHandles[p], val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetBool().");
    n_gltrace("nCgFXShader::SetBool().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetBoolArray(nShaderState::Param p, const bool* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetBoolArray(this->parameterHandles[p], array, count);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetBoolArray().");
    n_gltrace("nCgFXShader::SetBoolArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetInt(nShaderState::Param p, int val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("SetInt: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetInt(this->parameterHandles[p], val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetInt().");
    n_gltrace("nCgFXShader::SetInt().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetIntArray(nShaderState::Param p, const int* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetIntArray(this->parameterHandles[p], array, count);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetIntArray().");
    n_gltrace("nCgFXShader::SetIntArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetFloat(nShaderState::Param p, float val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("SetFloat: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetFloat(this->parameterHandles[p], val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetFloat().");
    n_gltrace("nCgFXShader::SetFloat().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetFloatArray(nShaderState::Param p, const float* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetFloatArray(this->parameterHandles[p], array, count);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetFloatArray().");
    n_gltrace("nCgFXShader::SetFloatArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetVector4(nShaderState::Param p, const vector4& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("SetVector4: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&val));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (float*) &val, 4);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetVector4().");
    n_gltrace("nCgFXShader::SetVector4().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetVector3(nShaderState::Param p, const vector3& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("SetVector3: %s\n", nShaderState::ParamToString(p));
    static vector4 v;
    v.set(val.x, val.y, val.z, 1.0f);
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&v));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (const float*) &val, 3);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetVector3().");
    n_gltrace("nCgFXShader::SetVector3().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetFloat4(nShaderState::Param p, const nFloat4& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("SetFloat4: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (float*) &val, 4);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetFloat4().");
    n_gltrace("nCgFXShader::SetFloat4().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetFloat4Array(nShaderState::Param p, const nFloat4* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetVectorArray(this->parameterHandles[p], (const float *) array, 4, count);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetFloat4Array().");
    n_gltrace("nCgFXShader::SetFloat4Array().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetVector4Array(nShaderState::Param p, const vector4* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetVectorArray(this->parameterHandles[p], (const float *) array, 4, count);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetVector4Array().");
    n_gltrace("nCgFXShader::SetVector4Array().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetMatrix(nShaderState::Param p, const matrix44& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("SetMatrix: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(&val));
    HRESULT hr = this->effect->SetMatrix(this->parameterHandles[p], (const float*) &val, 4, 4);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetMatrix().");
    n_gltrace("nCgFXShader::SetMatrix().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetMatrixArray(nShaderState::Param p, const matrix44* array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetMatrixArray(this->parameterHandles[p], (const float*) array, 4, 4, count);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetMatrixArray().");
    n_gltrace("nCgFXShader::SetMatrixArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetMatrixPointerArray(nShaderState::Param p, const matrix44** array, int count)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    HRESULT hr = this->effect->SetMatrixArray(this->parameterHandles[p], (const float*) *array, 4, 4, count);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_cgfxerror(hr, "nCgFXShader::SetMatrixPointerArray().");
    n_gltrace("nCgFXShader::SetMatrixPointerArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetTexture(nShaderState::Param p, nTexture2* tex)
{
    n_assert(tex);
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("SetTexture: %s\n", nShaderState::ParamToString(p));
    if ((!this->curParams.IsParameterValid(p)) ||
        (this->curParams.IsParameterValid(p) && (this->curParams.GetArg(p).GetTexture() != tex)))
    {
        this->curParams.SetArg(p, nShaderArg(tex));
        HRESULT hr = this->effect->SetTexture(this->parameterHandles[p], (DWORD)((nGLTexture*)tex)->GetTexID());
        #ifdef __NEBULA_STATS__
        //this->refGfxServer->statsNumTextureChanges++;
        #endif
        n_cgfxerror(hr, "nCgFXShader::SetTexture().");
        n_gltrace("nCgFXShader::SetTexture().");
    }
}

//------------------------------------------------------------------------------
/**
    Set a whole shader parameter block at once. This is slightly faster
    (and more convenient) then setting single parameters.
*/
void
nCgFXShader::SetParams(const nShaderParams& params)
{
    #ifdef __NEBULA_STATS__
    //nGLServer2* gfxServer = this->refGfxServer.get();
    #endif
    int i;
    HRESULT hr;
    //this->effect->BeginParameterBlock();
    bool changes = false;
    int numValidParams = params.GetNumValidParams();
    for (i = 0; i < numValidParams; i++)
    {
        nShaderState::Param curParam = params.GetParamByIndex(i);

        // parameter used in shader?
        CGFXHANDLE handle = this->parameterHandles[curParam];
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
                        //gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Int:
                        hr = this->effect->SetInt(handle, curArg.GetInt());
                        #ifdef __NEBULA_STATS__
                        //gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Float:
                        hr = this->effect->SetFloat(handle, curArg.GetFloat());
                        #ifdef __NEBULA_STATS__
                        //gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Float4:
                        hr = this->effect->SetVector(handle, (float*) &(curArg.GetFloat4()), 4);
                        #ifdef __NEBULA_STATS__
                        //gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Matrix44:
                        hr = this->effect->SetMatrix(handle, (float*) (curArg.GetMatrix44()), 4, 4); //& - ??
                        #ifdef __NEBULA_STATS__
                        //gfxServer->statsNumRenderStateChanges++;
                        #endif
                        break;

                    case nShaderState::Texture:
                        hr = this->effect->SetTexture(handle, (DWORD)((nGLTexture*)curArg.GetTexture())->GetTexID());
                        #ifdef __NEBULA_STATS__
                        //gfxServer->statsNumTextureChanges++;
                        #endif
                        break;
                }
                n_cgfxerror(hr, "nCgFXShader::SetParams(): Error while setting parameter.");
            }
        }
    }
    n_gltrace("nCgFXShader::SetParams().");
}

//------------------------------------------------------------------------------
/**
    Update the parameter handles table which maps nShader2 parameters to
    ICgFXEffect parameter handles. Will only check the current valid technique.
*/
void
nCgFXShader::UpdateParameterHandles()
{
    n_assert(this->deviceInit);
    n_assert(this->effect);
    HRESULT hr;

    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));

    n_printf("Start shader parameters lookup...\n");

    CgFXEFFECT_DESC fxDesc;
    hr = this->effect->GetDesc(&fxDesc);
    n_cgfxerror(hr, "nCgFXShader::UpdateParameterHandles(): Error while getting effect descriptor.");

    uint curParamIndex;
    for (curParamIndex = 0; curParamIndex < fxDesc.Parameters; curParamIndex++)
    {
        CGFXHANDLE curParamHandle = this->effect->GetParameter(NULL, curParamIndex);
        n_assert(NULL != curParamHandle);

        // get the associated Nebula2 parameter index
        CgFXPARAMETER_DESC paramDesc;
        hr = this->effect->GetParameterDesc(curParamHandle, &paramDesc);
        n_cgfxerror(hr, "nCgFXShader::UpdateParameterHandles(): Error while getting parameter descriptor.");
        nShaderState::Param nebParam = nShaderState::StringToParam(paramDesc.Name);
        if (nebParam != nShaderState::InvalidParameter)
        {
            this->parameterHandles[nebParam] = curParamHandle;
            n_printf("nShaderState::Param <%s>\n", paramDesc.Name);
        }
    }
    n_printf("End parameters lookup\n");

    n_gltrace("nCgFXShader::UpdateParameterHandles().");
}

//------------------------------------------------------------------------------
/**
    Return true if parameter is used by effect.
*/
bool
nCgFXShader::IsParameterUsed(nShaderState::Param p)
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
nCgFXShader::ValidateEffect()
{
    n_assert(!this->hasBeenValidated);
    n_assert(this->effect);
    n_assert(deviceInit);

    HRESULT hr;
    CgFXEFFECT_DESC fxDesc;
    CgFXTECHNIQUE_DESC tDesc;
    int numTechniques;

    n_printf("Start shader validating.\n");

    // Next step is to search for valid techniques.
    hr = this->effect->GetDesc(&fxDesc);
    n_cgfxerror(hr, "nCgFXShader::ValidateEffect(): Error while getting effect descriptor.");
    numTechniques = fxDesc.Techniques;
    n_printf("Num techniques %d\n", numTechniques);

    for( int t = 0; t < numTechniques; ++t )
    {
        HRESULT hr;
        nString errStr;
        CGFXHANDLE tchq = this->effect->GetTechnique(t);

        this->effect->GetTechniqueDesc(tchq,&tDesc);

        hr = this->effect->SetTechnique(tchq);
        errStr = "Failed to set technique <";
        errStr += this->GetFilename() + ">:<" + tDesc.Name + ">";
        if (n_cgfxwarning(hr, errStr.Get()))
        {
            return;
        }

        hr = this->effect->ValidateTechnique(tchq);
        errStr = "Failed to validate technique <";
        errStr += this->GetFilename() + ">:<" + tDesc.Name + ">";
        if (n_cgfxwarning(hr, errStr.Get()))
        {
            continue;
        }

        n_printf("Technique <%s>:<%s> was successfully validated\n", this->GetFilename().Get(), tDesc.Name);
        this->hasBeenValidated = true;
        this->didNotValidate = false;
        this->UpdateParameterHandles();
        n_gltrace("nCgFXShader::ValidateEffect(1).");
        return;
    }
    // no valid technique found
    this->hasBeenValidated = true;
    this->didNotValidate = true;
    n_printf("nCgFXShader() warning: shader '%s' did not validate!\n", this->GetFilename().Get());
    n_gltrace("nCgFXShader::ValidateEffect().");
}

//------------------------------------------------------------------------------
/**
    begin shader rendering
*/
int
nCgFXShader::Begin(bool saveState)
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
        else           flags = CGFX_DONOTSAVESTATE;

        HRESULT hr = this->effect->Begin(&numPasses, flags);
        n_cgfxerror(hr, "nCgFXShader::Begin(): Error while executing shader Begin function.");
        n_gltrace("nCgFXShader::Begin().");
        return numPasses;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::BeginPass(int pass)
{
    n_assert(this->effect);
    HRESULT hr;
    n_assert(this->effect);
    hr = this->effect->Pass(pass);
    n_cgfxerror(hr, "nCgFXShader::BeginPass(): Error while executing shader BeginPass function.");
    n_gltrace("nCgFXShader::BeginPass().");
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::CommitChanges()
{
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::EndPass()
{
}

//------------------------------------------------------------------------------
/**
    stop shader rendering
*/
void
nCgFXShader::End()
{
    HRESULT hr;
    n_assert(this->effect);
    if (!this->didNotValidate)
    {
        hr = this->effect->End();
        n_cgfxerror(hr, "nCgFXShader::End(): Error while executing shader End function.");
        n_gltrace("nCgFXShader::End().");
    }
}
