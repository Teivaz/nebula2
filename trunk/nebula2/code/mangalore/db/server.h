#ifndef DB_SERVER_H
#define DB_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Db::Server
    
    The DB server is a comfortable wrapper around the world database as
    created by the level exporter. It provides highlevel services which
    require a specific database layout, but also allows low level access
    directly to the database using SQL statements.
    
    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "sql/nsqldatabase.h"
#include "util/nstring.h"
#include "util/narray.h"
#include "db/attribute.h"
#include "db/attributecontainer.h"

//------------------------------------------------------------------------------
namespace Db
{
class Query;
    
class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get instance pointer
    static Server* Instance();
    
    //=== basic stuff ===

    /// set world database filename
    void SetDatabaseFilename(const nString& n);
    /// get world database filename
    const nString& GetDatabaseFilename() const;
    /// open the world database
    virtual bool Open();
    /// close the world database
    virtual void Close();
    /// return true if open
    bool IsOpen() const;
    /// return true if table exists in world database
    bool HasTable(const nString& tableName) const;
    /// begin a database transaction
    void BeginTransaction();
    /// end a database transaction
    void EndTransaction();
    /// gain direct access to database object
    virtual nSqlDatabase* GetSqlDatabase() const;

    //=== reading and writing attributes from/to database ===

    /// reading and writing data from/to database
    virtual bool WriteAttrs(const nString& tableName, const Attribute& key, const nArray<Attribute>& attrs);
    /// read a complete row of attributes from the db
    virtual nArray<Attribute> ReadAttrs(const nString& tableName, const Attribute& key);
    /// write a single attribute to the db, will create or update table if needed
    virtual bool WriteAttr(const nString& tableName, const Attribute& key, const Attribute& attr);
    /// read a single attribute from the db
    virtual bool ReadAttr(const nString& tableName, const Attribute& key, Attribute& inOutAttr);
    /// create an empty query object
    virtual Query* CreateQuery() const;

    //=== global attributes ===

    /// return true if global attribute exists
    bool HasGlobalAttr(Attr::AttributeID attrId) const;
    /// set a global string attribute
    void SetGlobalString(Attr::StringAttributeID attrId, const nString& value);
    /// get a global string attribute
    nString GetGlobalString(Attr::StringAttributeID attrId) const;
    /// set a global int attribute
    void SetGlobalInt(Attr::IntAttributeID attrId, int value);
    /// get a global int attribute
    int GetGlobalInt(Attr::IntAttributeID attrId) const;
    /// set a global float attribute
    void SetGlobalFloat(Attr::FloatAttributeID attrId, float value);
    /// get a global float attribute
    float GetGlobalFloat(Attr::FloatAttributeID attrId) const;
    /// set a global bool attribute
    void SetGlobalBool(Attr::BoolAttributeID attrId, bool value);
    /// get a global bool attribute
    bool GetGlobalBool(Attr::BoolAttributeID attrId) const;
    /// set a global vector3 attribute
    void SetGlobalVector3(Attr::Vector3AttributeID attrId, const vector3& value);
    /// get a global vector3 attribute
    const vector3& GetGlobalVector3(Attr::Vector3AttributeID attrId) const;
    /// set a global vector4 attribute
    void SetGlobalVector4(Attr::Vector4AttributeID attrId, const vector4& value);
    /// get a global vector4 attribute
    const vector4& GetGlobalVector4(Attr::Vector4AttributeID attrId) const;
    /// set a global matrix44 attribute
    void SetGlobalMatrix44(Attr::Matrix44AttributeID attrId, const matrix44& value);
    /// get a global matrix44 attribute
    const matrix44& GetGlobalMatrix44(Attr::Matrix44AttributeID attrId) const;
    /// save global attributes back to the database
    void SaveGlobalAttributes();

