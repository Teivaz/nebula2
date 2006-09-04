//------------------------------------------------------------------------------
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nsystem.h"
#include "kernel/nkernelserver.h"
#include "network/nbuddydatabase.h"
#include "sql/nsqlserver.h"
#include "util/nstring.h"
#include "sql/nsqlquery.h"
#include "util/nmd5.h"
#include "sqlite/sqlite3.h"
#include "sql/nsqlite3database.h"


nNebulaClass(nBuddyDatabase, "nroot");

nBuddyDatabase* nBuddyDatabase::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nBuddyDatabase::nBuddyDatabase() :
    isOpen(false),
    dbFilename("buddyserver_db.db3")
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nBuddyDatabase::~nBuddyDatabase()
{
    n_assert(Singleton);
    if (this->IsOpen())
    {
        this->Close();
    }
    Singleton = 0;
}




//------------------------------------------------------------------------------
/**
    Open the world database.
*/
bool
nBuddyDatabase::Open()
{
    n_assert(!this->IsOpen());
    this->refSqlDatabase = nSqlServer::Instance()->NewDatabase(this->dbFilename);
    this->isOpen = this->refSqlDatabase.isvalid();
    return this->isOpen;
}


//------------------------------------------------------------------------------
/**
    This releases the database
*/
void
nBuddyDatabase::Close()
{
    n_assert(this->IsOpen());
    this->refSqlDatabase->Release();
    n_assert(!this->refSqlDatabase.isvalid());
    this->isOpen = false;
}


bool nBuddyDatabase::SetProfileAttr(nString& user,nString& game,nString& key,nString& value)
{
    bool ret = false;
    nString sql;
    sql.Format("SELECT UserGameProfilID FROM UserGameProfil WHERE GameId=(SELECT GameId from Games where GUID='%s') and userid=(select userid from users where username LIKE LOWER('%s'))", game.Get(),user.Get());
    nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);

    // you have to double ' char (masking)
    nString masked_value = value.Substitute("'","''");

    if (sqlQuery->Execute())
    {

        if (sqlQuery->GetNumRows() > 0)
        {
            // profile found
            nSqlRow row = sqlQuery->GetRow(0);
            nString ProfilID = row.Get("UserGameProfilID");

            sql.Format("SELECT * FROM UserGameProfileAttribute WHERE UserGameProfilID='%s' and key='%s'",ProfilID.Get(),key.Get());
            sqlQuery->SetSqlStatement(sql);

            if (sqlQuery->Execute())
            {
                if (sqlQuery->GetNumRows() == 0)
                {
                    // key not found , create one
                    sql.Format("INSERT INTO UserGameProfileAttribute Values('%s','%s','%s')",ProfilID.Get(),key.Get(),masked_value.Get());
                    sqlQuery->SetSqlStatement(sql);
                    if (sqlQuery->Execute()) ret = true;
                }
                else
                {
                    // update key
                    sql.Format("UPDATE UserGameProfileAttribute SET VALUE='%s' WHERE KEY='%s' AND UserGameProfilID='%s'",masked_value.Get(),key.Get(),ProfilID.Get());
                    sqlQuery->SetSqlStatement(sql);
                    if (sqlQuery->Execute()) ret = true;
                }
            }
        }
    }

    sqlQuery->Release();
    return ret;
}

bool nBuddyDatabase::GetProfileAttr(nString& user,nString& game,nString& key,nString& value)
{
  bool ret = false;
    nString sql;
    sql.Format("SELECT UserGameProfilID FROM UserGameProfil WHERE GameId=(SELECT GameId from Games where GUID='%s') and userid=(select userid from users where username LIKE LOWER('%s'))", game.Get(),user.Get());
    nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);

    if (sqlQuery->Execute())
    {

        if (sqlQuery->GetNumRows() > 0)
        {
            // profile found
            nSqlRow row = sqlQuery->GetRow(0);
            nString ProfilID = row.Get("UserGameProfilID");

            sql.Format("SELECT * FROM UserGameProfileAttribute WHERE UserGameProfilID='%s' and key='%s'",ProfilID.Get(),key.Get());
            sqlQuery->SetSqlStatement(sql);

            if (sqlQuery->Execute())
            {
                if (sqlQuery->GetNumRows() == 0)
                {
                     ret = false;
                }
                else
                {
                    // get value
                    sql.Format("SELECT VALUE FROM UserGameProfileAttribute WHERE KEY='%s' AND UserGameProfilID='%s'",key.Get(),ProfilID.Get());
                    sqlQuery->SetSqlStatement(sql);
                    if (sqlQuery->Execute() && sqlQuery->GetNumRows() > 0)
                    {
                        nSqlRow row = sqlQuery->GetRow(0);
                        value = row.Get("Value");
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
        }
    }

    sqlQuery->Release();
    return ret;
}


bool nBuddyDatabase::UpdateUserGameProfile(nString& user,nString& gameGuid)
{
    bool ret = false;
    nString sql;
    sql.Format("SELECT UserGameProfilID FROM UserGameProfil WHERE GameId=(SELECT GameId from Games where GUID='%s') and userid=(select userid from users where username LIKE LOWER('%s'))", gameGuid.Get(),user.Get());
    nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);

    if (sqlQuery->Execute())
    {
        nString date = "DATETIME('NOW')";
        if (sqlQuery->GetNumRows() == 0)
        {
            sql.Format("INSERT INTO UserGameProfil Values(NULL,(select userid from users where username='%s'),(SELECT GameId from Games where GUID='%s'),%s)",user.Get(),gameGuid.Get(),date.Get());
            sqlQuery->SetSqlStatement(sql);
            if (sqlQuery->Execute()) ret = true;
        }
        else
        {
            // updates last login timestamp
            sql.Format("UPDATE UserGameProfil SET LastLogin=%s WHERE GameId=(SELECT GameId from Games where GUID='%s') and userid=(select userid from users where username='%s')",date.Get(),gameGuid.Get(),user.Get());
            sqlQuery->SetSqlStatement(sql);
            if (sqlQuery->Execute()) ret = true;
        }
    }

    sqlQuery->Release();
    return ret;
}


bool nBuddyDatabase::DoesGameExist(nString& game)
{
    nString sql;
    sql.Format("SELECT * FROM GAMES WHERE GUID='%s'", game.Get());
    nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);

    if (sqlQuery->Execute())
    {
        if (sqlQuery->GetNumRows() > 0)
        {
            sqlQuery->Release();
            return true;
        }
    }

    sqlQuery->Release();
    return false;
}


