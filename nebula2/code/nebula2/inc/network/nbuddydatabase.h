#ifndef N_BUDDYDATABASE_H
#define N_BUDDYDATABASE_H
//------------------------------------------------------------------------------
/**
    @class nBuddyDatabase
    @ingroup Network

    @brief Central server for the buddydatabase


    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "util/nstring.h"
#include "sql/nsqldatabase.h"


class nBuddyDatabase : public nRoot
{
public:
    /// constructor
    nBuddyDatabase();
    /// destructor
    virtual ~nBuddyDatabase();
    /// get instance pointer
    static nBuddyDatabase* Instance();

    /// set buddy database filename
    void SetDatabaseFilename(const nString& n);
    /// get buddy database filename
    const nString& GetDatabaseFilename() const;

    /// open the buddy database
    virtual bool Open();
    /// close the buddy database
    virtual void Close();
    /// return true if open
    bool IsOpen() const;


    /// sets and creates a single profile attribute
    bool SetProfileAttr(nString& user,nString& game,nString& key,nString& value);
    /// gets a single profile attribute / returns false if attrbute does not exist
    bool GetProfileAttr(nString& user,nString& game,nString& key,nString& value);
    /// Creates a user in the Database
    bool CreateUser(nString& user,nString& password);
    /// Checks whether user exists or not
    bool DoesUserExist(nString& user);
    /// Checks whether game exists or not
    bool DoesGameExist(nString& game);
    /// Updates user game profile or creates one
    bool UpdateUserGameProfile(nString& user,nString& gameGuid);
    /// fetches the user passwords
    bool GetUserPassword(nString& user,nString& password);
    /// adds a buddy
    bool AddBuddy(nString& user,nString& buddy);
    /// get user id by username
    bool GetUserIdByName(nString& user,nString& id);
    /// generates a buddy list
    bool GetBuddyList(nString& user,nArray<nString>& buddylist);


private:

    static nBuddyDatabase* Singleton;

    nRef<nSqlDatabase> refSqlDatabase;
    nString dbFilename;
    bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline
nBuddyDatabase*
nBuddyDatabase::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBuddyDatabase::IsOpen() const
{
    return this->isOpen;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddyDatabase::SetDatabaseFilename(const nString& n)
{
    this->dbFilename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nBuddyDatabase::GetDatabaseFilename() const
{
    return this->dbFilename;
}


#endif
