#ifndef N_CHARACTER2_H
#define N_CHARACTER2_H
//------------------------------------------------------------------------------
/**
    @class nCharacter2
    @ingroup NebulaCharacterAnimationSystem

    @brief Holds all the data necessary to animate an character in one place.

    (C) 2003 RadonLabs GmbH
*/
#include "anim2/nanimstatearray.h"
#include "character/ncharskeleton.h"

//------------------------------------------------------------------------------
class nCharacter2
{
public:
    /// constructor
    nCharacter2();
    /// copy constructor
    nCharacter2(const nCharacter2& src);
    /// destructor
    ~nCharacter2();
    /// get the embedded character skeleton
    nCharSkeleton& GetSkeleton();
    /// set pointer to an anim state array (not owned)
    void SetAnimStateArray(nAnimStateArray* animStates);
    /// get pointer to anim state array
    const nAnimStateArray* GetAnimStateArray() const;
    /// set pointer to an animation source which delivers the source data (not owned)
    void SetAnimation(nAnimation* anim);
    /// get pointer to animation source
    const nAnimation* GetAnimation() const;
    /// set the currently active state
    void SetActiveState(int stateIndex, nTime time);
    /// get the current state index
    int GetActiveState() const;
    /// Is `i' a valid state index?
    bool ValidStateIndex(int i) const;
    /// evaluate the joint skeleton
    void EvaluateSkeleton(float time, nVariableContext* varContext);
    /// find a state index by state name
    int FindStateIndexByName(const nString& n);
    /// return animation duration of an animation state
    nTime GetStateDuration(int stateIndex) const;
    /// return fadein time of an animation state
    nTime GetStateFadeInTime(int stateIndex) const;

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
        /// is valid?
        bool IsValid() const;
            
        int stateIndex;
        float stateStarted;
    };

    nCharSkeleton charSkeleton;
    nAnimStateArray* animStateArray;
    nAnimation* animation;
    StateInfo prevStateInfo;
    StateInfo curStateInfo;

    static vector4 scratchKeyArray[MaxCurves];
    static vector4 keyArray[MaxCurves];
    static vector4 transitionKeyArray[MaxCurves];
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharacter2::StateInfo::StateInfo() :
    stateIndex(-1),
    stateStarted(0.0f)
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
nCharacter2::ValidStateIndex(int i) const
{
    return ((0 <= i) && (i < animStateArray->GetNumStates()));
}

//------------------------------------------------------------------------------
#endif



