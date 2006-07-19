#ifndef N_SQLQUERY_H
#define N_SQLQUERY_H
//------------------------------------------------------------------------------
/**
    @class nSqlQuery
    
    The nSqlQuery class represents a query into an SQL database and provides
    simple access to the query's result.
    
    (C) 2005 Radon Labs GmbH
*/
#include "kernel/nrefcounted.h"
#include "util/nstring.h"
#include "sql/nsqlrow.h"

//------------------------------------------------------------------------------
class nSqlQuery : public nRefCounted
{
public:
    /// set the SQL statement
    virtual void SetSqlStatement(const nString& sql) = 0;
    /// get the SQL statement
    virtual const nString& GetSqlStatement() const = 0;
    /// execute the query, this updates the stored result
    virtual bool Execute(bool failOnError = true) = 0;
    /// get non-empty columns in the result
    virtual const nArray<nString>& GetColumns() const = 0;
    /// return number of rows in the result
    virtual int GetNumRows() const = 0;
    /// get a row of data, only non-empty fields will be returned
    virtual nSqlRow GetRow(int rowIndex) const = 0;
    /// return the complete result as a string (only useful for scripting)
    virtual nString GetResultAsString() const = 0;
};
//------------------------------------------------------------------------------
#endif
