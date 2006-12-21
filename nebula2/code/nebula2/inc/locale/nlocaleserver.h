#ifndef N_LOCALESERVER_H
#define N_LOCALESERVER_H
//------------------------------------------------------------------------------
/**
    @class nLocaleServer
    @ingroup Locale

    The nLocaleServer object simplifies localization of Nebula2 applications
    to international markets. It reads a "locale table", which is an
    Excel spreadsheet in XML format, which contains 2 columns for original
    text and localized text. Actual translation is done through the
    nLocale() macro.
*/
#include "kernel/nroot.h"
#include "util/nhashmap.h"

//------------------------------------------------------------------------------
class nLocaleServer : public nRoot
{
public:
    /// constructor
    nLocaleServer();
    /// destructor
    virtual ~nLocaleServer();
    /// get instance pointer
    static nLocaleServer* Instance();
    /// add locale table filename
    void AddLocaleTable(const char* filename);
    /// open the server, load filename and parse entries
    bool Open();
    /// close server, cleanup all
    void Close();
    /// is server open?
    bool IsOpen() const;
    /// get localized text for id
    const char* GetLocaleText(const char* id);

private:
    static nLocaleServer* Singleton;

    nString& ParseText(nString& text);

    bool isOpen;
    nHashMap idHashMap; // map of all know ID's from the table
    nArray<nString> textArray; // array of texts from the table, key from idHashMap is index
    nArray<nString> loadedTables; // array of already loaded tables
};

//------------------------------------------------------------------------------
/**
    defines the short form of the text lookup
*/
#define nLocale(text) nLocaleServer::Instance()->GetLocaleText(text)

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLocaleServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
nLocaleServer*
nLocaleServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif
