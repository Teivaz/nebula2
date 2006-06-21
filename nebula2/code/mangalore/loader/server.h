#ifndef LOADER_SERVER_H
#define LOADER_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Loader::Server

    The Loader::Server is the central object of the loader subsystem
    Usually you don't work directly with the Loader subsystem, but instead
    use higher level classes like the Game::SetupManager and
    Game::SaveGameManager.

    (C) 2003 RadonLabs GmbH
*/
#include "loader/userprofile.h"
#include "foundation/ptr.h"
#include "loader/entityloaderbase.h"

//------------------------------------------------------------------------------
namespace Loader
{
class LevelLoader;

class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get server pointer
    static Server* Instance();
    /// open the loader subsystem
    virtual bool Open();
    /// close the loader subsystem
    virtual void Close();
    /// create a new user profile object
    virtual UserProfile* CreateUserProfile() const;
    /// set the current user profile
    void SetUserProfile(UserProfile* p);
    /// get the current user profile
    UserProfile* GetUserProfile() const;
    /// load a new level, this method is usually called by Game::SetupManager
    virtual bool LoadLevel(const nString& levelName);
    /// attach loader
    void AttachLoader(EntityLoaderBase* loader);
    /// remove loader
    void RemoveLoader(EntityLoaderBase* loader);
    /// remove all loader
    void RemoveAllLoader();

private:
    friend class LevelLoader;
    /// load entities from db with entityloader
    void LoadEntities(const nString& levelName);
    /// get entity loader index
    int GetEntityLoaderIndex(EntityLoaderBase* loader) const;

    static Server* Singleton;
    bool isOpen;
    Ptr<UserProfile> userProfile;
    nArray<Ptr<EntityLoaderBase> > entityLoader;
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
    This sets the current user profile.
*/
inline
void
Server::SetUserProfile(UserProfile* p)
{
    this->userProfile = p;
}

//------------------------------------------------------------------------------
/**
    Returns the current user profile
*/
inline
UserProfile*
Server::GetUserProfile() const
{
    return this->userProfile;
}

} // namespace Input
//------------------------------------------------------------------------------
#endif
