//------------------------------------------------------------------------------
//  nlocaleserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "locale/nlocaleserver.h"
#include "xml/nxmlspreadsheet.h"

nNebulaClass(nLocaleServer, "nroot");
nLocaleServer* nLocaleServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nLocaleServer::nLocaleServer() :
    isOpen(false),
    idHashMap(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nLocaleServer::~nLocaleServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    n_assert(this == Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nLocaleServer::Open()
{
    n_assert(!this->IsOpen());
    n_assert(!this->filename.IsEmpty());
    n_assert(0 == this->idHashMap);
    n_assert(0 == this->textArray.Size());

    // load table
    nXmlSpreadSheet sheet;
    sheet.SetFilename(this->filename);
    if (!sheet.Open())
    {
        n_error("nLocaleServer::Open(): Can't open file '%s'!\n", this->filename.Get());
        return false;
    }
    
    if (0 == sheet.NumTables())
    {
        n_error("nLocaleServer::Open(): File '%s' don't contain tables.\n", this->filename.Get());
        return false;
    }

    if (sheet.NumTables() > 1)
    {
        n_message("nLocaleServer::Open(): Warning: File '%s' contain more than one table, reading only the first one.\n", this->filename.Get());
    }
    
    nXmlTable& table = sheet.TableAt(0);
    
    int numRows = table.NumRows();
    if (numRows <= 0)
    {
        n_error("nLocaleServer::Open(): File '%s' table 1 don't contain data!\n", this->filename.Get());
        return false;
    }
    
    // init array and hashmap
    this->idHashMap = n_new(nHashMap(numRows, 8));
    this->textArray.SetSize(numRows);
    
    // parse table entrys
    int row;
    for(row = 0; row < numRows; row++)
    {
        nString ID = table.Cell(row, "ID").AsString();
        

        if (this->idHashMap->Exists(ID.Get()))
        {
            n_error("nLocaleServer::Open(): ID '%s' is not unique!\n", ID.Get());
            return false;
        }
        else
        {
            // fill hashmap
            const ushort key = (*this->idHashMap)[ID.Get()];
            
            // set string in text array
            nString text = table.Cell(row, "Text").AsString();
            this->textArray[key] = this->ParseText(text);
        }
    }

    // cleanup
    sheet.Close();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
nString&
nLocaleServer::ParseText(nString& text)
{
    // replace \n and \t
    text = text.Substitute("\\t", "\t");
    text = text.Substitute("\\n", "\n");
    return text;
}

//------------------------------------------------------------------------------
/**
*/
void
nLocaleServer::Close()
{
    n_assert(this->IsOpen());
    n_assert(0 != this->idHashMap);
    n_assert(this->textArray.Size() > 0);
    
    // cleanup
    n_delete(this->idHashMap);
    this->idHashMap = 0;
    
    this->textArray.SetSize(0);
    
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nLocaleServer::GetLocaleText(const char* id) const
{
    static nString msg;
    n_assert(this->IsOpen());
    
    if (this->idHashMap->Exists(id))
    {
        const ushort key = (*this->idHashMap)[id];
        n_assert(key >= 0 && key < this->textArray.Size());
        return this->textArray[key].Get();
    }
    else
    {
        //n_error("nLocaleServer::GetLocaleText: ID '%s' not found in table '%s'!\n", id, this->filename.Get());
        msg = "Localize: ";
        msg.Append(id);
        return msg.Get();
    }
}
