#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
//------------------------------------------------------------------------------
/**
    @class Game::Manager

    Managers are Singleton objects which care about some "specific global
    stuff". They should be subclassed by Mangalore applications to implement
    globals aspects of the application (mainly global game play related stuff).

    Managers are created directly through Factory<> and attached to the
    Game::Server object which will call several predefined virtual methods
    which are overridden by the subclasses.

    Standard-Mangalore uses several Managers to offer timing information
    (TimeManager), setup the game world (SetupManager), create entities
    and properties (FactoryManager), manage game entities (EntityManager)
    and so forth.

    Managers are derived from Message::Port, so you *can* optionally use them to
    receive and process messages.

    (C) 2005 Radon Labs GmbH
*/
#include "message/port.h"

//------------------------------------------------------------------------------
namespace Game
{
class Manager : public Message::Port
{
    DeclareRtti;
public:
    /// constructor
    Manager();
    /// destructor
    virtual ~Manager() = 0;

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// return true if currently active
    bool IsActive() const;
    /// called per-frame by the game server
    virtual void OnFrame();
    /// called after loading game state
    virtual void OnLoad();
    /// called before saving game state
    virtual void OnSave();
    /// called by Game::Server::Start() when the world is started
    virtual void OnStart();
    /// render a debug visualization
    virtual void OnRenderDebug();

private:
    bool isActive;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
Manager::IsActive() const
{
    return this->isActive;
}

} // namespace Game
//------------------------------------------------------------------------------
#endif

