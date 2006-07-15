#ifndef N_CGFXSHADER_H
#define N_CGFXSHADER_H
//------------------------------------------------------------------------------
/**
	@class nCgFXShader
	@ingroup OpenGL

    A nShader2 subclass using the CgFX framework.

    13-Dec-2003  Haron  created
	27-Mar-2004  Haron  CgFX 1.2 support added
*/
#include "gfx2/nshader2.h"
#include "gfx2/nshaderparams.h"
#include <CgFX/ICgFXEffect.h>

class nGLServer2;

//------------------------------------------------------------------------------
class nCgFXShader : public nShader2
{
public:
    /// constructor
    nCgFXShader();
    /// destructor
    virtual ~nCgFXShader();
	
	/// is parameter used by effect?
    virtual bool IsParameterUsed(nShaderState::Param p);
    /// set bool parameter
    virtual void SetBool(nShaderState::Param p, bool val);
    /// set int parameter
    virtual void SetInt(nShaderState::Param p, int val);
    /// set float parameter
    virtual void SetFloat(nShaderState::Param p, float val);
    /// set vector4 parameter
    virtual void SetVector4(nShaderState::Param p, const vector4& val);
    /// set vector3 parameter
    virtual void SetVector3(nShaderState::Param p, const vector3& val);
    /// set float4 parameter
    virtual void SetFloat4(nShaderState::Param p, const nFloat4& val);
    /// set matrix parameter
    virtual void SetMatrix(nShaderState::Param p, const matrix44& val);
    /// set texture parameter
    virtual void SetTexture(nShaderState::Param p, nTexture2* tex);

    /// set int[] parameter
    virtual void SetBoolArray(nShaderState::Param p, const bool* array, int count);
    /// set int[] parameter
    virtual void SetIntArray(nShaderState::Param p, const int* array, int count);
    /// set float[] parameter
    virtual void SetFloatArray(nShaderState::Param p, const float* array, int count);
    /// set vector[] parameter
    virtual void SetFloat4Array(nShaderState::Param p, const nFloat4* array, int count);
    /// set vector4[] parameter
    virtual void SetVector4Array(nShaderState::Param p, const vector4* array, int count);
    /// set matrix array parameter
    virtual void SetMatrixArray(nShaderState::Param p, const matrix44* array, int count);
    /// set matrix pointer array parameter
    virtual void SetMatrixPointerArray(nShaderState::Param p, const matrix44** array, int count);
    /// set a whole shader parameter block at once
    virtual void SetParams(const nShaderParams& params);

    /// begin applying the shader, returns number of passes
    virtual int Begin(bool saveState);
    /// begin a pass
    virtual void BeginPass(int pass);
    /// commit changes during pass before rendering
    virtual void CommitChanges();
    /// end a pass
    virtual void EndPass();
    /// finish applying the shader
    virtual void End();

protected:
    /// load the shader resource file
    virtual bool LoadResource();
    /// unload shader resources
    virtual void UnloadResource();

protected:
    /// find the first valid technique and make current
    void ValidateEffect();
    /// update the parameter handle mapper table
    void UpdateParameterHandles();

	ICgFXEffect *effect;						///< pointer to shader effect object
    bool hasBeenValidated;
    bool didNotValidate;
    CGFXHANDLE parameterHandles[nShaderState::NumParameters]; ///< map shader states to CgFX handles
    nShaderParams curParams;					///< mirrored to avoid redundant parameters setting

	friend class nGLServer2;

	/// singletone variables and functions for CgFX device initialization
	static bool deviceInit;	///< initialization flag
	static CgFXMode mode;	///< one of CgFX modes: CgFX_Unknown, CgFX_OpenGL, CgFX_Direct3D8, CgFX_Direct3D9
	static bool CreateDevice();
	static bool ReleaseDevice();
	static bool IsDeviceInit();
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCgFXShader::IsDeviceInit()
{
	return deviceInit;
}
//------------------------------------------------------------------------------
#endif

