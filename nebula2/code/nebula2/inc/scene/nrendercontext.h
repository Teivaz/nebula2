#ifndef N_RENDERCONTEXT_H
#define N_RENDERCONTEXT_H
//------------------------------------------------------------------------------
/**
    A nRenderContext object holds frame persistent data for nSceneNode
    hierarchies and serves as the central communication point between
    the client app and nSceneNode hierarchies.
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_VARIABLECONTEXT_H
#include "variable/nvariablecontext.h"
#endif

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

private:
    uint frameId;
    matrix44 transform;
};

//------------------------------------------------------------------------------
/**
*/
inline
nRenderContext::nRenderContext() :
    frameId(0xffffffff)
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
#endif

