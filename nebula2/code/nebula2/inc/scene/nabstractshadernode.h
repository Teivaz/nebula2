#ifndef N_ABSTRACTSHADERNODE_H
#define N_ABSTRACTSHADERNODE_H
//------------------------------------------------------------------------------
/**
    @class nAbstractShaderNode
    @ingroup SceneNodes

    @brief This is the base class for all shader related scene node classes
    (for instance material and light nodes).
    
    All those classes need to hold named, typed shader variables, as well
    as texture resource management.

    See also @ref N2ScriptInterface_nabstractshadernode
    
    (C) 2003 RadonLabs GmbH
*/
#include "scene/ntransformnode.h"
#include "kernel/nautoref.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nshaderparams.h"
#include "mathlib/transform33.h"

//------------------------------------------------------------------------------
class nAbstractShaderNode : public nTransformNode
{
public:
    /// constructor
    nAbstractShaderNode();
    /// destructor
    virtual ~nAbstractShaderNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// set uv position for texture layer
    void SetUvPos(uint layer, const vector2& p);
    /// get uv position for texture layer
    const vector2& GetUvPos(uint layer) const;
    /// set uv euler rotation for texture layer
    void SetUvEuler(uint layer, const vector2& p);
    /// get uv euler rotation for texture layer
    const vector2& GetUvEuler(uint layer) const;
    /// set uv scale for texture layer
    void SetUvScale(uint layer, const vector2& p);
    /// get uv scale for texture layer
    const vector2& GetUvScale(uint layer) const;

    /// bind a texture resource to a shader variable
    void SetTexture(nShaderState::Param param, const char* texName);
    /// get texture resource bound to variable
    const char* GetTexture(nShaderState::Param param) const;
    /// bind a int value to a a shader variable
    void SetInt(nShaderState::Param param, int val);
    /// get an int value bound to a shader variable
    int GetInt(nShaderState::Param param) const;
    /// bind a bool value to a a shader variable
    void SetBool(nShaderState::Param param, bool val);
    /// get an bool value bound to a shader variable
    bool GetBool(nShaderState::Param param) const;
    /// bind a float value to a shader variable
    void SetFloat(nShaderState::Param param, float val);
    /// get a float value bound to a shader variable
    float GetFloat(nShaderState::Param param) const;
    /// bind a vector value to a shader variable
    void SetVector(nShaderState::Param param, const vector4& val);
    /// get a vector value bound to a shader variable
    vector4 GetVector(nShaderState::Param param) const;

    /// get number of textures
    int GetNumTextures() const;
    /// get texture resource name at index
    const char* GetTextureAt(int index) const;
    /// get texture shader parameter at index
    nShaderState::Param GetTextureParamAt(int index) const;

protected:
    /// load a texture resource
    bool LoadTexture(int index);
    /// unload a texture resource
    void UnloadTexture(int index);
    /// abstract method: returns always true
    virtual bool IsTextureUsed(nShaderState::Param param);

    class TexNode
    {
    public:
        /// default constructor
        TexNode();
        /// constructor
        TexNode(nShaderState::Param shaderParam, const char* texName);

        nShaderState::Param shaderParameter;
        nString texName;
        nRef<nTexture2> refTexture;
    };

    nArray<TexNode> texNodeArray;
    transform33 textureTransform[nGfxServer2::MaxTextureStages];
    nShaderParams shaderParams;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAbstractShaderNode::TexNode::TexNode() :
    shaderParameter(nShaderState::InvalidParameter)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAbstractShaderNode::TexNode::TexNode(nShaderState::Param shaderParam, const char* name) :
    shaderParameter(shaderParam),
    texName(name)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetUvPos(uint layer, const vector2& p)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    this->textureTransform[layer].settranslation(p);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nAbstractShaderNode::GetUvPos(uint layer) const
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    return this->textureTransform[layer].gettranslation();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetUvEuler(uint layer, const vector2& e)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    this->textureTransform[layer].seteulerrotation(e);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nAbstractShaderNode::GetUvEuler(uint layer) const
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    return this->textureTransform[layer].geteulerrotation();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetUvScale(uint layer, const vector2& s)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    this->textureTransform[layer].setscale(s);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nAbstractShaderNode::GetUvScale(uint layer) const
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    return this->textureTransform[layer].getscale();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetInt(nShaderState::Param param, int val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderParams.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAbstractShaderNode::GetInt(nShaderState::Param param) const
{
    return this->shaderParams.GetArg(param).GetInt();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetBool(nShaderState::Param param, bool val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderParams.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAbstractShaderNode::GetBool(nShaderState::Param param) const
{
    return this->shaderParams.GetArg(param).GetBool();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetFloat(nShaderState::Param param, float val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderParams.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAbstractShaderNode::GetFloat(nShaderState::Param param) const
{
    return this->shaderParams.GetArg(param).GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetVector(nShaderState::Param param, const vector4& val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    nFloat4 float4Val = 
    {
        val.x, val.y, val.z, val.w
    };
    this->shaderParams.SetArg(param, nShaderArg(float4Val));
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nAbstractShaderNode::GetVector(nShaderState::Param param) const
{
    const nFloat4& float4Val = this->shaderParams.GetArg(param).GetFloat4();
    return vector4(float4Val.x, float4Val.y, float4Val.z, float4Val.w);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAbstractShaderNode::GetNumTextures() const
{
    return this->texNodeArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAbstractShaderNode::GetTextureAt(int index) const
{
    return this->texNodeArray[index].texName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderState::Param
nAbstractShaderNode::GetTextureParamAt(int index) const
{
    return this->texNodeArray[index].shaderParameter;
}

//------------------------------------------------------------------------------
#endif
