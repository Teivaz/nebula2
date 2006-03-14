#ifndef FSM_CONDITION_H
#define FSM_CONDITION_H
//------------------------------------------------------------------------------
/**
    @class FSM::Condition

    (C) 2005 RadonLabs GmbH
*/
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace FSM
{
class Condition : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// constructor
    Condition();
    /// destructor
    virtual ~Condition();

    /// reset condition; called when the fsm state changes
    virtual void Reset();
    /// check for condition; return value of 0 means it isn't present, 1 it is
    virtual float Check() = 0;

protected:
};

}; // namespace FSM
//------------------------------------------------------------------------------
#endif