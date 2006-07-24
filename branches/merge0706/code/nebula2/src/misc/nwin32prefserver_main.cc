//------------------------------------------------------------------------------
//  nwin32prefserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nwin32prefserver.h"

nNebulaClass(nWin32PrefServer, "nprefserver");

//------------------------------------------------------------------------------
/**
*/
nWin32PrefServer::nWin32PrefServer() :
    rootKey(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nWin32PrefServer::~nWin32PrefServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nWin32PrefServer::Open()
{
    n_assert(0 == this->rootKey);

    if (nPrefServer::Open())
    {
        // open our root key, or create it if it doesn't exist
        nString keyName = "SOFTWARE\\";
        keyName.Append(this->GetCompanyName());
        keyName.Append("\\");
        keyName.Append(this->GetApplicationName());

        LONG error = RegCreateKeyEx(HKEY_CURRENT_USER,          // hKey
                                    keyName.Get(),              // lpSubKey
                                    0,                          // Reserved
                                    "",                         // lpClass
                                    REG_OPTION_NON_VOLATILE,    // dwOptions
                                    KEY_ALL_ACCESS,             // samDesired
                                    NULL,                       // lpSecurityAttribute
                                    &(this->rootKey),           // phkResult
                                    NULL);                      // lpdwDisposition
        if (ERROR_SUCCESS == error)
        {
            return true;
        }
        else
        {
            this->rootKey = 0;
            nPrefServer::Close();
            return false;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32PrefServer::Close()
{
    n_assert(this->isOpen);
    n_assert(0 != this->rootKey);

    LONG error = RegCloseKey(this->rootKey);
    n_assert(ERROR_SUCCESS == error);
    this->rootKey = 0;

    nPrefServer::Close();
}

//------------------------------------------------------------------------------
/**
*/
bool
nWin32PrefServer::KeyExists(const nString& key)
{
    n_assert(0 != this->rootKey);
    n_assert(!key.IsEmpty());

    HKEY hKey = 0;
    nString keyName = "SOFTWARE\\";
    keyName.Append(this->GetCompanyName());
    keyName.Append("\\");
    keyName.Append(this->GetApplicationName());

    LONG error = RegOpenKeyEx(HKEY_CURRENT_USER,    // hKey
                              keyName.Get(),        // lpSubKey
                              0,                    // ulOptions (reserved)
                              KEY_READ,             // samDesired
                              &hKey);

    if (ERROR_SUCCESS != error)
    {
        // key does not exist
        return false;
    }

    error = RegQueryValueEx(hKey,             // hKey
                            key.Get(),        // lpSubKey
                            0,                // ulOptions (reserved)
                            0,
                            0,
                            0);

    if (ERROR_SUCCESS == error)
    {
        // key exists
        RegCloseKey(hKey);
        return true;
    }
    else
    {
        // key does not exist
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    This method is called by the parent class nPrefServer to store
    persistent data in the registry.
*/
bool
nWin32PrefServer::WriteGeneric(const nString& key, const nString& value)
{
    n_assert(0 != this->rootKey);
    n_assert(!key.IsEmpty());

    // set value
    LONG error = RegSetValueEx(this->rootKey,               // hKey
                               key.Get(),                   // lpValueName
                               0,                           // Reserved
                               REG_SZ,                      // dwType (normal string)
                               (const BYTE*) value.Get(),   // lpData
                               value.Length() + 1);         // cbData
    return (ERROR_SUCCESS == error);
}

//------------------------------------------------------------------------------
/**
    This method is called by the parent class nPrefServer to read
    persistent data from the registry.
*/
nString
nWin32PrefServer::ReadGeneric(const nString& key)
{
    n_assert(0 != rootKey);
    n_assert(!key.IsEmpty());

    // get value (max 1024 bytes in size)
    nString returnValue;
    char buf[1024];
    DWORD bufSize = sizeof(buf);
    LONG error = RegQueryValueEx(this->rootKey,         // hKey
                                 key.Get(),             // lpValueName
                                 NULL,                  // lpReserved
                                 NULL,                  // lpType
                                 (BYTE*)&buf[0],        // lpData
                                 &bufSize);             // lpcbData
    if (ERROR_SUCCESS == error)
    {
        returnValue = buf;
    }
    return returnValue;
}

