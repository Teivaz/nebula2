#ifndef FSM_ACTION_H
#define FSM_ACTION_H
//------------------------------------------------------------------------------
/**
    @class FSM::Action

    (C) 2005 RadonLabs GmbH
*/
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace FSM
{
class Action : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// constructor
    Action();
    /// destructor
    virtual ~Action();

    /// called when action should be triggered
    virtual void Trigger() = 0;

protected:
};

} // namespace FSM
//------------------------------------------------------------------------------
#endif
