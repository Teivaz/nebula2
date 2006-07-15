#ifndef N_SQLITE3QUERY_H
#define N_SQLITE3QUERY_H
//------------------------------------------------------------------------------
/**
    @class nSQLite3Query
    
    Wraps an SQL query for SQLite3 and provides access to the result.
    
    (C) 2005 Radon Labs GmbH
*/
#include "sqlite/sqlite3.h"
#include "sql/nsqlquery.h"
#include "kernel/nref.h"

class nSQLite3Database;

//------------------------------------------------------------------------------
class nSQLite3Query : public nSqlQuery
{
public:
    /// set the SQL statement
    virtual void SetSqlStatement(const nString& sql);
    /// get the SQL statement
    virtual const nString& GetSqlStatement() const;
    /// execute the query, this updates the stored result
    virtual bool Execute();
    /// get non-empty columns in the result
    virtual const nArray<nString>& GetColumns() const;
    /// return number of rows in the result
    virtual int GetNumRows() const;
    /// get a row of data, only non-empty fields will be returned
    virtual nSqlRow GetRow(int rowIndex) const;
    /// return the complete result as a string (only useful for scripting)
    virtual nString GetResultAsString() const;

private:
    friend class nSQLite3Database;

    /// destructor
    virtual ~nSQLite3Query();
    /// constructor
    nSQLite3Query(nSQLite3Database* db);
    /// clear the currently stored result
    void Clear();
    /// find all non-empty columns in the result
    void FindValidColumns();

    nRef<nSQLite3Database> refDatabase;
    nString sqlStatement;
    nArray<nString> columns;
    int sqlite3ResNumRows;       // num rows in SQLite3's returned result
    int sqlite3ResNumCols;       // num columns in SQLite3's returned result
    char** sqlite3Result;        // SQLite3's returned result
};
//------------------------------------------------------------------------------
#endif
