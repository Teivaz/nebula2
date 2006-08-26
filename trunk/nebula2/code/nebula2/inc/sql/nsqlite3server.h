#ifndef N_SQLITE3SERVER_H
#define N_SQLITE3SERVER_H
//------------------------------------------------------------------------------
/**
    @class nSQLite3Server

    Offers access to SQLite3 databases.

    (C) 2005 Radon Labs GmbH
*/
#include "sql/nsqlserver.h"

//------------------------------------------------------------------------------
class nSQLite3Server : public nSqlServer
{
public:
    /// constructor
    nSQLite3Server();
    /// destructor
    virtual ~nSQLite3Server();
    /// create new database object
    nSqlDatabase* NewDatabase(const nString& resName);
};
//------------------------------------------------------------------------------
#endif
