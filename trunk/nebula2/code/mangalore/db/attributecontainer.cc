//------------------------------------------------------------------------------
//  db/attributecontainer.h
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "db/attributecontainer.h"

namespace Db
{

//------------------------------------------------------------------------------
/**
*/
AttributeContainer::AttributeContainer() :
    attrs(32, 64)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AttributeContainer::~AttributeContainer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Find index of attribute. Return -1 if attribute doesn't exist in the
    container.

    FIXME: This does a slow linear search. Consider using a binary search
    to speed up access!!!
*/
int
AttributeContainer::FindAttrIndex(const Attr::AttributeID& attrId) const
{
    int num = this->attrs.Size();
    for (int i = 0; i < num; i++)
    {
        if (this->attrs[i].GetAttributeID() == attrId)
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Return true if an attribute exists.
*/
bool
AttributeContainer::HasAttr(const Attr::AttributeID& id) const
{
    return (-1 != this->FindAttrIndex(id));
}

//------------------------------------------------------------------------------
/**
    Set a generic attribute. If the attribute exists, its
    value will be overwritten and a type check will be made (you can't
    overwrite an attribute with a different types one). If the attribute
    doesn't exist, a new attribute will be created.
*/
void
AttributeContainer::SetAttr(const Attribute& attr)
{
    int attrIndex = this->FindAttrIndex(attr.GetAttributeID());
    if (-1 == attrIndex)
    {
        this->attrs.Append(attr);
    }
    else
    {
        // overwrite existing attribute, but make sure they're of the same type
        n_assert2(attr.GetType() == this->attrs[attrIndex].GetType(), attr.GetName().Get());
        this->attrs[attrIndex].SetValue(attr.GetValue());
    }
}

//------------------------------------------------------------------------------
/**
    Get generic attribute. Throws a hard error if the
    attribute doesn't exist.
*/
const Attribute&
AttributeContainer::GetAttr(const Attr::AttributeID& attrId) const
{
    n_assert(attrId.IsValid());
    int attrIndex = this->FindAttrIndex(attrId);
    if (-1 != attrIndex)
    {
        return this->attrs[attrIndex];
    }
    else
    {
        n_error("Db::AttributeContainer::GetAttr(): attr '%s' not found!", attrId.GetName().Get());
        return this->attrs[0]; // silence the compiler
    }
}

//------------------------------------------------------------------------------
/**
    This method provides direct read access to the attributes.
*/
const nArray<Attribute>&
AttributeContainer::GetAttrs() const
{
    return this->attrs;
}

//------------------------------------------------------------------------------
/**
    This method clears the attributes in the attribute container.
*/
void
AttributeContainer::Clear()
{
    this->attrs.Clear();
}

};
