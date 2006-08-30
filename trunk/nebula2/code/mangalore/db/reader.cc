//------------------------------------------------------------------------------
//  reader.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "db/reader.h"

namespace Db
{
ImplementRtti(Db::Reader, Foundation::RefCounted);
ImplementFactory(Db::Reader);

//------------------------------------------------------------------------------
/**
*/
Reader::Reader() :
    isOpen(false),
    curRowIndex(-1)

{
    this->query = Query::Create();
}

//------------------------------------------------------------------------------
/**
*/
Reader::~Reader()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Set the name of database table the reader will work on.
*/
void
Reader::SetTableName(const nString& t)
{
    this->query->SetTableName(t);
}

//------------------------------------------------------------------------------
/**
    Add a filter attribute to the reader. The reader will create a
    database query where all filter attribute are ANDed.
*/
void
Reader::AddFilterAttr(const Attribute& attr)
{
    this->query->AddWhereAttr(attr);
}

//------------------------------------------------------------------------------
/**
    Open the reader. This will perform a query on the database and provide
    access to the results through the various GetXXXAttr() methods.
*/
bool
Reader::Open()
{
    n_assert(!this->isOpen);

    // build an SQL SELECT statement in the query and execute it
    this->query->BuildSelectStatement();
    if (this->query->Execute())
    {
        this->isOpen = true;
    }
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Close the reader. This deletes the query object created in the
    constructor.
*/
void
Reader::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
    this->query = 0;
}

//------------------------------------------------------------------------------
/**
    Returns the number of rows in the result. Only valid while open.
*/
int
Reader::GetNumRows() const
{
    n_assert(this->isOpen);
    return this->query->GetNumRows();
}

//------------------------------------------------------------------------------
/**
    Sets the reader cursor to the specified row. Only valid while open.
*/
void
Reader::SetToRow(int rowIndex)
{
    n_assert(this->isOpen);
    n_assert((rowIndex >= 0) && (rowIndex < this->GetNumRows()));
    this->curRowIndex = rowIndex;
}

//------------------------------------------------------------------------------
/**
    Returns all attributes in the current row.
*/
nArray<Attribute>
Reader::GetAttrs() const
{
    n_assert(this->isOpen);

    nArray<Attribute> attrs(64, 64);
    int colIndex;
    int numCols = this->query->GetNumColumns();
    for (colIndex = 0; colIndex < numCols; colIndex++)
    {
        if (this->query->HasAttr(colIndex, this->curRowIndex))
        {
            attrs.Append(this->query->GetAttr(colIndex, this->curRowIndex));
        }
    }
    return attrs;
}

//------------------------------------------------------------------------------
/**
    Return true if a specific attribute exists in the current row.
*/
bool
Reader::HasAttr(Attr::AttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->HasAttr(attrId, this->curRowIndex);
}

//------------------------------------------------------------------------------
/**
    Return a specific attribute in the current row.
*/
const Attribute&
Reader::GetAttr(Attr::AttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->GetAttr(attrId, this->curRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
bool
Reader::GetBool(Attr::BoolAttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->GetBool(attrId, this->curRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
int
Reader::GetInt(Attr::IntAttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->GetInt(attrId, this->curRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
float
Reader::GetFloat(Attr::FloatAttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->GetFloat(attrId, this->curRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
nString
Reader::GetString(Attr::StringAttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->GetString(attrId, this->curRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
vector3
Reader::GetVector3(Attr::Vector3AttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->GetVector3(attrId, this->curRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
vector4
Reader::GetVector4(Attr::Vector4AttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->GetVector4(attrId, this->curRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
matrix44
Reader::GetMatrix44(Attr::Matrix44AttributeID attrId) const
{
    n_assert(this->isOpen);
    return this->query->GetMatrix44(attrId, this->curRowIndex);
}

}; // namespace Db
