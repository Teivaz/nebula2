#ifndef LOADER_SERVER_H
#define LOADER_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Loader::Server

    The Loader::Server is the central object of the loader subsystem
    Usually you don't work directly with the Loader subsystem, but instead
    use higher level classes like the Game::SetupManager and
    Game::SaveGameManager.

    (C) 2006 RadonLabs GmbH
*/
#include "loader/userprofile.h"
#include "loader/characterprofile.h"
#include "foundation/ptr.h"
#include "loader/entityloaderbase.h"
#include "db/reader.h"
#include "ui/progressbarwindow.h"

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

    /// enable/disable debug text messages during load
    void SetDebugTextEnabled(bool b);
    /// get debug text enabled flag
    bool GetDebugTextEnabled() const;

    /// open the loader subsystem
    virtual bool Open();
    /// close the loader subsystem
    virtual void Close();
    /// return true if open
    bool IsOpen() const;

    /// create a new user profile object
    virtual UserProfile* CreateUserProfile() const;
    /// set the current user profile
    void SetUserProfile(UserProfile* p);
    /// get the current user profile
    UserProfile* GetUserProfile() const;

    /// create a new character profile object
    virtual CharacterProfile* CreateCharacterProfile() const;
    /// set the current character profile
    void SetCharacterProfile(CharacterProfile* p);
    /// get the current character profile
    CharacterProfile* GetCharacterProfile() const;

    /// load a new level, this method is usually called by Game::SetupManager
    virtual bool LoadLevel(const nString& levelName);
    /// attach loader
    void AttachLoader(EntityLoaderBase* loader);
    /// remove loader
    void RemoveLoader(EntityLoaderBase* loader);
    /// remove all loaders
    void RemoveAllLoaders();

    /// set progress indicator gui resource
    void SetProgressResource(const nString& r);
    /// get progress indicator gui resource
    const nString& GetProgressResource() const;
    /// set the max progress value
    void SetMaxProgressValue(int v);
    /// get the max progress value
    int GetMaxProgressValue() const;
    /// advance the progress indicator
    void AdvanceProgress(int amount);
    /// set optional progress text
    void SetProgressText(const nString& s);
    /// get optional progress text
    const nString& GetProgressText() const;
    /// update the progress indicator display
    void UpdateProgressDisplay();
    /// open the progress indicator
    void OpenProgressIndicator();
    /// close the progress indicator
    void CloseProgressIndicator();

    /// load entities from db with entityloader
    void LoadEntities(Db::Reader* dbReader);

private:
    friend class LevelLoader;
    /// get entity loader index
    int GetEntityLoaderIndex(EntityLoaderBase* loader) const;

    static Server* Singleton;
    bool isOpen;
    bool debugTextEnabled;
    Ptr<UserProfile> userProfile;
    Ptr<CharacterProfile> characterProfile;
    nArray<Ptr<EntityLoaderBase> > entityLoaders;

    Ptr<UI::ProgressBarWindow> progressIndicator;
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
*/
inline
void
Server::SetDebugTextEnabled(bool b)
{
    this->debugTextEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Server::GetDebugTextEnabled() const
{
    return this->debugTextEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Server::IsOpen() const
{
    return this->isOpen;
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



//------------------------------------------------------------------------------
/**
    This sets the current character profile.
*/
inline
void
Server::SetCharacterProfile(CharacterProfile* p)
{
    this->characterProfile = p;
}

//------------------------------------------------------------------------------
/**
    Returns the current user profile
*/
inline
CharacterProfile*
Server::GetCharacterProfile() const
{
    return this->characterProfile;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetProgressResource(const nString& s)
{
    n_assert(this->IsOpen());
    this->progressIndicator->SetResource(s);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Server::GetProgressResource() const
{
    n_assert(this->IsOpen());
    return this->progressIndicator->GetResource();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetMaxProgressValue(int v)
{
    n_assert(this->IsOpen());
    this->progressIndicator->SetMaxProgressValue(v);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Server::GetMaxProgressValue() const
{
    n_assert(this->IsOpen());
    return this->progressIndicator->GetMaxProgressValue();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::AdvanceProgress(int amount)
{
    n_assert(this->IsOpen());
    this->progressIndicator->AdvanceProgress(amount);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetProgressText(const nString& s)
{
    n_assert(this->IsOpen());
    this->progressIndicator->SetText(s);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Server::GetProgressText() const
{
    n_assert(this->IsOpen());
    return this->progressIndicator->GetText();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::OpenProgressIndicator()
{
    n_assert(this->IsOpen());
    this->progressIndicator->Open();
    this->progressIndicator->SetDebugTextEnabled(this->debugTextEnabled);
    this->progressIndicator->SetText("No Progress Indicator Text Set!");
    this->progressIndicator->Present();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::CloseProgressIndicator()
{
    n_assert(this->IsOpen());
    this->progressIndicator->Close();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::UpdateProgressDisplay()
{
    n_assert(this->IsOpen());
    if (this->progressIndicator->IsOpen())
    {
        this->progressIndicator->Present();
    }
}

} // namespace Input
//------------------------------------------------------------------------------
#endif
