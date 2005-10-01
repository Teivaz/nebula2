#ifndef N_INIPREFSERVER_H
#define N_INIPREFSERVER_H
//------------------------------------------------------------------------------
/**
    @class nIniPrefServer
    @ingroup Misc

    @brief A class for reading/writing INI file.

    @verbatim
    [section]
    key = value
    @endvebatim

    (C) 2005 Kim, Hyoun Wo
*/
#include "misc/nprefserver.h"

//------------------------------------------------------------------------------
class nIniPrefServer : public nPrefServer
{
public:
    /// constructor
    nIniPrefServer();
    /// destructor
    virtual ~nIniPrefServer();
    /// open the preferences server
    virtual bool Open();
    /// close the preferences server
    virtual void Close();
    /// check if a key exists
    virtual bool KeyExists(const nString& key);
    /// set .ini filename
    void SetFileName(const nString& filename);
    /// get .ini filename
    const nString& GetFileName() const;
    // set a section name 
    void SetSection(const nString& section);
    /// get a section name
    const nString& GetSection() const;
    /// delete specified section.   
    bool DeleteSection(const nString& section);
    /// delete specified key in a section
    bool DeleteKey(const nString& section, const nString& key);
    /// specify default value
    void SetDefault(const nString& value);
    /// retrieve default value.
    const nString& GetDefault() const;

protected:
    /// write a generic value in string form
    virtual bool WriteGeneric(const nString& key, const nString& value);
    /// read a generic value in string form
    virtual nString ReadGeneric(const nString& key);

private:
    /// max number of the retrieved value string.
    enum { N_MAX_INI_BUF = 1024 };

    /// .ini filename
    nString fileName;
    /// .ini file section name
    nString sectionName;
    /// default value which can be returned when there is no matched key found
    nString defValue;

};
//------------------------------------------------------------------------------
inline
void nIniPrefServer::SetFileName(const nString& filename)
{
    n_assert(!filename.IsEmpty());
    this->fileName = filename;
}
//------------------------------------------------------------------------------
inline
const nString& nIniPrefServer::GetFileName() const
{
    return this->fileName;
}
//------------------------------------------------------------------------------
inline
void nIniPrefServer::SetSection(const nString& section)
{
    this->sectionName = section;
}
//------------------------------------------------------------------------------
inline
const nString& nIniPrefServer::GetSection() const
{
    return this->sectionName;
}
//------------------------------------------------------------------------------
inline
void nIniPrefServer::SetDefault(const nString& value)
{
    this->defValue = value;
}
//------------------------------------------------------------------------------
inline
const nString& nIniPrefServer::GetDefault() const
{
    return this->defValue;
}
//------------------------------------------------------------------------------
#endif
