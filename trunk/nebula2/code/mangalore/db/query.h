#ifndef DB_QUERY_H
#define DB_QUERY_H
//------------------------------------------------------------------------------
/**
    @class Db::Query
    
    This is a wrapper around Nebula2's nSqlQuery and provides 
    attribute-type-safe queries into the world database.
    
    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "sql/nsqlquery.h"
#include "util/narray2.h"
#include "db/attribute.h"

//------------------------------------------------------------------------------
namespace Db
{
class Query : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Query);

public:
    /// constructor
    Query();
    /// destructor
    virtual ~Query();
    /// set optional universal the SQL statement
    void SetSqlStatement(const nString& sql);
    /// get optional the SQL statement
    const nString& GetSqlStatement() const;
    /// set table name for constructed SQL statement
    void SetTableName(const nString& n);
    /// add attribute-safe result column for constructed statement
    void AddResultAttr(const Attr::AttributeID& attrId);
    /// add attribute-safe update attribute for constructed statement
    void AddUpdateAttr(const Db::Attribute& attr);
    /// add attribute-safe WHERE column for constructed statement (will be ANDed)
    void AddWhereAttr(const Db::Attribute& attr);
    /// build a SELECT statement from the given data
    void BuildSelectStatement();
    /// build an UPDATE statement from the given data
    void BuildUpdateStatement();
    /// execute an sql query, this updates the stored query result
    bool Execute();
    /// return number of columns in the result
    int GetNumColumns() const;
    /// return number of rows in the result
    int GetNumRows() const;
    /// return true if attribute column exists in the result
    bool HasColumn(const Attr::AttributeID& attrId) const;
    /// return true if attribute at position is valid
    bool HasAttr(int colIndex, int rowIndex) const;
    /// return a single attribute by column/row index
    const Attribute& GetAttr(int colIndex, int rowIndex) const;
    /// return true if an attribute is valid in a specific row
    bool HasAttr(const Attr::AttributeID& attrId, int rowIndex) const;
    /// return a single attribute by attribute id
    const Attribute& GetAttr(const Attr::AttributeID& attrId, int rowIndex) const;

    /// return a single attribute value by attribute id
    bool GetBool(const Attr::BoolAttributeID& attrId, int rowIndex) const;
    /// return a single attribute value by attribute id
    int GetInt(const Attr::IntAttributeID& attrId, int rowIndex) const;
    /// return a single attribute value by attribute id
    float GetFloat(const Attr::FloatAttributeID& attrId, int rowIndex) const;
    /// return a single attribute value by attribute id
    nString GetString(const Attr::StringAttributeID& attrId, int rowIndex) const;
    /// return a single attribute value by attribute id
    vector3 GetVector3(const Attr::Vector3AttributeID& attrId, int rowIndex) const;
    /// return a single attribute value by attribute id
    vector4 GetVector4(const Attr::Vector4AttributeID& attrId, int rowIndex) const;
    /// return a single attribute value by attribute id
    matrix44 GetMatrix44(const Attr::Matrix44AttributeID& attrId, int rowIndex) const;

private:    
    nString tableName;
    nArray<nString> resultAttrs;
    nArray<Db::Attribute> whereAttrs;
    nArray<Db::Attribute> updateAttrs;
    nString sqlStatement;
    nArray2<Attribute> result;
};

RegisterFactory(Query);


//------------------------------------------------------------------------------
/**
*/
inline
void
Query::SetTableName(const nString& n)
{
    this->tableName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Query::AddResultAttr(const Attr::AttributeID& attrId)
{
    this->resultAttrs.Append(attrId.GetName());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Query::AddWhereAttr(const Db::Attribute& attr)
{
    this->whereAttrs.Append(attr);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Query::AddUpdateAttr(const Db::Attribute& attr)
{
    this->updateAttrs.Append(attr);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Query::SetSqlStatement(const nString& sql)
{
    this->sqlStatement = sql;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Query::GetSqlStatement() const
{
    return this->sqlStatement;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Query::GetNumColumns() const
{
    return this->result.GetWidth();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Query::GetNumRows() const
{
    return this->result.GetHeight();
}

//------------------------------------------------------------------------------
/**
    Returns true if the attribute at the given position is valid. If not,
    the corresponding database field is a null field.
*/
inline
bool
Query::HasAttr(int colIndex, int rowIndex) const
{
    return !this->result.At(colIndex, rowIndex).IsEmpty();
}

//------------------------------------------------------------------------------
/**
    Return attribute at given location. Please be aware that the attribute
    field may be empty if the corresponding database field is a null field!
*/
inline
const Attribute&
Query::GetAttr(int colIndex, int rowIndex) const
{
    return this->result.At(colIndex, rowIndex);
}

//------------------------------------------------------------------------------
/**
    Return value of attribute at given location. Please be aware that the attribute
    field may be empty if the corresponding database field is a null field!
    Also make sure to get the right type of value!
*/
inline
bool
Query::GetBool(const Attr::BoolAttributeID& attrId, int rowIndex) const
{
    const Attribute& attribute = this->GetAttr( attrId, rowIndex );
    n_assert2( attribute.IsValid(), "Attribute is in result table at given rowindex" );
    n_assert2( attribute.GetAttributeID() == attrId, "Attribute of correct AttributeID type" );

    return attribute.GetBool();
}

//------------------------------------------------------------------------------
/**
    Return value of attribute at given location. Please be aware that the attribute
    field may be empty if the corresponding database field is a null field!
    Also make sure to get the right type of value!
*/
inline
int
Query::GetInt(const Attr::IntAttributeID& attrId, int rowIndex) const
{
    const Attribute& attribute = this->GetAttr( attrId, rowIndex );
    n_assert2( attribute.IsValid(), "Attribute is in result table at given rowindex" );
    n_assert2( attribute.GetAttributeID() == attrId, "Attribute of correct AttributeID type" );

    return attribute.GetInt();
}

//------------------------------------------------------------------------------
/**
    Return value of attribute at given location. Please be aware that the attribute
    field may be empty if the corresponding database field is a null field!
    Also make sure to get the right type of value!
*/
inline
float
Query::GetFloat(const Attr::FloatAttributeID& attrId, int rowIndex) const
{
    const Attribute& attribute = this->GetAttr( attrId, rowIndex );
    n_assert2( attribute.IsValid(), "Attribute is in result table at given rowindex" );
    n_assert2( attribute.GetAttributeID() == attrId, "Attribute of correct AttributeID type" );

    return attribute.GetFloat();
}

//------------------------------------------------------------------------------
/**
    Return value of attribute at given location. Please be aware that the attribute
    field may be empty if the corresponding database field is a null field!
    Also make sure to get the right type of value!
*/
inline
nString
Query::GetString(const Attr::StringAttributeID& attrId, int rowIndex) const
{
    const Attribute& attribute = this->GetAttr( attrId, rowIndex );
    n_assert2( attribute.IsValid(), "Attribute is in result table at given rowindex" );
    n_assert2( attribute.GetAttributeID() == attrId, "Attribute of correct AttributeID type" );

    return attribute.GetString();
}

//------------------------------------------------------------------------------
/**
    Return value of attribute at given location. Please be aware that the attribute
    field may be empty if the corresponding database field is a null field!
    Also make sure to get the right type of value!
*/
inline
vector3
Query::GetVector3(const Attr::Vector3AttributeID& attrId, int rowIndex) const
{
    const Attribute& attribute = this->GetAttr( attrId, rowIndex );
    n_assert2( attribute.IsValid(), "Attribute is in result table at given rowindex" );
    n_assert2( attribute.GetAttributeID() == attrId, "Attribute of correct AttributeID type" );

    return attribute.GetVector3();
}

//------------------------------------------------------------------------------
/**
    Return value of attribute at given location. Please be aware that the attribute
    field may be empty if the corresponding database field is a null field!
    Also make sure to get the right type of value!
*/
inline
vector4
Query::GetVector4(const Attr::Vector4AttributeID& attrId, int rowIndex) const
{
    const Attribute& attribute = this->GetAttr( attrId, rowIndex );
    n_assert2( attribute.IsValid(), "Attribute is in result table at given rowindex" );
    n_assert2( attribute.GetAttributeID() == attrId, "Attribute of correct AttributeID type" );

    return attribute.GetVector4();
}

//------------------------------------------------------------------------------
/**
    Return value of attribute at given location. Please be aware that the attribute
    field may be empty if the corresponding database field is a null field!
    Also make sure to get the right type of value!
*/
inline
matrix44
Query::GetMatrix44(const Attr::Matrix44AttributeID& attrId, int rowIndex) const
{
    const Attribute& attribute = this->GetAttr( attrId, rowIndex );
    n_assert2( attribute.IsValid(), "Attribute is in result table at given rowindex" );
    n_assert2( attribute.GetAttributeID() == attrId, "Attribute of correct AttributeID type" );

    return attribute.GetMatrix44();
}

}; // namespace Db
//------------------------------------------------------------------------------
#endif
