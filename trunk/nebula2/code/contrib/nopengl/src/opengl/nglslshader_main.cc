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

//#include "util/nstrlist.h"

nNebulaClass(nGLSLShader, "nshader2");

//------------------------------------------------------------------------------
/** OGRE example
    void checkForGLSLError(const String& ogreMethod, const String& errorTextPrefix, const GLhandleARB obj, const bool forceInfoLog, const bool forceException)
    {
		GLenum glErr;
		bool errorsFound = false;
		String msg = errorTextPrefix;

		// get all the GL errors
		glErr = glGetError();
		while (glErr != GL_NO_ERROR)
        {
			msg += "\n" + String((char*)gluErrorString(glErr)); 
			glErr = glGetError();
			errorsFound = true;
        }


		// if errors were found then put them in the Log and raise and exception
		if (errorsFound || forceInfoLog)
		{
			// if shader or program object then get the log message and send to the log manager
			msg += logObjectInfo( msg, obj );

            if (forceException) 
			{
				OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, msg, ogreMethod);
			}
		}
    }

    //-----------------------------------------------------------------------------
	String logObjectInfo(const String& msg, const GLhandleARB obj)
	{
		String logMessage = msg;

		if (obj > 0)
		{
			int infologLength = 0;

			glGetObjectParameterivARB_ptr(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

			if (infologLength > 0)
			{
				int charsWritten  = 0;

				GLcharARB * infoLog = new GLcharARB[infologLength];

				glGetInfoLogARB_ptr(obj, infologLength, &charsWritten, infoLog);
				logMessage += String(infoLog) + "\n";
				LogManager::getSingleton().logMessage(logMessage);

				delete [] infoLog;
			}
		}

		return logMessage;

	}
*/
void
n_glsltrace(const nString& msg, const GLhandleARB obj)
{
}

