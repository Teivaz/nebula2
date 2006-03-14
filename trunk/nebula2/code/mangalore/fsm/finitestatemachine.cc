//------------------------------------------------------------------------------
//  fsm/finitestatemachine.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "fsm/finitestatemachine.h"

#include "xml/nxmlspreadsheet.h"
#include "fsm/timeoutcondition.h"

namespace FSM
{
ImplementRtti(FSM::FiniteStateMachine, Foundation::RefCounted);

//------------------------------------------------------------------------------
/**
*/
FiniteStateMachine::FiniteStateMachine(FSMFactory* factory)
{
    n_assert(0 != factory);
    this->factory = factory;

    this->currentState = 0;
    this->lastState = 0;
}

//------------------------------------------------------------------------------
/**
*/
FiniteStateMachine::~FiniteStateMachine()
{
}

//------------------------------------------------------------------------------
/**
    Get the name of the current state for debug purposes.
*/
const char*
FiniteStateMachine::GetCurrentState()
{
    n_assert(0 != this->currentState);
    return this->currentState->state->GetName();
}

//------------------------------------------------------------------------------
/**
*/
FiniteStateMachine::Transition*
FiniteStateMachine::ParseTransition(const char* condition, const char* targetAndActions, const nHashMap2<StateTableEntry*>& stateMap)
{
    if (!strcmp(targetAndActions, "-/-"))
    {
        return 0;
    }

    Transition* transition = n_new(Transition);
    transition->condition = this->factory->CreateCondition(condition);

	nArray<nString> transitionTokens;
	int transitionTokenCount = nString(targetAndActions).Tokenize("/", transitionTokens);

    n_assert(2 == transitionTokenCount);

    // set target state
    if (!strcmp(transitionTokens[0].Get(), "last"))
    {
        transition->nextState = 0;
        transition->backToLastState = true;
    }
    else if (strcmp(transitionTokens[0].Get(), "-"))
    {
        if (!stateMap.Contains(transitionTokens[0].Get()))
        {
            n_error("Invalid target state '%s'", transitionTokens[0].Get());
        }
        transition->nextState = stateMap[transitionTokens[0].Get()];
        transition->backToLastState = false;
    }
    else
    {
        transition->nextState = 0;
        transition->backToLastState = false;
    }

    // set actions
    if (strcmp(transitionTokens[1].Get(), "-"))
    {
    	nArray<nString> actionTokens;
        int actionTokenCount = transitionTokens[1].Tokenize("; \t\n", actionTokens);

        for (int i = 0; i < actionTokenCount; ++i)
        {
            nArray<nString> typeAndArg;
            actionTokens[i].Tokenize(":", typeAndArg);

            Action* action = 0;
            if (typeAndArg.Size() >= 2)
            {
                action = this->factory->CreateAction(typeAndArg[0].Get(), typeAndArg[1].Get());
            }
            else
            {
                action = this->factory->CreateAction(typeAndArg[0].Get(), 0);
            }

            n_assert(0 != action);
            transition->actions.Append(action);
        }
    }

    return transition;
}

//------------------------------------------------------------------------------
/**
    Builds the states and transitions from a xml table
*/
bool
FiniteStateMachine::Load(const nString& filename, const nString& tablename)
{
    n_assert(0 == this->currentState);

    nString path = "data:tables/";
    path.Append(filename);
    path.Append(".xml");

    // load file into spread sheet object
    nXmlSpreadSheet xmlSpreadSheet;
    xmlSpreadSheet.SetFilename(path);
    if (!xmlSpreadSheet.Open())
    {
        n_error("FSM::FiniteStateMachine::LoadXMLTable(): could not open xml file '%s'", path.Get());
        return false;
    }

    n_assert(xmlSpreadSheet.NumTables() > 0);
    nXmlTable* xmlTable = xmlSpreadSheet.FindTable(tablename);
    if(!xmlTable)
    {
        n_error("FSM::FiniteStateMachine::LoadXMLTable(): table '%s' does not exists in xml file '%s'", tablename.Get(), path.Get());
        return false;
    }

    nHashMap2<StateTableEntry*> stateTable(0);

    int row;
    int column;
    int numRows = xmlTable->NumRows();
    int numColumns = xmlTable->NumColumns();
    n_assert(numRows >= 1);
    n_assert(numColumns >= 1);

    n_assert(numRows - 1 > 0);
	this->states.SetFixedSize(numRows - 1);

    // init states
    for (row = 1; row < numRows; row++)
    {
		int stateIndex = row - 1;
        const nXmlTableCell& idCell = xmlTable->Cell(row, 0);

        this->states[stateIndex].create();
        this->states[stateIndex]->state = this->factory->CreateState(idCell.AsString().Get());

        stateTable.Add(idCell.AsString().Get(), this->states[stateIndex]);
    }

    // set state transitions
    for (row = 1; row < numRows; row++)
    {
		int stateIndex = row - 1;
        
		const nXmlTableCell& updateActionCell = xmlTable->Cell(row, 1);
        const nXmlTableCell& timeOutTimeCell = xmlTable->Cell(row, 2);
        const nXmlTableCell& timeOutTransitionCell = xmlTable->Cell(row, 3);

        if (strcmp(updateActionCell.AsString().Get(), "-"))
        {
            n_error("Update Actions are not supported anymore");
        }

        Transition* timeoutTransition = this->ParseTransition("TimeOut", timeOutTransitionCell.AsString().Get(), stateTable);
        if (0 != timeoutTransition)
        {
            TimeOutCondition* condition = (TimeOutCondition*)timeoutTransition->condition.get();
            n_assert(0 != condition && condition->IsA(TimeOutCondition::RTTI));

            condition->SetState(this->states[stateIndex]->state);
            condition->SetTimeOut(timeOutTimeCell.AsFloat());

            this->states[stateIndex]->transitions.Append(timeoutTransition);
        }

		// parse further transitions
		for (column = 4; column < numColumns; column++)
		{
			const nXmlTableCell& conditionCell = xmlTable->Cell(0, column);
			const nXmlTableCell& transitionCell = xmlTable->Cell(row, column);

            Transition* transition = this->ParseTransition(conditionCell.AsString().Get(), transitionCell.AsString().Get(), stateTable);
            if (0 != transition)
            {
                this->states[stateIndex]->transitions.Append(transition);
            }
		}
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
FiniteStateMachine::LeaveState()
{
    this->currentState->state->OnLeave();
    this->lastState = this->currentState;
}

//------------------------------------------------------------------------------
/**
*/
void
FiniteStateMachine::EnterState(FiniteStateMachine::StateTableEntry* newState)
{
    this->currentState = newState;
    this->currentState->state->OnEnter();

    for (int i = 0; i < newState->transitions.Size(); ++i)
    {
        newState->transitions[i]->condition->Reset();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
FiniteStateMachine::Trigger()
{
    if (0 == this->currentState)
    {
        this->EnterState(this->states[0]);
    }

    this->currentState->state->Trigger();

    // try all transitions
    int numTransitions = this->currentState->transitions.Size();
    for(int index = 0; index < numTransitions; index++)
    {
        Transition* transition = this->currentState->transitions[index];

        if (transition->condition->Check() > 0.0f)
        {
            // leave current state
            if ((transition->backToLastState && this->lastState != 0) || (0 != transition->nextState))
            {
                this->LeaveState();
            }

            // run transition actions
            int numTransitionActions = transition->actions.Size();
            for (int transitionActionIndex = 0; transitionActionIndex < numTransitionActions; transitionActionIndex++)
            {
                transition->actions[transitionActionIndex]->Trigger();
            }

            // select new state
            StateTableEntry* nextState = 0;
            if (transition->backToLastState && this->lastState != 0)
		    {
                nextState = this->lastState;
			}
            else if (0 != transition->nextState)
            {
                nextState = transition->nextState;
            }

            // enter new state
            if (0 != nextState)
            {
                this->EnterState(nextState);

                // break loop
                break;
            }
		}
    }

}

} // namespace FSM