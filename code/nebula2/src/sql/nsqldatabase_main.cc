//------------------------------------------------------------------------------
//  nsqldatabase_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "sql/nsqldatabase.h"

nNebulaScriptClass(nSqlDatabase, "nresource");

//------------------------------------------------------------------------------
/**
*/
nSqlDatabase::nSqlDatabase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSqlDatabase::~nSqlDatabase()
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
nSqlDatabase::CreateTable(const nString& name, const nArray<nString>& columns, const nString& primayColumn)
{
    // empty, override in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::DeleteTable(const nString& name)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::RenameTable(const nString& tableName, const nString& newTableName)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::UpdateTable(const nString& tableName, const nArray<nString>& columns)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
bool
nSqlDatabase::HasTable(const nString& name)
{
    // empty, override in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nSqlDatabase::GetTables()
{
    // empty, override in subclass
    nArray<nString> emptyArray;
    return emptyArray;
}

//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nSqlDatabase::GetColumns(const nString& tableName)
{
    // empty, override in subclass
    nArray<nString> emptyArray;
    return emptyArray;
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::CreateIndex(const nString& tableName, const nArray<nString>& indexedColumns)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
bool
nSqlDatabase::HasIndex(const nString& tableName)
{
    // override in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::DeleteIndex(const nString& tableName)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
bool
nSqlDatabase::HasRow(const nString& tableName, const nString& keyColumn, const nString& keyValue)
{
    // empty, override in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSqlDatabase::HasRow(const nString& tableName, const nString& whereClause)
{
    // empty, override in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
nSqlRow
nSqlDatabase::GetRow(const nString& tableName, const nString& keyColumn, const nString& keyValue)
{
    // empty, override in subclass
    nSqlRow emptyRow;
    return emptyRow;
}

//------------------------------------------------------------------------------
/**
*/
nSqlRow
nSqlDatabase::GetRow(const nString& tableName, const nString& whereClause)
{
    // empty, override in subclass
    nSqlRow emptyRow;
    return emptyRow;
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::InsertRow(const nString& tableName, const nSqlRow& row)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::ReplaceRow(const nString& tableName, const nSqlRow& row)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::UpdateRow(const nString& tableName, const nString& keyColumn, const nString& keyValue, const nSqlRow& row)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::UpdateRow(const nString& tableName, const nString& whereClause, const nSqlRow& row)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::DeleteRow(const nString& tableName, const nString& keyColumn, const nString& keyValue)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::DeleteRow(const nString& tableName, const nString& whereClause)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
nSqlQuery*
nSqlDatabase::CreateQuery(const nString& sqlStatement)
{
    // empty, override in subclass
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::BeginTransaction()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nSqlDatabase::EndTransaction()
{
    // empty, override in subclass
}
