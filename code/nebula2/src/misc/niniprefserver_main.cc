//------------------------------------------------------------------------------
//  niniprefserver.h
//  (C) 2005 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/niniprefserver.h"

nNebulaClass(nIniPrefServer, "misc::nprefserver");

//------------------------------------------------------------------------------
/**
*/
nIniPrefServer::nIniPrefServer()
{
}

//------------------------------------------------------------------------------
/**
*/
nIniPrefServer::~nIniPrefServer()
{
}

//------------------------------------------------------------------------------
/**
*/
bool nIniPrefServer::Open()
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void nIniPrefServer::Close()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nIniPrefServer::KeyExists(const nString& key)
{
    n_assert(!key.IsEmpty());

    n_assert(!this->fileName.IsEmpty());
    n_assert(!key.IsEmpty());

    char buf[N_MAX_INI_BUF] = {'\0'};
    return GetPrivateProfileString(this->sectionName.Get(),
                                        key.Get(),
                                        this->defValue.Get(), // should not be null.
                                        buf,
                                        N_MAX_INI_BUF,
                                        this->fileName.Get()) ? true :false;
}

//------------------------------------------------------------------------------
/**
*/
bool nIniPrefServer::WriteGeneric(const nString& key, const nString& value)
{
    n_assert(!this->fileName.IsEmpty());
    n_assert(!key.IsEmpty());

    return WritePrivateProfileString(this->sectionName.Get(),
                                     key.Get(),
                                     value.Get(),
                                     this->fileName.Get()) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
nString nIniPrefServer::ReadGeneric(const nString& key)
{
    n_assert(!this->fileName.IsEmpty());
    n_assert(!key.IsEmpty());

    char buf[N_MAX_INI_BUF] = {'\0'};
    long len = GetPrivateProfileString(this->sectionName.Get(),
                                       key.Get(),
                                       this->defValue.Get(), // should not be null.
                                       buf,
                                       N_MAX_INI_BUF,
                                       this->fileName.Get());

    nString val;
    if (len)
    {
        val.Set(buf, len);
    }
    return val;
}

//------------------------------------------------------------------------------
/**
*/
bool nIniPrefServer::DeleteSection(const nString& section)
{
    return WritePrivateProfileString(section.Get(), NULL, NULL,
                                     this->fileName.Get()) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
bool nIniPrefServer::DeleteKey(const nString& section, const nString& key)
{
    return WritePrivateProfileString(section.Get(), key.Get(), NULL,
                                     this->fileName.Get()) ? true : false;
}
