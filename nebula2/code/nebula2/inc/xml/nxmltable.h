#ifndef N_XMLTABLE_H
#define N_XMLTABLE_H
//------------------------------------------------------------------------------
/**
    @class nXmlTable
    @ingroup Xml

    A table in a nXmlSpreadSheet object.

    (C) 2004 RadonLabs GmbH
*/
#include "util/nfixedarray.h"
#include "xml/nxmltablecell.h"

//------------------------------------------------------------------------------
class nXmlTable
{
public:
    /// constructor
    nXmlTable();
    /// destructor
    ~nXmlTable();
    /// set table name
    void SetName(const nString& n);
    /// get table name
    const nString& Name() const;
    /// set dimensions, initializes the table
    void SetDimensions(int rows, int cols);
    /// get number of rows
    int NumRows() const;
    /// get number of columns
    int NumColumns() const;
    /// access to cell in table
    nXmlTableCell& Cell(int row, int col) const;
    /// access to cell in table with column title name
    nXmlTableCell& Cell(int row, const nString& colName) const;
    /// get column index by name (return -1 if invalid column name)
    int FindColIndex(const nString& colName) const;
    /// return true if column title exists
    bool HasColumn(const nString& colName) const;

private:
    nString name;
    int numRows;
    int numCols;
    nFixedArray<nXmlTableCell> cells;
};

//------------------------------------------------------------------------------
/**
*/
inline
nXmlTable::nXmlTable() :
    numRows(0),
    numCols(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nXmlTable::~nXmlTable()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nXmlTable::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nXmlTable::Name() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nXmlTable::SetDimensions(int rows, int cols)
{
    n_assert((rows > 0) && (cols > 0));
    this->numRows = rows;
    this->numCols = cols;
    this->cells.SetSize(rows * cols);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nXmlTable::NumRows() const
{
    return this->numRows;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nXmlTable::NumColumns() const
{
    return this->numCols;
}

//------------------------------------------------------------------------------
/**
*/
inline
nXmlTableCell&
nXmlTable::Cell(int row, int col) const
{
    return this->cells[row * this->numCols + col];
}

//------------------------------------------------------------------------------
/**
    Find a column index by column title name. Return -1 if column doesn't
    exist.
*/
inline
int
nXmlTable::FindColIndex(const nString& colName) const
{
    int col;
    for (col = 0; col < this->numCols; col++)
    {
        if (colName == this->Cell(0, col).AsString())
        {
            return col;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Returns true if a column with given title exists.
*/
inline
bool
nXmlTable::HasColumn(const nString& colName) const
{
    return (this->FindColIndex(colName) != -1);
}

//------------------------------------------------------------------------------
/**
    Get a cell by its row index and title string (the string in row 0
    of the table). Fails hard if the title does not exist.
*/
inline
nXmlTableCell&
nXmlTable::Cell(int row, const nString& colName) const
{
    int col = this->FindColIndex(colName);
    if (-1 != col)
    {
        return this->Cell(row, col);
    }
    else
    {
        n_error("nXmlTable::Cell: invalid column title '%s'!", colName);
        return this->Cell(0, 0);
    }
}

//------------------------------------------------------------------------------
#endif