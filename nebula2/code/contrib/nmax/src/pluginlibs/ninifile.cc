//------------------------------------------------------------------------------
//  ninifile.h
//
//  (C)2004 Kim, Hyoun Woo
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "util/nstring.h"
#include "pluginlibs/ninifile.h"

//------------------------------------------------------------------------------
/**
*/
nIniFile::nIniFile(const nString& strIniFileName, const nString& strSectionName)
: iniFilename(strIniFileName)
, sectionName(strSectionName)
{
}

//------------------------------------------------------------------------------
/**
*/
nIniFile::~nIniFile()
{
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::DeleteSection(const nString& strSectionName)
{
    if (!strSectionName.IsEmpty())
    {
        this->sectionName = strSectionName;
    }

    LPCSTR sectionname = this->sectionName.Get();
    LPCSTR filename    = this->iniFilename.Get();
    return WritePrivateProfileString(sectionname, NULL, NULL, filename);
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::DeleteEntry(const nString& strEntryName, const nString& strSectionName)
{
    if (!strSectionName.IsEmpty())
    {
        this->sectionName = strSectionName;
    }

    LPCSTR sectionname = this->sectionName.Get();
    LPCSTR filename    = this->iniFilename.Get();
    return WritePrivateProfileString(sectionname, strEntryName.Get(), NULL, filename);
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::WriteString(const nString& strEntryName, const nString& strValue,
                           const nString& strSectionName)
{
    if (!strSectionName.IsEmpty())
    {
        this->sectionName = strSectionName;
    }

    LPCSTR sectionname = this->sectionName.Get();
    LPCSTR filename    = this->iniFilename.Get();
    return WritePrivateProfileString(sectionname, strEntryName.Get(), strValue.Get(), filename);
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::WriteInt(const nString& strEntryName, int iValue, const nString& strSectionName)
{
    nString strValue;

    strValue.Format("%i", iValue);

    return WriteString(strEntryName, strValue, strSectionName);
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::WriteUINT(const nString& strEntryName, UINT uiValue, const nString& strSectionName)
{
    nString strValue;

    strValue.Format("%u", uiValue);

    return WriteString(strEntryName, strValue, strSectionName);
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::WriteFloat(const nString& strEntryName, float fValue, const nString& strSectionName)
{
    nString strValue;

    strValue.Format("%g", fValue);

    return WriteString(strEntryName, strValue, strSectionName);
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::WriteStruct(const nString& strEntryName, LPVOID lpStruct, UINT uSizeStruct, 
                           const nString& strSectionName)
{
    if (!strSectionName.IsEmpty())
    {
        sectionName = strSectionName;
    }

    LPCSTR sectionName = this->sectionName.Get();
    LPCSTR filename    = this->iniFilename.Get();
    return WritePrivateProfileStruct(sectionName, strEntryName.Get(), lpStruct, uSizeStruct, filename);
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::ReadString(const nString& strEntryName, nString& strValue,
                          const nString& strDefValue, const nString& strSectionName)
{
    if (!strSectionName.IsEmpty())
    {
        sectionName = strSectionName;
    }

    LPCSTR sectionname = this->sectionName.Get();
    LPCSTR filename = this->iniFilename.Get();

#define MAX_INI_BUFFER 103
    char buf[MAX_INI_BUFFER];
    BOOL result = GetPrivateProfileString(sectionname, strEntryName.Get(), strDefValue.Get(),
                                          buf, MAX_INI_BUFFER, filename);
    int len = strlen(buf);
    strValue.Set(buf,len);

    return result;
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::ReadInt(const nString& strEntryName, int& iValue, int iDefValue,
                       const nString& strSectionName)
{
    nString strValue;
    nString strDefValue;

    strDefValue.Format("%i", iDefValue);

    if (ReadString(strEntryName, strValue, strDefValue, strSectionName))
    {
        iValue = atoi((LPCTSTR) strValue.Get());

        return TRUE;
    }

    return FALSE;
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::ReadUINT(const nString& strEntryName, UINT& uiValue, UINT uiDefValue,
                        const nString& strSectionName)
{
    nString strValue;
    nString strDefValue;

    strDefValue.Format("%u", uiDefValue);

    if (ReadString(strEntryName, strValue, strDefValue, strSectionName))
    {
        uiValue = (UINT) atoi((LPCTSTR) strValue.Get());

        return TRUE;
    }

    return FALSE;
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::ReadFloat(const nString& strEntryName, float& fValue, float fDefValue,
                         const nString& strSectionName)
{
    nString strValue;
    nString strDefValue;

    strDefValue.Format("%g", fDefValue);

    if (ReadString(strEntryName, strValue, strDefValue, strSectionName))
    {
        fValue = (float)(atof((LPCTSTR) strValue.Get()));

        return TRUE;
    }

    return FALSE;
}

//------------------------------------------------------------------------------
/**
*/
BOOL nIniFile::ReadStruct(const nString& strEntryName, LPVOID lpStruct, UINT uSizeStruct, 
                          const nString& strSectionName)
{
    if (!strSectionName.IsEmpty())
    {
        this->sectionName = strSectionName;
    }

    LPCSTR sectionname = this->sectionName.Get();
    LPCSTR filename    = this->iniFilename.Get();
    BOOL result =  GetPrivateProfileStruct(sectionname, strEntryName.Get(), lpStruct, uSizeStruct, filename);

    return result;
}
