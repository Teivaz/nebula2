//------------------------------------------------------------------------------
//  nsqlite3database_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "sql/nsqlite3database.h"
#include "kernel/nfileserver2.h"
#include "sql/nsqlite3query.h"

nNebulaClass(nSQLite3Database, "sql::nsqldatabase");

//------------------------------------------------------------------------------
/**
*/
nSQLite3Database::nSQLite3Database() :
    sqliteHandle(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSQLite3Database::~nSQLite3Database()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nSQLite3Database::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(0 == this->sqliteHandle);
    nFileServer2* fileServer = nFileServer2::Instance();

    // if db file doesn't exist yet, make sure the path to the file exists
    if (!fileServer->FileExists(this->GetFilename()))
    {
        fileServer->MakePath(this->GetFilename().ExtractDirName());
    }

    // mangle path because SQLite doesn't use Nebula2 file routines
    nString mangledPath = fileServer->ManglePath(this->GetFilename());
    mangledPath.ANSItoUTF8();
    int err = sqlite3_open(mangledPath.Get(), &this->sqliteHandle);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::LoadResource(): sqlite3_open(%s) failed with '%s'",
            mangledPath.Get(),
            sqlite3_errmsg(this->sqliteHandle));
        return false;
    }

    // run a few pragmas to improve performance...
    err = sqlite3_exec(this->sqliteHandle, "PRAGMA cache_size=10000;", 0, 0, 0);   // allow 15 Meg for cache
    n_assert(SQLITE_OK == err);
    err = sqlite3_exec(this->sqliteHandle, "PRAGMA synchronous=OFF;", 0, 0, 0);    // don't care about power failures...
    n_assert(SQLITE_OK == err);
    err = sqlite3_exec(this->sqliteHandle, "PRAGMA temp_store=MEMORY", 0, 0, 0);   // put temp stuff into memory
    n_assert(SQLITE_OK == err);

    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nSQLite3Database::UnloadResource()
{
    n_assert(this->IsLoaded());
    n_assert(0 != this->sqliteHandle);

    int err = sqlite3_close(this->sqliteHandle);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::Close(): sqlite3_close() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }
    this->sqliteHandle = 0;
    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
    This method creates a new SQL query object. The query is setup with
    the query string, but isn't executed.
*/
nSqlQuery*
nSQLite3Database::CreateQuery(const nString& sqlStatement)
{
    nSqlQuery* query = new nSQLite3Query(this);
    query->SetSqlStatement(sqlStatement);
    return query;
}

//------------------------------------------------------------------------------
/**
    This method checks if a table of the given name exists in the
    SQLite database.
*/
bool
nSQLite3Database::HasTable(const nString& tableName)
{
    n_assert(tableName.IsValid());
    n_assert(this->IsLoaded());
    n_assert(0 != this->sqliteHandle);

    // build an SQL statement...
    nString sql;
    sql.Format("SELECT name FROM 'sqlite_master' WHERE type='table' AND name='%s'", tableName.Get());

    // execute SQL statement...
    char** result = 0;
    int resNumRows = 0;
    int resNumCols = 0;
    int err = sqlite3_get_table(this->sqliteHandle, sql.Get(), &result, &resNumRows, &resNumCols, 0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::HasTable(): sqlite3_get_table() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }
    sqlite3_free_table(result);
    return (resNumRows != 0);
}

//------------------------------------------------------------------------------
/**
    The method returns a string array with the names of all tables in the
    database.
*/
nArray<nString>
nSQLite3Database::GetTables()
{
    n_assert(this->IsLoaded());
    n_assert(0 != this->sqliteHandle);

    // execute SQL statement which returns the tables...
    char** result = 0;
    int resNumRows = 0;
    int resNumCols = 0;
    int err = sqlite3_get_table(this->sqliteHandle,
                                "SELECT name FROM sqlite_master WHERE type='table'",
                                &result,
                                &resNumRows,
                                &resNumCols,
                                0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::UpdateTableArray(): sqlite3_get_table() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }

    // analyze the result
    n_assert(result);
    nArray<nString> tableArray;
    if (resNumCols > 0)
    {
        int row;
        for (row = 0; row < resNumRows; row++)
        {
            const char* tableName = result[row + 1];
            tableArray.Append(tableName);
        }
    }

    // free memory allocated by get_table()
    sqlite3_free_table(result);
    return tableArray;
}

