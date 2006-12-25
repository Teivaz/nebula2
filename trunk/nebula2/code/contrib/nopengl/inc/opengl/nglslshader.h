#ifndef N_GLSLSHADER_H
#define N_GLSLSHADER_H
//------------------------------------------------------------------------------
/**
	@class nGLSLShader
	@ingroup OpenGL

    A nShader2 subclass using the GLSL shaders.

    23-Mar-2005  Haron  created
*/
#include "gfx2/nshader2.h"
#include "gfx2/nshaderparams.h"

class nGLServer2;

//------------------------------------------------------------------------------
class nGLSLShader : public nShader2
{
public:
    /// constructor
    nGLSLShader();
    /// destructor
    virtual ~nGLSLShader();
    /// create or append an instance stream declaration for this shader
    //virtual int UpdateInstanceStreamDecl(nInstanceStream::Declaration& decl);
    /// return true if technique exists in shader
    virtual bool HasTechnique(const char* t) const;
    /// set a technique
    virtual bool SetTechnique(const char* t);
    /// get current technique
    virtual const char* GetTechnique() const;
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

    /// set bool[] parameter
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

    /// begin updating shader uniform parameters
    void BeginParamUpdate();
    /// finish updating uniforms
    void EndParamUpdate();

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

private:
    /// load the shader resource file
    virtual bool LoadResource();
    /// unload shader resources
    virtual void UnloadResource();
    /// called when contained resource may become lost 
    virtual void OnLost();
    /// called when contained resource may be restored
    virtual void OnRestored();
    /// find the first valid technique and make current
    void ValidateEffect();
    /// update the parameter handle mapper table
    void UpdateParameterHandles();

    enum GLSLShaderType
    {
        VERTEX,
        FRAGMENT
    };

    /// create GLSL specific shader
    uint CreateGLSLShader(GLSLShaderType type, const nString& path);

    /// put proper GL command to GL list
    bool ParsePassParam(const char* name, nString& val);

    friend class nGLServer2;

    uint programObj;
    uint vertShader;
    uint fragShader;

    bool hasBeenValidated;
    bool didNotValidate;

    int parameterHandles[nShaderState::NumParameters]; ///< map shader states to GL handles
    nShaderParams curParams;					       ///< mirrored to avoid redundant parameters setting
    //bool uniformsUpdated;                              ///< true if glsl uniform parameters IDs is up to date

    struct nGLTechnique
    {
        nArray<nString> passName;
        nArray<uint>    pass;                          ///< GLlist IDs to call with glCallList
    };

    nArray<nString>      techniqueName;
    nArray<nGLTechnique> technique;
    int                  activeTechniqueIdx;
};

//------------------------------------------------------------------------------
#endif
