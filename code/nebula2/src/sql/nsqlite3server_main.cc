//------------------------------------------------------------------------------
//  sql/nsqlite3server_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "sql/nsqlite3server.h"
#include "resource/nresourceserver.h"
#include "sql/nsqldatabase.h"

nNebulaClass(nSQLite3Server, "sql::nsqlserver");

//------------------------------------------------------------------------------
/**
*/
nSQLite3Server::nSQLite3Server()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSQLite3Server::~nSQLite3Server()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSqlDatabase*
nSQLite3Server::NewDatabase(const nString& resName)
{
    // extract the actual database filename for sharing, this is not the usual
    // Nebula2 way, but keeps the database resource directory somewhat readable
    nString strippedName = resName.ExtractFileName();
    strippedName.StripExtension();
    nSqlDatabase* db = (nSqlDatabase*) nResourceServer::Instance()->NewResource("nsqlite3database", strippedName, nResource::Database);
    if (!db->IsLoaded())
    {
        db->SetFilename(resName);
        if (db->Load())
        {
            return db;
        }
    }
    // fallthrough: couldn't create database
    db->Release();
    return 0;
}
