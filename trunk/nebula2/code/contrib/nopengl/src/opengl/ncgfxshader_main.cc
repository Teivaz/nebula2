//------------------------------------------------------------------------------
//  ncgfxshader_main.cc
//  13-Dec-2003 Haron
//------------------------------------------------------------------------------
#include "opengl/ncgfxshader.h"
#include "opengl/nglserver2.h"
#include "opengl/ngltexture.h"
#include "gfx2/nshaderparams.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

#include "util/nstrlist.h"
#include "util/npathstring.h"

nNebulaClass(nCgFXShader, "nshader2");

const char* cgFX_modes[] = {"","OpenGL","DirectX8","DirectX9"};

// init nCgFXShader static variables
bool nCgFXShader::deviceInit = false;
CgFXMode nCgFXShader::mode = CgFX_Unknown;

//------------------------------------------------------------------------------
/**
    Create CgFX device
*/
bool
nCgFXShader::CreateDevice(nGfxServer2 *srv)
{
    if (deviceInit) return true;

    LPCSTR errorString = 0;
    LPVOID device;
    const char* gfxname = srv->GetClass()->GetName();

    if (n_stricmp(gfxname,"nglserver2") == 0) mode = CgFX_OpenGL;
    else if (n_stricmp(gfxname,"nd3d8server") == 0) mode = CgFX_Direct3D8;
    else if (n_stricmp(gfxname,"nd3d9server") == 0) mode = CgFX_Direct3D9;
    else
    {
        n_error("nCgFXShader::CreateDevice(): failed to determain gfx server. gfx name: %sn",gfxname);
        return false;
    }

    if (mode == CgFX_OpenGL)
    {
        device = (LPVOID)((nGLServer2*)srv)->context;
    }
    else
    {
        n_error("nCgFXShader::CreateDevice(): CgFX for <%s> not supported yet!",
            cgFX_modes[mode]);
        return false;
    }

    n_assert(!((nGLServer2*)srv)->getGLErrors("nCgFXShader::CreateDevice1"));

    //n_printf("Device %p\n", device);

    // Set device
    if (FAILED(CgFXSetDevice(cgFX_modes[mode], device)))
    {
        CgFXGetErrors(&errorString);
        n_error("nCgFXShader::CreateDevice(): failed to set device\n\tin nCgFXShader::LoadResource()->CgFXSetDevice(%s)\nError: %s\n",
            cgFX_modes[mode], errorString);
        return false;
    }
    
    ((nGLServer2*)srv)->getGLErrors("nCgFXShader::CreateDevice2");

    deviceInit = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Release CgFX device
*/
bool
nCgFXShader::ReleaseDevice(nGfxServer2 *srv)
{
    if (!deviceInit) return true;

    deviceInit = false;

    LPCSTR errorString = 0;
    LPVOID device;

    if (mode == CgFX_OpenGL)
    {
        device = (LPVOID)((nGLServer2*)srv)->context;
    }
    else
    {
        n_error("nCgFXShader::ReleaseDevice(): CgFX for <%s> not supported yet!",
            cgFX_modes[mode]);
        return false;
    }

    // Destroy CgFX context
    if (FAILED(CgFXFreeDevice(cgFX_modes[mode], device)))
    {
        CgFXGetErrors(&errorString);
        n_error("Error (nCgFXShader::ReleaseDevice): Error in CgFXFreeDevice - %s\n", errorString);
        return false;
    }

/*    // there is no such function in CgFX 1.2
    if (FAILED(CgFXRelease()))
    {
        CgFXGetErrors(&errorString);
        n_error("Error (nCgFXShader::ReleaseDevice): Error in CgFXRelease - %s\n", errorString);
        return false;
    }
*/
    return true;
}

//------------------------------------------------------------------------------
/**
    Recursive #include directives substituting
*/
bool
nCgFXShader::resolveIncludes(nFile *dstfile, nPathString *srcfile, nArray<nString> &includes)
{
    //n_printf("<CHECK>%s</CHECK>\n", srcfile->Get());

    nPathString shaderdir(srcfile->ExtractDirName());
    nFile* f = this->refFileServer->NewFileObject();
    n_assert(f);

    // open the file
    if (!f->Open(srcfile->Get(), "r"))
    {
        n_error("nCgFXShader::resolveIncludes(): Could not open file %s\n", srcfile->Get());
        f->Close();
        return false;
    }

    char line[N_MAXPATH];
    nPathString includeFile;

    while (f->GetS(line, sizeof(line))) // scan each line from source file
    {
        bool needSave = true;
/*        
        char *sampler;
        while ((sampler = strstr(line, "sampler")) !=NULL)
            if (isalpha(sampler - 1) == 0 && isalpha(sampler + 7) == 0) // check that sampler is not a part of other word
            {
            }
*/
        char ifname[400];
        const char *word;
        nString sline(line);

        word = sline.GetFirstToken(" \t"); // first not whitespace

        //n_printf("<LINE>%s\n<WORD>%s\n", line, word);

        includeFile.Set(""); // clear content

        if (NULL != word)
            if (n_stricmp(word, "#include") == 0) // we found 'include' directive
            {
                word = sline.GetNextToken(" \t\n");

                n_strncpy2(ifname, &word[1], strlen(word)-2); // cut first and last (") (#include "lib.fx")
                nPathString fn = nPathString(ifname).ExtractFileName();
                if (0 == includes.Find(fn)) // check if this file was not included previously
                {
                    //n_printf("<FILE>%s\n", fn.Get());
                    needSave = false;
                    includeFile.Append(shaderdir);
                    includeFile.Append(ifname);
                    includes.Append(fn);

                    if (!resolveIncludes(dstfile, &includeFile, includes))
                    {
                        f->Close();
                        return false;
                    }
                    //n_printf("\t<INCLUDE>%s</INCLUDE>\n", includeFile.Get());
                }
            }
            else if (n_stricmp(word, "shared") == 0) // we found 'shared' keyword
            {
                //dstfile->PutS("uniform ");
                dstfile->PutS(sline.GetNextToken("\n")); // just skip it
                needSave = false;
            }
            //else if (n_stricmp(word, "#define") == 0) // we found 'define' directive
            //{
            //    dstfile->PutS("const float "); // translate #define to 'const float' variable

            //    word = sline.GetNextToken(" \t");
            //    dstfile->PutS(word); // variable name
            //    dstfile->PutS(" = ");

            //    word = sline.GetNextToken(" \t\n"); // variable value
            //    int sz = strlen(word);
            //    if (word[sz-1] == (char)0x0D)
            //    {
            //        char strtmp[256];
            //        n_strncpy2(strtmp, word, sz-1);
            //        dstfile->PutS(strtmp);
            //    }
            //    else dstfile->PutS(word);
            //    dstfile->PutS("; ");

            //    word = sline.GetNextToken("\n"); // read some comments
            //    if (NULL != word) dstfile->PutS(word);
            //    dstfile->PutS("\n// ");
            //}

        if (needSave) // simply copy a string
        {
            dstfile->PutS(line);
            dstfile->PutChar((char)0x0A); // only for CgFX needs
        }
    }

    f->Close();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
nCgFXShader::nCgFXShader() :
    refGfxServer("/sys/servers/gfx"),
    refFileServer("/sys/servers/file2"),
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
    if (this->refGfxServer->GetShader() == this)
    {
        this->refGfxServer->SetShader(0);
    }
/*
    // Destroy CgFX context
    CgFXFreeDevice(cgFX_modes[mode],(void*)this->refGfxServer->context);
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
    // mangle pathname
    nString filename = this->GetFilename();
    nString mangledPath;
    mangledPath = nFileServer2::Instance()->ManglePath(filename.Get());

    // check if the shader file actually exist, a non-existing shader file is
    // not a fatal error (result is that no rendering will be done)
    if (!nFileServer2::Instance()->FileExists(mangledPath.Get()))
    {
        n_printf("nCgFXShader::LoadResource() WARNING: shader file '%s' does not exist!\n", mangledPath.Get());
        return false;
    }

    nFile* f = nFileServer2::Instance()->NewFileObject();
    n_assert(f);

    nPathString _tmpfile;
    _tmpfile.Append(nPathString((mangledPath.Get())).ExtractDirName().Get());
    //_tmpfile.Append("__tmp__.fx");
    _tmpfile.Append("goochy.fx");
#if 0
    // why??? it would truncate the file!
    if (!f->Open(_tmpfile.Get(), "w"))
    {
        n_error("nCgFXShader::LoadResource(): Could not open file %s\n", _tmpfile.Get());
        f->Release();
        return false;
    }

//    #warning "recursive including"
//    resolveIncludes(f, new nPathString(mangledPath.Get()), nArray<nString>());

    f->Close();
#endif
#ifdef __WIN32__
    _tmpfile.ConvertBackslashes();
#endif

    // Load new effect
    LPCSTR errorString = 0;

    if (FAILED(CgFXCreateEffectFromFileA(mangledPath.Get(), 0, &this->effect, &errorString)))
    {
        n_error("nCgFXShader::LoadResource(): failed to load fx file '%s' \n\tin nCgFXShader::LoadResource()->CgFXCreateEffectFromFileA\n with:\n<BEGIN ERROR>\n%s\n<END ERROR>\n",
                mangledPath.Get(),
                errorString ? errorString : "No CgFX error message (no shader file?)");
        return false;
    }
    n_assert(this->effect);

    f->Release();

    // success
    this->hasBeenValidated = false;
    this->didNotValidate = false;
    this->SetValid(true);

    // validate the effect
    this->ValidateEffect();

    if (FAILED(CgFXGetErrors(&errorString)))
    {
        n_error("nCgFXShader: error (%s)\n\tin nCgFXShader::LoadResource()\n", errorString);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetBool(nShaderState::Param p, bool val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));

    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetBool(this->parameterHandles[p], val);

    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
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
    n_assert(SUCCEEDED(hr));
}
//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetInt(nShaderState::Param p, int val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetInt(this->parameterHandles[p], val);
      n_assert(SUCCEEDED(hr));
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
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetFloat(nShaderState::Param p, float val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));

    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetFloat(this->parameterHandles[p], val);
    
    n_assert(SUCCEEDED(hr));
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
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetVector4(nShaderState::Param p, const vector4& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&val));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (float*) &val, 4);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetVector3(nShaderState::Param p, const vector3& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    static vector4 v;
    v.set(val.x, val.y, val.z, 1.0f);
    this->curParams.SetArg(p, nShaderArg(*((nFloat4*)&v)));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (float*) &v, 3);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    const char * errors = NULL;
    CgFXGetErrors(&errors);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetFloat4(nShaderState::Param p, const nFloat4& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    this->curParams.SetArg(p, nShaderArg(val));
    HRESULT hr = this->effect->SetVector(this->parameterHandles[p], (float*) &val, 4);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
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
    n_assert(SUCCEEDED(hr));
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
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetMatrix(nShaderState::Param p, const matrix44& val)
{
    n_assert(this->effect && (p < nShaderState::NumParameters));
    n_printf("p=%d, NumParameters=%d\n", p, nShaderState::NumParameters);
    this->curParams.SetArg(p, nShaderArg(&val));
    HRESULT hr = this->effect->SetMatrix(this->parameterHandles[p], (const float*) &val, 4, 4);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_assert(SUCCEEDED(hr));
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
    n_assert(SUCCEEDED(hr));
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
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::SetTexture(nShaderState::Param p, nTexture2* tex)
{
    n_assert(tex);
    n_assert(this->effect && (p < nShaderState::NumParameters));

    if ((!this->curParams.IsParameterValid(p)) ||
        (this->curParams.IsParameterValid(p) && (this->curParams.GetArg(p).GetTexture() != tex)))
    {
        this->curParams.SetArg(p, nShaderArg(tex));
        HRESULT hr = this->effect->SetTexture(this->parameterHandles[p], (DWORD)((nGLTexture*)tex)->GetTexID());
        #ifdef __NEBULA_STATS__
        //this->refGfxServer->statsNumTextureChanges++;
        #endif
        n_assert(SUCCEEDED(hr));
    }
}

//------------------------------------------------------------------------------
/**
    Set a whole shader parameter block at once. This is slightly faster
    (and more convenient) then setting single parameters. This works by batching
    the parameter changes and then applying them all at once.
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
    for (i = 0; i < nShaderState::NumParameters; i++)
    {
        nShaderState::Param p = (nShaderState::Param) i;

        // source parameter valid?
        if (params.IsParameterValid(p))
        {
            // parameter used in shader?
            CGFXHANDLE handle = this->parameterHandles[p];
            if (handle != 0)
            {
                // avoid redundant state switches
                const nShaderArg& curArg = params.GetArg(p);
                if (!(curArg == this->curParams.GetArg(p)))
                {
                    changes = true;
                    this->curParams.SetArg(p, curArg);
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
                    n_assert(SUCCEEDED(hr));
                }
            }
        }
    }
    /*
    D3DXHANDLE paramblock = this->effect->EndParameterBlock();
    if (changes)
    {
        hr = this->effect->ApplyParameterBlock(paramblock);
        n_assert(SUCCEEDED(hr));
    }
    */
}

//------------------------------------------------------------------------------
/**
    Update the parameter handles table which maps nShader2 parameters to
    ICgFXEffect parameter handles. Will only check the current valid technique.
*/
void
nCgFXShader::UpdateParameterHandles()
{
    n_assert(deviceInit);
    n_assert(this->effect);

    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));

    // CGFXHANDLE curTechnique;
    // curTechnique = this->effect->GetCurrentTechnique();
    // n_assert(NULL != curTechnique);
    //HRESULT hr = this->effect->GetTechniqueByName(curTechnique);
    //n_assert(SUCCEEDED(hr));

    n_printf("Start shader parameters lookup...\n");
    uint param;

    CgFXEFFECT_DESC fxDesc;
    n_assert(SUCCEEDED(this->effect->GetDesc(&fxDesc)));

    for (param = 0; param < fxDesc.Parameters; param++)
    {
        CgFXPARAMETER_DESC pdesc;
        CGFXHANDLE paramh;
        paramh = this->effect->GetParameter(NULL, param);
        if (FAILED(this->effect->GetParameterDesc(paramh,&pdesc)))
            continue;
        nShaderState::Param p = nShaderState::StringToParam(pdesc.Name);
        if (p == nShaderState::InvalidParameter)
            continue;
        this->parameterHandles[p] = paramh;
        n_printf("nShaderState::Param <%s:%s>\n", pdesc.Name, paramh);
    }
    n_printf("End parameters lookup\n");
}

