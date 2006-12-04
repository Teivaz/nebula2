//------------------------------------------------------------------------------
//  nnpkdirectory.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nnpkdirectory.h"
#include "file/nnpkfileserver.h"
#include "file/nnpktocentry.h"

//------------------------------------------------------------------------------
/**
*/
nNpkDirectory::nNpkDirectory() :
    isNpkDir(false),
    npkEntryOverride(false),
    tocEntry(0),
    curSearchEntry(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNpkDirectory::~nNpkDirectory()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkDirectory::Open(const nString& dirName)
{
    n_assert(dirName.IsValid());
    n_assert(!this->IsOpen());

    // first check if the directory can be opened as a normal filesystem directory
    this->isNpkDir = false;
    this->npkEntryOverride = false;
    this->tocEntry = 0;
    this->curSearchEntry = 0;
    if (nDirectory::Open(dirName))
    {
        return true;
    }

    // not a filesystem directory
    this->apath = nNpkFileServer::Instance()->ManglePath(dirName);

    this->tocEntry = ((nNpkFileServer*)nFileServer2::Instance())->FindTocEntry(this->apath);
    if (this->tocEntry && (nNpkTocEntry::DIR == this->tocEntry->GetType()))
    {
        this->isNpkDir = true;
        this->SetToFirstEntry();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nNpkDirectory::Close()
{
    n_assert(this->IsOpen());

    if (this->isNpkDir)
    {
        this->isNpkDir = false;
        this->npkEntryOverride = false;
        this->tocEntry = 0;
        this->curSearchEntry = 0;
    }
    else
    {
        nDirectory::Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkDirectory::IsOpen() const
{
    if (this->isNpkDir)
    {
        return true;
    }
    else
    {
        return nDirectory::IsOpen();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkDirectory::SetToFirstEntry()
{
    if (this->isNpkDir)
    {
        // dir is inside npk file, this is simple
        return this->SetToFirstNpkEntry();
    }
    else
    {
        // Dir is in filesystem, but may contain npk files, this is actually
        // quite simple, since one npk file must contain exactly one root
        // dir. So if we encounter an npk file while enumerating, we simply
        // set the current entry to the root dir in the npk file, and skip
        // the npk file itself.
        bool result = nDirectory::SetToFirstEntry();
        if (result)
        {
            this->CheckNpkEntryOverride();
        }
        return result;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkDirectory::SetToNextEntry()
{
    if (this->isNpkDir)
    {
        // dir is inside npk file
        return this->SetToNextNpkEntry();
    }
    else
    {
        bool result = nDirectory::SetToNextEntry();
        if (result)
        {
            this->CheckNpkEntryOverride();
        }
        return result;
    }
}

//------------------------------------------------------------------------------
/**
*/
nString
nNpkDirectory::GetEntryName()
{
    if (this->isNpkDir || this->npkEntryOverride)
    {
        n_assert(this->curSearchEntry);
        return this->curSearchEntry->GetFullName();
    }
    else
    {
        return nDirectory::GetEntryName();
    }
}

//------------------------------------------------------------------------------
/**
*/
nDirectory::EntryType
nNpkDirectory::GetEntryType()
{
    if (this->isNpkDir || this->npkEntryOverride)
    {
        n_assert(this->curSearchEntry);
        switch (curSearchEntry->GetType())
        {
            case nNpkTocEntry::DIR:
                return nDirectory::DIRECTORY;

            case nNpkTocEntry::FILE:
            default:
                return nDirectory::FILE;

        }
    }
    else
    {
        return nDirectory::GetEntryType();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkDirectory::SetToFirstNpkEntry()
{
    n_assert(this->isNpkDir);
    n_assert(this->tocEntry);

    this->curSearchEntry = this->tocEntry->GetFirstEntry();
    return (0 != this->curSearchEntry);
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkDirectory::SetToNextNpkEntry()
{
    n_assert(this->isNpkDir);
    n_assert(this->tocEntry);
    n_assert(this->curSearchEntry);

    this->curSearchEntry = this->tocEntry->GetNextEntry(this->curSearchEntry);
    return (0 != this->curSearchEntry);
}

//------------------------------------------------------------------------------
/**
    Checks whether the current nDirectory entry is actually an npk file,
    and if yes, intercepts the entry, and replaces it with the
    root directory inside the npk file. This way, an npk file will always
    look like a directory. Interception happens only if the npk file
    is actually known to the fileserver of course, otherwise it will simply
    show up as an npk file.
*/
void
nNpkDirectory::CheckNpkEntryOverride()
{
    n_assert(!this->isNpkDir);

    this->curSearchEntry = 0;
    this->npkEntryOverride = false;

    nString entryName = nDirectory::GetEntryName();
    EntryType entryType  = nDirectory::GetEntryType();
    if (entryName.CheckExtension("npk") && (FILE == entryType))
    {
        // intercept!
        this->curSearchEntry = ((nNpkFileServer*)nFileServer2::Instance())->FindTocEntry(entryName);
        if (this->curSearchEntry)
        {
            this->npkEntryOverride = true;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkDirectory::IsEmpty()
{
    if (this->isNpkDir)
    {
        n_assert(this->tocEntry);
        return (0 == this->tocEntry->GetFirstEntry());
    }
    else
    {
        return nDirectory::IsEmpty();
    }
}

