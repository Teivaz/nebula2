//------------------------------------------------------------------------------
//  loader/userprofile.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "loader/userprofile.h"
#include "loader/server.h"
#include "kernel/nfileserver2.h"

namespace Loader
{
ImplementRtti(Loader::UserProfile, Foundation::RefCounted);
ImplementFactory(Loader::UserProfile);

//------------------------------------------------------------------------------
/**
*/
UserProfile::UserProfile() :
    isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
UserProfile::~UserProfile()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    This static method returns the path to the profiles root directory
    for this application.
*/
nString
UserProfile::GetProfileRootDirectory()
{
    nString path;
    const nString& vendorName = Foundation::Server::Instance()->GetVendorName();
    const nString& appName    = Foundation::Server::Instance()->GetAppName();
    path.Format("user:%s/%s", vendorName.Get(), appName.Get());
    return path;
}

//------------------------------------------------------------------------------
/**
    Returns the path to the user's profile directory using the Nebula2
    filesystem path conventions.
*/
nString
UserProfile::GetProfileDirectory() const
{
    nString path;
    path.Format("%s/profiles/%s", GetProfileRootDirectory().Get(), this->GetName().Get());
    return path;
}

//------------------------------------------------------------------------------
/**
    Returns the path to the current world database.
*/
nString
UserProfile::GetDatabasePath() const
{
    nString path = this->GetProfileDirectory();
    path.Append("/world.db3");
    return path;
}

//------------------------------------------------------------------------------
/**
    Returns the path to the user's savegame directory (inside the profile
    directory) using the Nebula2 filesystem path conventions.
*/
nString
UserProfile::GetSaveGameDirectory() const
{
    nString path;
    path.Format("%s/save", this->GetProfileDirectory().Get());
    return path;
}

//------------------------------------------------------------------------------
/**
    Get the complete filename to a savegame file.
*/
nString
UserProfile::GetSaveGamePath(const nString& saveGameName) const
{
    nString path = this->GetSaveGameDirectory();
    path.Append("/");
    path.Append(saveGameName);
    return path;
}

//------------------------------------------------------------------------------
/**
    Set the user profile to its default state. This is empty in the 
    base class but should be overriden to something meaningful in
    application specific subclasses.
*/
void
UserProfile::SetToDefault()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This is a static method which returns the names of all user profiles
    which currently exist on disk.
*/
nArray<nString>
UserProfile::EnumProfiles()
{
    return nFileServer2::Instance()->ListDirectories(GetProfileRootDirectory());
}

//------------------------------------------------------------------------------
/**
    This static method deletes an existing user profile on disk.
*/
void
UserProfile::DeleteProfile(const nString& name)
{
    // FIXME
}

//------------------------------------------------------------------------------
/**
    Open the profile for reading and writing. This will open the embedded
    stream object.
*/
bool
UserProfile::Open()
{
    // build filename of profile file
    nString filename;
    filename.Format("%s/profile.xml", this->GetProfileDirectory().Get());

    this->stream.SetFilename(filename);
    if (this->stream.Open(nStream::ReadWrite))
    {
        // if new file, write a root node
        if (this->stream.FileCreated())
        {
            this->stream.BeginNode("Profile");
            this->stream.EndNode();
        }
        this->stream.SetToNode("/Profile");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Close the profile. This will save the profile back to disc.
*/
void
UserProfile::Close()
{
    this->stream.Close();
}

}; // namespace Loader



