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
    /// create or append an instance stream declaration for this shader
    virtual int UpdateInstanceStreamDecl(nInstanceStream::Declaration& decl);
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

    /// begin applying the shader, returns number of passes
    virtual int Begin(bool saveState);
#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    /// begin a render pass
    virtual void BeginPass(int pass);
    /// end a render pass
    virtual void EndPass();
#else
    /// render a pass
    virtual void Pass(int pass);
#endif
    /// finish applying the shader
    virtual void End();

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
    /// convert a D3DX parameter handle to a nShaderState parameter
    nShaderState::Param D3DXParamToShaderStateParam(D3DXHANDLE h);

    friend class nD3D9Server;

    nAutoRef<nD3D9Server> refGfxServer;
    ID3DXEffect* effect;
    bool hasBeenValidated;
    bool didNotValidate;
    D3DXHANDLE parameterHandles[nShaderState::NumParameters];     // map shader states to D3DX handles
    nShaderParams curParams;    // mirrored to avoid redundant parameters setting

#if (D3D_SDK_VERSION >= 32) //summer 2004 update sdk
    bool inBeginPass;
#endif
};

//------------------------------------------------------------------------------
#endif

