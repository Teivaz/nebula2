#ifndef DB_WRITER_H
#define DB_WRITER_H
//------------------------------------------------------------------------------
/**
    @class Db::Writer

    Used to batch-write data in a more abstract and efficient way than
    Query. Updates the database layout if needed.

    (C) 2006 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "db/query.h"
#include "attr/attributeid.h"

//------------------------------------------------------------------------------
namespace Db
{
class Writer : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(Writer);
public:
    /// constructor
    Writer();
    /// destructor
    virtual ~Writer();

    /// set the database table name
    void SetTableName(const nString& n);
    /// set primary key attribute id (if table must be created)
    void SetPrimaryKey(Attr::AttributeID id);
    /// set true if the table should be flushed before write
    void SetFlushTable(bool flushTable);

    /// open the writer
    bool Open();
    /// return true if open
    bool IsOpen() const;
    /// close the writer - write to DB
    void Close();

    /// begin writing a new row
    void BeginRow();
    /// set a generic attribute in the current row
    void SetAttr(const Attribute& attr);
    /// set bool attribute in current row
    void SetBool(Attr::BoolAttributeID id, bool b);
    /// set int attribute in current row
    void SetInt(Attr::IntAttributeID id, int i);
    /// set float attribute in current row
    void SetFloat(Attr::FloatAttributeID id, float f);
    /// set string attribute in current row
    void SetString(Attr::StringAttributeID id, const nString& s);
    /// set vector3 attribute in current row
    void SetVector3(Attr::Vector3AttributeID id, const vector3& v);
    /// set vector4 attribute in current row
    void SetVector4(Attr::Vector4AttributeID id, const vector4& v);
    /// set matrix44 attribute in current row
    void SetMatrix44(Attr::Matrix44AttributeID id, const matrix44& m);
    /// set all row attributes
    void SetAttrs(const nArray<Attribute>& attrs);
    /// end current row
    void EndRow();

private:
    /// check if attribute exists in current row
    int FindAttrIndex(Attr::AttributeID id) const;

    nString tableName;
    Attr::AttributeID primaryKeyAttrId;
    nArray<Attr::AttributeID> columns;
    nArray<nArray<Attribute> > rows;
    bool isOpen;
    bool inBeginRow;
    bool flushTable;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
Writer::FindAttrIndex(Attr::AttributeID id) const
{
    int num = this->rows.Back().Size();
    for (int i = 0; i < num; i++)
    {
        if (this->rows.Back()[i].GetAttributeID() == id)
        {
            return i;
        }
    }
    // not found
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Writer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Writer::SetBool(Attr::BoolAttributeID id, bool b)
{
    this->SetAttr(Attribute(id, b));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Writer::SetInt(Attr::IntAttributeID id, int i)
{
    this->SetAttr(Attribute(id, i));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Writer::SetFloat(Attr::FloatAttributeID id, float f)
{
    this->SetAttr(Attribute(id, f));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Writer::SetString(Attr::StringAttributeID id, const nString& s)
{
    this->SetAttr(Attribute(id, s));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Writer::SetVector3(Attr::Vector3AttributeID id, const vector3& v)
{
    this->SetAttr(Attribute(id, v));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Writer::SetVector4(Attr::Vector4AttributeID id, const vector4& v)
{
    this->SetAttr(Attribute(id, v));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Writer::SetMatrix44(Attr::Matrix44AttributeID id, const matrix44& m)
{
    this->SetAttr(Attribute(id, m));
}


//------------------------------------------------------------------------------
/**
*/
inline
void
Writer::SetFlushTable(bool flush)
{
    this->flushTable = flush;
}

}; // namespace Db
//------------------------------------------------------------------------------
#endif