bool nBuddyDatabase::DoesUserExist(nString& user)
{
    nString sql;
    sql.Format("SELECT * FROM USERS WHERE Username LIKE LOWER('%s')", user.Get());
    nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);

    if (sqlQuery->Execute())
    {
        if (sqlQuery->GetNumRows() > 0)
        {
            sqlQuery->Release();
            return true;
        }
    }

    sqlQuery->Release();
    return false;
}

bool nBuddyDatabase::CreateUser(nString& user,nString& password)
{
    nMD5 alg;
    nString encodedPassword = alg.String2MD5(password.Get());

    nString sql;
    sql.Format("INSERT INTO Users VALUES(NULL,'%s','%s')", user.Get(),encodedPassword.Get());
    nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);
    if (sqlQuery->Execute())
    {
       sqlQuery->Release();
       return true;
    }

    sqlQuery->Release();
    return false;
}


bool nBuddyDatabase::GetUserPassword(nString& user,nString& password)
{
    nString sql;
    sql.Format("SELECT password FROM USERS WHERE Username LIKE LOWER('%s')", user.Get());
    nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);
    if (sqlQuery->Execute())
    {
       if (sqlQuery->GetNumRows() > 0)
       {
           nSqlRow row = sqlQuery->GetRow(0);
           password = row.Get("Password");
           sqlQuery->Release();
           return true;
       }
    }

    sqlQuery->Release();
    return false;
}



bool nBuddyDatabase::GetUserIdByName(nString& user,nString& id)
{
    nString sql;
    sql.Format("SELECT UserID FROM USERS WHERE Username LIKE LOWER('%s')", user.Get());
    nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);
    if (sqlQuery->Execute())
    {
       if (sqlQuery->GetNumRows() > 0)
       {
           nSqlRow row = sqlQuery->GetRow(0);
           id = row.Get("UserID");
           sqlQuery->Release();
           return true;
       }
    }

    sqlQuery->Release();
    return false;
}

bool nBuddyDatabase::AddBuddy(nString& user,nString& buddy)
{
    bool ret = false;
    nString userid;
    nString buddyid;

    nSqlQuery* sqlQuery = NULL;

    if (this->GetUserIdByName(user,userid) && this->GetUserIdByName(buddy,buddyid))
    {
        nString sql;
        sql.Format("SELECT * FROM Buddies WHERE userid='%s' and buddyid='%s'",userid.Get(),buddyid.Get());
        sqlQuery = this->refSqlDatabase->CreateQuery(sql);

        if (sqlQuery->Execute())
        {

           if (sqlQuery->GetNumRows() == 0)
           {
                sql.Format("INSERT INTO Buddies VALUES(%s,%s);",userid.Get(),buddyid.Get());
                sqlQuery->SetSqlStatement(sql);
                if (sqlQuery->Execute())
                {
                    ret = true;
                }
           }
           else  ret = false;
        }
    }

    if (sqlQuery) sqlQuery->Release();
    return ret;
}


bool nBuddyDatabase::GetBuddyList(nString& user,nArray<nString>& buddylist)
{

    nString userid;

    if (this->GetUserIdByName(user,userid))
    {
        nString sql;
        sql.Format("SELECT Username from USERS INNER JOIN (Select BuddyId from BUDDIES where userID='%s') AS B ON USERS.UserID = B.BuddyID", userid.Get());
        nSqlQuery* sqlQuery = this->refSqlDatabase->CreateQuery(sql);
        if (sqlQuery->Execute())
        {
           if (sqlQuery->GetNumRows() > 0)
           {
               for (int i=0;i < sqlQuery->GetNumRows();i++)
               {
                   buddylist.Append(sqlQuery->GetRow(i).Get("Username"));
               }

               sqlQuery->Release();
               return true;
           }
        }
    }

    return false;
}