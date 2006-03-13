#ifndef N_SQLROW_H
#define N_SQLROW_H
//------------------------------------------------------------------------------
/**
    A wrapper object for a row of data. Used to set and get data in/from 
    a database.
    
    (C) 2005 Radon Labs GmbH
*/
#include "util/nstring.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nSqlRow
{
public:
    /// default constructor
    nSqlRow();
    /// constructor
    nSqlRow(const nArray<nString>& cols, const nArray<nString>& vals);
    /// equality operator
    bool operator==(const nSqlRow& rhs) const;
    /// inequality operator
    bool operator!=(const nSqlRow& rhs) const;
    /// clear content
    void Clear();
    /// set value of column/value pair, if column exists, its value will be overwritten
    void Set(const nString& column, const nString& value);
    /// get the value of a column, fails hard if column doesn't exist
    const nString& Get(const nString& column) const;
    /// return true if column exists
    bool HasColumn(const nString& column) const;
    /// directly set columns
    void SetColumns(const nArray<nString>& cols);
    /// get columns
    const nArray<nString>& GetColumns() const;
    /// directly set values
    void SetValues(const nArray<nString>& vals);
    /// get values
    const nArray<nString>& GetValues() const;

private:
    nArray<nString> columns;
    nArray<nString> values;
};

//------------------------------------------------------------------------------
/**
*/
inline
nSqlRow::nSqlRow() :
    columns(32, 32),
    values(32, 32)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nSqlRow::nSqlRow(const nArray<nString>& cols, const nArray<nString>& vals) :
    columns(cols),
    values(vals)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSqlRow::operator==(const nSqlRow& rhs) const
{
    return (this->columns == rhs.columns) && (this->values == rhs.values);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSqlRow::operator!=(const nSqlRow& rhs) const
{
    return (this->columns != rhs.columns) || (this->values != rhs.values);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSqlRow::Clear()
{
    this->columns.Clear();
    this->values.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSqlRow::Set(const nString& column, const nString& value)
{
    int columnIndex = this->columns.FindIndex(column);
    if (-1 == columnIndex)
    {
        this->columns.Append(column);
        this->values.Append(value);
    }
    else
    {
        this->values[columnIndex] = value;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nSqlRow::Get(const nString& column) const
{
    int columnIndex = this->columns.FindIndex(column);
    n_assert(-1 != columnIndex);
    return this->values[columnIndex];
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSqlRow::HasColumn(const nString& column) const
{
    return (-1 != this->columns.FindIndex(column));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSqlRow::SetColumns(const nArray<nString>& cols)
{
    this->columns = cols;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
nSqlRow::GetColumns() const
{
    return this->columns;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSqlRow::SetValues(const nArray<nString>& vals)
{
    this->values = vals;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
nSqlRow::GetValues() const
{
    return this->values;
}

//------------------------------------------------------------------------------
#endif