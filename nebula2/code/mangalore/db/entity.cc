//------------------------------------------------------------------------------
//  db/entity.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "db/entity.h"
#include "db/server.h"
#include "db/query.h"
#include "attr/attributes.h"

namespace Db
{
ImplementRtti(Db::Entity, Foundation::RefCounted);
ImplementFactory(Db::Entity);

//------------------------------------------------------------------------------
/**
*/
Entity::Entity()
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
    This loads all attributes from the world database into the entity. Existing
    attribute values will be overwritten with values from the db. If 
    existing attributes don't exist in the db, they won't be touched.
*/
void
Entity::Load()
{
    nArray<Attribute> dbAttrs = Server::Instance()->ReadAttrs("_Entities", this->GetAttr(Attr::GUID));

    // overwrite entity attributes one by one, so that existing entity attributes
    // which are not in the db won't be erased
    int i;
    int num = dbAttrs.Size();
    for (i = 0; i < num; i++)
    {
        this->attrs.SetAttr(dbAttrs[i]);
    }
}

//------------------------------------------------------------------------------
/**
    Write the current attributes from the db entity back into the database.
    Only attributes which are storable will be written back. This will
    update an existing or create a new row in the database. Also, new columns
    will be added to the _Entities table in the database if there are storable
    attributes on the entity which don't have a database column yet.

    saveAllAttributes - Save all attributes if flag is set.
*/
void
Entity::Save(bool saveAllAttributes)
{
    if (this->HasAttr(Attr::GUID))
    {
        // get all storable attributes from the entity
        const nArray<Attribute>& attrArray = this->attrs.GetAttrs();
        nArray<Attribute> storableAttrs(32, 32);
        
        int num = attrArray.Size();
        int i;
        for (i = 0; i < num; i++)
        {
            if (saveAllAttributes || attrArray[i].IsStorable())
            {
                storableAttrs.Append(attrArray[i]);
            }
        }
        Server::Instance()->WriteAttrs("_Entities", this->attrs.GetAttr(Attr::GUID), storableAttrs);
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