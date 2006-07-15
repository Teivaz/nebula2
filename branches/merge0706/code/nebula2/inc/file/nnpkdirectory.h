#ifndef N_NPKDIRECTORY_H
#define N_NPKDIRECTORY_H
//------------------------------------------------------------------------------
/**
    @class nNpkDirectory
    @ingroup NPKFile

    npk File Directory Wrapper.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_DIRECTORY_H
#include "kernel/ndirectory.h"
#endif

//------------------------------------------------------------------------------
class nNpkFileServer;
class nNpkTocEntry;
class nNpkDirectory : public nDirectory
{
public:
    /// constructor
    nNpkDirectory();
    /// destructor
    virtual ~nNpkDirectory();
    /// open directory
    virtual bool Open(const nString& dirName);
    /// close directory
    virtual void Close();
    /// determines whether the directory is opened
    virtual bool IsOpen() const;
    /// check if directory is empty
    virtual bool IsEmpty();
    /// set first entry as current
    virtual bool SetToFirstEntry();
    /// set next entry as current
    virtual bool SetToNextEntry();
    /// get name of current entry
    virtual nString GetEntryName();
    /// get type of current entry
    virtual EntryType GetEntryType();

private:
    /// set to first npk entry
    bool SetToFirstNpkEntry();
    /// set to next npk entry
    bool SetToNextNpkEntry();
    /// handle npk file during nDirectory entry enumeration
    void CheckNpkEntryOverride();

    bool isNpkDir;                    ///< true if clean npk dir, otherwise may be mixed filesys/npk dir
    bool npkEntryOverride;            ///< true if npk file entered during filesystem dir enum
    nNpkTocEntry* tocEntry;           ///< pointer to current npk entry, or 0 if current entry is a filesys entry
    nNpkTocEntry* curSearchEntry;     ///< current search entry inside npk file
};

//------------------------------------------------------------------------------
#endif

