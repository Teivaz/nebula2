//------------------------------------------------------------------------------
//  db/server.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "db/server.h"
#include "foundation/factory.h"
#include "db/query.h"
#include "sql/nsqlserver.h"
#include "kernel/nguid.h"

namespace Db
{
ImplementRtti(Db::Server, Foundation::RefCounted);
ImplementFactory(Db::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    dbFilename("db:world.db3")
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(Singleton);
    if (this->IsOpen())
    {
        this->Close();
    }
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the world database.
*/
bool
Server::Open()
{
    n_assert(!this->IsOpen());
    this->refSqlDatabase = nSqlServer::Instance()->NewDatabase(this->dbFilename);
    this->isOpen = this->refSqlDatabase.isvalid();    
    this->LoadGlobalAttributes();
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Close the database server. This releases all loaded databases.
*/
void
Server::Close()
{
    n_assert(this->IsOpen());
    this->SaveGlobalAttributes();
    this->refSqlDatabase->Release();
    n_assert(!this->refSqlDatabase.isvalid());
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Begins a database transaction.
*/
void
Server::BeginTransaction()
{
    if (this->IsOpen())
    {
        this->refSqlDatabase->BeginTransaction();
    }
}

//------------------------------------------------------------------------------
/**
    Finish a database transaction.
*/
void
Server::EndTransaction()
{
    if (this->IsOpen())
    {
        this->refSqlDatabase->EndTransaction();
    }
}

//------------------------------------------------------------------------------
/**
    This method will write a row of data to the database.
    A new table is created if necessary, and the table layout will be updated
    if necessary, so this is an extremely flexible way to store data in the
    database.
*/
bool
Server::WriteAttrs(const nString& tableName, const Attribute& key, const nArray<Attribute>& attrs)
{
    // convert attributes to a sql row
    nSqlRow sqlRow;
    int i;
    int num = attrs.Size();
    sqlRow.Set(key.GetName(), key.AsString());
    for (i = 0; i < num; i++)
    {
        sqlRow.Set(attrs[i].GetName(), attrs[i].AsString());
    }

    // make sure the table is alright
    if (this->refSqlDatabase->HasTable(tableName))
    {
        // table exists, make sure the table layout has all the required columns
        this->refSqlDatabase->UpdateTable(tableName, sqlRow.GetColumns());
    }
    else
    {
        // table doesn't exist yet, create a new one
        this->refSqlDatabase->CreateTable(tableName, sqlRow.GetColumns());
    }

    // check if the row exists, if yes, do an update row, if no insert row
    if (this->refSqlDatabase->HasRow(tableName, key.GetName(), key.AsString()))
    {
        this->refSqlDatabase->UpdateRow(tableName, key.GetName(), key.AsString(), sqlRow);
    }
    else
    {
        this->refSqlDatabase->InsertRow(tableName, sqlRow);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Write a single attribute to the database. This is just a convenient
    wrapper around WriteAttrs() if only one attribute should be written.
*/
bool
Server::WriteAttr(const nString& tableName, const Attribute& key, const Attribute& attr)
{
    nArray<Attribute> attrArray;
    attrArray.Append(attr);
    return this->WriteAttrs(tableName, key, attrArray);
}

//------------------------------------------------------------------------------
/**
    This method reads a row of attributes from the database. An empty 
    array is returned if the table or row doesn't exist in the database.
*/
nArray<Attribute>
Server::ReadAttrs(const nString& tableName, const Attribute& key)
{
    nArray<Attribute> result;
    if (this->refSqlDatabase->HasTable(tableName))
    {
        nString sql;
        sql.Format("SELECT * FROM '%s' WHERE %s='%s'", tableName.Get(), key.GetName().Get(), key.AsString().Get());
        Ptr<Query> query = this->CreateQuery(sql);
        query->Execute();
        if (query->GetNumRows() > 0)
        {
            if (query->GetNumRows() > 1)
            {
                // more then one result row, this could be an incosistent database
                n_error("Server::ReadAttrs(): query for key '%s'='%s' in table '%s' has more then one match!", 
                    tableName.Get(), key.GetName().Get(), key.AsString().Get());
            }
            else
            {
                int colIndex;
                int numColumns = query->GetNumColumns();
                for (colIndex = 0; colIndex < numColumns; colIndex++)
                {
                    result.Append(query->GetAttr(colIndex, 0));
                }
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Read a single attribute value from the database.
*/
bool
Server::ReadAttr(const nString& tableName, const Attribute& key, Attribute& inOutAttr)
{
    if (this->refSqlDatabase->HasTable(tableName))
    {
        nString sql;
        sql.Format("SELECT %s FROM '%s' WHERE %s='%s'", inOutAttr.GetName().Get(), tableName.Get(), key.GetName().Get(), key.AsString().Get());
        Ptr<Query> query = this->CreateQuery(sql);
        query->Execute();
        if (query->GetNumRows() > 0)
        {
            if (query->GetNumRows() > 1)
            {
                // more then one result row, this could be an incosistent database
                n_error("Server::ReadAttr(): query for key '%s'='%s' in table '%s' has more then one match!", 
                    tableName.Get(), key.GetName().Get(), key.AsString().Get());
            }
            else
            {
                inOutAttr = query->GetAttr(0, 0);
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Create an empty query object.
*/
Query*
Server::CreateQuery() const
{
    return Query::Create();
}

//------------------------------------------------------------------------------
/**
    Create a universal query object.
*/
Query*
Server::CreateQuery(const nString& sqlStatement) const
{
    Query* query = Query::Create();
    query->SetSqlStatement(sqlStatement);
    return query;
}

//------------------------------------------------------------------------------
/**
    Create a predefined query object which returns all the level names in
    the world database.
*/
Query*
Server::CreateLevelsQuery() const
{
    Query* query = Query::Create();
    query->SetSqlStatement("SELECT * FROM _Entities WHERE _Type='TEMPLATE' AND '_Category'='.Levels'");
    return query;
}

//------------------------------------------------------------------------------
/**
    Create a predefined query object which returns the data for a specific 
    level.
*/
Query*
Server::CreateLevelQuery(const nString& levelName) const
{
    Query* query = Query::Create();
    nString sql;
    sql.Format("SELECT * FROM _Entities WHERE _Type='TEMPLATE' AND _Category='.Levels' AND Id='%s'", levelName.Get());
    query->SetSqlStatement(sql);
    return query;
}

//------------------------------------------------------------------------------
/**
    Create a predefined query object which returns all entity categories in
    the world database.
*/
Query*
Server::CreateCategoriesQuery() const
{
    Query* query = Query::Create();
    query->SetSqlStatement("SELECT * FROM '_Categories'");
    return query;
}

//------------------------------------------------------------------------------
/**
    Create a predefined query object which return the data for a specific
    entity category.
*/
Query*
Server::CreateCategoryQuery(const nString& categoryName) const
{
    Query* query = Query::Create();
    nString sql;
    sql.Format("SELECT * FROM '_Categories' WHERE Name='%s'", categoryName.Get());
    query->SetSqlStatement(sql);
    return query;
}

//------------------------------------------------------------------------------
/**
    Create a predefined query object which returns the guids for all entities in 
    a level which belong to a specific category.
*/
Query*
Server::CreateLevelCategoryGuidQuery(const nString &levelName, const nString &categoryName) const
{
    n_assert(levelName.IsValid() && categoryName.IsValid());
    Query* query = Query::Create();
    nString sql;
    sql.Format("SELECT GUID FROM '_Entities' WHERE _Type='INSTANCE' AND _Level='%s' AND _Category='%s'", levelName.Get(), categoryName.Get());
    query->SetSqlStatement(sql);
    return query;
}

//------------------------------------------------------------------------------
/**
    Create a predefined query object which returns all data for all entities in 
    a level which belong to a specific category.
*/
Query*
Server::CreateLevelCategoryQuery(const nString &levelName, const nString &categoryName) const
{
    n_assert(levelName.IsValid() && categoryName.IsValid());
    Query* query = Query::Create();
    nString sql;
    sql.Format("SELECT * FROM '_Entities' WHERE _Type='INSTANCE' AND _Level='%s' AND _Category='%s'", levelName.Get(), categoryName.Get());
    query->SetSqlStatement(sql);
    return query;
}

//------------------------------------------------------------------------------
/**
    Create a predefined query object which returns all templates of a specific
    category.
*/
Query*
Server::CreateCategoryTemplateQuery(const nString& categoryName) const
{
    n_assert(categoryName.IsValid());
    Query* query = Query::Create();
    nString sql;
    sql.Format("SELECT * FROM '_Entities' WHERE _Type='TEMPLATE' AND _Category='%s'", categoryName.Get());
    query->SetSqlStatement(sql);
    return query;
}

//------------------------------------------------------------------------------
/**
    create specialized query which returns all templates of given category and a given id
*/
Query*
Server::CreateCategoryTemplateIdQuery(const nString& categoryName, const nString& templateName) const
{
    n_assert(categoryName.IsValid());
    Db::Query* query = Db::Query::Create();
    nString sql;
    sql.Format("SELECT * FROM '_Entities' WHERE _Type='TEMPLATE' AND _Category='%s' AND Id='%s'", categoryName.Get(),templateName.Get());
    query->SetSqlStatement(sql);
    return query;
}

//------------------------------------------------------------------------------
/**
    Create a predefined query object which returns the complete attribute
    row identified by an entity guid.
*/
Query*
Server::CreateGuidQuery(const nString& guid) const
{
    n_assert(guid.IsValid());
    Query* query = Query::Create();
    nString sql;
    sql.Format("SELECT * FROM '_Entities' WHERE _Type='INSTANCE' AND GUID='%s'", guid.Get());
    query->SetSqlStatement(sql);
    return query;
}

//------------------------------------------------------------------------------
/**
    Loads the global attributes from the database (if they exist).
*/
void
Server::LoadGlobalAttributes()
{
    n_assert(this->refSqlDatabase.isvalid());
    this->globalAttrs.Clear();
    if (this->refSqlDatabase->HasTable("GlobalAttributes"))
    {
        Ptr<Query> query = Query::Create();
        query->SetTableName("GlobalAttributes");
        query->BuildSelectStatement();
        if (query->Execute())
        {
            n_assert(query->GetNumRows() == 1);
            int colIndex;
            int numCols = query->GetNumColumns();
            for (colIndex = 0; colIndex < numCols; colIndex++)
            {
                this->globalAttrs.SetAttr(query->GetAttr(colIndex, 0));
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Saves the global attributes back into the database. Attributes
    not previously in the database will be added, if the globals table
    is missing altogether, it will be created.
*/
void
Server::SaveGlobalAttributes()
{
    if (this->IsOpen())
    {
        // first, clear the global attributes table...
        if (this->refSqlDatabase->HasTable("GlobalAttributes"))
        {
            this->refSqlDatabase->DeleteTable("GlobalAttributes");
        }

        // save global attributes back
        const nArray<Attribute>& attrs = this->globalAttrs.GetAttrs();
        if (attrs.Size() > 0)
        {
            this->WriteAttrs("GlobalAttributes", attrs[0], attrs);
        }
    }
}

} // namespace Db
