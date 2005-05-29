#ifndef N_RPPASS_H
#define N_RPPASS_H
//------------------------------------------------------------------------------
/**
    @class nRpPass
    @ingroup NebulaRenderPathSystem

    Represents a pass in a render target.
    
    (C) 2004 RadonLabs GmbH
*/
#include "renderpath/nrpphase.h"
#include "variable/nvariablecontext.h"
#include "variable/nvariableserver.h"
#include "gfx2/nmesh2.h"

//------------------------------------------------------------------------------
class nRpPass
{
public:
    /// constructor
    nRpPass();
    /// destructor
    ~nRpPass();
    /// assignment operator
    void operator=(const nRpPass& rhs);
    /// set pass name
    void SetName(const nString& n);
    /// get pass name
    const nString& GetName() const;
    /// set the pass shader's path
    void SetShaderPath(const nString& n);
    /// get the pass shader's path
    const nString& GetShaderPath() const;
    /// set optional technique in shader
    void SetTechnique(const nString& n);
    /// get optional shader technique
    const nString& GetTechnique() const;
    /// set the render target's name (none for default render target)
    void SetRenderTargetName(const nString& n);
    /// get the render target's name
    const nString& GetRenderTargetName() const;
    /// set clear flags (nGfxServer2::BufferType)
    void SetClearFlags(int f);
    /// get clear flags
    int GetClearFlags() const;
    /// set clear color
    void SetClearColor(const vector4& c);
    /// get clear color
    const vector4& GetClearColor() const;
    /// set clear depth
    void SetClearDepth(float d);
    /// get clear depth
    float GetClearDepth() const;
    /// set clear stencil value
    void SetClearStencil(int v);
    /// get clear stencil value
    int GetClearStencil() const;
    /// set the draw fullscreen quad flag
    void SetDrawFullscreenQuad(bool b);
    /// get the draw fullscreen quad flag
    bool GetDrawFullscreenQuad() const;
    /// set the "draw shadow volumes" flag
    void SetDrawShadowVolumes(bool b);
    /// get the "draw shadow volumes" flag
    bool GetDrawShadowVolumes() const;
    /// set the "shadow enabled condition" flag
    void SetShadowEnabledCondition(bool b);
    /// get the "shadow enabled condition" flag
    bool GetShadowEnabledCondition() const;
    /// set the "draw gui" flag
    void SetDrawGui(bool b);
    /// get the "draw gui" flag
    bool GetDrawGui() const;
    /// add constant shader parameter value
    void AddConstantShaderParam(nShaderState::Param p, const nShaderArg& arg);
    /// add variable shader parameter value
    void AddVariableShaderParam(const nString& varName, nShaderState::Param p, const nShaderArg& arg);
    /// access to shader parameter block
    const nShaderParams& GetShaderParams() const;
    /// add optional phase objects
    void AddPhase(const nRpPhase& phase);
    /// get array of phases
    const nArray<nRpPhase>& GetPhases() const;
    /// begin rendering the pass
    int Begin();
    /// get phase object at index
    nRpPhase& GetPhase(int i) const;
    /// finish rendering the pass
    void End();

private:
    friend class nRenderPath2;

    /// validate the pass object
    void Validate();
    /// update the variable shader parameters
    void UpdateVariableShaderParams();
    /// update quad mesh coordinates
    void UpdateMeshCoords();
    /// draw a fullscreen quad 
    void DrawFullscreenQuad();

    struct ShaderParam
    {
        nShaderState::Type type;
        nString stateName;
        nString value;
    };

    nArray<ShaderParam> constShaderParams;
    nArray<ShaderParam> varShaderParams;

    bool inBegin;
    nString name;
    nString technique;
    nString shaderPath;
    nFourCC shaderFourCC;
    nString renderTargetName;

    nShaderParams shaderParams;
    nRef<nShader2> refShader;
    nRef<nMesh2> refQuadMesh;

    nArray<nRpPhase> phases;
    nVariableContext varContext;

    int clearFlags;
    vector4 clearColor;
    float clearDepth;
    int clearStencil;

