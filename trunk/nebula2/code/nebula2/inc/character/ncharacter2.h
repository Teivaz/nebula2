#ifndef N_CHARACTER2_H
#define N_CHARACTER2_H
//------------------------------------------------------------------------------
/**
    @class nCharacter2
    @ingroup Character

    @brief Holds all the data necessary to animate an character in one place.

    (C) 2003 RadonLabs GmbH
*/
#include "anim2/nanimstatearray.h"
#include "character/ncharskeleton.h"
#include "kernel/nrefcounted.h"
#include "anim2/nanimeventhandler.h"

//------------------------------------------------------------------------------
class nCharacter2 : public nRefCounted
{
public:
    /// constructor
    nCharacter2();
    /// copy constructor
    nCharacter2(const nCharacter2& src);
    /// destructor
    virtual ~nCharacter2();
    /// get the embedded character skeleton
    nCharSkeleton& GetSkeleton();
    /// get the embedded variated character skeleton
    nCharSkeleton& GetVariationSkeleton();
    /// set pointer to an anim state array (not owned)
    void SetAnimStateArray(nAnimStateArray* animStates);
    /// get pointer to anim state array
    const nAnimStateArray* GetAnimStateArray() const;
    /// set pointer to an animation source which delivers the source data (not owned)
    void SetAnimation(nAnimation* anim);
    /// get pointer to animation source
    const nAnimation* GetAnimation() const;
    /// set optional anim event handler (incrs refcount of handler)
    void SetAnimEventHandler(nAnimEventHandler* handler);
    /// get optional anim event handler
    nAnimEventHandler* GetAnimEventHandler() const;
    /// set the currently active state
    void SetActiveState(int stateIndex, nTime time, nTime offset);
    /// get the current state index
    int GetActiveState() const;
    /// is `i' a valid state index?
    bool IsValidStateIndex(int i) const;
    /// evaluate the joint skeleton
    void EvaluateSkeleton(float time, nVariableContext* varContext);
    /// emit animation events between 2 times
    void EmitAnimEvents(float startTime, float stopTime);
    /// find a state index by state name
    int FindStateIndexByName(const nString& n);
    /// return animation duration of an animation state
    nTime GetStateDuration(int stateIndex) const;
    /// return fadein time of an animation state
    nTime GetStateFadeInTime(int stateIndex) const;
    /// enable/disable animation
    void SetAnimEnabled(bool b);
    /// get manual joint animation
    bool IsAnimEnabled() const;
    /// set the frame id when the character was last evaluated
    void SetLastEvaluationFrameId(uint id);
    /// get the frame id when the character was last evaluated
    uint GetLastEvaluationFrameId() const;
    /// resets the current state
    void ResetCurrentState();

private:

    enum
    {
        MaxJoints = 1024,
        MaxCurves = MaxJoints * 3,      // translate, rotate, scale per curve
    };

    class StateInfo
    {
    public:
        /// constructor
        StateInfo();
        /// set state index
        void SetStateIndex(int i);
        /// get state index
        int GetStateIndex() const;
        /// set state started time
        void SetStateStarted(float t);
        /// get state started time
        float GetStateStarted() const;
        /// set state time offset
        void SetStateOffset(float t);
        /// get state time offset
        float GetStateOffset() const;
        /// is valid?
        bool IsValid() const;

        int stateIndex;
        float stateStarted;
        float stateOffset;
    };

    nCharSkeleton charSkeleton;
    nCharSkeleton charVariatedSkeleton;
    nAnimStateArray* animStateArray;
    nRef<nAnimation> animation;
    nAnimEventHandler* animEventHandler;
    StateInfo prevStateInfo;
    StateInfo curStateInfo;

    static vector4 scratchKeyArray[MaxCurves];
    static vector4 keyArray[MaxCurves];
    static vector4 transitionKeyArray[MaxCurves];

    bool animEnabled;
    uint lastEvaluationFrameId;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharacter2::StateInfo::StateInfo() :
    stateIndex(-1),
    stateStarted(0.0f),
    stateOffset(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::StateInfo::SetStateIndex(int i)
{
    this->stateIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter2::StateInfo::GetStateIndex() const
{
    return this->stateIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::StateInfo::SetStateStarted(float t)
{
    this->stateStarted = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCharacter2::StateInfo::GetStateStarted() const
{
    return this->stateStarted;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::StateInfo::SetStateOffset(float t)
{
    this->stateOffset= t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCharacter2::StateInfo::GetStateOffset() const
{
    return this->stateOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacter2::StateInfo::IsValid() const
{
    return (this->stateIndex != -1);
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton&
nCharacter2::GetSkeleton()
{
    return this->charSkeleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton&
nCharacter2::GetVariationSkeleton()
{
    return this->charVariatedSkeleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetAnimStateArray(nAnimStateArray* animStates)
{
    this->animStateArray = animStates;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nAnimStateArray*
nCharacter2::GetAnimStateArray() const
{
    return this->animStateArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetAnimation(nAnimation* anim)
{
    n_assert(anim);
    this->animation = anim;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nAnimation*
nCharacter2::GetAnimation() const
{
    return this->animation;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter2::GetActiveState() const
{
    return this->curStateInfo.GetStateIndex();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacter2::IsValidStateIndex(int i) const
{
    return ((0 <= i) && (i < animStateArray->GetNumStates()));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetAnimEnabled(bool b)
{
    this->animEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacter2::IsAnimEnabled() const
{
    return this->animEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetLastEvaluationFrameId(uint id)
{
    this->lastEvaluationFrameId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
nCharacter2::GetLastEvaluationFrameId() const
{
    return this->lastEvaluationFrameId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetAnimEventHandler(nAnimEventHandler* handler)
{
    if (this->animEventHandler)
    {
        this->animEventHandler->Release();
        this->animEventHandler = 0;
    }
    if (handler)
    {
        this->animEventHandler = handler;
        this->animEventHandler->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimEventHandler*
nCharacter2::GetAnimEventHandler() const
{
    return this->animEventHandler;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::ResetCurrentState()
{
    this->curStateInfo.SetStateIndex(-1);
}


//------------------------------------------------------------------------------
#endif



