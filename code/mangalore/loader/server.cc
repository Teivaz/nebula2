//------------------------------------------------------------------------------
//  loader/server.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "loader/server.h"
#include "foundation/factory.h"
#include "loader/levelloader.h"

namespace Loader
{
ImplementRtti(Loader::Server, Foundation::RefCounted);
ImplementFactory(Loader::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    debugTextEnabled(true)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the loader subsystem.

    @param  applicationName     the app name
    @param  fileFormatVersion   string in the form "x.y" describing the file format version
    @return                     true if loader subsystem successfully opened
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);

    // setup default user profile
    UserProfile* userProfile = this->CreateUserProfile();
    userProfile->SetName("default");
    userProfile->Open();
    this->SetUserProfile(userProfile);
  
    // create progress indicator window
    this->progressIndicator = UI::ProgressBarWindow::Create();
    this->progressIndicator->SetResource("gui/progressbar");

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the loader subsystem.
*/
void
Server::Close()
{
    n_assert(this->isOpen);

    // close progress indicator window
    if (this->progressIndicator->IsOpen())
    {
        this->progressIndicator->Close();
    }
    this->progressIndicator = 0;

    // remove all loader from list
    this->RemoveAllLoaders();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Creates a new user profile object. Override in subclass to create
    your own user profile subclass instances.
*/
UserProfile*
Server::CreateUserProfile() const
{
    return UserProfile::Create();
}


//------------------------------------------------------------------------------
/**
    Creates a new character profile object. Override in subclass to create
    your own character profile subclass instances.
*/
CharacterProfile*
Server::CreateCharacterProfile() const
{
    return CharacterProfile::Create();
}

//------------------------------------------------------------------------------
/**
    Load a new game level from the world database.

    @param  filename    a level name (this is not a filename!)
    @return             success
*/
bool
Server::LoadLevel(const nString& levelName)
{
    n_assert(levelName.IsValid());
    
    // load the level...
    bool success = LevelLoader::Load(levelName);

    // done
    return success;
}

//------------------------------------------------------------------------------
/**
    Attach new entity loader to server.

    @param  loader    loader class
*/
void
Server::AttachLoader(EntityLoaderBase* loader)
{
    n_assert(0 != loader);

    this->entityLoaders.Append(loader);
}

//------------------------------------------------------------------------------
/**
    Remove given loader from server.

    @param  loader    loader class
*/
void
Server::RemoveLoader(EntityLoaderBase* loader)
{
    n_assert(0 != loader);

    int index = GetEntityLoaderIndex(loader);
    n_assert(index > -1);

    this->entityLoaders.Erase(index);
}

//------------------------------------------------------------------------------
/**
    Remove all loaders.
*/
void
Server::RemoveAllLoaders()
{
    this->entityLoaders.Clear();
}

//------------------------------------------------------------------------------
/**
    Get index of given loader

    @param  loader    loader class
    @return index     index of loader in list, returns -1 if no one found.
*/
int
Server::GetEntityLoaderIndex(EntityLoaderBase* loader) const
{
    n_assert(0 != loader);

    int loaderIndex = -1;
    int i;
    for (i = 0; i < this->entityLoaders.Size(); i++)
    {
        if (this->entityLoaders[i].get() == loader)
        {
            loaderIndex = i;
        }
    }

    return loaderIndex;
}

//------------------------------------------------------------------------------
/**
    Go thru all entity loader and call its Load function.
*/
void
Server::LoadEntities(Db::Reader* dbReader)
{
    n_assert(0 != dbReader);
    int i;
    for (i = 0; i < this->entityLoaders.Size(); i++)
    {
        this->entityLoaders[i]->Load(dbReader);
    }
}

} // namespace Loader
