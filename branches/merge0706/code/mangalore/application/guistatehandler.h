#ifndef APPLICATION_GUISTATEHANDLER_H
#define APPLICATION_GUISTATEHANDLER_H
//------------------------------------------------------------------------------
/**
    A state handler which displays and manages a UI subsystem user interface.

    (C) 2005 Radon Labs GmbH
*/
#include "application/statehandler.h"
#include "ui/event.h"
#include "ui/window.h"
#include "application/guieventhandler.h"

//------------------------------------------------------------------------------
namespace Application
{
class GuiStateHandler : public StateHandler
{
    DeclareRtti;
	DeclareFactory(GuiStateHandler);

public:
    /// constructor
    GuiStateHandler();
    /// destructor
    virtual ~GuiStateHandler();
    /// set name of gui resource
    void SetGuiResource(const nString& res);
    /// get name of gui resource
    const nString& GetGuiResource() const;
    /// handle a user interface event
    virtual void OnEvent(UI::Event* event);
    /// called when the state represented by this state handler is entered
    virtual void OnStateEnter(const nString& prevState);
    /// called when the state represented by this state handler is left
    virtual void OnStateLeave(const nString& nextState);
    /// called each frame as long as state is current, return new state
    virtual nString OnFrame();

private:
    nString resName;
    Ptr<UI::Window> window;
    Ptr<GuiEventHandler> eventHandler;
};

RegisterFactory(GuiStateHandler);

//------------------------------------------------------------------------------
/**
*/
inline
void
GuiStateHandler::SetGuiResource(const nString& n)
{
    this->resName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GuiStateHandler::GetGuiResource() const
{
    return this->resName;
}

} // namespace Application
//------------------------------------------------------------------------------
#endif
