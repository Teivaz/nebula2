#ifndef N_CUTSCENEAPPSTATE_H
#define N_CUTSCENEAPPSTATE_H
//------------------------------------------------------------------------------
/**
    @class nCutSceneAppState
    @ingroup Application

    @brief An application state which will playback a video and then proceed
    to the next configured application state.
    
    (C) 2004 RadonLabs GmbH
*/
#include "application/nappstate.h"

//------------------------------------------------------------------------------
class nCutSceneAppState : public nAppState
{
public:
    /// constructor
    nCutSceneAppState();
    /// destructor
    virtual ~nCutSceneAppState();
    /// called when state is becoming active
    virtual void OnStateEnter(const nString& prevState);
    /// called when state is becoming inactive
    virtual void OnStateLeave(const nString& nextState);
    /// called on state to perform state logic 
    virtual void OnFrame();
    /// set filename of video file
    void SetVideoFilename(const nString& n);
    /// get filename of video file
    const nString& GetVideoFilename() const;
    /// set scale to fullscreen flag
    void SetEnableScaling(bool b);
    /// get scale to fullscreen flag
    bool GetEnableScaling() const;
    /// set the followup state
    void SetNextState(const nString& s);
    /// get the followup state
    const nString& GetNextState() const;

private:
    nString videoFilename;
    bool enableScaling;
    nString nextState;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nCutSceneAppState::SetVideoFilename(const nString& n)
{
    this->videoFilename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nCutSceneAppState::GetVideoFilename() const
{
    return this->videoFilename;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCutSceneAppState::SetEnableScaling(bool b)
{
    this->enableScaling = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCutSceneAppState::GetEnableScaling() const
{
    return this->enableScaling;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCutSceneAppState::SetNextState(const nString& s)
{
    this->nextState = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nCutSceneAppState::GetNextState() const
{
    return this->nextState;
}

//------------------------------------------------------------------------------
#endif
    
