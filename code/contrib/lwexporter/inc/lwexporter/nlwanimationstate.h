#ifndef N_LW_ANIMATION_STATE_H
#define N_LW_ANIMATION_STATE_H
//----------------------------------------------------------------------------
#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class nLWAnimationState
    @brief Stores animation state data used by the animation state dialogs.
*/
class nLWAnimationState
{
public:
    nLWAnimationState();

    bool operator==(const nLWAnimationState&) const;
    bool operator!=(const nLWAnimationState&) const;

    nString name;
    int startFrame;
    int endFrame;
    float fadeInTime;
    bool repeat;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWAnimationState::nLWAnimationState() :
    startFrame(0),
    endFrame(0),
    fadeInTime(0.0f),
    repeat(false)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWAnimationState::operator==(const nLWAnimationState& rhs) const
{
    if (this->name != rhs.name)
        return false;

    if (this->startFrame != rhs.startFrame)
        return false;

    if (this->endFrame != rhs.endFrame)
        return false;

    if (this->fadeInTime != rhs.fadeInTime)
        return false;

    if (this->repeat != rhs.repeat)
        return false;

    return true;
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWAnimationState::operator!=(const nLWAnimationState& rhs) const
{
    return !(*this == rhs);
}

//----------------------------------------------------------------------------
#endif // N_LW_ANIMATION_STATE_H
