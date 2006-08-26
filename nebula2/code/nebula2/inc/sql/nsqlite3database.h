#ifndef N_SQLITE3DATABASE_H
#define N_SQLITE3DATABASE_H
//------------------------------------------------------------------------------
/**
    @class nSQLite3Database

    Database frontend for SQLite3.

    (C) 2005 Radon Labs GmbH
*/
#include "sql/nsqldatabase.h"
#include "sqlite/sqlite3.h"

//------------------------------------------------------------------------------
class nSQLite3Database : nSqlDatabase
{
public:
    /// constructor
    nSQLite3Database();
    /// destructor
    virtual ~nSQLite3Database();
    /// create a new table in the database
    virtual bool CreateTable(const nString& tableName, const nArray<nString>& columns);
    /// delete a table in the database
    virtual void DeleteTable(const nString& tableName);
    /// rename an existing table
    virtual void RenameTable(const nString& tableName, const nString& newTableName);
    /// update table column layout
    virtual void UpdateTable(const nString& tableName, const nArray<nString>& columns);
    /// return true if a table exists in the database
    virtual bool HasTable(const nString& tableName);
    /// return array of tables in the database
    virtual nArray<nString> GetTables();
    /// return column titles in a table
    virtual nArray<nString> GetColumns(const nString& tableName);
    /// create an index for given table over the given columns
    virtual void CreateIndex(const nString& tableName, const nArray<nString>& indexedColumns);
    /// return true if an index for a table exists
    virtual bool HasIndex(const nString& tableName);
    /// delete an index from the database
    virtual void DeleteIndex(const nString& tableName);
    /// check if a row exists by single key/value pair
    virtual bool HasRow(const nString& tableName, const nString& keyColumn, const nString& keyVal);
    /// check if a row exists with complex WHERE clause
    virtual bool HasRow(const nString& tableName, const nString& whereClause);
    /// get a complete row from the database with single key/value pair
    virtual nSqlRow GetRow(const nString& tableName, const nString& keyColumn, const nString& keyVal);
    /// get a complete row with complex WHERE clause
    virtual nSqlRow GetRow(const nString& tableName, const nString& whereClause);
    /// simple row insertion
    virtual void InsertRow(const nString& tableName, const nSqlRow& row);
    /// row insertion or update (if it exists already)
    virtual void ReplaceRow(const nString& tableName, const nSqlRow& row);
    /// simple update of existing row with single key/value pair
    virtual void UpdateRow(const nString& tableName, const nString& keyColumn, const nString& keyVal, const nSqlRow& row);
    /// update of existing row with complexe WHERE clause
    virtual void UpdateRow(const nString& tableName, const nString& whereClause, const nSqlRow& row);
    /// delete an existing row with single key/value pair
    virtual void DeleteRow(const nString& tableName, const nString& keyColumn, const nString& keyVal);
    /// delete existing row with complex WHERE clause
    virtual void DeleteRow(const nString& tableName, const nString& whereClause);
    /// create an SQL query (create, but don't execute!)
    virtual nSqlQuery* CreateQuery(const nString& sqlStatement);
    /// get a pointer to the SQLite3 database handle
    sqlite3* GetDatabaseHandle() const;
    /// begin a transaction
    virtual void BeginTransaction();
    /// end a transaction
    virtual void EndTransaction();

protected:
    /// load texture resource (create rendertarget if render target resource)
    virtual bool LoadResource();
    /// unload texture resource
    virtual void UnloadResource();

private:
    sqlite3* sqliteHandle;
};

//------------------------------------------------------------------------------
/**
*/
inline
sqlite3*
nSQLite3Database::GetDatabaseHandle() const
{
    n_assert(this->sqliteHandle);
    return this->sqliteHandle;
}

//------------------------------------------------------------------------------
#endif