//------------------------------------------------------------------------------
/**
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
    LPCSTR errorString = 0;

    n_printf("Start shader validating.\n");

    // Next step is to search for valid techniques.
    hr = this->effect->GetDesc(&fxDesc);
    n_assert(SUCCEEDED(hr));
    numTechniques = fxDesc.Techniques;
    n_printf("Num techniques %d\n", numTechniques);

    for( int t = 0; t < numTechniques; ++t )
    {
        CGFXHANDLE tchq = this->effect->GetTechnique(t);
        this->effect->GetTechniqueDesc(tchq,&tDesc);
        if (FAILED(this->effect->SetTechnique(tchq)))
        {
            n_printf("Failed to set technique <%s>\n", tDesc.Name);
            //SAFE_RELEASE(pEffect);
            //DISPCONSOLE_ERROR("Internal error: Failed to set technique " << tDesc.Name);
            return;
        }
        else if (FAILED(this->effect->ValidateTechnique(tchq)))
        {
            n_printf("Failed to validate technique <%s>\n", tDesc.Name);
            CgFXGetErrors(&errorString);
            if (errorString)
                n_printf("Error: %s\n", errorString);
            continue;
        }

        n_printf("Technique <%s> was successfully validated\n", tDesc.Name);
        this->hasBeenValidated = true;
        this->didNotValidate = false;
        this->UpdateParameterHandles();
        CgFXGetErrors(&errorString);
        if (errorString)
            n_printf("Error: %s\n", errorString);
        return;
    }
    // no valid technique found
    this->hasBeenValidated = true;
    this->didNotValidate = true;
    n_printf("nCgFXShader() warning: shader '%s' did not validate!\n", this->GetFilename().Get());
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
        n_assert(SUCCEEDED(hr));
        return numPasses;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShader::BeginPass(int pass)
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
        n_assert(SUCCEEDED(hr));
    }
}
