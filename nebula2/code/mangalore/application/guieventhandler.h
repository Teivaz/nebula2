#ifndef APPLICATION_GUIEVENTHANDLER_H
#define APPLICATION_GUIEVENTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Application::GuiEventHandler

    A GUI event handler for the GuiStateHandler application state handler.
    This generic event handler class just routes the events it receives
    to the GuiStateHandler::HandleEvent() method.
    
    (C) 2005 Radon Labs GmbH
*/
#include "ui/eventhandler.h"

//------------------------------------------------------------------------------
namespace Application
{
class GuiStateHandler;

class GuiEventHandler : public UI::EventHandler
{
    DeclareRtti;
	DeclareFactory(GuiEventHandler);

public:
    /// constructor
    GuiEventHandler();
    /// destructor
    virtual ~GuiEventHandler();
    /// set pointer to GuiStateHandler object
    void SetGuiStateHandler(GuiStateHandler* h);
    /// get pointer to GuiStateHandler object
    GuiStateHandler* GetGuiStateHandler() const;
    /// override this method to handle your user interface events
    virtual void HandleEvent(UI::Event* e);

private:
    GuiStateHandler* guiStateHandler;
};

RegisterFactory(GuiEventHandler);

//------------------------------------------------------------------------------
/**
*/
inline
void
GuiEventHandler::SetGuiStateHandler(GuiStateHandler* h)
{
    this->guiStateHandler = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
GuiStateHandler*
GuiEventHandler::GetGuiStateHandler() const
{
    return this->guiStateHandler;
}

}; // namespace Application
//------------------------------------------------------------------------------
#endif