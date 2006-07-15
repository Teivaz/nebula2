#ifndef N_SHADER2_H
#define N_SHADER2_H
//------------------------------------------------------------------------------
/**
    @class nShader2
    @ingroup Gfx2

    A shader object loads itself from a shader resource file, and contains
    everything to render a mesh and texture. It may be completely
    render state based, use vertex and pixel shader programs, or both.
    Shaders usually use a 3rd party subsystem, like D3DX effects, or CgFX.
    This is done by subclasses of the nShader2 class.

    How the shader is rendered is totally up to the gfx server.

    For the sake of efficiency, shader parameters are now enums with
    associated string names. The use of enums allows to do parameter lookup
    as simple indexed array lookup. The disadvantage is of course, that
    new shader states require this file to be extended or replaced.

    (C) 2002 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "mathlib/matrix.h"
#include "variable/nvariable.h"
#include "gfx2/nshaderstate.h"
#include "gfx2/nshaderparams.h"
#include "gfx2/ninstancestream.h"

class nTexture2;
class nVariableServer;

//------------------------------------------------------------------------------
class nShader2 : public nResource
{
public:
    /// constructor
    nShader2();
    /// destructor
    virtual ~nShader2();

    /// return true if technique exists in shader
    virtual bool HasTechnique(const char* t) const;
    /// set a technique
    virtual bool SetTechnique(const char* t);
    /// get current technique
    virtual const char* GetTechnique() const;
    /// create or append an instance stream declaration for this shader
    virtual int UpdateInstanceStreamDecl(nInstanceStream::Declaration& decl);

    /// is parameter used by effect?
    virtual bool IsParameterUsed(nShaderState::Param p);
    /// set bool parameter
    virtual void SetBool(nShaderState::Param p, bool val);
    /// set int parameter
    virtual void SetInt(nShaderState::Param p, int val);
    /// set float parameter
    virtual void SetFloat(nShaderState::Param p, float val);
    /// set vector parameter as vector4 (fastest)
    virtual void SetVector4(nShaderState::Param p, const vector4& val);
    /// set vector parameter as vector3
    virtual void SetVector3(nShaderState::Param p, const vector3& val);
    /// set vector parameter as nFloat4
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
    /// set float4[] parameter
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
};

//------------------------------------------------------------------------------
#endif
