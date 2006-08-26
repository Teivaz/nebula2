#ifndef N_SQLSERVER_H
#define N_SQLSERVER_H
//------------------------------------------------------------------------------
/**
    @class nSqlServer

    The server object of the Nebula2 SQL subsystem

    (C) 2005 Radon Labs GmbH
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nSqlDatabase;

class nSqlServer : public nRoot
{
public:
    /// constructor
    nSqlServer();
    /// destructor
    virtual ~nSqlServer();
    /// get instance pointer
    static nSqlServer* Instance();
    /// create an sql database object
    virtual nSqlDatabase* NewDatabase(const nString& resName);

private:
    static nSqlServer* Singleton;
};

//------------------------------------------------------------------------------
/**
*/
inline
nSqlServer*
nSqlServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif
