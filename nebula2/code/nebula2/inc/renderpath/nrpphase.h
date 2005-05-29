#ifndef N_RPPHASE_H
#define N_RPPHASE_H
//------------------------------------------------------------------------------
/**
    @class nRpPhase
    @ingroup NebulaRenderPathSystem

    A phase object inside a render path pass encapsulates sequence shaders
    and sets common render state for sequence shaders.
    
    (C) 2004 RadonLabs GmbH
*/
#include "util/nstring.h"
#include "renderpath/nrpsequence.h"

//------------------------------------------------------------------------------
class nRpPhase
{
public:
    /// sorting orders
    enum SortingOrder
    {
        None,
        FrontToBack,
        BackToFront,
    };

    /// constructor
    nRpPhase();
    /// destructor
    ~nRpPhase();
    /// assignment operator
    void operator=(const nRpPhase& rhs);
    /// set phase name
    void SetName(const nString& n);
    /// get phase name
    const nString& GetName() const;
    /// set phase shader's path
    void SetShaderPath(const nString& p);
    /// get phase shader's path
    const nString& GetShaderPath() const;
    /// set optional technique in shader
    void SetTechnique(const nString& n);
    /// get optional shader technique
    const nString& GetTechnique() const;
    /// set shader fourcc code
    void SetFourCC(nFourCC fcc);
    /// get shader fourcc code
    nFourCC GetFourCC() const;
    /// set sorting order
    void SetSortingOrder(SortingOrder o);
    /// get sorting order
    SortingOrder GetSortingOrder() const;
    /// enable/disable lighting
    void SetLightsEnabled(bool b);
    /// get light enabled flag
    bool GetLightsEnabled() const;
    /// add a sequence object
    void AddSequence(const nRpSequence& seq);
    /// get array of sequences
    const nArray<nRpSequence>& GetSequences() const;
    /// begin rendering the phase
    int Begin();
    /// get sequence by index
    nRpSequence& GetSequence(int i) const;
    /// finish rendering the phase
    void End();
    /// convert string to sorting order
    static SortingOrder StringToSortingOrder(const char* str);

private:
    friend class nRpPass;

    /// validate the pass object
    void Validate();

    bool inBegin;
    nString name;
    nString shaderPath;
    nString technique;
    nFourCC shaderFourCC;
    SortingOrder sortingOrder;
    bool lightsEnabled;
    nArray<nRpSequence> sequences;
    nRef<nShader2> refShader;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::operator=(const nRpPhase& rhs)
{
    this->inBegin       = rhs.inBegin;
    this->name          = rhs.name;
    this->shaderPath    = rhs.shaderPath;
    this->technique     = rhs.technique;
    this->shaderFourCC  = rhs.shaderFourCC;
    this->sortingOrder  = rhs.sortingOrder;
    this->lightsEnabled = rhs.lightsEnabled;
    this->sequences     = rhs.sequences;
    this->refShader     = rhs.refShader;
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
nRpPhase::SetTechnique(const nString& t)
{
    this->technique = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpPhase::GetTechnique() const
{
    return this->technique;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString& 
nRpPhase::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetShaderPath(const nString& p)
{
    this->shaderPath = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpPhase::GetShaderPath() const
{
    return this->shaderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetFourCC(nFourCC fcc)
{
    this->shaderFourCC = fcc;
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
nRpPhase::GetFourCC() const
{
    return this->shaderFourCC;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetSortingOrder(SortingOrder o)
{
    this->sortingOrder = o;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRpPhase::SortingOrder
nRpPhase::GetSortingOrder() const
{
    return this->sortingOrder;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetLightsEnabled(bool b)
{
    this->lightsEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpPhase::GetLightsEnabled() const
{
    return this->lightsEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::AddSequence(const nRpSequence& seq)
{
    this->sequences.Append(seq);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nRpSequence>&
nRpPhase::GetSequences() const
{
    return this->sequences;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRpSequence&
nRpPhase::GetSequence(int i) const
{
    return this->sequences[i];
}

//------------------------------------------------------------------------------
/**
*/
inline
nRpPhase::SortingOrder
nRpPhase::StringToSortingOrder(const char* str)
{
    n_assert(str);
    if (0 == strcmp("None", str)) return None;
    else if (0 == strcmp("FrontToBack", str)) return FrontToBack;
    else if (0 == strcmp("BackToFront", str)) return BackToFront;
    else
    {
        n_error("nRpPhase::StringToSortingOrder(): invalid string '%s'!", str);
        return None;
    }
}

//------------------------------------------------------------------------------
#endif    
