#ifndef APPLICATION_STATEHANDLER_H
#define APPLICATION_STATEHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Application::StateHandler

    State handlers implement actual application state behaviour in subclasses
    of Application::StateHandler. The Application class calls state handler
    objects when a new state is entered, when the current state is left, and
    for each frame.

    State handlers must implement the OnStateEnter(), OnStateLeave() and
    OnStateFrame() methods accordingly.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "application/app.h"

//------------------------------------------------------------------------------
namespace Application
{
class StateHandler : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// constructor
    StateHandler();
    /// destructor
    virtual ~StateHandler() = 0;

    /// set state name
    void SetName(const nString& n);
    /// get state name
    const nString& GetName() const;
    /// called when attached to application object
    virtual void OnAttachToApplication();
    /// called when removed from the application object
    virtual void OnRemoveFromApplication();
    /// called when the state represented by this state handler is entered
    virtual void OnStateEnter(const nString& prevState);
    /// called when the state represented by this state handler is left
    virtual void OnStateLeave(const nString& nextState);
    /// called each frame as long as state is current, return new state
    virtual nString OnFrame();
    /// called after Db is opened, and before entities are loaded
    virtual void OnLoadBefore();
    /// called after entities are loaded
    virtual void OnLoadAfter();

protected:
    nString stateName;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
StateHandler::SetName(const nString& n)
{
    this->stateName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
StateHandler::GetName() const
{
    return this->stateName;
}

};
//------------------------------------------------------------------------------
#endif
