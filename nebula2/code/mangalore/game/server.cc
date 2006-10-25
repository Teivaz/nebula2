//------------------------------------------------------------------------------
//  game/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "game/server.h"
#include "foundation/factory.h"
#include "game/manager.h"
#include "managers/entitymanager.h"
#include "db/server.h"
#include "db/writer.h"
#include "attr/attributes.h"
#include "loader/server.h"
#include "gfx2/ngfxserver2.h"

namespace Game
{
ImplementRtti(Game::Server, Foundation::RefCounted);
ImplementFactory(Game::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    isStarted(false)
{
    n_assert(0 == Singleton);
    Singleton = this;

    PROFILER_INIT(this->profGameServerFrame, "profMangaGameServerFrame");
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Initialize the game server object. This will create and initialize all
    subsystems.
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->isStarted);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Start the game world, called after loading has completed.
*/
bool
Server::Start()
{
    n_assert(this->isOpen);
    n_assert(!this->isStarted);

    // call the OnStart method on all managers
    int num = this->managers.Size();
    for (int i = 0; i < num; i++)
    {
        this->managers[i]->OnStart();
    }

    // call the OnStart() method on all entities
    const nArray<Ptr<Entity> >& entities = Managers::EntityManager::Instance()->GetEntities();
    int entityIndex;
    int numEntities = entities.Size();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        entities[entityIndex]->OnStart();
    }

    this->isStarted = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::HasStarted() const
{
    return this->isStarted;
}

//------------------------------------------------------------------------------
/**
    Stop the game world, called before the world(current level) is cleaned up.
*/
void
Server::Stop()
{
    n_assert(this->isOpen);
    n_assert(this->isStarted);

    this->isStarted = false;
}

//------------------------------------------------------------------------------
/**
    Close the game server object.
*/
void
Server::Close()
{
    n_assert(!this->isStarted);
    n_assert(this->isOpen);

    // remove all managers
    int num = this->managers.Size();
    for (int i = 0; i < num; i++)
    {
        this->managers[i]->OnDeactivate();
        this->managers[i] = 0;
    }
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Trigger the game server. If your application introduces new or different
    manager objects, you may also want to override the Game::Server::Trigger()
    method if those managers need per-frame callbacks.
*/
void
Server::OnFrame()
{
    // call OnFrame() on managers
    PROFILER_START(this->profGameServerFrame);
    int num = this->managers.Size();
    for (int i = 0; i < num; i++)
    {
        this->managers[i]->OnFrame();
    }
    PROFILER_STOP(this->profGameServerFrame);
}

//------------------------------------------------------------------------------
/**
    Attach a manager object to the game world. The manager's OnActivate()
    method will be called once right away, and then its OnFrame() method
    once per frame.
*/
void
Server::AttachManager(Manager* manager)
{
    n_assert(manager);
    manager->OnActivate();
    this->managers.Append(manager);
}

//------------------------------------------------------------------------------
/**
    Remove a manager object from the game world. The manager's OnDeactivate()
    method will be called.
*/
void
Server::RemoveManager(Manager* manager)
{
    n_assert(manager);
    int index = this->managers.FindIndex(manager);
    if (-1 != index)
    {
        this->managers[index]->OnDeactivate();
        this->managers.Erase(index);
    }
}

//------------------------------------------------------------------------------
/**
    Finds pointer to an attached manager object by its class id. Returns
    0 if no such manager is attached.
*/
Manager*
Server::FindManager(const Foundation::Rtti& classId) const
{
    int num = this->managers.Size();
    for (int i = 0; i < num; i++)
    {
        if (this->managers[i]->IsA(classId))
        {
            return this->managers[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    This method returns true if a manager of the given class is currently
    attached.
*/
bool
Server::HasManager(const Foundation::Rtti& classId) const
{
    return (0 != this->FindManager(classId));
}

//------------------------------------------------------------------------------
/**
    This method flushes all unsaved state to the world database. It will
    call the Save() and OnSave() methods on all entities and managers.
*/
void
Server::Save()
{
    Db::Server* dbServer = Db::Server::Instance();

    dbServer->BeginTransaction();

    // finally, let the managers save their state to the database
    int managerIndex;
    int numManagers = this->managers.Size();
    for (managerIndex = 0; managerIndex < numManagers; managerIndex++)
    {
        this->managers[managerIndex]->OnSave();
    }

    // first call the OnSave() method on all entities, so they can prepare their
    // attributes for saving to the world database
    const nArray<Ptr<Entity> >& entities = Managers::EntityManager::Instance()->GetEntities();
    int entityIndex;
    int numEntities = entities.Size();
    if (numEntities > 0)
    {
        for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
        {
            entities[entityIndex]->OnSave();
        }

        // now let the entities actually save themselves to the database
        Ptr<Db::Writer> dbWriter = Db::Writer::Create();
        dbWriter->SetTableName("_Entities");
        dbWriter->SetPrimaryKey(Attr::GUID);
        if (dbWriter->Open())
        {
            for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
            {
                entities[entityIndex]->SaveAttributesToDbWriter(dbWriter);
            }
            dbWriter->Close();
        }
    }

    // and save the global attributes...
    dbServer->SaveGlobalAttributes();
    dbServer->EndTransaction();
}

//------------------------------------------------------------------------------
/**
    This method calls the OnLoad() method on all entities and all managers.
    Unlike the Save() method, this method will not do any loading from
    the database, instead it will be called after actual loading has already
    happened. The method is here to give entities and managers a chance
    to do some additional initialization after all world state has
    been loaded form the database.
*/
void
Server::Load()
{
    Loader::Server* loaderServer = Loader::Server::Instance();

    // call the OnLoad() method on all entities
    loaderServer->SetProgressText("Calling OnLoad() on loaded entities...");
    loaderServer->UpdateProgressDisplay();
    const nArray<Ptr<Entity> >& entities = Managers::EntityManager::Instance()->GetEntities();
    int entityIndex;
    int numEntities = entities.Size();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        entities[entityIndex]->OnLoad();
    }

    // now call the OnLoad() method on all managers
    int managerIndex;
    int numManagers = this->managers.Size();
    for (managerIndex = 0; managerIndex < numManagers; managerIndex++)
    {
        // update progress display
        nString progressText;
        progressText.Format("Calling OnLoad() on manager '%s'", this->managers[managerIndex]->GetClassName().Get());
        loaderServer->SetProgressText(progressText);
        loaderServer->UpdateProgressDisplay();

        // invoke OnLoad() on manager
        this->managers[managerIndex]->OnLoad();
    }
    loaderServer->SetProgressText("Game::Server::Load() done!");
    loaderServer->UpdateProgressDisplay();
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of the level.
*/
void
Server::RenderDebug()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->BeginShapes();
    for (int i = 0; i < this->managers.Size(); i++)
    {
        n_assert(this->managers[i].isvalid());
        if (this->managers[i]->IsActive())
        {
            this->managers[i]->OnRenderDebug();
        }
    }
    gfxServer->EndShapes();
}

} // namespace Game