//------------------------------------------------------------------------------
/**
*/
nGLSLShader::nGLSLShader() :
    hasBeenValidated(false),
    didNotValidate(false),
    programObj(0),
    vertexShader(0),
    fragmentShader(0)
{
    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));
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
    glDeleteObjectARB(this->vertexShader);
    glDeleteObjectARB(this->fragmentShader);
    glDeleteObjectARB(this->programObj);

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
    int res;
    char str[4096];
    const char *shaderStrings[1];

    fname.StripExtension();

    n_printf("Start shader loading...\n");

    this->programObj = glCreateProgramObjectARB(); 

    // attach vertex shader
    if (si.Begin(fname + ".vert"))
    {
        si.GetSource(src);
        shaderStrings[0] = src.Get();

        this->vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        glShaderSourceARB(this->vertexShader, 1, shaderStrings, NULL);
        glCompileShaderARB(this->vertexShader);
        glGetObjectParameterivARB(this->vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &res); 

        if(res)
        {
            glAttachObjectARB(this->programObj, this->vertexShader);
        }
	    else
	    {
		    glGetInfoLogARB(this->vertexShader, sizeof(str), NULL, str);
            n_error("nGLSLShader::LoadResource(%s): Vertex Shader Compile Error.\n\t%s",
                fname.Get(), str);
	    }
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
        glGetObjectParameterivARB(this->fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &res); 

        if(res)
        {
            glAttachObjectARB(this->programObj, this->fragmentShader);
        }
	    else
	    {
		    glGetInfoLogARB(this->fragmentShader, sizeof(str), NULL, str);
            n_error("nGLSLShader::LoadResource(%s): Fragment Shader Compile Error.\n\t%s",
                    fname.Get(), str);
	    }
        si.End();
    }

    glLinkProgramARB(this->programObj);
    glGetObjectParameterivARB(this->programObj, GL_OBJECT_LINK_STATUS_ARB, &res);

    if(res == GL_FALSE)
	{
		glGetInfoLogARB(this->programObj, sizeof(str), NULL, str);
        n_error("nGLSLShader::LoadResource(%s): Linking Error.\n\t%s", fname.Get(), str);
	}

    // success
    this->hasBeenValidated = false;
    this->didNotValidate = false;
    this->SetState(Valid);

    // validate the effect
    this->ValidateEffect();

    src.Set("nGLSLShader::LoadResource(");
    src.Append(fname);
    src.Append(").");
    n_gltrace(src.Get());
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetBool(nShaderState::Param p, bool val)
{
    n_assert(p < nShaderState::NumParameters);
    n_printf("SetBool: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    glUniform1iARB(this->parameterHandles[p], val ? 1 : 0);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetBool().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetBoolArray(nShaderState::Param p, const bool* array, int count)
{
    n_assert(p < nShaderState::NumParameters);
    GLint* ba;
    ba = n_new_array(GLint, count);

    int i;
    for (i = 0; i < count; i++)
        if (array[i]) ba[i] = 1;
        else ba[i] = 0;
    glUniform1ivARB(this->parameterHandles[p], count, ba);
    n_delete_array(ba);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetBoolArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetInt(nShaderState::Param p, int val)
{
    n_assert(p < nShaderState::NumParameters);
    n_printf("SetInt: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    glUniform1iARB(this->parameterHandles[p], val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetInt().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetIntArray(nShaderState::Param p, const int* array, int count)
{
    n_assert(p < nShaderState::NumParameters);
    glUniform1ivARB(this->parameterHandles[p], count, array);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetIntArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetFloat(nShaderState::Param p, float val)
{
    n_assert(p < nShaderState::NumParameters);
    n_printf("SetFloat: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    glUniform1fARB(this->parameterHandles[p], val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetFloat().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetFloatArray(nShaderState::Param p, const float* array, int count)
{
    n_assert(p < nShaderState::NumParameters);
    glUniform1fvARB(this->parameterHandles[p], count, array);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetFloatArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetVector4(nShaderState::Param p, const vector4& val)
{
    n_assert(p < nShaderState::NumParameters);
    n_printf("SetVector4: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&val));
    glUniform4fvARB(this->parameterHandles[p], 1, (float*) &val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetVector4().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetVector3(nShaderState::Param p, const vector3& val)
{
    n_assert(p < nShaderState::NumParameters);
    n_printf("SetVector3: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(*(nFloat4*)&val));
    glUniform3fvARB(this->parameterHandles[p], 1, (float*) &val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetVector3().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetFloat4(nShaderState::Param p, const nFloat4& val)
{
    n_assert(p < nShaderState::NumParameters);
    n_printf("SetFloat4: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(val));
    glUniform4fvARB(this->parameterHandles[p], 1, (float*) &val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetFloat4().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetFloat4Array(nShaderState::Param p, const nFloat4* array, int count)
{
    n_assert(p < nShaderState::NumParameters);
    glUniform4fvARB(this->parameterHandles[p], count, (float*) array);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetFloat4Array().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetVector4Array(nShaderState::Param p, const vector4* array, int count)
{
    n_assert(p < nShaderState::NumParameters);
    glUniform4fvARB(this->parameterHandles[p], count, (float*) array);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetVector4Array().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetMatrix(nShaderState::Param p, const matrix44& val)
{
    n_assert(p < nShaderState::NumParameters);
    n_printf("SetMatrix: %s\n", nShaderState::ParamToString(p));
    this->curParams.SetArg(p, nShaderArg(&val));
    glUniformMatrix4fvARB(this->parameterHandles[p], 1, GL_FALSE, (float*) &val);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetMatrix().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetMatrixArray(nShaderState::Param p, const matrix44* array, int count)
{
    n_assert(p < nShaderState::NumParameters);
    glUniformMatrix4fvARB(this->parameterHandles[p], count, GL_FALSE, (float*) array);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetMatrixArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetMatrixPointerArray(nShaderState::Param p, const matrix44** array, int count)
{
    n_assert(p < nShaderState::NumParameters);
    glUniformMatrix4fvARB(this->parameterHandles[p], count, GL_FALSE, (float*) *array);
    #ifdef __NEBULA_STATS__
    //this->refGfxServer->statsNumRenderStateChanges++;
    #endif
    n_gltrace("nGLSLShader::SetMatrixPointerArray().");
}

//------------------------------------------------------------------------------
/**
*/
void
nGLSLShader::SetTexture(nShaderState::Param p, nTexture2* tex)
{
    n_assert(tex);
    n_assert(p < nShaderState::NumParameters);
    n_printf("SetTexture: %s\n", nShaderState::ParamToString(p));
    if ((!this->curParams.IsParameterValid(p)) ||
        (this->curParams.IsParameterValid(p) && (this->curParams.GetArg(p).GetTexture() != tex)))
    {
        this->curParams.SetArg(p, nShaderArg(tex));
        glUniform1iARB(this->parameterHandles[p], (GLint)((nGLTexture*)tex)->GetTexID());
        #ifdef __NEBULA_STATS__
        //this->refGfxServer->statsNumTextureChanges++;
        #endif
        n_gltrace("nGLSLShader::SetTexture().");
    }
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
    int i;
    int numValidParams = params.GetNumValidParams();

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
    n_gltrace("nGLSLShader::SetParams().");
}

//------------------------------------------------------------------------------
/**
    Update the parameter handles table which maps nShader2 parameters to
    GLSL parameter handles.
*/
void
nGLSLShader::UpdateParameterHandles()
{
    memset(this->parameterHandles, 0, sizeof(this->parameterHandles));

    n_printf("Start parameters lookup for shader <%s>...\n", this->GetFilename().Get());

    // TODO: trace all parameters in shaders, not only nebula ones
    GLint p;
    const char* pname;
    int i;
    for (i = 0; i < nShaderState::NumParameters; i++)
    {
        pname = nShaderState::ParamToString((nShaderState::Param)i);
        p = glGetUniformLocationARB(this->programObj, pname);
        if (p != -1)
        {
            n_printf("\tParam <%s>\n", pname);
        }
        this->parameterHandles[i] = p;

    }
    n_printf("End parameters lookup\n");

    n_gltrace("nGLSLShader::UpdateParameterHandles().");
}

//------------------------------------------------------------------------------
/**
    Return true if parameter is used by effect.
*/
bool
nGLSLShader::IsParameterUsed(nShaderState::Param p)
{
    n_assert(p < nShaderState::NumParameters);
    return (-1 != this->parameterHandles[p]);
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
    int res;

    glValidateProgramARB(this->programObj);
    glGetProgramivARB(this->programObj, GL_VALIDATE_STATUS, &res);
    if (res == GL_TRUE)
    {
        this->hasBeenValidated = true;
        this->didNotValidate = false;
        this->UpdateParameterHandles();
    }
    else
    {
        n_printf("nGLSLShader() warning: shader '%s' did not validated!\n", this->GetFilename().Get());
        this->hasBeenValidated = true;
        this->didNotValidate = true;
    }
    n_gltrace("nGLSLShader::ValidateEffect().");
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
        glUseProgramObjectARB(NULL);
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
