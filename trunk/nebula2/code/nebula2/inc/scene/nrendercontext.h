#ifndef N_RENDERCONTEXT_H
#define N_RENDERCONTEXT_H
//------------------------------------------------------------------------------
/**
    @class nRenderContext
    @ingroup NebulaSceneSystem

    A nRenderContext object holds frame persistent data for nSceneNode
    hierarchies and serves as the central communication point between
    the client app and nSceneNode hierarchies.
    
    (C) 2002 RadonLabs GmbH
*/
#include "variable/nvariablecontext.h"
#include "gfx2/nshaderparams.h"

class nSceneNode;

//------------------------------------------------------------------------------
class nRenderContext : public nVariableContext
{
public:
    /// constructor
    nRenderContext();
    /// destructor
    ~nRenderContext();
    /// set the current frame id
    void SetFrameId(uint id);
    /// get the current frame id
    uint GetFrameId() const;
    /// set the current transformation
    void SetTransform(const matrix44& m);
    /// get the current transformation
    const matrix44& GetTransform() const;
    /// set the render context's root scene node
    void SetRootNode(nSceneNode* node);
    /// get the render context's root scene node
    nSceneNode* GetRootNode() const;
    /// reset the link array
    void ClearLinks();
    /// add a link to another render context
    void AddLink(nRenderContext* link);
    /// get the number of links
    int GetNumLinks() const;
    /// get linked render context at index
    nRenderContext* GetLinkAt(int index) const;
    /// access to shader parameter overrides
    nShaderParams& GetShaderOverrides();

    /// appends a new var to localVarArray, returns the index
    int AddLocalVar(const nVariable& value);
    /// returns local variable at given index
    nVariable& GetLocalVar(int index);

protected:
    uint frameId;
    matrix44 transform;
    nRef<nSceneNode> rootNode;
    float priority;
    nArray<nRenderContext*> linkArray;
    nShaderParams shaderOverrides;
    nArray<nVariable> localVarArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nRenderContext::nRenderContext() :
    frameId(0xffffffff),
    priority(1.0f),
    linkArray(64, 64)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderContext::~nRenderContext()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderContext::SetFrameId(uint id)
{
    this->frameId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
nRenderContext::GetFrameId() const
{
    return this->frameId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderContext::SetTransform(const matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nRenderContext::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderContext::SetRootNode(nSceneNode* node)
{
    n_assert(node);
    this->rootNode = node;
}

//------------------------------------------------------------------------------
/**
*/
inline
nSceneNode*
nRenderContext::GetRootNode() const
{
    return this->rootNode.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderContext::ClearLinks()
{
    this->linkArray.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderContext::AddLink(nRenderContext* link)
{
    n_assert(link);
    this->linkArray.Append(link);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRenderContext::GetNumLinks() const
{
    return this->linkArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderContext*
nRenderContext::GetLinkAt(int index) const
{
    return this->linkArray[index];
}

//------------------------------------------------------------------------------
/**    
*/
inline
nShaderParams&
nRenderContext::GetShaderOverrides()
{
    return this->shaderOverrides;
}

//------------------------------------------------------------------------------
/**    
*/
inline
int nRenderContext::AddLocalVar(const nVariable& value)
{
    this->localVarArray.Append(value);
    return this->localVarArray.Size()-1;
}

//------------------------------------------------------------------------------
/**    
*/
inline
nVariable& nRenderContext::GetLocalVar(int index)
{
    return this->localVarArray.At(index);
}

//------------------------------------------------------------------------------
#endif
