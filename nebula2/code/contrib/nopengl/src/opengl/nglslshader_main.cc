//------------------------------------------------------------------------------
//  nglslshader_main.cc
//  23-Mar-2005 Haron
//------------------------------------------------------------------------------

#include "opengl/nglslshader.h"
#include "opengl/nglshaderinclude.h"

#include "opengl/nglserver2.h"
#include "opengl/ngltexture.h"
#include "opengl/nglextensionserver.h"

#include "gfx2/nshaderparams.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

nNebulaClass(nGLSLShader, "nshader2");

void
n_glsltrace(GLhandleARB obj, const nString& msg)
{
    bool errorPresent = false;
    uint errNum = 0;
    GLenum error = glGetError();
    nString message;

    while (error != GL_NO_ERROR)
    {
        errorPresent = true;

        if (errNum < 20)
        {
            switch (error)
            {
                case GL_OUT_OF_MEMORY:
                    message += "<GL_OUT_OF_MEM>";
                    break;
                case GL_INVALID_ENUM:
                    message += "<GL_INVALID_ENUM>";
                    break;
                case GL_INVALID_VALUE:
                    message += "<GL_INVALID_VALUE>";
                    break;
                case GL_INVALID_OPERATION:
                    message += "<GL_INVALID_OPERATION>";
                    break;
                default:
                    message += "<GL_ERROR_TYPE: ";
                    message.AppendInt(error);
                    message += ">";
            }
        }
        else
        {
            message += "...";
            break;
        }

        errNum++;
        error = glGetError();
    }

    if (errorPresent)
    {
        if (obj > 0)
        {
            int logLength = 0;

            glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength);

            if (logLength > 0)
            {
                GLcharARB* infoLog = new GLcharARB[logLength];

                glGetInfoLogARB(obj, logLength, NULL, infoLog);

                n_message("\n%s\n    Error: %s\n    GLSL Error: %s\n", msg.Get(), message.Get(), infoLog);

                delete [] infoLog;
            }
        }
        else
        {
            n_message("\n%s\n    Error: %s\n", msg.Get(), message.Get());
        }
    }
}

void
n_assert_glslstatus(GLhandleARB obj, const nString& msg, GLenum param)
{
    GLint res;
    glGetObjectParameterivARB(obj, param, &res);

    if (res == GL_FALSE)
    {
        GLint logLen = 0;
        GLint charsWritten = 0;

        glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLen);

        char *log = n_new_array(char, logLen);

        glGetInfoLogARB(obj, logLen, &charsWritten, log);
        n_error("%s\n[%s]", msg.Get(), log);

        n_delete_array(log);
    }
}

//------------------------------------------------------------------------------
/**
*/
nGLSLShader::nGLSLShader() :
    hasBeenValidated(false),
    didNotValidate(false),
    //uniformsUpdated(false),
    programObj(0),
    vertexShader(0),
    fragmentShader(0)
{
    memset(this->parameterHandles, -1, sizeof(this->parameterHandles));
}

