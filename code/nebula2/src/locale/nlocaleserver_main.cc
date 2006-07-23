//------------------------------------------------------------------------------
//  nlocaleserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "locale/nlocaleserver.h"
#include "xml/nxmlspreadsheet.h"

nNebulaClass(nLocaleServer, "kernel::nroot");
nLocaleServer* nLocaleServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nLocaleServer::nLocaleServer() :
    isOpen(false)
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
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nLocaleServer::GetLocaleText(const char* id)
{
    static nString msg;
    n_assert(this->IsOpen());

    if (this->idHashMap.Exists(id))
    {
        const ushort key = this->idHashMap[id];
        n_assert(key >= 0 && key < this->textArray.Size());
        return this->textArray[key].Get();
    }
    else
    {
        // print warning to log
        n_printf("WARNING: nLocaleServer: Not found ID '%s'!\n", id);
        // create locale string
        msg = "Localize: ";
        msg.Append(id);
        return msg.Get();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLocaleServer::AddLocaleTable(const char* filename)
{
    n_assert(this->IsOpen());
    n_assert(filename);

    // check if the table was already loaded
    int i;
    for (i = 0; i < this->loadedTables.Size(); i++)
    {
        if (this->loadedTables[i] == filename)
        {
            // skip add
            n_printf("nLocaleServer::AddLocaleTable(): file '%s' already added skip it now.\n", filename);
            return;
        }
    }

    // load table
    nXmlSpreadSheet sheet;
    sheet.SetFilename(filename);
    if (!sheet.Open())
    {
        n_error("nLocaleServer::Open(): Can't open file '%s'!\n", filename);
    }

    if (0 == sheet.NumTables())
    {
        n_error("nLocaleServer::Open(): File '%s' don't contain tables.\n", filename);
    }

    if (sheet.NumTables() > 1)
    {
        n_message("nLocaleServer::Open(): Warning: File '%s' contain more than one table, reading only the first one.\n", filename);
    }

    nXmlTable& table = sheet.TableAt(0);

    int numRows = table.NumRows();
    if (numRows <= 0)
    {
        n_error("nLocaleServer::Open(): File '%s' table 1 don't contain data!\n", filename);
    }

    // parse table entrys
    int row;
    for(row = 1; row < numRows; row++)
    {
        nString ID = table.Cell(row, "ID").AsString();

        if (this->idHashMap.Exists(ID.Get()))
        {
            n_error("nLocaleServer::Open(): ID '%s' is not unique!\n", ID.Get());
        }
        else
        {
            // fill hashmap
            const ushort key = this->idHashMap[ID.Get()];

            // set string in text array
            nString text = table.Cell(row, "Text").AsString();
            if (key >= this->textArray.Size())
            {
                // preallocate
                this->textArray.Reserve(key - this->textArray.Size() + 1);
            }
            this->textArray[key] = this->ParseText(text);
        }
    }

    // cleanup
    sheet.Close();

    // remeber loaded table
    this->loadedTables.Append(filename);
}
