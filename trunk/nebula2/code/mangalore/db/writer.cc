//------------------------------------------------------------------------------
//  writer.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "db/writer.h"
#include "db/server.h"
#include "sql/nsqldatabase.h"

namespace Db
{
ImplementRtti(Db::Writer, Foundation::RefCounted);
ImplementFactory(Db::Writer);

//------------------------------------------------------------------------------
/**
*/
Writer::Writer() :
    columns(128, 128),
    rows(128, 128),
    isOpen(false),
    inBeginRow(false),
    flushTable(false)
{
    this->rows.SetFlags(nArray<nArray<Attribute> >::DoubleGrowSize);
}

//------------------------------------------------------------------------------
/**
*/
Writer::~Writer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Set the name of database table the writer will work on.
*/
void
Writer::SetTableName(const nString& t)
{
    this->tableName = t;
}

//------------------------------------------------------------------------------
/**
    Set the key attribute of the dataset. This is used to check whether
    a given row exists in the database, or whether a new one must be created.
*/
void
Writer::SetPrimaryKey(Attr::AttributeID attrId)
{
    this->primaryKeyAttrId = attrId;
}

//------------------------------------------------------------------------------
/**
    This opens the writer. Except some initialisations nothing will happen
    here.
*/
bool
Writer::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->inBeginRow);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the writer. The database update stuff will happen here.
*/
void
Writer::Close()
{
    n_assert(this->isOpen);
    n_assert(!this->inBeginRow);
    nSqlDatabase* sqlDatabase = Server::Instance()->GetSqlDatabase();
    n_assert(0 != sqlDatabase);

    // cleanup the current table entrys if requested
    if (this->flushTable)
    {
        if (sqlDatabase->HasTable(this->tableName))
        {
            // delete all existing entrys in the table
            sqlDatabase->DeleteRow(this->tableName, "");
        }
    }

    // only do something if rows have been added
    if (this->rows.Size() > 0)
    {
        // check if the table layout needs to be updated
        nArray<nString> columnTitles(128, 128);
        int i;
        for (i = 0; i < this->columns.Size(); i++)
        {
            columnTitles.Append(this->columns[i].GetName());
        }

        // update table layout, or create table
        if (sqlDatabase->HasTable(this->tableName))
        {
            sqlDatabase->UpdateTable(this->tableName, columnTitles);
        }
        else
        {
            sqlDatabase->CreateTable(this->tableName, columnTitles, this->primaryKeyAttrId.GetName());
        }

        // insert the row, this will automatically replace colliding
        // rows (usually GUID is set to collision)
        int rowIndex;
        for (rowIndex = 0; rowIndex < this->rows.Size(); rowIndex++)
        {
            const nArray<Attribute>& rowData = this->rows[rowIndex];
            nSqlRow sqlRow;
            int attrIndex;
            for (attrIndex = 0; attrIndex < rowData.Size(); attrIndex++)
            {
                sqlRow.Set(rowData[attrIndex].GetName(), rowData[attrIndex].AsString());
            }
            sqlDatabase->InsertRow(this->tableName, sqlRow);
        }
    }
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Begin writing a new row to the database.
*/
void
Writer::BeginRow()
{
    n_assert(this->isOpen);
    n_assert(!this->inBeginRow);
    this->inBeginRow = true;

    // add a new attribute array to the end of the rows array
    nArray<Attribute> newRow(0, 128);
    newRow.SetFlags(nArray<Attribute>::DoubleGrowSize);
    this->rows.Append(newRow);
}

//------------------------------------------------------------------------------
/**
    Set a new attribute to the current row. If the attribute exists it
    will be overwritten, otherwise appended.
*/
void
Writer::SetAttr(const Attribute& attr)
{
    n_assert(this->isOpen);
    n_assert(this->inBeginRow);
    int index = this->FindAttrIndex(attr.GetAttributeID());
    if (-1 == index)
    {
        this->rows.Back().Append(attr);
    }
    else
    {
        this->rows.Back()[index] = attr;
    }
}

//------------------------------------------------------------------------------
/**
    Sets all attributes in the current row at once.
*/
void
Writer::SetAttrs(const nArray<Attribute>& attrs)
{
    n_assert(this->isOpen);
    n_assert(this->inBeginRow);
    this->rows.Back() = attrs;
}

//------------------------------------------------------------------------------
/**
    Finish writing the current row. This will update the columns
    array with any new attribute ids in the current row.
*/
void
Writer::EndRow()
{
    n_assert(this->isOpen);
    n_assert(this->inBeginRow);
    this->inBeginRow = false;

    // update columns with new attributes...
    int rowAttrIndex;
    int numRowAttrs = this->rows.Back().Size();
    for (rowAttrIndex = 0; rowAttrIndex < numRowAttrs; rowAttrIndex++)
    {
        const Attr::AttributeID& attrId = this->rows.Back()[rowAttrIndex].GetAttributeID();
        if (-1 == this->columns.FindIndex(attrId))
        {
            this->columns.Append(attrId);
        }
    }
}

}; // namespace Db