//------------------------------------------------------------------------------
/**
    Delete a table from the database.
*/
void
nSQLite3Database::DeleteTable(const nString& tableName)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);
    n_assert(tableName.IsValid());
    nString sql;
    sql.Format("DROP TABLE '%s'", tableName.Get());
    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();
    query->Release();
}

//------------------------------------------------------------------------------
/**
    Creates a new table in the database. If the table already exists, the
    method fails.
*/
bool
nSQLite3Database::CreateTable(const nString& tableName, const nArray<nString>& columns, const nString& primaryKey)
{
    n_assert(this->IsLoaded());
    n_assert(0 != this->sqliteHandle);

    // check if table already exists
    if (this->HasTable(tableName))
    {
        return false;
    }

    // setup SQL statement to create the table
    nString sql;
    sql = "CREATE TABLE ";
    sql.Append(tableName);
    sql.Append("( ");
    int i;
    const int num = columns.Size();
    for(i = 0; i < num; i++)
    {
        if (i > 0)
        {
            sql.Append(", ");
        }
        sql.Append("'");
        sql.Append(columns[i]);
        sql.Append("' TEXT");
        if (primaryKey.IsValid() && (columns[i] == primaryKey))
        {
            sql.Append(" UNIQUE ON CONFLICT REPLACE");
        }
    }
    sql.Append(" )");

    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();
    query->Release();
    return true;
}

//------------------------------------------------------------------------------
/**
    This method renames a table in the database.
*/
void
nSQLite3Database::RenameTable(const nString& tableName, const nString& newTableName)
{
    nString sql;
    sql.Format("ALTER TABLE '%s' RENAME TO '%s'", tableName.Get(), newTableName.Get());

    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();
    query->Release();
}

//------------------------------------------------------------------------------
/**
    This makes sure that the table has the columns given as parameters.
    FIXME: this method will NOT remove columns from the table, since SQLite
    doesn't support the DROP COLUMN command in ALTER TABLE. To delete columns,
    a copy of the table has to be created like so:

    BEGIN TRANSACTION;
    CREATE TEMPORARY TABLE t1_backup(a,b);
    INSERT INTO t1_backup SELECT a,b FROM t1;
    DROP TABLE t1;
    CREATE TABLE t1(a,b);
    INSERT INTO t1 SELECT a,b FROM t1_backup;
    DROP TABLE t1_backup;
    COMMIT;
*/
void
nSQLite3Database::UpdateTable(const nString& tableName, const nArray<nString>& newColumns)
{
    nArray<nString> curColumns = this->GetColumns(tableName);
    nArray<nString> diffColumns = curColumns.Difference(newColumns);
    int i;
    int num = diffColumns.Size();
    for (i = 0; i < num; i++)
    {
        nString sql;
        sql.Format("ALTER TABLE '%s' ADD COLUMN '%s' TEXT", tableName.Get(), diffColumns[i].Get());

        nSqlQuery* query = this->CreateQuery(sql);
        query->Execute();
        query->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Returns the column titles of a table in the database.
*/
nArray<nString>
nSQLite3Database::GetColumns(const nString& tableName)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);

    // execute an SQLite specific SQL statement which returns at most one row of data
    nString sql;
    sql.Format("PRAGMA table_info(%s)", tableName.Get());
    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();

    nArray<nString> result;
    int i;
    for (i = 0; i < query->GetNumRows(); i++)
    {
        result.Append(query->GetRow(i).GetValues()[1]);
    }
    query->Release();
    return result;
}

