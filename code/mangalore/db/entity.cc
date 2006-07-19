//------------------------------------------------------------------------------
//  db/entity.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "db/entity.h"
#include "db/server.h"
#include "attr/attributes.h"
#include "db/reader.h"
#include "db/writer.h"

namespace Db
{
ImplementRtti(Db::Entity, Foundation::RefCounted);
ImplementFactory(Db::Entity);

//------------------------------------------------------------------------------
/**
*/
Entity::Entity() : tableName("_Entities")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Load entity from an existing Reader object. This method is fast for many
    objects, since only one query for all objects will be executed.
*/
void
Entity::LoadFromReader(Reader* reader)
{
    n_assert(reader);
    nArray<Attribute> attrs = reader->GetAttrs();
    int i;
    for (i = 0; i < attrs.Size(); i++)
    {
        this->attrs.SetAttr(attrs[i]);
    }
}

//------------------------------------------------------------------------------
/**
    This loads all attributes from the world database into the entity. Existing
    attribute values will be overwritten with values from the db. If 
    existing attributes don't exist in the db, they won't be touched.

    NOTE: for loading many entities, this method is slow, use LoadFromReader()
    instead!
*/
void
Entity::Load()
{
    Ptr<Reader> reader = Reader::Create();
    reader->SetTableName(this->tableName);
    reader->AddFilterAttr(this->GetAttr(Attr::GUID));
    if (reader->Open())
    {
        n_assert(reader->GetNumRows() == 1);
        reader->SetToRow(0);
        this->LoadFromReader(reader);
        reader->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Write the current attributes from the db entity back into a writer object.
    This method is fast for many objects since all writes will be cached
    and only one database access takes place for all entities.
*/
void
Entity::SaveToWriter(Writer* writer)
{
    n_assert(writer);
    if (this->HasAttr(Attr::GUID))
    {
        writer->BeginRow();
        writer->SetAttrs(this->attrs.GetAttrs());
        writer->EndRow();
    }
}

//------------------------------------------------------------------------------
/**
    Write the current attributes from the db entity back into the database.

    NOTE: for saving many entities, this method is very slow, use LoadFromReader()
    instead!
*/
void
Entity::Save()
{
    Ptr<Writer> writer = Writer::Create();
    writer->SetTableName(this->tableName);
    writer->SetPrimaryKey(Attr::GUID);
    if (writer->Open())
    {
        this->SaveToWriter(writer);
        writer->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Return true if an attribute exists on the db entity. Note that you
    can call this method also directly with a string attribute name, since
    Id has a constructor from string.
*/
bool
Entity::HasAttr(const Attr::AttributeID& id) const
{
    return this->attrs.HasAttr(id);
}

//------------------------------------------------------------------------------
/**
    Set a generic attribute on the entity. If the attribute exists, its
    value will be overwritten and a type check will be made (you can't
    overwrite an attribute with a different types one). If the attribute
    doesn't exist, a new attribute will be created and set to writable
    and storable.
*/
void
Entity::SetAttr(const Attribute& attr)
{
    this->attrs.SetAttr(attr);
}

//------------------------------------------------------------------------------
/**
    Get generic attribute from the entity. Throws a hard error if the
    attribute doesn't exist.
*/
const Attribute&
Entity::GetAttr(const Attr::AttributeID& attrId) const
{
    return this->attrs.GetAttr(attrId);
}

//------------------------------------------------------------------------------
/**
    This method provides direct read access to the attributes.
*/
const nArray<Attribute>&
Entity::GetAttrs() const
{
    return this->attrs.GetAttrs();
}

} // namespace Db
