#ifndef N_RENDERPATH2_H
#define N_RENDERPATH2_H
//------------------------------------------------------------------------------
/**
    @class nRenderPath2
    @ingroup NebulaRenderPathSystem

    A render path is an abstract description of HOW a scene should be rendered.
    This includes things like what shaders to apply and in which order, what render 
    targets to render to, highlevel differences between DX9 and DX7 scene rendering.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "renderpath/nrppass.h"
#include "renderpath/nrprendertarget.h"
#include "renderpath/nrpxmlparser.h"
#include "variable/nvariable.h"

//------------------------------------------------------------------------------
class nRenderPath2
{
public:
    /// constructor
    nRenderPath2();
    /// destructor
    ~nRenderPath2();
    /// get instance pointer
    static nRenderPath2* Instance();
    /// set xml filename
    void SetFilename(const nString& n);
    /// get xml filename
    const nString& GetFilename() const;
    /// open the XML document
    bool OpenXml();
    /// close the XML document
    void CloseXml();
    /// open the object, the XML document must be open
    bool Open();
    /// close the object
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// set the shader path
    void SetShaderPath(const nString& p);
    /// get the shader path, valid after OpenXmlDoc()
    const nString& GetShaderPath() const;
    /// set the render path object's name
    void SetName(const nString& n);
    /// get the render path object's name
    const nString& GetName() const;
    /// add a render target
    void AddRenderTarget(nRpRenderTarget& rt);
    /// get array of render targets
    const nArray<nRpRenderTarget>& GetRenderTargets() const;
    /// find render target by name
    nRpRenderTarget* FindRenderTarget(const nString& n) const;
    /// add a render pass
    void AddPass(nRpPass& rp);
    /// get array of passes
    const nArray<nRpPass>& GetPasses() const;
    /// add a global variable
    void AddVariable(const nVariable& var);
    /// get global variable handles
    const nArray<nVariable::Handle>& GetVariableHandles() const;
    /// update variable value
    void UpdateVariable(const nVariable& var);
    /// begin rendering the render path
    int Begin();
    /// get pass at index
    nRpPass& GetPass(int i) const;
    /// finish rendering the render path
    void End();
    /// return true if inside Begin()/End()
    bool InBegin() const;
    /// get current sequence shader index and increment by one
    int GetSequenceShaderAndIncrement();
    /// validate render path resources
    void Validate();

private:    
    /// draw a fullscreen quad
    void DrawQuad();

    static nRenderPath2* Singleton;

    bool isOpen;
    bool inBegin;
    nString xmlFilename;
    nString shaderPath;
    nString name;
    nRpXmlParser xmlParser;
    nArray<nRpPass> passes;
    nArray<nRpRenderTarget> renderTargets;
    nArray<nVariable::Handle> variableHandles;
    int sequenceShaderIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath2*
nRenderPath2::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPath2::SetShaderPath(const nString& p)
{
    this->shaderPath = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath2::GetShaderPath() const
{
    return this->shaderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPath2::UpdateVariable(const nVariable& var)
{
    nVariableServer::Instance()->SetGlobalVariable(var);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRenderPath2::GetSequenceShaderAndIncrement()
{
    return this->sequenceShaderIndex++;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRenderPath2::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRenderPath2::InBegin() const
{
    return this->inBegin;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPath2::SetFilename(const nString& n)
{
    this->xmlFilename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath2::GetFilename() const
{
    return this->xmlFilename;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPath2::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath2::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nRpRenderTarget>&
nRenderPath2::GetRenderTargets() const
{
    return this->renderTargets;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPath2::AddPass(nRpPass& p)
{
    this->passes.Append(p);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nRpPass>&
nRenderPath2::GetPasses() const
{
    return this->passes;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRpPass&
nRenderPath2::GetPass(int i) const
{
    return this->passes[i];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPath2::AddVariable(const nVariable& var)
{
    nVariableServer::Instance()->SetGlobalVariable(var);
    this->variableHandles.Append(var.GetHandle());
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nVariable::Handle>&
nRenderPath2::GetVariableHandles() const
{
    return this->variableHandles;
}

//------------------------------------------------------------------------------
#endif
