#ifndef XML_XMLTABLE_H
#define XML_XMLTABLE_H
//------------------------------------------------------------------------------
/**
    @class XmlTable
    @ingroup Obsolete

    Multi key table based on an (Excel exported) xml file.
    
    (C) 2003-2004 RadonLabs GmbH
*/

#include "util/nstring.h"
#include "xml/xmltablecell.h"

class TiXmlDocument;
class TiXmlElement;

//------------------------------------------------------------------------------
class XmlTable
{
public:
    /// Construct
    XmlTable();
    /// Destruct
    ~XmlTable();

    /// Load from file associated with filename `fn'.
    void Load(const nString& fn);
    /// Destroy table.
    void Unload();

    /// Item at index `row', `col' as boolean. Cell must be of type `bool'.
    bool GetBool(int row, int col) const;
    /// Item at index `row', `col' as float. Cell must be of type `float'.
    float GetFloat(int row, int col) const;
    /// Item at index `row', `col' as integer. Cell must be of type `int'.
    int GetInt(int row, int col) const;
    /// Item at index `row', `col' as string. Cell must be of type `string'.
    const nString& GetString(int row, int col) const;

    /// Name of file table is loaded from.
    const nString& GetFileName() const;
    /// Table height.
    int GetHeight() const;
    /// Table width.
    int GetWidth() const;

    /// Is table initialized?
    bool Loaded() const;
    /// Is index (i, j) a valid index?
    bool ValidIndex(int row, int col) const;

private:
    /// Reset to initial values.
    void Reset();
    /// Make table from xml file.
    void Setup(const nString& filename);
    /// Update table measures from values in `document'.
    void UpdateTableMeasures(TiXmlDocument* document);
    /// Extract data from xml document.
    void UpdateTableData(TiXmlDocument* document);

    /// Update table cell `cell' with values from xml cell `xmlCell'.
    void UpdateCell(XmlTableCell* cell, TiXmlElement* xmlCell);
    /// Value `cell' is containing as string.
    const char* GetCellValue(TiXmlElement* cell) const;

    // Data
    nString filename;
    XmlTableCell* table;
    int height;
    int width;
};

//------------------------------------------------------------------------------
#endif

