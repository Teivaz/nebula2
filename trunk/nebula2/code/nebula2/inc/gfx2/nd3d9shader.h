#ifndef N_D3D9SHADER_H
#define N_D3D9SHADER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Shader
    @ingroup NebulaD3D9GraphicsSystem

    A nShader2 subclass using the D3D9FX framework.

    (C) 2003 RadonLabs GmbH
*/
#include "gfx2/nshader2.h"
#include "gfx2/nshaderparams.h"
#include <d3dx9.h>

class nD3D9Server;

//------------------------------------------------------------------------------
class nD3D9Shader : public nShader2
{
public:
    /// constructor
    nD3D9Shader();
    /// destructor
    virtual ~nD3D9Shader();
    
	/// is parameter used by effect?
    virtual bool IsParameterUsed(Parameter p);
    /// set bool parameter
    virtual void SetBool(Parameter p, bool val);
    /// set int parameter
    virtual void SetInt(Parameter p, int val);
    /// set float parameter
    virtual void SetFloat(Parameter p, float val);
    /// set vector4 parameter
    virtual void SetVector4(Parameter p, const vector4& val);
    /// set vector3 parameter
    virtual void SetVector3(Parameter p, const vector3& val);
    /// set float4 parameter
    virtual void SetFloat4(Parameter p, const nFloat4& val);
    /// set matrix parameter
    virtual void SetMatrix(Parameter p, const matrix44& val);
    /// set texture parameter
    virtual void SetTexture(Parameter p, nTexture2* tex);

    /// set int[] parameter
    virtual void SetBoolArray(Parameter p, const bool* array, int count);
    /// set int[] parameter
    virtual void SetIntArray(Parameter p, const int* array, int count);
    /// set float[] parameter
    virtual void SetFloatArray(Parameter p, const float* array, int count);
    /// set vector[] parameter
    virtual void SetFloat4Array(Parameter p, const nFloat4* array, int count);
    /// set vector4[] parameter
    virtual void SetVector4Array(Parameter p, const vector4* array, int count);
    /// set matrix array parameter
    virtual void SetMatrixArray(Parameter p, const matrix44* array, int count);
    /// set matrix pointer array parameter
    virtual void SetMatrixPointerArray(Parameter p, const matrix44** array, int count);
    /// set a whole shader parameter block at once
    virtual void SetParams(const nShaderParams& params);

    /// begin applying the shader, returns number of passes
    virtual int Begin();
    /// render a pass
    virtual void Pass(int pass);
    /// finish applying the shader
    virtual void End();

    static nKernelServer* kernelServer;

protected:
    /// load the shader resource file
    virtual bool LoadResource();
    /// unload shader resources
    virtual void UnloadResource();

private:
    /// find the first valid technique and make current
    void ValidateEffect();
    /// update the parameter handle mapper table
    void UpdateParameterHandles();

    friend class nD3D9Server;

    nAutoRef<nD3D9Server> refGfxServer;
    ID3DXEffect* effect;
    bool hasBeenValidated;
    bool didNotValidate;
    D3DXHANDLE parameterHandles[NumParameters];     // map shader states to D3DX handles
    nShaderParams curParams;    // mirrored to avoid redundant parameters setting
};

//------------------------------------------------------------------------------
#endif

