#ifndef N_ANGULARPFEEDBACKLOOP_H
#define N_ANGULARPFEEDBACKLOOP_H
//------------------------------------------------------------------------------
/** 
    @class nAngularPFeedbackLoop
    @ingroup Util

    A proportional feedback loop with correct angular interpolation.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/nmath.h"

//------------------------------------------------------------------------------
class nAngularPFeedbackLoop
{
public:
    /// constructor
    nAngularPFeedbackLoop();
    /// reset the time
    void Reset(nTime time, float stepSize, float gain, float curState);
    /// set the gain
    void SetGain(float g);
    /// get the gain
    float GetGain() const;
    /// set the goal
    void SetGoal(float c);
    /// get the goal
    float GetGoal() const;
    /// set the current state directly
    void SetState(float s);
    /// get the current state the system is in
    float GetState() const;
    /// update the object, return new state
    void Update(nTime time);

private:
    nTime time;         // the time at which the simulation is
    float stepSize;
    float gain;
    float goal;
    float state;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAngularPFeedbackLoop::nAngularPFeedbackLoop() :
    time(0.0),
    stepSize(0.001f),
    gain(-1.0f),
    goal(0.0f),
    state(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAngularPFeedbackLoop::Reset(nTime t, float s, float g, float curState)
{
    this->time = t;
    this->stepSize = s;
    this->gain = g;
    this->state = curState;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAngularPFeedbackLoop::SetGain(float g)
{
    this->gain = g;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAngularPFeedbackLoop::GetGain() const
{
    return this->gain;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAngularPFeedbackLoop::SetGoal(float g)
{
    this->goal = g;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAngularPFeedbackLoop::GetGoal() const
{
    return this->goal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAngularPFeedbackLoop::SetState(float s)
{
    this->state = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAngularPFeedbackLoop::GetState() const
{
    return this->state;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAngularPFeedbackLoop::Update(nTime curTime)
{
    nTime dt = curTime - this->time;

    // catch time exceptions
    if (dt < 0.0)
    {
        this->time = curTime;
    }
    else if (dt > 0.5)
    {
        this->time = curTime - 0.5;
    }

    while (this->time < curTime)
    {
        // get angular distance error
        float error = n_angulardistance(this->state, this->goal);
        if (n_abs(error) > N_TINY)
        {
            this->state = n_normangle(this->state - (error * this->gain * this->stepSize));
            this->time += this->stepSize;
        }
        else
        {
            this->state = this->goal;
            this->time = curTime;
            break;
        }
    }
}

//------------------------------------------------------------------------------
#endif
