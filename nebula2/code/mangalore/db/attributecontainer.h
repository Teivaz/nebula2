#ifndef DB_ATTRIBUTECONTAINER_H
#define DB_ATTRIBUTECONTAINER_H
//------------------------------------------------------------------------------
/**
    @class Db::AttributeContainer
    
    A simple container class for attributes.
    
    (C) 2006 Radon Labs GmbH
*/
#include "util/narray.h"
#include "db/attribute.h"

//------------------------------------------------------------------------------
namespace Db
{
class AttributeContainer
{
public:
    /// constructor
    AttributeContainer();
    /// destructor
    ~AttributeContainer();
    /// check if an attribute exists in the container
    bool HasAttr(const Attr::AttributeID& attrId) const;
    /// set a single attribute
    void SetAttr(const Attribute& attr);
    /// get a single attribute
    const Attribute& GetAttr(const Attr::AttributeID& attrId) const;
    /// read access to the attribute array
    const nArray<Attribute>& GetAttrs() const;
    /// clear the attribute container
    void Clear();

private:
    /// find index for attribute (SLOW!)
    int FindAttrIndex(const Attr::AttributeID& attrId) const;

    nArray<Attribute> attrs;
};

}; // namespace Db
//------------------------------------------------------------------------------
#endif