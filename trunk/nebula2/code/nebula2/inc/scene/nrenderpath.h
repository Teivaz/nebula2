#ifndef N_RENDERPATH_H
#define N_RENDERPATH_H
//------------------------------------------------------------------------------
/**
    @class nRenderPath
    @ingroup Scene

    A render path object contains rules how a scene should be rendered
    which are described in an XML file.
    
    Scene rendering is split into Passes, Phases, Sequences and optional Posteffects.
    Pass, Phase and Sequence live under each other (Pass -> Phase -> Sequence) and have
    shaders assigned which are activated when each is rendered. Pass and Phase
    shaders define render states which are constant across the whole Pass
    or the whole Phase (this reduces redundancies). Actual shape node
    instance rendering happens within a Sequence. A Sequence shader contains the
    actual low level render states which are different from Sequence to Sequence
    and from instance to instance.

    Thus rendering a pass actually looks like this:

        * Begin Pass Shader
            * Begin Phase Shader
                * Begin Sequence Shader
                    * Render Instance
                    * Render Instance
                    * ...
                * End Sequence Shader
                * Begin Sequence Shader
                    * Render Instance
                    * Render Instance
                    * ...
                * End Sequence Shader
                * ...
            * End Phase Shader
            * Begin Phase Shader
                * ...
            * End Phase Shader
        * End Pass Shader
        * ...

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "tinyxml/tinyxml.h"
#include "util/nstring.h"
#include "util/narray.h"
#include "kernel/nref.h"

class nShader2;

//------------------------------------------------------------------------------
class nRenderPath
{
public:
    class Pass;
    class Phase;
    class Sequence;

    /// constructor
    nRenderPath();
    /// destructor
    ~nRenderPath();
    /// set the xml filename
    void SetFilename(const nString& name);
    /// get the xml filename
    const nString& GetFilename() const;
    /// open the object
    bool Open();
    /// close the object
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// get the render path name
    const nString& GetName() const;
    /// get the minimum required feature set
    const nString& GetFeatureSet() const;
    /// get the shader library path
    const nString& GetShaderPath() const;
    /// get number of scene passes
    uint GetNumPasses() const;
    /// begin a pass, return number of 
    void BeginPass(uint index);
    /// access to current pass
    const Pass& GetPass() const;
    /// get number of phases in current pass
    uint GetNumPhases() const;
    /// begin a phase
    void BeginPhase(uint index);
    /// access to current phase
    const Phase& GetPhase() const;
    /// get number of sequences in current
    uint GetNumSequences() const;
    /// get pointer to sequence shader
    nShader2* GetSequenceShader(uint index);
    /// get sequence by index in current phase
    const Sequence& GetSequence(uint index) const;
    /// end a phase
    void EndPhase();
    /// end current pass
    void EndPass();
    /// access to passes array
    const nArray<Pass>& PassArray() const;

    /// a pass description
    class Pass
    {
    public:
        /// constructor
        Pass();
        /// destructor
        ~Pass();
        /// get the pass name
        const nString& GetName() const;
        /// get pass shader
        nShader2* GetShader() const;
        /// access to phase array
        nArray<Phase>& PhaseArray();

    private:
        friend class nRenderPath;

        nString name;
        nRef<nShader2> refShader;
        nArray<Phase> phaseArray;
    };

    /// a phase description
    class Phase
    {
    public:
        /// sorting order
        enum SortingOrder
        {
            None,
            FrontToBack,
            BackToFront,
        };

        /// constructor
        Phase();
        /// destructor
        ~Phase();
        /// get pass name
        const nString& GetName() const;
        /// get shader pointer
        nShader2* GetShader() const;
        /// get shader fourcc code
        nFourCC GetFourCC() const;
        /// get sorting order
        SortingOrder GetSortingOrder() const;
        /// get lighting enabled flag
        bool GetLightsEnabled() const;
        /// access to Sequence array
        nArray<Sequence>& SequenceArray();

    private:
        friend class nRenderPath;
        /// convert string to sorting order enum
        static SortingOrder StringToSortingOrder(const char* str);
        /// convert string to bool
        static bool StringToBool(const char* str);

        nString name;
        nRef<nShader2> refShader;
        SortingOrder sortingOrder;
        nFourCC fourcc;
        bool lightsEnabled;
        nArray<Sequence> sequenceArray;
    };

    /// a sequence description
    class Sequence
    {
    public:
        friend class nRenderPath;
        /// constructor
        Sequence();
        /// destructor
        ~Sequence();
        /// get shader pointer
        nShader2* GetShader() const;

    private:
        friend class nRenderPath;
        nRef<nShader2> refShader;
    };

private:
    /// parse XML file into internal object hierarchy
    bool ParseXmlFile();
    /// load a shader
    nShader2* LoadShader(const nString& shaderName);

    bool isOpen;
    nString filename;
    nString name;
    nString featureSet;
    nString shaderPath;
    nArray<Pass> passArray;

    bool inPass;
    bool inPhase;
    uint curPassIndex;
    uint curPhaseIndex;
};

//------------------------------------------------------------------------------
/**
    Get number of passes in render path.
*/
inline
uint
nRenderPath::GetNumPasses() const
{
    n_assert(this->isOpen);
    return this->passArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get current pass.
*/
inline
const nRenderPath::Pass&
nRenderPath::GetPass() const
{
    n_assert(this->inPass);
    return this->passArray[this->curPassIndex];
}

//------------------------------------------------------------------------------
/**
    Get number of phases in current pass.
*/
inline
uint
nRenderPath::GetNumPhases() const
{
    n_assert(this->inPass);
    return this->GetPass().phaseArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get current phase.
*/
inline
const nRenderPath::Phase&
nRenderPath::GetPhase() const
{
    n_assert(this->inPhase);
    return this->GetPass().phaseArray[this->curPhaseIndex];
}

//------------------------------------------------------------------------------
/**
    Get number of sequences in current phase.
*/
inline
uint
nRenderPath::GetNumSequences() const
{
    n_assert(this->inPhase);
    return this->GetPhase().sequenceArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get sequence by sequence index in current phase.
*/
inline
const nRenderPath::Sequence&
nRenderPath::GetSequence(uint seqIndex) const
{
    n_assert(this->inPhase);
    return this->GetPhase().sequenceArray[seqIndex];
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRenderPath::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPath::SetFilename(const nString& name)
{
    this->filename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath::GetFilename() const
{
    return this->filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath::GetFeatureSet() const
{
    return this->featureSet;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath::GetShaderPath() const
{
    return this->shaderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath::Pass::Pass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath::Pass::~Pass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath::Pass::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nRenderPath::Pass::GetShader() const
{
    return this->refShader;
}

//------------------------------------------------------------------------------
/**
*/
inline
nArray<nRenderPath::Phase>&
nRenderPath::Pass::PhaseArray()
{
    return this->phaseArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath::Phase::Phase() :
    fourcc(0),
    sortingOrder(None),
    lightsEnabled(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath::Phase::~Phase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPath::Phase::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nRenderPath::Phase::GetShader() const
{
    return this->refShader;
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
nRenderPath::Phase::GetFourCC() const
{
    return this->fourcc;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath::Phase::SortingOrder
nRenderPath::Phase::GetSortingOrder() const
{
    return this->sortingOrder;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRenderPath::Phase::GetLightsEnabled() const
{
    return this->lightsEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
nArray<nRenderPath::Sequence>&
nRenderPath::Phase::SequenceArray()
{
    return this->sequenceArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath::Phase::SortingOrder
nRenderPath::Phase::StringToSortingOrder(const char* str)
{
    if (str)
    {
        if (0 == strcmp("None", str)) return None;
        else if (0 == strcmp("FrontToBack", str)) return FrontToBack;
        else if (0 == strcmp("BackToFront", str)) return BackToFront;
        else n_error("Invalid sorting order string '%s' (must be None, FrontToBack or BackToFront)!", str);
    }
    return None;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRenderPath::Phase::StringToBool(const char* str)
{
    n_assert(str);
    if (str)
    {
        if      (0 == strcmp("On", str)) return true;
        else if (0 == strcmp("Off", str)) return false;
        else n_error("Invalid bool string '%s' (must be On or Off)!", str);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath::Sequence::Sequence()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath::Sequence::~Sequence()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nRenderPath::Sequence::GetShader() const
{
    return this->refShader;
}

//------------------------------------------------------------------------------
#endif


            

