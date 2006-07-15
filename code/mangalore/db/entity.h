#ifndef DB_ENTITY_H
#define DB_ENTITY_H
//------------------------------------------------------------------------------
/**
    @class Db::Entity

    Db::Entity objects represent an instantiated row in a database. Each
    attribute in the row will be cached on the Db::Entity.

    (C) 2005 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "db/attributecontainer.h"

//------------------------------------------------------------------------------
namespace Db
{
class Entity : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Entity);

public:
    /// constructor
    Entity();
    /// destructor
    virtual ~Entity();
    /// update attribute values from db, requires a valid GUID attribute!
    void Load();
    /// write attribute values to db
    void Save(bool saveAllAtrributes);
    /// check if an attribute exists on the entity
    bool HasAttr(const Attr::AttributeID& attrId) const;
    /// set a single attribute on the entity
    void SetAttr(const Attribute& attr);
    /// get a single attribute from the entity
    const Attribute& GetAttr(const Attr::AttributeID& attrId) const;
    /// direct access to the attribute array
    const nArray<Attribute>& GetAttrs() const;

private:
    AttributeContainer attrs;
};

RegisterFactory(Entity);

};  // namespace Db
//------------------------------------------------------------------------------
#endif
