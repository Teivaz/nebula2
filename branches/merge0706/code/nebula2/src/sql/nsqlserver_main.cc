//------------------------------------------------------------------------------
//  sql/nsqlserver_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "sql/nsqlserver.h"

nNebulaScriptClass(nSqlServer, "nroot");
nSqlServer* nSqlServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nSqlServer::nSqlServer()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nSqlServer::~nSqlServer()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Create a new database object. Note that database objects are normal
    shared resources. Override this method in a subclass.
*/
nSqlDatabase*
nSqlServer::NewDatabase(const nString& resName)
{
    return 0;
}
