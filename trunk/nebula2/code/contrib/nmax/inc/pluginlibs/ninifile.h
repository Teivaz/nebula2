#ifndef N_INIFILE_H
#define N_INIFILE_H
//-----------------------------------------------------------------------------
/**
    @class nIniFile
    @ingroup NebulaMaxExport2Contrib

    @brief This class provides interfaces for reading and writing key values
           from given .ini file.
*/
class nString;

//-----------------------------------------------------------------------------
class nIniFile
{
public:
    nIniFile(const nString& fileName, const nString& sectionName = "");
    virtual ~nIniFile();

    BOOL DeleteSection(const nString& strSectionName = "");
    BOOL DeleteEntry(const nString& strEntryName, const nString& strSectionName = "");

    BOOL WriteString(const nString& strEntryName, const nString& strValue,
        const nString& strSectionName = "");
    BOOL WriteInt(const nString& strEntryName, int iValue, const nString& strSectionName = "");
    BOOL WriteUINT(const nString& strEntryName, UINT uiValue, const nString& strSectionName = "");
    BOOL WriteFloat(const nString& strEntryName, float fValue, const nString& strSectionName = "");
    BOOL WriteStruct(const nString& strEntryName, LPVOID lpStruct, UINT uSizeStruct, const nString& strSectionName = "");

    BOOL ReadString(const nString& strEntryName, nString& strValue,
        const nString& strDefValue = "", const nString& strSectionName = "");
    BOOL ReadInt(const nString& strEntryName, int& iValue, int iDefValue = 0,
        const nString& strSectionName = "");
    BOOL ReadUINT(const nString& strEntryName, UINT& uiValue, UINT uiDefValue = 0,
        const nString& strSectionName = "");
    BOOL ReadFloat(const nString& strEntryName, float& fValue, float fDefValue = 0.0f,
        const nString& strSectionName = "");
    BOOL ReadStruct(const nString& strEntryName, LPVOID lpStruct, UINT uSizeStruct, const nString& strSectionName = "");

protected:
    nString iniFilename;
    nString sectionName;
};
//-----------------------------------------------------------------------------
#endif
