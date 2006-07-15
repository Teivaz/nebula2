//------------------------------------------------------------------------------
//  db/query.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "db/query.h"
#include "db/server.h"

namespace Db
{
ImplementRtti(Db::Query, Foundation::RefCounted);
ImplementFactory(Db::Query);

//------------------------------------------------------------------------------
/**
*/
Query::Query()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Query::~Query()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method constructs an attribute-type-safe SELECT statement.
    If no result attributes are given, all attributes for the database
    record will be returned (SELECT * FROM). If no WHERE attributes are
    given, the entire table contents will be returned.
*/
void
Query::BuildSelectStatement()
{
    n_assert(this->tableName.IsValid());

    this->sqlStatement = "SELECT ";
    if (this->resultAttrs.Size() == 0)
    {
        this->sqlStatement.Append("*");
    }
    else
    {
        nArray<nString> resultStrings;
        int i;
        for (i = 0; i < this->resultAttrs.Size(); i++)
        {
            resultStrings.Append(this->resultAttrs[i]);
        }
        this->sqlStatement.Append(nString::Concatenate(resultStrings, ","));
    }
    this->sqlStatement.Append(" FROM ");
    this->sqlStatement.Append(this->tableName);
    if (this->whereAttrs.Size() > 0)
    {
        this->sqlStatement.Append(" WHERE ");
        int i;
        for (i = 0; i < this->whereAttrs.Size(); i++)
        {
            this->sqlStatement.Append("\"");
            this->sqlStatement.Append(this->whereAttrs[i].GetName());
            this->sqlStatement.Append("\"='");
            this->sqlStatement.Append(this->whereAttrs[i].AsString());
            if ((i + 1) < this->whereAttrs.Size())
            {
                this->sqlStatement.Append("' AND ");
            }
            else
            {
                this->sqlStatement.Append("'");
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method constructs an attribute-type-safe UPDATE statement.
*/
void
Query::BuildUpdateStatement()
{
    n_assert(this->tableName.IsValid());
    n_assert(this->updateAttrs.Size() > 0);    
    n_assert(this->whereAttrs.Size() > 0);

    this->sqlStatement = "UPDATE ";
    this->sqlStatement.Append(this->tableName);
    this->sqlStatement.Append(" SET ");
    int i;
    for (i = 0; i < this->updateAttrs.Size(); i++)
    {
        this->sqlStatement.Append("\"");
        this->sqlStatement.Append(this->updateAttrs[i].GetName());
        this->sqlStatement.Append("\"='");
        this->sqlStatement.Append(this->updateAttrs[i].AsString());
        if ((i + 1) < this->updateAttrs.Size())
        {
            this->sqlStatement.Append("', ");
        }
        else
        {
            this->sqlStatement.Append("'");
        }
    }
    this->sqlStatement.Append(" WHERE ");
    for (i = 0; i < this->whereAttrs.Size(); i++)
    {
        this->sqlStatement.Append("\"");
        this->sqlStatement.Append(this->whereAttrs[i].GetName());
        this->sqlStatement.Append("\"='");
        this->sqlStatement.Append(this->whereAttrs[i].AsString());
        if ((i + 1) < this->whereAttrs.Size())
        {
            this->sqlStatement.Append("' AND ");
        }
        else
        {
            this->sqlStatement.Append("'");
        }
    }
}

//------------------------------------------------------------------------------
/**
    This executes the stored SQL query on the world.db, converts the result into
    attribute form, and stores the result in the query object.
*/
bool
Query::Execute()
{
    n_assert(this->sqlStatement.IsValid());
    nSqlDatabase* db = Server::Instance()->GetSqlDatabase();
    
    // create a Nebula2 sqlQuery object
    nSqlQuery* sqlQuery = db->CreateQuery(this->sqlStatement);
    sqlQuery->Execute();

    // convert result to Mangalore attributes
    this->result.SetSize(sqlQuery->GetColumns().Size(), sqlQuery->GetNumRows());
    
    // create a reference array of attributes
    const nArray<nString>& resColumns = sqlQuery->GetColumns();
    int colIndex;
    int numCols = resColumns.Size();
    nArray<Attribute> attrs( 0, 16 ); // we know size beforehand, create empty
    attrs.SetFixedSize( numCols );    // then set fixed size
    for (colIndex = 0; colIndex < numCols; colIndex++)
    {
        const nString& columnTitle = resColumns[colIndex];
        Attr::AttributeID attrId = Attr::AttributeID::FindAttributeID(columnTitle);
        if ( !attrId.IsValid() )
        {
            n_error
            (
            "Error in table \"%s\", unknown attribute ID \"%s\"\n(SQL Statement: %s)",
                this->tableName.Get(),
                columnTitle.Get(),
                sqlQuery->GetSqlStatement().Get()
            );
        }

        attrs[ colIndex ].SetAttributeID( attrId );
    }

    // convert attributes and write to result array, hmm, this
    // is not very elegant because of the null-field handling
    // [np]: changed to only clear attribute if it cannot be set
    int rowIndex;
    int numRows = sqlQuery->GetNumRows();
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        nSqlRow sqlRow = sqlQuery->GetRow(rowIndex);
        int colIndex;
        int numCols = resColumns.Size();
        for (colIndex = 0; colIndex < numCols; colIndex++)
        {
            if (sqlRow.HasColumn(resColumns[colIndex]))
            {
                const nString& valueString = sqlRow.Get(resColumns[colIndex]);
                switch (attrs[colIndex].GetType())
                {
                    case Attribute::Int:
                        attrs[colIndex].SetInt(valueString.AsInt());
                        break;

                    case Attribute::Float:
                        attrs[colIndex].SetFloat(valueString.AsFloat());
                        break;

                    case Attribute::Bool:
                        attrs[colIndex].SetBool(valueString.AsBool());
                        break;

                    case Attribute::Vector3:
                        attrs[colIndex].SetVector3(valueString.AsVector3());
                        break;

                    case Attribute::Vector4:
                        attrs[colIndex].SetVector4(valueString.AsVector4());
                        break;

                    case Attribute::String:
                        attrs[colIndex].SetString(valueString);
                        break;

                    case Attribute::Matrix44:
                        attrs[colIndex].SetMatrix44(valueString.AsMatrix44());
                        break;

                    default:
                        n_assert2( false, "Known attribute type" );
                        attrs[colIndex].Clear();
                        break;

                }
                
                this->result.At(colIndex, rowIndex) = attrs[colIndex];
            }
            else
            {
                // no column
                this->result.At(colIndex, rowIndex).Clear();
            }   
        }
    }
    sqlQuery->Release();
    return (numRows > 0);
}

//------------------------------------------------------------------------------
/**
    This method returns true if a column exists in the result. This doesn't
    necessarly mean that every row in the result has that attribute set
    (use the HasAttr() method for this).
*/
bool
Query::HasColumn(const Attr::AttributeID& attrId) const
{
    if  (this->GetNumRows() > 0)
    {
        int colIndex;
        int numCols = this->GetNumColumns();
        for (colIndex = 0; colIndex < numCols; colIndex++)
        {
            if (this->result.At(colIndex, 0).GetAttributeID() == attrId)
            {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This method checks if an attribute in the given row is valid. Invalid
    attributes are generated when the database entry is a null field.
*/
bool
Query::HasAttr(const Attr::AttributeID& attrId, int rowIndex) const
{
    int colIndex;
    int numCols = this->GetNumColumns();
    for (colIndex = 0; colIndex < numCols; colIndex++)
    {
        if (this->result.At(colIndex, rowIndex).GetAttributeID() == attrId)
        {
            return !this->result.At(colIndex, rowIndex).IsEmpty();
        }
    }
    // not found means not valid
    return false;
}

//------------------------------------------------------------------------------
/**
    Return attribute at given row. Note that the resulting attribute
    may be empty, if the corresponding database field is a null field!
*/
const Attribute&
Query::GetAttr(const Attr::AttributeID& attrId, int rowIndex) const
{
    int colIndex;
    int numCols = this->GetNumColumns();
    for (colIndex = 0; colIndex < numCols; colIndex++)
    {
        if (this->result.At(colIndex, rowIndex).GetAttributeID() == attrId)
        {
            return this->result.At(colIndex, rowIndex);
        }
    }
    static Attribute invalidAttr;
    return invalidAttr;
}

} // namespace Db
