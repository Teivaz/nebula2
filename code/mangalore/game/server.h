#ifndef GAME_SERVER_H
#define GAME_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Game::Server

    The game server setups and runs the game world, consisting of a number
    of active "game entities". Functionality and queries on the game world
    are divided amongst several Manager objects, which are created as
    Singletons during the game server's Open() method. This keeps the
    game server's interface small and clean, and lets Mangalore applications
    easily extend functionality by implementing new, or deriving from
    existing managers.

    To add or replace Manager objects, derive from Game::Server and
    overwrite the SetupManagers() method.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "kernel/nprofiler.h"

//------------------------------------------------------------------------------
namespace Game
{
class Entity;
class Manager;

class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get instance pointer
    static Server* Instance();

    /// open the game world
    virtual bool Open();
    /// start the game world
    virtual bool Start();
    /// has the game world already started
    bool HasStarted() const;
    /// stop the game world
    virtual void Stop();
    /// close the game world
    virtual void Close();
    /// trigger the game world
    virtual void OnFrame();
    /// flush the entire game world state back into the database
    virtual void Save();
    /// invoke the OnLoad() method on the entire game world
    virtual void Load();
    /// attach a manager to the game world
    virtual void AttachManager(Manager* manager);
    /// remove a manager from the game world
    virtual void RemoveManager(Manager* manager);
    /// return true if a manager of this class is currently attached
    virtual bool HasManager(const Foundation::Rtti& classId) const;
    /// find a manager by its class id
    virtual Manager* FindManager(const Foundation::Rtti& classId) const;
    /// render a debug visualization 
    virtual void RenderDebug();

protected:
    friend class Entity;
    static Server* Singleton;

    bool isOpen;
    bool isStarted;
    nArray<Ptr<Manager> > managers;
    
    PROFILER_DECLARE(profGameServerFrame);
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Server::Singleton);
    return Server::Singleton;
}

} // namespace Game
//------------------------------------------------------------------------------
#endif
