//------------------------------------------------------------------------------
//  db/server.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "db/server.h"
#include "foundation/factory.h"
#include "db/query.h"
#include "sql/nsqlserver.h"
#include "kernel/nguid.h"
#include "db/reader.h"
#include "db/writer.h"
#include "attr/attributes.h"

namespace Db
{
ImplementRtti(Db::Server, Foundation::RefCounted);
ImplementFactory(Db::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false)
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
    Create an empty query object.
*/
Query*
Server::CreateQuery() const
{
    return Query::Create();
}

//------------------------------------------------------------------------------
/**
    Create an universal query object.
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
        Ptr<Reader> reader = Reader::Create();
        reader->SetTableName("GlobalAttributes");
        if (reader->Open())
        {
            reader->SetToRow(0);
            nArray<Attribute> attrs = reader->GetAttrs();
            for (int i = 0; i < attrs.Size(); i++)
            {
                this->globalAttrs.SetAttr(attrs[i]);
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
    if (this->IsOpen() && (this->globalAttrs.GetAttrs().Size() > 0))
    {
        // simplest way to force overwrite of first row...
        if (this->refSqlDatabase->HasTable("GlobalAttributes"))
        {
            this->refSqlDatabase->DeleteTable("GlobalAttributes");
        }

        // write global attributes
        Ptr<Writer> writer = Writer::Create();
        writer->SetTableName("GlobalAttributes");
        writer->SetPrimaryKey(Attr::Name);
        if (writer->Open())
        {
            writer->BeginRow();
            writer->SetAttrs(this->globalAttrs.GetAttrs());
            writer->EndRow();
            writer->Close();
        }
    }
}

} // namespace Db
