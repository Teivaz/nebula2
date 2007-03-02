#ifndef N_XMLSPREADSHEET_H
#define N_XMLSPREADSHEET_H
//------------------------------------------------------------------------------
/**
    @class nXmlSpreadSheet
    @ingroup Xml

    A XML spreadsheet can load an XML-exported Excel spreadsheet file.

    A database contains 1 or several XmlTable objects, which contain
    XmlTableCell objects.

    XmlTableCells are typeless, but access methods exist to set or
    get the content as several datatypes.

    (C) 2004 RadonLabs GmbH
*/
#include "util/narray.h"
#include "util/nstring.h"
#include "xml/nxmltable.h"
#include "tinyxml/tinyxml.h"

//------------------------------------------------------------------------------
class nXmlSpreadSheet
{
public:
    /// constructor
    nXmlSpreadSheet();
    /// destructor
    ~nXmlSpreadSheet();
    /// set file name of XML file
    void SetFilename(const nString& name);
    /// get file name of XML file
    const nString& GetFilename() const;
    /// open the spreadsheet
    bool Open();
    /// close the spreadsheet
    void Close();
    /// currently open?
    bool IsOpen() const;
    /// get number of tables in database
    int NumTables() const;
    /// get table at index
    nXmlTable& TableAt(int index) const;
    /// get table by name
    nXmlTable* FindTable(const nString& name) const;
    /// manually add a table, the table will be copied
    void AddTable(nXmlTable* t);

private:
    /// count valid rows in table
    int CountRows(TiXmlElement* xmlTable);
    /// count valid columns in table
    int CountCols(TiXmlElement* xmlTable);
    /// setup XML tables from XML document
    void SetupTables(TiXmlDocument* doc);

    nString filename;
    bool isOpen;
    nArray<nXmlTable*> tables;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nXmlSpreadSheet::SetFilename(const nString& name)
{
    this->filename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nXmlSpreadSheet::GetFilename() const
{
    return this->filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nXmlSpreadSheet::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nXmlSpreadSheet::NumTables() const
{
    return this->tables.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nXmlTable&
nXmlSpreadSheet::TableAt(int index) const
{
    return *this->tables[index];
}

//------------------------------------------------------------------------------
#endif
