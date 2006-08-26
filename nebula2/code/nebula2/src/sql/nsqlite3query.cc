//------------------------------------------------------------------------------
//  sql/nsqlite3query.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "sql/nsqlite3query.h"
#include "sql/nsqlite3database.h"
#include "util/nfixedarray.h"

//------------------------------------------------------------------------------
/**
*/
nSQLite3Query::nSQLite3Query(nSQLite3Database* db) :
    refDatabase(db),
    columns(32, 32),
    sqlite3ResNumRows(0),
    sqlite3ResNumCols(0),
    sqlite3Result(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSQLite3Query::~nSQLite3Query()
{
    this->Clear();
}

//------------------------------------------------------------------------------
/**
    This clears the current result if such exists.
*/
void
nSQLite3Query::Clear()
{
    this->columns.Clear();
    this->sqlite3ResNumRows = 0;
    this->sqlite3ResNumCols = 0;
    if (0 != this->sqlite3Result)
    {
        sqlite3_free_table(this->sqlite3Result);
        this->sqlite3Result = 0;
    }
}

//------------------------------------------------------------------------------
/**
    This sets the SQL query statement, but does not validate or execute it!
    It will however, clear the current result.
*/
void
nSQLite3Query::SetSqlStatement(const nString& sql)
{
    this->Clear();
    this->sqlStatement = sql;
}

//------------------------------------------------------------------------------
/**
    Returns the currently set SQL statement.
*/
const nString&
nSQLite3Query::GetSqlStatement() const
{
    return this->sqlStatement;
}

//------------------------------------------------------------------------------
/**
    This returns all non-empty columns in the result. A non-empty column is
    a column where at least one row-field is not empty. This is valid
    after Execute() has been invoked.
*/
const nArray<nString>&
nSQLite3Query::GetColumns() const
{
    return this->columns;
}

//------------------------------------------------------------------------------
/**
    Returns the number of rows in the result. This is valid after Execute()
    has been invoked.
*/
int
nSQLite3Query::GetNumRows() const
{
    return this->sqlite3ResNumRows;
}

//------------------------------------------------------------------------------
/**
    Returns a single row of data from the result. If there are any empty
    fields in the row they will be ignored. So the column layout of the
    row may be different from what you get from the GetColumns() method.
*/
nSqlRow
nSQLite3Query::GetRow(int rowIndex) const
{
    n_assert(this->sqlite3Result);
    n_assert((rowIndex >= 0) && (rowIndex < this->sqlite3ResNumRows));
    nSqlRow sqlRow;

    // check if there actually is a result, otherwise return an empty row object
    int colIndex;
    for (colIndex = 0; colIndex < this->sqlite3ResNumCols; colIndex++)
    {
        char* res = this->sqlite3Result[(rowIndex + 1) * this->sqlite3ResNumCols + colIndex];
        if (res)
        {
            // a non-null field, add it to the row
            sqlRow.Set(this->sqlite3Result[colIndex], res);
        }
    }
    return sqlRow;
}

//------------------------------------------------------------------------------
/**
    This executes the stored query and internally stores the result.
*/
bool
nSQLite3Query::Execute()
{
    // clear the previous result (if exists)
    this->Clear();

    // execute the query
    int err = sqlite3_get_table(this->refDatabase->GetDatabaseHandle(),
                                this->sqlStatement.Get(),
                                &this->sqlite3Result,
                                &this->sqlite3ResNumRows,
                                &this->sqlite3ResNumCols,
                                0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Query::Execute(): sqlite3_get_table() failed with '%s'", sqlite3_errmsg(this->refDatabase->GetDatabaseHandle()));
        return false;
    }

    // find all non-empty columns
    this->FindValidColumns();
    return true;
}

//------------------------------------------------------------------------------
/**
    This goes through SQLite3's query result, finds all non-empty
    columns, and builds the columns array from it.
*/
void
nSQLite3Query::FindValidColumns()
{
    n_assert(this->columns.Size() == 0);
    n_assert(this->sqlite3Result);

    // count number of valid fields in each column
    nFixedArray<int> validFields(this->sqlite3ResNumCols);
    validFields.Clear(0);
    int rowIndex;
    for (rowIndex = 0; rowIndex < this->sqlite3ResNumRows; rowIndex++)
    {
        int colIndex;
        for (colIndex = 0; colIndex < this->sqlite3ResNumCols; colIndex++)
        {
            char* res = this->sqlite3Result[(rowIndex + 1) * this->sqlite3ResNumCols + colIndex];
            if (0 != res)
            {
                // a valid field
                validFields[colIndex]++;
            }
        }
    }

    // gather all columns with at least one valid field
    int colIndex;
    for (colIndex = 0; colIndex < this->sqlite3ResNumCols; colIndex++)
    {
        if (validFields[colIndex] > 0)
        {
            this->columns.Append(this->sqlite3Result[colIndex]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Returns the complete result as a string. This is only useful
    for scripting.
*/
nString
nSQLite3Query::GetResultAsString() const
{
    return nString("NOT IMPLEMENTED YET");
}


