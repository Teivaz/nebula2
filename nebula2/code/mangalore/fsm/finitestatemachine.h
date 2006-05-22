#ifndef FSM_FINITESTATEMACHINE_H
#define FSM_FINITESTATEMACHINE_H
//------------------------------------------------------------------------------
/**
    @class FSM::FiniteStateMachine

    (C) 2005 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "fsm/fsmfactory.h"
#include "util/nhashmap2.h"

//------------------------------------------------------------------------------
namespace FSM
{

class FiniteStateMachine : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// constructor
    FiniteStateMachine(FSMFactory* factory);
    /// destructor
    virtual ~FiniteStateMachine();

    /// Return the current state name
    const nString& GetCurrentState();

    /// Load state machine from XML table
    bool Load(const nString& filename, const nString& tablename);

    /// Trigger the finite state machine.
    /// Switches the current state if a certain condition is present.
    void Trigger();

private:
    struct StateTableEntry;
    struct Transition;

    struct StateTableEntry : public Foundation::RefCounted
    {
        Ptr<State> state;
        nArray<Ptr<Transition> > transitions;
    };
    struct Transition : public Foundation::RefCounted
    {
        Ptr<Condition> condition;
        nArray<Ptr<Action> > actions;

        bool backToLastState;
        StateTableEntry* nextState;
    };

    Transition* ParseTransition(const nString& condition, const nString& targetAndActions, const nHashMap2<StateTableEntry*>& stateMap);

    void LeaveState();
    void EnterState(StateTableEntry* newState);

    Ptr<FSMFactory> factory;
    nArray<Ptr<StateTableEntry> > states;

    StateTableEntry* currentState;
    StateTableEntry* lastState;
};

}; // namespace FSM
//------------------------------------------------------------------------------
#endif
