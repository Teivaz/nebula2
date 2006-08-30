//------------------------------------------------------------------------------
//  fsm/timeoutcondition.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "fsm/timeoutcondition.h"

#include "game/time/gametimesource.h"

namespace FSM
{
    ImplementRtti(FSM::TimeOutCondition, FSM::Condition);

//------------------------------------------------------------------------------
/**
*/
TimeOutCondition::TimeOutCondition() :
    state(0),
    started(0.0),
    timeout(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TimeOutCondition::~TimeOutCondition()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TimeOutCondition::SetState(State* state)
{
    n_assert(0 != state && 0 == this->state);
    this->state = state;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeOutCondition::SetTimeOut(nTime timeout)
{
    this->timeout = timeout;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeOutCondition::Reset()
{
    this->started = Game::GameTimeSource::Instance()->GetTime();
}

//------------------------------------------------------------------------------
/**
*/
float
TimeOutCondition::Check()
{
    return (Game::GameTimeSource::Instance()->GetTime() - this->started) > this->timeout
        ? 1.0f
        : 0.0f;
}

} // namespace FSM
