//------------------------------------------------------------------------------
//  fsm/fsmfactory.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "fsm/fsmfactory.h"
#include "fsm/timeoutcondition.h"

namespace FSM
{
ImplementRtti(FSM::FSMFactory, Foundation::RefCounted);

//------------------------------------------------------------------------------
/**
*/
FSMFactory::FSMFactory()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FSMFactory::~FSMFactory()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Action*
FSMFactory::CreateAction(const nString& type, const nString& param)
{
    n_error("Unknown fsm action type '%s'", type);
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
Condition*
FSMFactory::CreateCondition(const nString& type)
{
    if (type == "TimeOut")
    {
        return n_new(TimeOutCondition);
    }

    n_error("Unknown fsm condition type '%s'", type);
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
State*
FSMFactory::CreateState(const nString& type)
{
    n_error("Unknown fsm state type '%s'", type);
    return 0;
}

} // namespace FSM
