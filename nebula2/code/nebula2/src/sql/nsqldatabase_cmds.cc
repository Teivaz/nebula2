//------------------------------------------------------------------------------
//  nsqldatabase_cmds.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "sql/nsqldatabase.h"
#include "sql/nsqlquery.h"

static void n_createtable(void* slf, nCmd* cmd);
static void n_deletetable(void* slf, nCmd* cmd);
static void n_renametable(void* slf, nCmd* cmd);
static void n_updatetable(void* slf, nCmd* cmd);
static void n_hastable(void* slf, nCmd* cmd);
static void n_gettables(void* slf, nCmd* cmd);
static void n_getcolumns(void* slf, nCmd* cmd);
static void n_createindex(void* slf, nCmd* cmd);
static void n_hasindex(void* slf, nCmd* cmd);
static void n_deleteindex(void* slf, nCmd* cmd);
static void n_hasrow(void* slf, nCmd* cmd);
static void n_getrow(void* slf, nCmd* cmd);
static void n_insertrow(void* slf, nCmd* cmd);
static void n_updaterow(void* slf, nCmd* cmd);
static void n_deleterow(void* slf, nCmd* cmd);
static void n_execute(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsqldatabase
    @cppclass
    nSqlDatabase
    @superclass
    nresource
    @classinfo
    Scripting interface for the SQL database class.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_createtable_ss",  'CRTB', n_createtable);
    cl->AddCmd("v_deletetable_s",   'DLTB', n_deletetable);
    cl->AddCmd("v_renametable_ss",  'RNTB', n_renametable);
    cl->AddCmd("v_updatetable_ss",  'UPTB', n_updatetable);
    cl->AddCmd("b_hastable_s",      'HSTB', n_hastable);
    cl->AddCmd("s_gettables_v",     'GTTB', n_gettables);
    cl->AddCmd("s_getcolumns_s",    'GTCL', n_getcolumns);
    cl->AddCmd("v_createindex_ss",  'CRIX', n_createindex);
    cl->AddCmd("b_hasindex_s",      'HSIX', n_hasindex);
    cl->AddCmd("v_deleteindex_s",   'DLIX', n_deleteindex);
    cl->AddCmd("b_hasrow_sss",      'HSRW', n_hasrow);
    cl->AddCmd("s_getrow_sss",      'GTRW', n_getrow);
    cl->AddCmd("v_insertrow_sss",   'INRW', n_insertrow);
    cl->AddCmd("v_updaterow_sssss", 'UPRW', n_updaterow);
    cl->AddCmd("v_deleterow_sss",   'DLRW', n_deleterow);
    cl->AddCmd("s_execute_s",       'EXEC', n_execute);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    createtable
    @input
    s(TableName), s(Columns)
    @output
    v
    @info
    Create a new table in the database.
*/
static void
n_createtable(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName = cmd->In()->GetS();
    nString columns   = cmd->In()->GetS();
    nArray<nString> columnTokens;
    columns.Tokenize(" \t", columnTokens);
    self->CreateTable(tableName, columnTokens);
}

//------------------------------------------------------------------------------
/**
    @cmd
    deletetable
    @input
    s(TableName)
    @output
    v
    @info
    Delete table from the database.
*/
static void
n_deletetable(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    self->DeleteTable(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    renametable
    @input
    s(TableName), s(NewTableName)
    @output
    v
    @info
    Rename a table in the database.
*/
static void
n_renametable(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName = cmd->In()->GetS();
    nString newTableName = cmd->In()->GetS();
    self->RenameTable(tableName, newTableName);
}

//------------------------------------------------------------------------------
/**
    @cmd
    updatetable
    @input
    s(TableName), s(Columns)
    @output
    v
    @info
    Add missing columns to table.
*/
static void
n_updatetable(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName = cmd->In()->GetS();
    nString columns = cmd->In()->GetS();
    nArray<nString> columnTokens;
    columns.Tokenize(" \t", columnTokens);
    self->UpdateTable(tableName, columnTokens);
}

//------------------------------------------------------------------------------
/**
    @cmd
    hastable
    @input
    s(TableName)
    @output
    b(TableExists)
    @info
    Return true if table exists in database.
*/
static void
n_hastable(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    cmd->Out()->SetB(self->HasTable(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettables
    @input
    v
    @output
    s(Tables)
    @info
    Return space separated string with all table names in the db.
*/
static void
n_gettables(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nArray<nString> tables = self->GetTables();
    cmd->Out()->SetS(nString::Concatenate(tables, " ").Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcolumns
    @input
    s(TableName)
    @output
    s(Columns)
    @info
    Return space separated string with all column names in a table.
*/
static void
n_getcolumns(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nArray<nString> columns = self->GetColumns(cmd->In()->GetS());
    cmd->Out()->SetS(nString::Concatenate(columns, " ").Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    createindex
    @input
    s(TableName), s(Columns)
    @output
    v
    @info
    Creates an index for the given table over the given columns.
*/
static void
n_createindex(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName = cmd->In()->GetS();
    nString columns   = cmd->In()->GetS();
    nArray<nString> columnTokens;
    columns.Tokenize(" \t", columnTokens);
    self->CreateIndex(tableName, columnTokens);
}

//------------------------------------------------------------------------------
/**
    @cmd
    hasindex
    @input
    s(TableName)
    @output
    b(IndexExists)
    @info
    Returns true if an index for a given table exists.
*/
static void
n_hasindex(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    cmd->Out()->SetB(self->HasIndex(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    deleteindex
    @input
    s(TableName)
    @output
    v
    @info
    Delete existing index for given table.
*/
static void
n_deleteindex(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    self->DeleteIndex(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    hasrow
    @input
    s(TableName), s(KeyColumn), s(KeyValue)
    @output
    b(Success)
    @info
    Return true if a row with the given key/value pair exists.
*/
static void
n_hasrow(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName = cmd->In()->GetS();
    nString keyColumn = cmd->In()->GetS();
    nString keyValue  = cmd->In()->GetS();
    nSqlRow row = self->GetRow(tableName, keyColumn, keyValue);
    cmd->Out()->SetS(nString::Concatenate(row.GetValues(), " ").Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrow
    @input
    s(TableName), s(KeyColumn), s(KeyValue)
    @output
    s(RowData)
    @info
    Get a complete row from the database identified by a key column/value
    pair. Returns space separated row data.
*/
static void
n_getrow(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName = cmd->In()->GetS();
    nString keyColumn = cmd->In()->GetS();
    nString keyValue  = cmd->In()->GetS();
    nSqlRow row = self->GetRow(tableName, keyColumn, keyValue);
    cmd->Out()->SetS(nString::Concatenate(row.GetValues(), " ").Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    insertrow
    @input
    s(TableName), s(Columns), s(Values)
    @output
    v
    @info
    Insert a new row into the database and initialize it with the given 
    space-separated values.
*/
static void
n_insertrow(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName = cmd->In()->GetS();
    nString columns = cmd->In()->GetS();
    nString values = cmd->In()->GetS();
    nArray<nString> columnTokens;
    columns.Tokenize(" \t", columnTokens);
    nArray<nString> valueTokens;
    values.Tokenize(" \t", valueTokens);
    nSqlRow row(columnTokens, valueTokens);
    self->InsertRow(tableName, row);
}

//------------------------------------------------------------------------------
/**
    @cmd
    updaterow
    @input
    s(TableName), s(KeyColumn), s(KeyValue), s(Columns), s(Values)
    @output
    v
    @info
    Update existing complete or partial row with new values.
*/
static void
n_updaterow(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName = cmd->In()->GetS();
    nString keyColumn = cmd->In()->GetS();
    nString keyValue  = cmd->In()->GetS();
    nString columns   = cmd->In()->GetS();
    nString values    = cmd->In()->GetS();
    nArray<nString> columnTokens;
    nArray<nString> valueTokens;
    columns.Tokenize(" \t", columnTokens);
    values.Tokenize(" \t", valueTokens);
    nSqlRow row(columnTokens, valueTokens);
    self->UpdateRow(tableName, keyColumn, keyValue, row);
}

//------------------------------------------------------------------------------
/**
    @cmd
    deleterow
    @input
    s(TableName), s(KeyColumn), s(KeyValue)
    @output
    v
    @info
    Delete a row from the database.
*/
static void
n_deleterow(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nString tableName  = cmd->In()->GetS();
    nString keyColumns = cmd->In()->GetS();
    nString keyValue   = cmd->In()->GetS();
    self->DeleteRow(tableName, keyColumns, keyValue);
}

//------------------------------------------------------------------------------
/**
    @cmd
    execute
    @input
    s(SQLStatement)
    @output
    s(Result)
    @info
    Execute an SQL statement and return the result as a string (see 
    nSqlResult::ToString() for information how this is structured).
*/
static void
n_execute(void* slf, nCmd* cmd)
{
    nSqlDatabase* self = (nSqlDatabase*) slf;
    nSqlQuery* query = self->CreateQuery(cmd->In()->GetS());
    query->Execute();
    cmd->Out()->SetS(query->GetResultAsString().Get());
    query->Release();
}
