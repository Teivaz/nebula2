#ifndef N_SHADER2_H
#define N_SHADER2_H
//------------------------------------------------------------------------------
/**
    A shader object loads itself from a shader resource file, and contains
    everything to render a mesh and texture. It may be completely
    render state based, use vertex and pixel shader programs, or both.
    Shaders usually use a 3rd party subsystem, like D3DX effects, or CgFX.
    This is done by subclasses the nShader2 class.

    How the shader is rendered is totally up to the gfx server.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_RESOURCE_H
#include "resource/nresource.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_VARIABLE_H
#include "variable/nvariable.h"
#endif

#undef N_DEFINES
#define N_DEFINES nD3D8Mesh2
#include "kernel/ndefdllclass.h"

class nTexture2;
class nVariableServer;

//------------------------------------------------------------------------------
class N_PUBLIC nShader2 : public nResource
{
public:
    /// constructor
    nShader2();
    /// destructor
    virtual ~nShader2();
    /// set int parameter
    virtual void SetInt(nVariable::Handle h, int val);
    /// set int[] parameter
    virtual void SetIntArray(nVariable::Handle h, const int* array, int count);
    /// set float parameter
    virtual void SetFloat(nVariable::Handle h, float val);
    /// set float[] parameter
    virtual void SetFloatArray(nVariable::Handle h, const float* array, int count);
    /// set vector parameter
    virtual void SetVector(nVariable::Handle h, const float4& val);
    /// set vector[] parameter
    virtual void SetVectorArray(nVariable::Handle h, const float4* array, int count);
    /// set matrix parameter
    virtual void SetMatrix(nVariable::Handle h, const matrix44& val);
    /// set matrix array parameter
    virtual void SetMatrixArray(nVariable::Handle h, const matrix44* array, int count);
    /// set matrix pointer array parameter
    virtual void SetMatrixPointerArray(nVariable::Handle h, const matrix44** array, int count);
    /// set texture parameter
    virtual void SetTexture(nVariable::Handle h, nTexture2* tex);
    /// return true if parameter is used
    virtual bool IsParameterUsed(nVariable::Handle h);
    /// begin applying the shader, returns number of passes
    virtual int Begin();
    /// render a pass
    virtual void Pass(int pass);
    /// finish applying the shader
    virtual void End();
 
    static nKernelServer* kernelServer;

protected:
    nAutoRef<nVariableServer> refVariableServer;
};
//------------------------------------------------------------------------------
#endif
