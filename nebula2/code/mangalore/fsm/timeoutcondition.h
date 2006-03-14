#ifndef FSM_TIMEOUTCONDITION_H
#define FSM_TIMEOUTCONDITION_H
//------------------------------------------------------------------------------
/**
    @class FSM::TimeOutCondition

    (C) 2005 RadonLabs GmbH
*/
#include "fsm/condition.h"

#include "fsm/state.h"

//------------------------------------------------------------------------------
namespace FSM
{
class TimeOutCondition : public Condition
{
    DeclareRtti;

public:
    /// constructor
    TimeOutCondition();
    /// destructor
    virtual ~TimeOutCondition();

    /// Set state
    void SetState(State* state);
    /// Set timeout
    void SetTimeOut(nTime timeout);

    /// reset timer
    virtual void Reset();

    /// check for TimeOutCondition; return value of 0 means it isn't present, 1 it is
    virtual float Check();

protected:
    State* state;
    nTime started;
    nTime timeout;
};

}; // namespace FSM
//------------------------------------------------------------------------------
#endif
