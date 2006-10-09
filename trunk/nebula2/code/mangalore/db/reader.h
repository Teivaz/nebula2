#ifndef DB_READER_H
#define DB_READER_H
//------------------------------------------------------------------------------
/**
    @class Db::Reader

    Used to read data from the database in a more abstract
    (but also more efficient) way than Query.

    (C) 2006 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "db/attribute.h"
#include "db/query.h"

//------------------------------------------------------------------------------
namespace Db
{
class Reader : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(Reader);
public:
    /// constructor
    Reader();
    /// destructor
    virtual ~Reader();

    /// set the database table name
    void SetTableName(const nString& n);
    /// add a filter/WHERE attribute
    void AddFilterAttr(const Attribute& attr);

    /// open the reader, this will perform a Query on the database
    bool Open();
    /// return true if reader is open
    bool IsOpen() const;
    /// close the reader
    void Close();

    /// get number of rows in the result
    int GetNumRows() const;
    /// set read cursor to specified row
    void SetToRow(int rowIndex);

    /// return all valid attributes in current row
    nArray<Attribute> GetAttrs() const;
    /// return true if attribute exists at current row
    bool HasAttr(Attr::AttributeID attrId) const;
    /// return a generic attribute in current row
    const Attribute& GetAttr(Attr::AttributeID attrId) const;
    /// return a single attribute value by attribute id
    bool GetBool(Attr::BoolAttributeID attrId) const;
    /// return a single attribute value by attribute id
    int GetInt(Attr::IntAttributeID attrId) const;
    /// return a single attribute value by attribute id
    float GetFloat(Attr::FloatAttributeID attrId) const;
    /// return a single attribute value by attribute id
    nString GetString(Attr::StringAttributeID attrId) const;
    /// return a single attribute value by attribute id
    vector3 GetVector3(Attr::Vector3AttributeID attrId) const;
    /// return a single attribute value by attribute id
    vector4 GetVector4(Attr::Vector4AttributeID attrId) const;
    /// return a single attribute value by attribute id
    matrix44 GetMatrix44(Attr::Matrix44AttributeID attrId) const;

private:
    Ptr<Query> query;
    int curRowIndex;
    bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
Reader::IsOpen() const
{
    return this->isOpen;
}

} // namespace Db
//------------------------------------------------------------------------------
#endif

