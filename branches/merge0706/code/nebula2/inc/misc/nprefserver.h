#ifndef N_PREFSERVER_H
#define N_PREFSERVER_H
//------------------------------------------------------------------------------
/**
    @class nPrefServer
    @ingroup Misc

    @brief The pref server stores key/value pairs in a location where it will
    persist between application invocations (for instance by using the
    registry under Win32).

    NOTE: the interesting stuff happens in platform-specific subclasses.

    NOTE #2: it may be advisory to only open and close the nPrefServer
    right before/after writing/reading key/value pairs. Some
    implementations might open/close the registry, or ini files
    between open/close. To prevent excessively long locks on those
    resources you should try to keep the prefs server open
    for as short periods as possible.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nPrefServer : public nRoot
{
public:
    /// constructor
    nPrefServer();
    /// destructor
    virtual ~nPrefServer();
    /// return singleton instance pointer
    static nPrefServer* Instance();
    /// set company name
    void SetCompanyName(const nString& n);
    /// get company name
    const nString& GetCompanyName() const;
    /// set application name
    void SetApplicationName(const nString& n);
    /// get application name
    const nString& GetApplicationName() const;
    /// open the preferences server
    virtual bool Open();
    /// close the preferences server
    virtual void Close();
    /// check if a key exists
    virtual bool KeyExists(const nString& key);
    /// return true if server is open
    bool IsOpen() const;
    /// write a string value
    bool WriteString(const nString& key, const nString& value);
    /// write an integer value
    bool WriteInt(const nString& key, int value);
    /// write a float value
    bool WriteFloat(const nString& key, float value);
    /// write a bool value
    bool WriteBool(const nString& key, bool value);
    /// write a vector4 value
    bool WriteVector4(const nString& key, const vector4& value);
    /// write a vector3 value
    bool WriteVector3(const nString& key, const vector3& value);
    /// read a string value
    nString ReadString(const nString& key);
    /// read an integer value
    int ReadInt(const nString& key);
    /// read a float value
    float ReadFloat(const nString& key);
    /// read a bool value
    bool ReadBool(const nString& key);
    /// read a vector4 value
    vector4 ReadVector4(const nString& key);
    /// read a vector3 value
    vector3 ReadVector3(const nString& key);

private:
    static nPrefServer* Singleton;

protected:
    /// write a generic value in string form
    virtual bool WriteGeneric(const nString& key, const nString& value);
    /// read a generic value in string form
    virtual nString ReadGeneric(const nString& key);

    nString companyName;
    nString appName;
    bool isOpen;
    bool isWriting;
    bool isReading;
};

//------------------------------------------------------------------------------
/**
*/
inline
nPrefServer*
nPrefServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nPrefServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nPrefServer::SetCompanyName(const nString& s)
{
    this->companyName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nPrefServer::GetCompanyName() const
{
    return this->companyName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nPrefServer::SetApplicationName(const nString& s)
{
    this->appName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nPrefServer::GetApplicationName() const
{
    return this->appName;
}

//------------------------------------------------------------------------------
#endif