//------------------------------------------------------------------------------
/**
    Create an index for the given table over the given columns.
*/
void
nSQLite3Database::CreateIndex(const nString& tableName, const nArray<nString>& indexedColumns)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);
    nString sql;
    sql.Format("CREATE INDEX %s_Index ON %s ( %s )", tableName.Get(), tableName.Get(), nString::Concatenate(indexedColumns, ","));
    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();
    query->Release();
}

//------------------------------------------------------------------------------
/**
    Return true if an index has been created for a table.
*/
bool
nSQLite3Database::HasIndex(const nString& tableName)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);
    nString sql;
    sql.Format("SELECT name FROM sqlite_master WHERE name='%s_Index'", tableName.Get());
    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();
    bool success = query->GetNumRows() > 0;
    query->Release();
    return success;
}

//------------------------------------------------------------------------------
/**
    Delete the index for given table.
*/
void
nSQLite3Database::DeleteIndex(const nString& tableName)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);
    nString sql;
    sql.Format("DROP INDEX %s_Index", tableName.Get());
    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();
    query->Release();
}

//------------------------------------------------------------------------------
/**
    Return true if at least one row identified by a WHERE clause
    exists in the database.
*/
bool
nSQLite3Database::HasRow(const nString& tableName, const nString& whereClause)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);
    nString sql;
    sql.Format("SELECT * FROM %s WHERE %s", tableName.Get(), whereClause.Get());
    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();
    bool hasRow = (query->GetNumRows() >= 1);
    query->Release();
    return hasRow;
}

//------------------------------------------------------------------------------
/**
    Return true if a row with the given key/value pair exists in the database.
*/
bool
nSQLite3Database::HasRow(const nString& tableName, const nString& keyColumn, const nString& keyValue)
{
    nString whereClause;
    whereClause.Format("%s='%s'", keyColumn.Get(), keyValue.Get());
    return this->HasRow(tableName, whereClause);
}

//------------------------------------------------------------------------------
/**
    Return a complete row of data identified by a WHERE clause.
*/
nSqlRow
nSQLite3Database::GetRow(const nString& tableName, const nString& whereClause)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);

    // construct and execute SQL statement which returns the wanted data
    nString sql;
    sql.Format("SELECT * FROM %s WHERE %s", tableName.Get(), whereClause.Get());
    nSqlQuery* query = this->CreateQuery(sql);
    query->Execute();
    if (query->GetNumRows() >= 1)
    {
        nSqlRow row = query->GetRow(0);
        query->Release();
        return row;
    }
    else
    {
        // no match, return empty row
        query->Release();
        nSqlRow emptyRow;
        return emptyRow;
    }
}

//------------------------------------------------------------------------------
/**
    Return a complete row of data identified by a key column/value pair.
*/
nSqlRow
nSQLite3Database::GetRow(const nString& tableName, const nString& keyColumn, const nString& keyValue)
{
    nString whereClause;
    whereClause.Format("\"%s\"='%s'", keyColumn.Get(), keyValue.Get());
    return this->GetRow(tableName, whereClause);
}

//------------------------------------------------------------------------------
/**
    Inserts a complete new row of data into the database.
*/
void
nSQLite3Database::InsertRow(const nString& tableName, const nSqlRow& row)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);

    // construct an SQL statement which does the insert operation
    nString sql("INSERT INTO ");
    sql.Append(tableName);
    sql.Append(" ('");
    sql.Append(nString::Concatenate(row.GetColumns(), "', '"));
    sql.Append("') VALUES ('");
    sql.Append(nString::Concatenate(row.GetValues(),  "', '"));
    sql.Append("')");
    int err = sqlite3_exec(this->sqliteHandle, sql.Get(), 0, 0, 0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::InsertRow(): sqlite3_exec() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }
}

