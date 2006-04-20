#ifndef FSM_FSMFACTORY_H
#define FSM_FSMFACTORY_H
//------------------------------------------------------------------------------
/**
    @class FSM::FSMFactory

    (C) 2005 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "fsm/action.h"
#include "fsm/condition.h"
#include "fsm/state.h"

//------------------------------------------------------------------------------
namespace FSM
{

class FSMFactory : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// Constructor
    FSMFactory();
    /// Destructor
    virtual ~FSMFactory();

    /// Create an Action object
    virtual Action* CreateAction(const nString& type, const nString& param);
    /// Create a Condition object
    virtual Condition* CreateCondition(const nString& type);
    /// Create a State object
    virtual State* CreateState(const nString& type);
};

};
//------------------------------------------------------------------------------

#endif
