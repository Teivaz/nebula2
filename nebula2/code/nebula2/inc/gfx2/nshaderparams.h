#ifndef N_SHADERPARAMS_H
#define N_SHADERPARAMS_H
//------------------------------------------------------------------------------
/**
    @class nShaderParams

    A container for shader parameters. A shader parameter block
    can be applied to a shader with one call (instead of issuing dozens
    of method calls to set parameters).
    
    Note that only simple datatypes (not arrays) can be kept in shader
    parameter blocks.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gfx2/nshader2.h"
#include "gfx2/nshaderarg.h"

//------------------------------------------------------------------------------
class nShaderParams
{
public:
    /// constructor
    nShaderParams();
    /// destructor
    ~nShaderParams();
    /// return true if parameter is valid
    bool IsParameterValid(nShader2::Parameter p) const;
    /// set a single parameter
    void SetArg(nShader2::Parameter p, const nShaderArg& arg);
    /// get a single parameter
    const nShaderArg& GetArg(nShader2::Parameter p) const;
    /// set int parameter
    void SetInt(nShader2::Parameter p, int val);
    /// get int parameter
    int GetInt(nShader2::Parameter p) const;
    /// set bool parameter
    void SetBool(nShader2::Parameter p, bool val);
    /// get bool parameter
    bool GetBool(nShader2::Parameter p) const;
    /// set float parameter
    void SetFloat(nShader2::Parameter p, float val);
    /// get float parameter
    float GetFloat(nShader2::Parameter p) const;
    /// set vector parameter
    void SetFloat4(nShader2::Parameter p, const nFloat4& val);
    /// get vector parameter
    const nFloat4& GetFloat4(nShader2::Parameter p) const;
    /// set matrix parameter
    void SetMatrix44(nShader2::Parameter p, const matrix44* val);
    /// get matrix parameter
    const matrix44* GetMatrix44(nShader2::Parameter p) const;
    /// set texture parameter
    void SetTexture(nShader2::Parameter p, nTexture2* tex);
    /// get texture parameter
    nTexture2* GetTexture(nShader2::Parameter p) const;
    /// convenience method: set vector parameter as vector4 (slower then SetFloat4())
    void SetVector4(nShader2::Parameter p, const vector4& v);
    /// convenience method: get vector parameter as vector4 (slower then GetFloat4())
    vector4 GetVector4(nShader2::Parameter p) const;
    /// reset all parameters
    void Reset();

private:
    bool valid[nShader2::NumParameters];
    nShaderArg args[nShader2::NumParameters];
};

//------------------------------------------------------------------------------
/**
*/
inline
nShaderParams::nShaderParams()
{
    memset(this->valid, 0, sizeof(this->valid));
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderParams::~nShaderParams()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShaderParams::IsParameterValid(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    return this->valid[p];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetArg(nShader2::Parameter p, const nShaderArg& arg)
{
    n_assert(p < nShader2::NumParameters);
    this->args[p] = arg;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nShaderArg&
nShaderParams::GetArg(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    return this->args[p];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetInt(nShader2::Parameter p, int val)
{
    n_assert(p < nShader2::NumParameters);
    this->valid[p] = true;
    this->args[p].SetInt(val);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShaderParams::GetInt(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    return this->args[p].GetInt();
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetBool(nShader2::Parameter p, bool val)
{
    n_assert(p < nShader2::NumParameters);
    this->valid[p] = true;
    this->args[p].SetBool(val);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShaderParams::GetBool(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    return this->args[p].GetBool();
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetFloat(nShader2::Parameter p, float val)
{
    n_assert(p < nShader2::NumParameters);
    this->valid[p] = true;
    this->args[p].SetFloat(val);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nShaderParams::GetFloat(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    return this->args[p].GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetFloat4(nShader2::Parameter p, const nFloat4& val)
{
    n_assert(p < nShader2::NumParameters);
    this->valid[p] = true;
    this->args[p].SetFloat4(val);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nFloat4&
nShaderParams::GetFloat4(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    return this->args[p].GetFloat4();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetMatrix44(nShader2::Parameter p, const matrix44* m)
{
    n_assert(p < nShader2::NumParameters);
    this->valid[p] = true;
    this->args[p].SetMatrix44(m);
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44*
nShaderParams::GetMatrix44(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    return this->args[p].GetMatrix44();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetTexture(nShader2::Parameter p, nTexture2* tex)
{
    n_assert(p < nShader2::NumParameters);
    this->valid[p] = true;
    this->args[p].SetTexture(tex);
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture2*
nShaderParams::GetTexture(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    return this->args[p].GetTexture();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::SetVector4(nShader2::Parameter p, const vector4& val)
{
    n_assert(p < nShader2::NumParameters);
    this->valid[p] = true;
    this->args[p].SetFloat4(*((nFloat4*)&val));
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nShaderParams::GetVector4(nShader2::Parameter p) const
{
    n_assert(p < nShader2::NumParameters);
    const nFloat4& f4 = this->args[p].GetFloat4();
    return vector4(f4.x, f4.y, f4.z, f4.w);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderParams::Reset()
{
    memset(this->valid, 0, sizeof(this->valid));
    for(int i=0; i<nShader2::NumParameters; i++)
        this->args[i].SetType(nShaderArg::Void);
}

//------------------------------------------------------------------------------
#endif
