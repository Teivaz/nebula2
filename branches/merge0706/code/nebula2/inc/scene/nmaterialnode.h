#ifndef N_MATERIALNODE_H
#define N_MATERIALNODE_H
//------------------------------------------------------------------------------
/**
    @class nMaterialNode
    @ingroup Scene

    @brief A material node defines a shader resource and associated shader
    variables. A shader resource is an external file (usually a text file)
    which defines a surface shader.

    Material nodes themselves cannot render anything useful, they can just
    adjust render states as preparation of an actual rendering process.
    Thus, subclasses should be derived which implement some sort of
    shape rendering.

    - 27-Jan-05 floh    removed shader FourCC code stuff

    (C) 2002 RadonLabs GmbH
*/
#include "scene/nabstractshadernode.h"
#include "kernel/nref.h"
#include "kernel/nautoref.h"

class nShader2;
class nGfxServer2;

//------------------------------------------------------------------------------
class nMaterialNode : public nAbstractShaderNode
{
public:
    /// constructor
    nMaterialNode();
    /// destructor
    virtual ~nMaterialNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// indicate to scene graph that we provide a surface shader
    virtual bool HasShader() const;
    /// perform pre-instancing rending of shader
    virtual bool ApplyShader(nSceneServer* sceneServer);
    /// perform per-instance-rendering of shader
    virtual bool RenderShader(nSceneServer* sceneServer, nRenderContext* renderContext);

    /// set shader resource name
    void SetShader(const char* name);
    /// get shader resource name
    const char* GetShader() const;
    /// get bucket index of shader
    int GetShaderIndex();
    /// get pointer to shader object
    nShader2* GetShaderObject();
    /// set maya shader name
    void SetMayaShaderName(nString name);
    /// get maya shader name
    nString GetMayaShaderName() const;

protected:
    /// recursively append instance parameters to provided instance stream declaration
    virtual void UpdateInstStreamDecl(nInstanceStream::Declaration& decl);

private:
    /// load the shader resource
    bool LoadShader();
    /// unload the shader resource
    void UnloadShader();
    /// checks if shader uses texture passed in param
    virtual bool IsTextureUsed(nShaderState::Param param);

    nString mayaShaderName;
    nString shaderName;
    int shaderIndex;
    nRef<nShader2> refShader;
};

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nMaterialNode::GetShaderObject()
{
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
    return this->refShader.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMaterialNode::GetShaderIndex()
{
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
    return this->shaderIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::SetMayaShaderName(nString name)
{
    this->mayaShaderName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nMaterialNode::GetMayaShaderName() const
{
    return this->mayaShaderName;
}


//------------------------------------------------------------------------------
#endif