//------------------------------------------------------------------------------
/**
    Inserts a complete new row of data into the database.
*/
void
nSQLite3Database::ReplaceRow(const nString& tableName, const nSqlRow& row)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);

    // construct an SQL statement which does the insert operation
    nString sql("INSERT OR REPLACE INTO ");
    sql.Append(tableName);
    sql.Append(" ('");
    sql.Append(nString::Concatenate(row.GetColumns(), "', '"));
    sql.Append("') VALUES ('");
    sql.Append(nString::Concatenate(row.GetValues(),  "', '"));
    sql.Append("')");
    int err = sqlite3_exec(this->sqliteHandle, sql.Get(), 0, 0, 0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::InsertRow(): sqlite3_exec() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }
}

//------------------------------------------------------------------------------
/**
    Update a complete row, or part of a row in the database, the row is
    identified by a WHERE clause.
*/
void
nSQLite3Database::UpdateRow(const nString& tableName, const nString& whereClause, const nSqlRow& row)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);
    n_assert(row.GetColumns().Size() == row.GetValues().Size());

    // construct an SQL statement which performs the update
    nString sql;
    sql.Format("UPDATE %s SET ", tableName.Get());
    int i;
    int num = row.GetColumns().Size();
    for (i = 0; i < num; i++)
    {
        nString assignment;
        assignment.Format("\"%s\"='%s'", row.GetColumns()[i].Get(), row.GetValues()[i].Get());
        if (i < (num - 1))
        {
            assignment.Append(", ");
        }
        sql.Append(assignment);
    }
    nString whereStr;
    whereStr.Format(" WHERE %s", whereClause.Get());
    sql.Append(whereStr);
    int err = sqlite3_exec(this->sqliteHandle, sql.Get(), 0, 0, 0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::UpdateRow(): sqlite3_exec() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }
}


//------------------------------------------------------------------------------
/**
    Update a complete row, or part of a row in the database identified
    by a single key/value pair.
*/
void
nSQLite3Database::UpdateRow(const nString& tableName, const nString& keyColumn, const nString& keyValue, const nSqlRow& row)
{
    nString whereClause;
    whereClause.Format("\"%s\"='%s'", keyColumn.Get(), keyValue.Get());
    return this->UpdateRow(tableName, whereClause, row);
}

//------------------------------------------------------------------------------
/**
    Delete a complete row from the database identified by a WHERE clause.
*/
void
nSQLite3Database::DeleteRow(const nString& tableName, const nString& whereClause)
{
    n_assert(this->IsLoaded());
    n_assert(this->sqliteHandle);

    nString sql;
    sql.Format("DELETE FROM %s", tableName.Get());

    // add optional where condition
    if (whereClause.IsValid())
    {
        sql.Append(nString(" WHERE ") + whereClause);
    }

    int err = sqlite3_exec(this->sqliteHandle, sql.Get(), 0, 0, 0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::DeleteRow(): sqlite3_exec() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }
}

//------------------------------------------------------------------------------
/**
    Delete a complete row from the database identified by a single
    key/value pair.
*/
void
nSQLite3Database::DeleteRow(const nString& tableName, const nString& keyColumn, const nString& keyValue)
{
    nString whereClause;
    whereClause.Format("%s='%s'", keyColumn.Get(), keyValue.Get());
    return this->DeleteRow(tableName, whereClause);
}

//------------------------------------------------------------------------------
/**
*/
void
nSQLite3Database::BeginTransaction()
{
    nString sql("BEGIN");
    int err = sqlite3_exec(this->sqliteHandle, sql.Get(), 0, 0, 0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::DeleteRow(): sqlite3_exec() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSQLite3Database::EndTransaction()
{
    nString sql("END");
    int err = sqlite3_exec(this->sqliteHandle, sql.Get(), 0, 0, 0);
    if (SQLITE_OK != err)
    {
        n_error("nSQLite3Database::DeleteRow(): sqlite3_exec() failed with '%s'", sqlite3_errmsg(this->sqliteHandle));
    }
}