    bool drawFullscreenQuad;        // true if pass should render a fullscreen quad
    bool drawShadowVolumes;         // true if this pass should render shadow volumes
    bool drawGui;                   // true if this pass should render the gui
    bool shadowEnabledCondition;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::operator=(const nRpPass& rhs)
{
    this->constShaderParams         = rhs.constShaderParams;
    this->varShaderParams           = rhs.varShaderParams;
    this->inBegin                   = rhs.inBegin;
    this->name                      = rhs.name;
    this->technique                 = rhs.technique;
    this->shaderPath                = rhs.shaderPath;
    this->shaderFourCC              = rhs.shaderFourCC;
    this->renderTargetName          = rhs.renderTargetName;
    this->shaderParams              = rhs.shaderParams;
    this->refShader                 = rhs.refShader;
    this->refQuadMesh               = rhs.refQuadMesh;
    this->phases                    = rhs.phases;
    this->varContext                = rhs.varContext;
    this->clearFlags                = rhs.clearFlags;
    this->clearColor                = rhs.clearColor;
    this->clearDepth                = rhs.clearDepth;
    this->clearStencil              = rhs.clearStencil;
    this->drawFullscreenQuad        = rhs.drawFullscreenQuad;
    this->drawShadowVolumes         = rhs.drawShadowVolumes;
    this->drawGui                   = rhs.drawGui;
    this->shadowEnabledCondition    = rhs.shadowEnabledCondition;
    if (this->refShader.isvalid())
    {
        this->refShader->AddRef();
    }
    if (this->refQuadMesh.isvalid())
    {
        this->refQuadMesh->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetTechnique(const nString& t)
{
    this->technique = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpPass::GetTechnique() const
{
    return this->technique;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetDrawFullscreenQuad(bool b)
{
    this->drawFullscreenQuad = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpPass::GetDrawFullscreenQuad() const
{
    return this->drawFullscreenQuad;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetDrawShadowVolumes(bool b)
{
    this->drawShadowVolumes = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpPass::GetDrawShadowVolumes() const
{
    return this->drawShadowVolumes;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetDrawGui(bool b)
{
    this->drawGui = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpPass::GetDrawGui() const
{
    return this->drawGui;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetClearFlags(int f)
{
    this->clearFlags = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpPass::GetClearFlags() const
{
    return this->clearFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetClearColor(const vector4& c)
{
    this->clearColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nRpPass::GetClearColor() const
{
    return this->clearColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetClearDepth(float d)
{
    this->clearDepth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nRpPass::GetClearDepth() const
{
    return this->clearDepth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetClearStencil(int v)
{
    this->clearStencil = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpPass::GetClearStencil() const
{
    return this->clearStencil;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpPass::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetShaderPath(const nString& p)
{
    this->shaderPath = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpPass::GetShaderPath() const
{
    return this->shaderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetRenderTargetName(const nString& n)
{
    this->renderTargetName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpPass::GetRenderTargetName() const
{
    return this->renderTargetName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::AddConstantShaderParam(nShaderState::Param p, const nShaderArg& arg)
{
    // add the shader param to the parameter block
    this->shaderParams.SetArg(p, arg);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::AddVariableShaderParam(const nString& varName, nShaderState::Param p, const nShaderArg& arg)
{
    // add shader param to parameter block
    this->shaderParams.SetArg(p, arg);

    // add a variable name to shader state mapping to the variable context
    nVariable::Handle h = nVariableServer::Instance()->GetVariableHandleByName(varName.Get());
    nVariable var(h, int(p));
    this->varContext.AddVariable(var);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nShaderParams&
nRpPass::GetShaderParams() const
{
    return this->shaderParams;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::AddPhase(const nRpPhase& p)
{
    this->phases.Append(p);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nRpPhase>&
nRpPass::GetPhases() const
{
    return this->phases;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRpPhase&
nRpPass::GetPhase(int i) const
{
    return this->phases[i];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPass::SetShadowEnabledCondition(bool b)
{
    this->shadowEnabledCondition = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpPass::GetShadowEnabledCondition() const
{
    return this->shadowEnabledCondition;
}

//------------------------------------------------------------------------------
#endif

    
