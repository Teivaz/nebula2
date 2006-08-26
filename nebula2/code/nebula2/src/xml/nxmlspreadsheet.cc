//------------------------------------------------------------------------------
//  nxmlspreadsheet.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "xml/nxmlspreadsheet.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
*/
nXmlSpreadSheet::nXmlSpreadSheet() :
    isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nXmlSpreadSheet::~nXmlSpreadSheet()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open the spread sheet file. If a filename is set, this will load the
    Excel-exported XML file via TinyXML and create nXmlTable objects for each
    contained WorkSheet. If no filename is set, this will just set the
    object to the Open() status.
*/
bool
nXmlSpreadSheet::Open()
{
    n_assert(!this->isOpen);
    n_assert(0 == this->tables.Size());

    // create a TinyXML document
    if (this->filename.IsValid())
    {
        TiXmlDocument doc(this->filename.Get());
        if (!doc.LoadFile())
        {
            n_error("nXmlSpreadSheet failed to open file '%s'!", this->filename.Get());
            return false;
        }

        // setup tables
        this->SetupTables(&doc);
    }

    // return successfully
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the spread sheet.
*/
void
nXmlSpreadSheet::Close()
{
    n_assert(this->isOpen);

    // clear tables
    int i;
    int num = this->tables.Size();
    for (i = 0; i < num; i++)
    {
        n_assert(0 != this->tables[i]);
        n_delete(this->tables[i]);
        this->tables[i] = 0;
    }
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Count the number of valid rows in a table element. This is the number of
    consecutive rows with at least one valid cell.
*/
int
nXmlSpreadSheet::CountRows(TiXmlElement* xmlTable)
{
    n_assert(xmlTable);

    int numRows = 0;
    TiXmlElement* xmlRow = xmlTable->FirstChildElement("Row");
    for (; 0 != xmlRow; xmlRow = xmlRow->NextSiblingElement("Row"))
    {
        TiXmlHandle rowHandle(xmlRow);

        // a valid cell must contain a data element
        if (rowHandle.FirstChildElement("Cell").FirstChildElement("Data").Element())
        {
            numRows++;
        }
        else
        {
            break;
        }
    }
    return numRows;
}

//------------------------------------------------------------------------------
/**
    Count the number of valid columns in a table element. This is the number
    of cells in the first row with valid data elements.
*/
int
nXmlSpreadSheet::CountCols(TiXmlElement* xmlTable)
{
    n_assert(xmlTable);

    int numCols = 0;
    TiXmlHandle tableHandle(xmlTable);
    TiXmlElement* xmlCell = tableHandle.FirstChildElement("Row").FirstChildElement("Cell").Element();
    for (; 0 != xmlCell; xmlCell = xmlCell->NextSiblingElement("Cell"))
    {
        if (xmlCell->FirstChildElement("Data"))
        {
            numCols++;
        }
    }
    return numCols;
}

//------------------------------------------------------------------------------
/**
    Setup the spread sheet table objects.
*/
void
nXmlSpreadSheet::SetupTables(TiXmlDocument* doc)
{
    // for each table...
    TiXmlHandle docHandle(doc);
    TiXmlElement* xmlWorkSheet = docHandle.FirstChildElement("Workbook").FirstChildElement("Worksheet").Element();
    int tableIndex = 0;
    for (; xmlWorkSheet; xmlWorkSheet = xmlWorkSheet->NextSiblingElement("Worksheet"), tableIndex++)
    {
        // create a table object
        nXmlTable* table = n_new(nXmlTable);
        this->tables.Append(table);

        // get table name
        const char* nameAttr = xmlWorkSheet->Attribute("ss:Name");
        table->SetName(nameAttr);

        // get table dimensions, do not use the ExpandedColumnCount
        // and ExpandedRowCount, because they may be misleading
        TiXmlElement* xmlTable = xmlWorkSheet->FirstChildElement("Table");
        n_assert(xmlTable);
        int numRows = this->CountRows(xmlTable);
        int numCols = this->CountCols(xmlTable);
        if ((numRows > 0) && (numCols > 0))
        {
            table->SetDimensions(numRows, numCols);

            // fill table
            int curRow;
            TiXmlElement* xmlRow = xmlTable->FirstChildElement("Row");
            for (curRow = 0; curRow < numRows; curRow++)
            {
                if (xmlRow)
                {
                    int curCol;
                    TiXmlElement* xmlCol = xmlRow->FirstChildElement("Cell");
                    for (curCol = 0; curCol < numCols; curCol++)
                    {
                        if (xmlCol)
                        {
                            // check if cells are skipped
                            int val;
                            int retval = xmlCol->QueryIntAttribute("ss:Index", &val);
                            if (TIXML_SUCCESS == retval)
                            {
                                curCol = val - 1;
                            }
                            TiXmlHandle colHandle(xmlCol);

                            // get direct data element in cell
                            TiXmlText* xmlText = colHandle.FirstChildElement("Data").FirstChild().Text();
                            if (xmlText)
                            {
                                table->Cell(curRow, curCol).Set(xmlText->Value());
                            }
                            else
                            {
                                // check for indirect text (hidden in Font-Statements
                                TiXmlElement* xmlData = colHandle.FirstChildElement("ss:Data").Element();
                                if (xmlData)
                                {
                                    // iterate through child elements, and get their text
                                    nString text;
                                    TiXmlElement* xmlFont;
                                    for (xmlFont = xmlData->FirstChildElement("Font");
                                         0 != xmlFont;
                                         xmlFont = xmlFont->NextSiblingElement("Font"))
                                    {
                                        TiXmlHandle xmlFontHandle(xmlFont);
                                        TiXmlText* xmlFontText = xmlFontHandle.FirstChild().Text();
                                        if (xmlFontText)
                                        {
                                            text.Append(xmlFontText->Value());
                                        }
                                    }
                                    if (!text.IsEmpty())
                                    {
                                        table->Cell(curRow, curCol).Set(text);
                                    }
                                }
                            }
                            xmlCol = xmlCol->NextSiblingElement("Cell");
                        }
                        else
                        {
                            break;
                        }
                    }
                    xmlRow = xmlRow->NextSiblingElement("Row");
                }
                else
                {
                    break;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Find a table by name, return 0 if not found.
*/
nXmlTable*
nXmlSpreadSheet::FindTable(const nString& name) const
{
    n_assert(this->isOpen);
    int i;
    int numTables = this->tables.Size();
    for (i = 0; i < numTables; i++)
    {
        n_assert(this->tables[i]);
        if (this->tables[i]->Name() == name)
        {
            return this->tables[i];
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    Add an external table to the spread sheet object. A copy of the
    table will be created internally.
*/
void
nXmlSpreadSheet::AddTable(nXmlTable* t)
{
    n_assert(t);
    nXmlTable* table = n_new(nXmlTable);
    *table = *t;
    this->tables.Append(table);
}
