//------------------------------------------------------------------------------
//  xmltable.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tinyxml/tinyxml.h"
#include "xml/xmltable.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
*/
XmlTable::XmlTable()
{
    Reset();
}

//------------------------------------------------------------------------------
/**
*/
XmlTable::~XmlTable()
{
    if (table != 0)
    {
        n_delete_array(table);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
XmlTable::Load(const nString& fn)
{
    n_assert(!Loaded());
    n_assert(!fn.IsEmpty());

    filename = fn;
    Setup(filename);
}

//------------------------------------------------------------------------------
/**
*/
void 
XmlTable::Unload()
{
    n_assert(Loaded());

    n_delete_array(table);
    Reset();

    n_assert(!Loaded());
}

//------------------------------------------------------------------------------
/**
*/
bool 
XmlTable::GetBool(int row, int col) const
{
    return table[row * width + col].GetBool();
}

//------------------------------------------------------------------------------
/**
*/
float 
XmlTable::GetFloat(int row, int col) const
{
    return table[row * width + col].GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
int 
XmlTable::GetInt(int row, int col) const
{
    return table[row * width + col].GetInt();
}

//------------------------------------------------------------------------------
/**
*/
const nString& 
XmlTable::GetString(int row, int col) const
{
    return table[row * width + col].GetString();
}

//------------------------------------------------------------------------------
/**
*/
const nString& 
XmlTable::GetFileName() const
{
    return filename;
}

//------------------------------------------------------------------------------
/**
*/
int 
XmlTable::GetHeight() const
{
    return height;
}

//------------------------------------------------------------------------------
/**
*/
int 
XmlTable::GetWidth() const
{
    return width;
}

//------------------------------------------------------------------------------
/**
*/
bool 
XmlTable::Loaded() const
{
    return table != 0;
}

//------------------------------------------------------------------------------
/**
*/
bool 
XmlTable::ValidIndex(int row, int col) const
{
    return 0 <= row && row < height &&
           0 <= col && col < width;
}

//------------------------------------------------------------------------------
/**
*/
void 
XmlTable::Reset()
{
    filename = "";
    table = 0;
    height = 0;
    width = 0;
}

//------------------------------------------------------------------------------
/**
    - 22-Mar-04     floh    filename will be mangled internally
*/
void 
XmlTable::Setup(const nString& filename)
{
    n_assert(filename != "");
    n_assert(table == 0);

    nString mangledPath = nFileServer2::Instance()->ManglePath(filename.Get());

    // Try to load file.
    TiXmlDocument document(mangledPath.Get());

    if (!document.LoadFile())
    {
        n_printf("Can't load file `%s'. Aborting.\n", filename.Get());
        return;
    }

    // Determine table measures.
    UpdateTableMeasures(&document);
    if (height <= 0 || width <= 0)
    {
        n_printf("Invalid xml table measures (Height: %d, Width: %d). Aborting.\n",
                 height,
                 width);
        return;
    }

    // Setup table and load data.
    n_printf("Setting up xml table with height=%d and width=%d.\n",
             height,
             width);
    table = n_new_array(XmlTableCell, height * width);
    n_assert(table != 0);
    
    // Extract data from file.
    UpdateTableData(&document);
}

//------------------------------------------------------------------------------
/**
    Retrieve table measures from `document'. Make result available in `width'
    and `height'.
*/
void 
XmlTable::UpdateTableMeasures(TiXmlDocument* document)
{
    // Initialize values.
    height = 0;
    width = 0;

    // Create document handle.
    TiXmlHandle documentHandle(document);
    TiXmlElement* table = 
        documentHandle.FirstChild("Workbook").FirstChild("Worksheet").FirstChild("Table").Element();

    // Valid xml-file?
    if (table != 0)
    {
        // Get measures.
        int temp;
        if (table->QueryIntAttribute("ss:ExpandedRowCount", &temp) == TIXML_SUCCESS)
        {
            height = temp;
        }
        if (table->QueryIntAttribute("ss:ExpandedColumnCount", &temp) == TIXML_SUCCESS)
        {
            width = temp;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Extract data from `document' and put it into table cells.
*/
void 
XmlTable::UpdateTableData(TiXmlDocument* document)
{
    // Create document handle.
    TiXmlHandle documentHandle(document);
    TiXmlElement* xmlTable = 
        documentHandle.FirstChild("Workbook").FirstChild("Worksheet").FirstChild("Table").Element();

    // Valid xml-file?
    if (table != 0)
    {
        int i = 0;
        for (TiXmlElement* row = static_cast<TiXmlElement*>(xmlTable->FirstChildElement("Row"));
             row != 0;
             row = static_cast<TiXmlElement*>(xmlTable->IterateChildren(row)), i++)
        {
            int j = 0;
            for (TiXmlElement* cell = static_cast<TiXmlElement*>(row->FirstChildElement("Cell"));
                 cell != 0;
                 cell = static_cast<TiXmlElement*>(row->IterateChildren(cell)), j++)
            {
                // Update table cell with values from `cell'.
                UpdateCell(&table[i * width + j], cell);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
XmlTable::UpdateCell(XmlTableCell* cell, TiXmlElement* xmlCell)
{
    n_assert(cell != 0);
    n_assert(xmlCell != 0);
    cell->SetString(GetCellValue(xmlCell));
}

//------------------------------------------------------------------------------
/**
*/
const char* 
XmlTable::GetCellValue(TiXmlElement* cell) const
{
    n_assert(cell != 0);

    TiXmlElement* data = cell->FirstChildElement("Data");   
    n_assert(data != 0);

    TiXmlText* text = static_cast<TiXmlText*>(data->FirstChild());
    n_assert(text != 0);
    n_assert(text->Value() != 0);

    return text->Value();
}