//------------------------------------------------------------------------------
/**
*/
nGLSLShader::~nGLSLShader()
{
    //    if (!deviceInit) return;
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::UnloadResource()
{
    n_assert(this->IsLoaded());

    // if this is the currently set shader, unlink from gfx server
    if (nGfxServer2::Instance()->GetShader() == this)
    {
        nGfxServer2::Instance()->SetShader(0);
    }

    glDetachObjectARB(this->programObj, this->vertexShader);
    glDeleteObjectARB(this->vertexShader);

    glDetachObjectARB(this->programObj, this->fragmentShader);
    glDeleteObjectARB(this->fragmentShader);

    glDeleteObjectARB(this->programObj);

    //uniformsUpdated = false;

    // clear current parameter settings
    this->curParams.Clear();

    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
Load GLSL-shader files.
*/
bool
nGLSLShader::LoadResource()
{
    n_assert(!this->IsLoaded());
    //n_assert(deviceInit);

    nGLShaderInclude si;
    nString fname = this->GetFilename();
    nString src;
    const char *shaderStrings[1];

    fname.StripExtension();

    n_printf("Start shader <%s> loading...\n", this->GetFilename().Get());

    this->programObj = glCreateProgramObjectARB(); 

    // attach vertex shader
    if (si.Begin(fname + ".vert"))
    {
        si.GetSource(src);
        shaderStrings[0] = src.Get();

        this->vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        glShaderSourceARB(this->vertexShader, 1, shaderStrings, NULL);
        glCompileShaderARB(this->vertexShader);

        n_assert_glslstatus(this->vertexShader, "nGLSLShader::LoadResource(): Vertex Shader Compile Error.", GL_OBJECT_COMPILE_STATUS_ARB);

        glAttachObjectARB(this->programObj, this->vertexShader);

        si.End();
    }

    // attach fragment shader
    if (si.Begin(fname + ".frag"))
    {
        si.GetSource(src);
        shaderStrings[0] = src.Get();

        this->fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        glShaderSourceARB(this->fragmentShader, 1, shaderStrings, NULL);
        glCompileShaderARB(this->fragmentShader);
        
        n_assert_glslstatus(this->fragmentShader, "nGLSLShader::LoadResource(): Fragment Shader Compile Error.", GL_OBJECT_COMPILE_STATUS_ARB);

        glAttachObjectARB(this->programObj, this->fragmentShader);

        si.End();
    }

    glLinkProgramARB(this->programObj);
    n_assert_glslstatus(this->programObj, "nGLSLShader::LoadResource(): Program linking Error.", GL_OBJECT_LINK_STATUS_ARB);

    // success
    this->hasBeenValidated = false;
    this->didNotValidate = false;
    this->SetState(Valid);

    // validate the effect
    this->ValidateEffect();

    //uniformsUpdated = false;
    this->UpdateParameterHandles();

    // TEST
    //matrix44 m;
    //glUseProgramObjectARB(this->programObj);
    //GLint p = glGetUniformLocationARB(this->programObj, "ModelViewProjection");
    //glUniformMatrix4fvARB(p, 1, GL_FALSE, _matrix44_ident);
    //n_glsltrace(this->programObj, "Testing.");

    src.Set("nGLSLShader::LoadResource(");
    src.Append(fname);
    src.Append(").");

    n_gltrace(src.Get());
    n_glsltrace(this->programObj, src.Get());

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetBool(nShaderState::Param p, bool val)
{
    n_assert(p < nShaderState::NumParameters);

    //n_printf("SetBool: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    glUniform1iARB(this->parameterHandles[p], val ? 1 : 0);

    n_gltrace("nGLSLShader::SetBool().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetBoolArray(nShaderState::Param p, const bool* arr, int count)
{
    n_assert(p < nShaderState::NumParameters);

    GLint* ba = n_new_array(GLint, count);
    int i;

    for (i = 0; i < count; i++)
    {
        ba[i] = arr[i] ? 1 : 0;
    }
    
    glUniform1ivARB(this->parameterHandles[p], count, ba);
    n_delete_array(ba);

    n_gltrace("nGLSLShader::SetBoolArray().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetInt(nShaderState::Param p, int val)
{
    n_assert(p < nShaderState::NumParameters);

    //n_printf("SetInt: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    glUniform1iARB(this->parameterHandles[p], val);

    n_gltrace("nGLSLShader::SetInt().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetIntArray(nShaderState::Param p, const int* arr, int count)
{
    n_assert(p < nShaderState::NumParameters);

    glUniform1ivARB(this->parameterHandles[p], count, arr);

    n_gltrace("nGLSLShader::SetIntArray().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetFloat(nShaderState::Param p, float val)
{
    n_assert(p < nShaderState::NumParameters);

    //n_printf("SetFloat: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    glUniform1fARB(this->parameterHandles[p], val);

    n_gltrace("nGLSLShader::SetFloat().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetFloatArray(nShaderState::Param p, const float* arr, int count)
{
    n_assert(p < nShaderState::NumParameters);

    glUniform1fvARB(this->parameterHandles[p], count, arr);

    n_gltrace("nGLSLShader::SetFloatArray().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetVector4(nShaderState::Param p, const vector4& val)
{
    n_assert(p < nShaderState::NumParameters);

    //n_printf("SetVector4: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&val));
    glUniform4fvARB(this->parameterHandles[p], 1, (float*) &val);

    n_gltrace("nGLSLShader::SetVector4().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetVector3(nShaderState::Param p, const vector3& val)
{
    n_assert(p < nShaderState::NumParameters);

    //n_printf("SetVector3: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&val));
    glUniform3fvARB(this->parameterHandles[p], 1, (float*) &val);

    n_gltrace("nGLSLShader::SetVector3().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetFloat4(nShaderState::Param p, const nFloat4& val)
{
    n_assert(p < nShaderState::NumParameters);

    //n_printf("SetFloat4: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    glUniform4fvARB(this->parameterHandles[p], 1, (float*) &val);

    n_gltrace("nGLSLShader::SetFloat4().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetFloat4Array(nShaderState::Param p, const nFloat4* arr, int count)
{
    n_assert(p < nShaderState::NumParameters);

    // TODO: check "(float*) arr" expression correctness
    glUniform4fvARB(this->parameterHandles[p], count, (float*) arr);

    n_gltrace("nGLSLShader::SetFloat4Array().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetVector4Array(nShaderState::Param p, const vector4* arr, int count)
{
    n_assert(p < nShaderState::NumParameters);

    // TODO: check "(float*) arr" expression correctness
    glUniform4fvARB(this->parameterHandles[p], count, (float*) arr);

    n_gltrace("nGLSLShader::SetVector4Array().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetMatrix(nShaderState::Param p, const matrix44& val)
{
    n_assert(p < nShaderState::NumParameters);

    //GLint pp = glGetUniformLocationARB(this->programObj, nShaderState::ParamToString(p));

    //n_printf("SetMatrix: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(&val));
    glUniformMatrix4fvARB(this->parameterHandles[p], 1, GL_FALSE, (const GLfloat*)val.m);

    nString msg;
    msg.Format("nGLSLShader(%s)::SetMatrix(%s:%d).", this->GetFilename().Get(), nShaderState::ParamToString(p), this->parameterHandles[p]);
    //n_gltrace(msg.Get());
    n_glsltrace(this->programObj, msg.Get());

    //printf("%s\n", msg.Get());

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetMatrixArray(nShaderState::Param p, const matrix44* arr, int count)
{
    n_assert(p < nShaderState::NumParameters);

    // TODO: check "(float*) arr" expression correctness
    glUniformMatrix4fvARB(this->parameterHandles[p], count, GL_FALSE, (float*) arr);

    n_gltrace("nGLSLShader::SetMatrixArray().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetMatrixPointerArray(nShaderState::Param p, const matrix44** arr, int count)
{
    n_assert(p < nShaderState::NumParameters);

    // TODO: check "(float*) *arr" expression correctness
    glUniformMatrix4fvARB(this->parameterHandles[p], count, GL_FALSE, (float*) *arr);

    n_gltrace("nGLSLShader::SetMatrixPointerArray().");

#ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetTexture(nShaderState::Param p, nTexture2* tex)
{
    n_assert(tex);
    n_assert(p < nShaderState::NumParameters);

    //n_printf("SetTexture: %s\n", nShaderState::ParamToString(p));

    if ((!this->curParams.IsParameterValid(p)) ||
        (this->curParams.IsParameterValid(p) && (this->curParams.GetArg(p).GetTexture() != tex)))
    {
        this->curParams.SetArg(p, nShaderArg(tex));
        glUniform1iARB(this->parameterHandles[p], (GLint)((nGLTexture*)tex)->GetTexID());

        n_gltrace("nGLSLShader::SetTexture().");

#ifdef __NEBULA_STATS__
        //this->refGfxServer->statsNumTextureChanges++;
#endif
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::BeginParamUpdate()
{
    glUseProgramObjectARB(this->programObj);
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::EndParamUpdate()
{
    glUseProgramObjectARB(0);
}

//------------------------------------------------------------------------------
/**
Set a whole shader parameter block at once. This is slightly faster
(and more convenient) then setting single parameters.
*/
void
nGLSLShader::SetParams(const nShaderParams& params)
{
#ifdef __NEBULA_STATS__
    //nGLServer2* gfxServer = this->refGfxServer.get();
#endif
    int numValidParams = params.GetNumValidParams();
    int i;

    glUseProgramObjectARB(this->programObj);

    for (i = 0; i < numValidParams; i++)
    {
        nShaderState::Param curParam = params.GetParamByIndex(i);

        // parameter used in shader?
        GLint handle = this->parameterHandles[curParam];
        if (handle != -1)
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
#ifdef __NEBULA_STATS__
                    //gfxServer->statsNumRenderStateChanges++;
#endif
                    break;

                case nShaderState::Int:
                    glUniform1iARB(handle, curArg.GetInt());
#ifdef __NEBULA_STATS__
                    //gfxServer->statsNumRenderStateChanges++;
#endif
                    break;

                case nShaderState::Float:
                    glUniform1fARB(handle, curArg.GetFloat());
#ifdef __NEBULA_STATS__
                    //gfxServer->statsNumRenderStateChanges++;
#endif
                    break;

                case nShaderState::Float4:
                    glUniform4fvARB(handle, 1, (float*) &(curArg.GetFloat4()));
#ifdef __NEBULA_STATS__
                    //gfxServer->statsNumRenderStateChanges++;
#endif
                    break;

                case nShaderState::Matrix44:
                    glUniformMatrix4fvARB(handle, 1, GL_FALSE, (float*) curArg.GetMatrix44());
#ifdef __NEBULA_STATS__
                    //gfxServer->statsNumRenderStateChanges++;
#endif
                    break;

                case nShaderState::Texture:
                    glUniform1iARB(handle, (GLint)((nGLTexture*)curArg.GetTexture())->GetTexID());
#ifdef __NEBULA_STATS__
                    //gfxServer->statsNumTextureChanges++;
#endif
                    break;
                }
                //n_error("nGLSLShader::SetParams(): Error while setting parameter.");
            }
        }
    }

    glUseProgramObjectARB(0);

    //n_gltrace("nGLSLShader::SetParams().");
    n_glsltrace(this->programObj, "nGLSLShader::SetParams().");
}

//------------------------------------------------------------------------------
/**
Update the parameter handles table which maps nShader2 parameters to
GLSL parameter handles.
*/
void
nGLSLShader::UpdateParameterHandles()
{
    //if (uniformsUpdated) return;

    memset(this->parameterHandles, -1, sizeof(this->parameterHandles));

    n_printf("Start parameters lookup\n");

    // TODO: trace all parameters in shaders, not only nebula ones
    int i, n;
    GLcharARB* uniformName;
    GLsizei maxLen, len;
    GLint sz;
    GLenum tp;

    glUseProgramObjectARB(this->programObj);

    glGetObjectParameterivARB(this->programObj, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &n);
    glGetObjectParameterivARB(this->programObj, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLen);

    if (n > 0)
    {
        n_printf("  Shader contain:");
        uniformName = n_new_array(GLcharARB, maxLen);
        for (i = 0; i < n; i++)
        {
            glGetActiveUniformARB(this->programObj, i, maxLen, &len, &sz, &tp, uniformName);
            n_printf(" %s", uniformName);
        }
        n_delete_array(uniformName);
        n_printf("\n");
        
        GLint p;
        const char* pname;
        for (i = 0; i < nShaderState::NumParameters; i++)
        {
            pname = nShaderState::ParamToString((nShaderState::Param)i);
            p = glGetUniformLocationARB(this->programObj, pname);

            if (p != -1)
            {
                n_printf("  %4d: %s\n", p, pname);
            }

            //n_printf("  Found: %s = %d\n", pname, p);
            this->parameterHandles[i] = p;
        }
    }
    else
    {
        n_printf("  There are no parameters in this shader.\n");
    }

    glUseProgramObjectARB(0);

    n_printf("End parameters lookup\n");

    //uniformsUpdated = true;

    //n_gltrace("nGLSLShader::UpdateParameterHandles().");
    n_glsltrace(this->programObj, "nGLSLShader::UpdateParameterHandles().");
}

//------------------------------------------------------------------------------
/**
Return true if parameter is used by effect.
*/
bool
nGLSLShader::IsParameterUsed(nShaderState::Param p)
{
    n_assert(p < nShaderState::NumParameters);
    return (0 <= this->parameterHandles[p]);
}

//------------------------------------------------------------------------------
/**
Find the first valid technique and set it as current.
This sets the hasBeenValidated and didNotValidate members
*/
void
nGLSLShader::ValidateEffect()
{
    n_assert(!this->hasBeenValidated);
    //n_assert(deviceInit);

    //int res;

    this->hasBeenValidated = true;
    this->didNotValidate = true;

    glValidateProgramARB(this->programObj);
    n_assert_glslstatus(this->programObj, "nGLSLShader() warning: shader did not validated!", GL_OBJECT_VALIDATE_STATUS_ARB);

    //this->hasBeenValidated = true;
    this->didNotValidate = false;
    //this->UpdateParameterHandles();

    //n_gltrace("nGLSLShader::ValidateEffect().");
    n_glsltrace(this->programObj, "nGLSLShader::ValidateEffect().");
}

//------------------------------------------------------------------------------
/**
begin shader rendering
*/
int
nGLSLShader::Begin(bool saveState)
{
    // check if we already have been validated, if not, find the first
    // valid technique and set it as current
    if (!this->hasBeenValidated)
    {
        this->ValidateEffect();
    }

    if (this->didNotValidate)
    {
        //TODO: FIXME - validation not working!
        //return 0;
        return 1;
    }
    else
    {
        glUseProgramObjectARB(this->programObj);
        //this->UpdateParameterHandles();
        return 1;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::BeginPass(int pass)
{
    n_gltrace("nGLSLShader::BeginPass().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::CommitChanges()
{
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::EndPass()
{
}

//------------------------------------------------------------------------------
/**
stop shader rendering
*/
void
nGLSLShader::End()
{
    if (!this->didNotValidate)
    {
        glUseProgramObjectARB(0);
        n_gltrace("nGLSLShader::End().");
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nGLSLShader::HasTechnique(const char* t) const
{
    //n_assert(t);
    //n_assert(this->effect);
    //D3DXHANDLE h = this->effect->GetTechniqueByName(t);
    //return (0 != h);

    // TODO: add checking technique
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGLSLShader::SetTechnique(const char* t)
{
    //n_assert(t);
    //n_assert(this->effect);
    //
    //// get handle to technique
    //D3DXHANDLE hTechnique = this->effect->GetTechniqueByName(t);
    //if (0 == hTechnique)
    //{
    //    n_error("nD3D9Shader::SetTechnique(%s): technique not found in shader file %s!\n", t, this->GetFilename());
    //    return false;
    //}

    //// check if technique needs software vertex processing (this is the
    //// case if the 3d device is a mixed vertex processing device, and 
    //// the current technique includes a vertex shader
    //this->curTechniqueNeedsSoftwareVertexProcessing = false;
    //if (nGfxServer2::Instance()->AreVertexShadersEmulated())
    //{
    //    D3DXHANDLE hPass = this->effect->GetPass(hTechnique, 0);
    //    n_assert(0 != hPass);
    //    D3DXPASS_DESC passDesc = { 0 };
    //    HRESULT hr = this->effect->GetPassDesc(hPass, &passDesc);
    //    n_assert(SUCCEEDED(hr));
    //    if (passDesc.pVertexShaderFunction)
    //    {
    //        this->curTechniqueNeedsSoftwareVertexProcessing = true;
    //    }
    //}

    //// finally, set the technique
    //HRESULT hr = this->effect->SetTechnique(hTechnique);
    //if (FAILED(hr))
    //{
    //    n_printf("nD3D9Shader::SetTechnique(%s) on shader %s failed!\n", t, this->GetFilename());
    //    return false;
    //}

    //return true;

    // TODO: add setting technique
    return false;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGLSLShader::GetTechnique() const
{
    //n_assert(this->effect);
    //return this->effect->GetCurrentTechnique();

    // TODO: add getting technique
    return NULL;
}

//------------------------------------------------------------------------------
/**
This method is called when the gl device is lost.
*/
void
nGLSLShader::OnLost()
{
    n_assert(Lost != this->GetState());
    //n_assert(this->effect);
    //this->effect->OnLostDevice();
    this->SetState(Lost);

    // flush my current parameters (important! otherwise, seemingly redundant
    // state will not be set after OnRestore())!
    this->curParams.Clear();
}

//------------------------------------------------------------------------------
/**
This method is called when the gl device has been restored.
*/
void
nGLSLShader::OnRestored()
{
    n_assert(Lost == this->GetState());
    //n_assert(this->effect);
    //this->effect->OnResetDevice();
    this->SetState(Valid);
}
