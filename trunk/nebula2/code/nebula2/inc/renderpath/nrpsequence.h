#ifndef N_RPSEQUENCE_H
#define N_RPSEQUENCE_H
//------------------------------------------------------------------------------
/**
    @class nRpSequence
    @ingroup NebulaRenderPathSystem

    Encapsulates a sequence shader in a render path. This is the lowest
    level component which defines the shader states for mesh rendering.
    
    (C) 2004 RadonLabs GmbH
*/    
#include "gfx2/nshader2.h"

//------------------------------------------------------------------------------
class nRpSequence
{
public:
    /// constructor
    nRpSequence();
    /// destructor
    ~nRpSequence();
    /// assignment operator
    void operator=(const nRpSequence& rhs);
    /// set shader path
    void SetShaderPath(const nString& p);
    /// get shader path
    const nString& GetShaderPath() const;
    /// get shader pointer
    nShader2* GetShader() const;

private:
    friend class nRpPhase;

    /// validate the sequence object
    void Validate();

    nString shaderPath;
    nRef<nShader2> refShader;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpSequence::operator=(const nRpSequence& rhs)
{
    this->shaderPath = rhs.shaderPath;
    this->refShader  = rhs.refShader;
    if (this->refShader.isvalid())
    {
        this->refShader->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpSequence::SetShaderPath(const nString& p)
{
    this->shaderPath = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpSequence::GetShaderPath() const
{
    return this->shaderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nRpSequence::GetShader() const
{
    return this->refShader;
}

//------------------------------------------------------------------------------
#endif
