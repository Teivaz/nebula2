#ifndef N_ABSTRACTSHADERNODE_H
#define N_ABSTRACTSHADERNODE_H
//------------------------------------------------------------------------------
/**
    This is the base class for all shader related scene node classes
    (for instance material and light nodes). All those classes need
    to hold named, typed shader variables, as well as texture resource
    management.
    
    (C) 2003 RadonLabs GmbH
*/
#ifndef N_TRANSFORMNODE_H
#include "scene/ntransformnode.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_VARIABLE_H
#include "variable/nvariable.h"
#endif

#ifndef N_VARIABLECONTEXT_H
#include "variable/nvariablecontext.h"
#endif

#ifndef N_VARIABLESERVER_H
#include "variable/nvariableserver.h"
#endif

#ifndef N_TEXTURE2_H
#include "gfx2/ntexture2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nAbstractShaderNode
#include "kernel/ndefdllclass.h"

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
    /// return true if resources for this object are valid
    virtual bool AreResourcesValid() const;

    /// bind a texture resource to a shader variable
    void SetTexture(nVariable::Handle varHandle, const char* texName);
    /// get texture resource bound to variable
    const char* GetTexture(nVariable::Handle varHandle) const;
    /// bind a int value to a a shader variable
    void SetInt(nVariable::Handle varHandle, int val);
    /// get an int value bound to a shader variable
    int GetInt(nVariable::Handle varHandle) const;
    /// bind a float value to a shader variable
    void SetFloat(nVariable::Handle varHandle, float val);
    /// get a float value bound to a shader variable
    float GetFloat(nVariable::Handle varHandle) const;
    /// bind a vector value to a shader variable
    void SetVector(nVariable::Handle varHandle, const vector4& val);
    /// get a vector value bound to a shader variable
    vector4 GetVector(nVariable::Handle varHandle) const;

    /// SetTexture() with variable name (slow)
    bool SetTextureS(const char* varName, const char* texName);
    /// GetTexture() with variable name (slow)
    const char* GetTextureS(const char* varName);
    /// SetInt() with variable name (slow)
    bool SetIntS(const char* varName, int val);
    /// GetInt() with variable name (slow)
    int GetIntS(const char* varName);
    /// SetFloat() with variable name (slow)
    bool SetFloatS(const char* varName, float val);
    /// GetFloat() with variable name (slow)
    float GetFloatS(const char* varName);
    /// SetVector() with variable name (slow)
    bool SetVectorS(const char* varName, const vector4& val);
    /// GetVector() with variable name (slow)
    vector4 GetVectorS(const char* varName);

    static nKernelServer* kernelServer;

protected:
    /// load a texture resource
    bool LoadTexture(int index);
    /// unload a texture resource
    void UnloadTexture(int index);

    class TexNode
    {
    public:
        /// default constructor
        TexNode();
        /// constructor
        TexNode(nVariable::Handle varHandle, const char* texName);

        nVariable::Handle varHandle;
        nString texName;
        nRef<nTexture2> refTexture;
    };

    nAutoRef<nGfxServer2> refGfxServer;
    nAutoRef<nVariableServer> refVariableServer;
    nVariableContext varContext;
    nArray<TexNode> texNodeArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAbstractShaderNode::TexNode::TexNode() :
    varHandle(nVariable::INVALID_HANDLE)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAbstractShaderNode::TexNode::TexNode(nVariable::Handle varHandle, const char* name) :
    varHandle(varHandle),
    texName(name)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAbstractShaderNode::SetTextureS(const char* varName, const char* texName)
{
    n_assert(varName);
    nVariable::Handle varHandle= this->refVariableServer->GetVariableHandleByName(varName);
    this->SetTexture(varHandle, texName);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAbstractShaderNode::GetTextureS(const char* varName)
{
    n_assert(varName);
    nVariable::Handle varHandle = this->refVariableServer->GetVariableHandleByName(varName);
    return this->GetTexture(varHandle);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAbstractShaderNode::SetIntS(const char* varName, int val)
{
    n_assert(varName);
    nVariable::Handle varHandle= this->refVariableServer->GetVariableHandleByName(varName);
    this->SetInt(varHandle, val);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAbstractShaderNode::GetIntS(const char* varName)
{
    n_assert(varName);
    nVariable::Handle varHandle = this->refVariableServer->GetVariableHandleByName(varName);
    return this->GetInt(varHandle);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAbstractShaderNode::SetFloatS(const char* varName, float val)
{
    n_assert(varName);
    nVariable::Handle varHandle= this->refVariableServer->GetVariableHandleByName(varName);
    this->SetFloat(varHandle, val);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAbstractShaderNode::GetFloatS(const char* varName)
{
    n_assert(varName);
    nVariable::Handle varHandle = this->refVariableServer->GetVariableHandleByName(varName);
    return this->GetFloat(varHandle);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAbstractShaderNode::SetVectorS(const char* varName, const vector4& val)
{
    n_assert(varName);
    nVariable::Handle varHandle = this->refVariableServer->GetVariableHandleByName(varName);
    this->SetVector(varHandle, val);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nAbstractShaderNode::GetVectorS(const char* varName)
{
    n_assert(varName);
    nVariable::Handle varHandle = this->refVariableServer->GetVariableHandleByName(varName);
    return this->GetVector(varHandle);
}
//------------------------------------------------------------------------------
#endif