    // FIXME: make the following methods OBSOLETE?
    /// create a universal query object
    virtual Query* CreateQuery(const nString& sqlStatement) const;
    /// create specialized query which returns the data for all levels in the world database
    virtual Query* CreateLevelsQuery() const;
    /// create a specialzed query which returns the data for one level in the world database
    virtual Query* CreateLevelQuery(const nString& levelName) const;
    /// create specialized query which returns the data for all categories in the world database
    virtual Query* CreateCategoriesQuery() const;
    /// create specialized query which returns the data for one category in the world database
    virtual Query* CreateCategoryQuery(const nString& categoryQuery) const;
    /// create specialized query which returns all entity guids of given category in given level
    virtual Query* CreateLevelCategoryGuidQuery(const nString& levelName, const nString& categoryName) const;
    /// create specialized query which returns all data for all entities for a given category in a given level
    virtual Query* CreateLevelCategoryQuery(const nString& levelName, const nString& categoryName) const;
    /// create specialized query which returns all templates of given category
    virtual Query* CreateCategoryTemplateQuery(const nString& categoryName) const;
	/// create specialized query which returns all templates of given category and a given id
	virtual Query* CreateCategoryTemplateIdQuery(const nString& categoryName, const nString& templateName) const;
    /// create a specialized query which returns all attributes for a given entity GUID
    virtual Query* CreateGuidQuery(const nString& guid) const;

private:
    /// load global attributes from the database
    void LoadGlobalAttributes();

    static Server* Singleton;
    nString dbFilename;
    nRef<nSqlDatabase> refSqlDatabase;
    bool isOpen;
    AttributeContainer globalAttrs;
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Server::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
nSqlDatabase*
Server::GetSqlDatabase() const
{
    return this->refSqlDatabase;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetDatabaseFilename(const nString& n)
{
    this->dbFilename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Server::GetDatabaseFilename() const
{
    return this->dbFilename;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Server::HasTable(const nString& tableName) const
{
    n_assert(this->refSqlDatabase.isvalid());
    return this->refSqlDatabase->HasTable(tableName);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Server::HasGlobalAttr(Attr::AttributeID attrId) const
{
    n_assert(this->IsOpen());
    return this->globalAttrs.HasAttr(attrId);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetGlobalString(Attr::StringAttributeID attrId, const nString& value)
{
    n_assert(this->IsOpen());
    this->globalAttrs.SetAttr(Attribute(attrId, value));
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
Server::GetGlobalString(Attr::StringAttributeID attrId) const
{
    n_assert(this->IsOpen());
    return this->globalAttrs.GetAttr(attrId).GetString();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetGlobalInt(Attr::IntAttributeID attrId, int value)
{
    n_assert(this->IsOpen());
    this->globalAttrs.SetAttr(Attribute(attrId, value));
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Server::GetGlobalInt(Attr::IntAttributeID attrId) const
{
    n_assert(this->IsOpen());
    return this->globalAttrs.GetAttr(attrId).GetInt();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetGlobalFloat(Attr::FloatAttributeID attrId, float value)
{
    n_assert(this->IsOpen());
    this->globalAttrs.SetAttr(Attribute(attrId, value));
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Server::GetGlobalFloat(Attr::FloatAttributeID attrId) const
{
    n_assert(this->IsOpen());
    return this->globalAttrs.GetAttr(attrId).GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetGlobalBool(Attr::BoolAttributeID attrId, bool value)
{
    n_assert(this->IsOpen());
    this->globalAttrs.SetAttr(Attribute(attrId, value));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Server::GetGlobalBool(Attr::BoolAttributeID attrId) const
{
    n_assert(this->IsOpen());
    return this->globalAttrs.GetAttr(attrId).GetBool();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetGlobalVector3(Attr::Vector3AttributeID attrId, const vector3& value)
{
    n_assert(this->IsOpen());
    this->globalAttrs.SetAttr(Attribute(attrId, value));
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Server::GetGlobalVector3(Attr::Vector3AttributeID attrId) const
{
    n_assert(this->IsOpen());
    return this->globalAttrs.GetAttr(attrId).GetVector3();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetGlobalVector4(Attr::Vector4AttributeID attrId, const vector4& value)
{
    n_assert(this->IsOpen());
    this->globalAttrs.SetAttr(Attribute(attrId, value));
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
Server::GetGlobalVector4(Attr::Vector4AttributeID attrId) const
{
    n_assert(this->IsOpen());
    return this->globalAttrs.GetAttr(attrId).GetVector4();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetGlobalMatrix44(Attr::Matrix44AttributeID attrId, const matrix44& value)
{
    n_assert(this->IsOpen());
    this->globalAttrs.SetAttr(Attribute(attrId, value));
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Server::GetGlobalMatrix44(Attr::Matrix44AttributeID attrId) const
{
    n_assert(this->IsOpen());
    return this->globalAttrs.GetAttr(attrId).GetMatrix44();
}

};
//------------------------------------------------------------------------------
#endif    
