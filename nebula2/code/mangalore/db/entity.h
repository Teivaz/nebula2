#ifndef DB_ENTITY_H
#define DB_ENTITY_H
//------------------------------------------------------------------------------
/**
    @class Db::Entity

    Db::Entity objects represent an instantiated row in a database. Each
    attribute in the row will be cached on the Db::Entity.

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "db/attributecontainer.h"

//------------------------------------------------------------------------------
namespace Db
{
class Reader;
class Writer;

class Entity : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Entity);

public:
    /// constructor
    Entity();
    /// destructor
    virtual ~Entity();
    /// update attribute from an existing Reader object
    void LoadFromReader(Reader* reader);
    /// write attributes to writer object
    void SaveToWriter(Writer* writer);
    /// update attribute values from db, requires a valid GUID attribute! (SLOW!)
    void Load();
    /// write attribute values to db (SLOW!)
    void Save();
    /// check if an attribute exists on the entity
    bool HasAttr(const Attr::AttributeID& attrId) const;
    /// set a single attribute on the entity
    void SetAttr(const Attribute& attr);
    /// get a single attribute from the entity
    const Attribute& GetAttr(const Attr::AttributeID& attrId) const;
    /// direct access to the attribute array
    const nArray<Attribute>& GetAttrs() const;

    /// Set the table name for Load()/Save() methods
    void SetTableName(const nString& table);
    /// Get the table name that used from Load()/Save() methods
    const nString& GetTableName() const;

private:
    nString tableName;
    AttributeContainer attrs;
};

RegisterFactory(Entity);

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetTableName(const nString& table)
{
    n_assert(table.IsValid());

    this->tableName = table;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Entity::GetTableName() const
{
    return this->tableName;
}

};  // namespace Db
//------------------------------------------------------------------------------
#endif
