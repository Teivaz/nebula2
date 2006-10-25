#ifndef LOADER_USERPROFILE_H
#define LOADER_USERPROFILE_H
//------------------------------------------------------------------------------
/**
    @class Loader::UserProfile

    An user profile represents a storage where all user specific
    data is kept across application restarts. This usually includes
    save games, options, and other per-user data. Mangalore applications should
    at least support a default profile, but everything is there to
    support more then one user profile.

    User profiles are stored in "user:[appname]/profiles/[profilename]".

    (C) 2006 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "util/nstream.h"

//------------------------------------------------------------------------------
namespace Loader
{
class UserProfile : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(UserProfile);

public:
    /// constructor
    UserProfile();
    /// destructor
    virtual ~UserProfile();
    /// static method to enumerate all existing user profiles
    static nArray<nString> EnumProfiles();
    /// static method to delete an existing user profile by name
    static void DeleteProfile(const nString& name);
    /// static method which returns a path to the profile root directory
    static nString GetProfileRootDirectory();
    /// set the name of the user profile
    void SetName(const nString& n);
    /// get the name of the user profile
    const nString& GetName() const;
    /// set the user profile to its default state, override in subclass
    virtual void SetToDefault();
    /// open the profile, profile will be created if it doesn't exist yet
    virtual bool Open();
    /// close the profile, profile will be saved back to disc on close
    virtual void Close();
    /// currently open?
    bool IsOpen() const;
    /// get the filesystem path to the user profile directory
    nString GetProfileDirectory() const;
    /// get the filesystem path to the savegame directory
    nString GetSaveGameDirectory() const;
    /// get path to world database
    nString GetDatabasePath() const;
    /// get path to a complete savegame
    nString GetSaveGamePath(const nString& saveGameName) const;
    /// return true if attribute exists in the profile
    bool HasAttr(const nString& name) const;
    /// set a string attribute in the profile
    void SetString(const nString& name, const nString& val);
    /// set an int attribute in the profile
    void SetInt(const nString& name, int val);
    /// set a float attribute in the profile
    void SetFloat(const nString& name, float val);
    /// set a bool attribute in the profile
    void SetBool(const nString& name, bool val);
    /// set a vector3 attribute in the profile
    void SetVector3(const nString& name, const vector3& val);
    /// set a vector4 attribute in the profile
    void SetVector4(const nString& name, const vector4& val);
    /// get string attribute from the profile
    nString GetString(const nString& name) const;
    /// get int attribute from the profile
    int GetInt(const nString& name) const;
    /// get float attribute from the profile
    float GetFloat(const nString& name) const;
    /// get bool attribute from the profile
    bool GetBool(const nString& name) const;
    /// get vector3 attribute from the profile
    vector3 GetVector3(const nString& name) const;
    /// get vector4 attribute from the profile
    vector4 GetVector4(const nString& name) const;

protected:
    bool isOpen;
    nString name;
    nStream stream;
};

RegisterFactory(UserProfile);

//------------------------------------------------------------------------------
/**
*/
inline
void
UserProfile::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
UserProfile::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
UserProfile::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
UserProfile::SetString(const nString& name, const nString& val)
{
    this->stream.SetString(name, val);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
UserProfile::SetInt(const nString& name, int val)
{
    this->stream.SetInt(name, val);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
UserProfile::SetFloat(const nString& name, float val)
{
    this->stream.SetFloat(name, val);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
UserProfile::SetBool(const nString& name, bool val)
{
    this->stream.SetBool(name, val);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
UserProfile::SetVector3(const nString& name, const vector3& val)
{
    this->stream.SetVector3(name, val);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
UserProfile::SetVector4(const nString& name, const vector4& val)
{
    this->stream.SetVector4(name, val);
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
UserProfile::GetString(const nString& name) const
{
    return this->stream.GetString(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
UserProfile::GetInt(const nString& name) const
{
    return this->stream.GetInt(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
UserProfile::GetFloat(const nString& name) const
{
    return this->stream.GetFloat(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
UserProfile::GetBool(const nString& name) const
{
    return this->stream.GetBool(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3
UserProfile::GetVector3(const nString& name) const
{
    return this->stream.GetVector3(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
UserProfile::GetVector4(const nString& name) const
{
    return this->stream.GetVector4(name);
}

} // namespace Loader
//------------------------------------------------------------------------------
#endif
