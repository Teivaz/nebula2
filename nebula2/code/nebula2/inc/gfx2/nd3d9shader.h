#ifndef N_D3D9SHADER_H
#define N_D3D9SHADER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Shader
    @ingroup NebulaD3D9GraphicsSystem

    A nShader2 subclass using the D3D9FX framework.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_SHADER2_H
#include "gfx2/nshader2.h"
#endif

#ifndef N_VARIABLECONTEXT_H
#include "variable/nvariablecontext.h"
#endif

#include <d3dx9.h>

#undef N_DEFINES
#define N_DEFINES nD3D9Shader
#include "kernel/ndefdllclass.h"

class nD3D9Server;

//------------------------------------------------------------------------------
class nD3D9Shader : public nShader2
{
public:
    /// constructor
    nD3D9Shader();
    /// destructor
    virtual ~nD3D9Shader();
    /// load the shader resource file
    virtual bool Load();
    /// unload shader resources
    virtual void Unload();
    /// set uniform int variable
    virtual void SetInt(nVariable::Handle h, int val);
    /// set int[] parameter
    virtual void SetIntArray(nVariable::Handle h, const int* array, int count);
    /// set uniform float variable
    virtual void SetFloat(nVariable::Handle h, float val);
    /// set float[] parameter
    virtual void SetFloatArray(nVariable::Handle h, const float* array, int count);
    /// set a uniform vector variable
    virtual void SetVector(nVariable::Handle h, const float4& val);
    /// set vector[] parameter
    virtual void SetVectorArray(nVariable::Handle h, const float4* array, int count);
    /// set uniform matrix variable
    virtual void SetMatrix(nVariable::Handle h, const matrix44& val);
    /// set matrix array parameter
    virtual void SetMatrixArray(nVariable::Handle h, const matrix44* array, int count);
    /// set matrix pointer array parameter
    virtual void SetMatrixPointerArray(nVariable::Handle h, const matrix44** array, int count);
    /// set texture variable
    virtual void SetTexture(nVariable::Handle h, nTexture2* tex);
    /// return true if variable is used
    virtual bool IsParameterUsed(nVariable::Handle h);
    /// begin applying the shader, returns number of passes
    virtual int Begin();
    /// render a pass
    virtual void Pass(int pass);
    /// finish applying the shader
    virtual void End();

    static nKernelServer* kernelServer;

private:
    /// find the first valid technique and make current
    void ValidateEffect();

    friend class nD3D9Server;

    nAutoRef<nD3D9Server> refGfxServer;
    nVariableContext varContext;
    ID3DXEffect* effect;
    bool hasBeenValidated;
    bool didNotValidate;
};

//------------------------------------------------------------------------------
#endif

