#ifndef N_NPKTOC_H
#define N_NPKTOC_H
//------------------------------------------------------------------------------
/**
    @class nNpkToc
    @ingroup NPKFile

    Hold table of content entries for npk files.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_NPKTOCENTRY_H
#include "file/nnpktocentry.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

//------------------------------------------------------------------------------
class nNpkToc
{
public:
    /// constructor
    nNpkToc();
    /// destructor
    ~nNpkToc();
    /// begin a directory specification
    nNpkTocEntry* BeginDirEntry(const char* dirName);
    /// add a file to the current directory
    nNpkTocEntry* AddFileEntry(const char* fileName, int fileOffset, int fileLength);
    /// finish a directory specification
    void EndDirEntry();
    /// find an entry by its absolute path name
    nNpkTocEntry* FindEntry(const char* name);
    /// get the root entry
    nNpkTocEntry* GetRootEntry() const;
    /// set the filesystem path to the root entry
    void SetRootPath(const char* path);
    /// get the filesystem path to the root entry
    const char* GetRootPath() const;

private:
    /// push a directory entry on stack
    void Push(nNpkTocEntry* entry);
    /// pop a directory entry from stack
    nNpkTocEntry* Pop();
    /// strip the root path from an absolute pathname
    const char* StripRootPath(const char* path);

    enum
    {
        STACKSIZE = 16,
    };
    
    nNpkTocEntry* rootDir;      // the top level directory
    nString rootPath;           // filesystem path to the root entry (i.e. d:/nomads)
    nNpkTocEntry* curDir;       // current directory (only valid during BeginDirEntry());
    int curStackIndex;
    nNpkTocEntry* stack[STACKSIZE];
};

//------------------------------------------------------------------------------
/**
    Push toc entry onto stack.
*/
inline
void
nNpkToc::Push(nNpkTocEntry* entry)
{
    n_assert(this->curStackIndex < STACKSIZE);
    this->stack[this->curStackIndex++] = entry;
}

//------------------------------------------------------------------------------
/**
    Pop toc entry from stack.
*/
inline
nNpkTocEntry*
nNpkToc::Pop()
{
    n_assert(this->curStackIndex > 0);
    return this->stack[--this->curStackIndex];
}

//------------------------------------------------------------------------------
/**
*/
inline
nNpkToc::nNpkToc() :
    rootDir(0),
    curDir(0),
    curStackIndex(0)
{
    memset(this->stack, 0, sizeof(this->stack));
}

//------------------------------------------------------------------------------
/**
*/
inline
nNpkToc::~nNpkToc()
{
    if (this->rootDir)
    {
        n_delete(this->rootDir);
    }
}

//------------------------------------------------------------------------------
/**
    Begin defining a directory.
*/
inline
nNpkTocEntry*
nNpkToc::BeginDirEntry(const char* dirName)
{
    nNpkTocEntry* entry = 0;

    if (this->curDir)
    {
        // add as subentry to current dir
        entry = this->curDir->AddDirEntry(dirName);
    }
    else
    {
        n_assert(0 == this->rootDir);

        // create the root entry
        entry = n_new(nNpkTocEntry(this->GetRootPath(), 0, dirName));
        this->rootDir = entry;
    }

    // push current dir on stack and set new current dir (current dir can be 0)
    this->Push(this->curDir);
    this->curDir = entry;
    return entry;
}

//------------------------------------------------------------------------------
/**
    Add a file entry to the current directory.
*/
inline
nNpkTocEntry*
nNpkToc::AddFileEntry(const char* fileName, int fileOffset, int fileLength)
{
    n_assert(this->curDir);

    // create new toc entry and add to subdir
    return this->curDir->AddFileEntry(fileName, fileOffset, fileLength);
}

//------------------------------------------------------------------------------
/**
    Finish defining a directory.
*/
inline
void
nNpkToc::EndDirEntry()
{
    n_assert(curDir);
    
    // pop previous curDir from stack
    this->curDir = this->Pop();
}

//------------------------------------------------------------------------------
/**
    Find an entry by its full name.
*/
inline
nNpkTocEntry*
nNpkToc::FindEntry(const char* absName)
{
    n_assert(this->rootDir);

    // strip the root path
    const char* strippedPath = this->StripRootPath(absName);
    if (!strippedPath)
    {
        return 0;
    }

    // make a copy of the name because we need to modify it
    nString tmpName = strippedPath;
    tmpName.ToLower();

    // for each path component...
    char* curComponent = strtok((char*)tmpName.Get(), "/\\");
    nNpkTocEntry* curEntry = this->rootDir;
    if (0 == strcmp(curEntry->GetName(), curComponent))
    {
        while (curEntry && (curComponent = strtok(0, "/\\")))
        {
            // handle special directory names
            if (strcmp(curComponent, ".") == 0)
            {
                // stay on current level
            }
            else if (strcmp(curComponent, "..") == 0)
            {
                // up to parent
                curEntry = curEntry->GetParent();
            }
            else
            {
                // normal case: one level down
                curEntry = curEntry->FindEntry(curComponent);
            }
        }
        return curEntry;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Return pointer to root entry.
*/
inline
nNpkTocEntry*
nNpkToc::GetRootEntry() const
{
    return this->rootDir;
}

//------------------------------------------------------------------------------
/**
    Set the filesystem path leading to the root entry.
*/
inline
void
nNpkToc::SetRootPath(const char* path)
{
    n_assert(path);
    this->rootPath = path;
}

//------------------------------------------------------------------------------
/**
    Get the filesystem path leading to the root entry.
*/
inline
const char*
nNpkToc::GetRootPath() const
{
    return this->rootPath.IsEmpty() ? 0 : this->rootPath.Get();
}

//------------------------------------------------------------------------------
/**
    Strip the root path from an absolute path. Return 0 if the given
    absolute path does not start with the root path.
*/
inline
const char*
nNpkToc::StripRootPath(const char* absPath)
{
    const char* rootPath = this->GetRootPath();
    n_assert(rootPath);
    int rootPathLen = strlen(rootPath);
    if (0 == strncmp(rootPath, absPath, rootPathLen))
    {
        int absPathLen  = strlen(absPath);
        if (absPathLen > (rootPathLen + 1))
        {
            return &(absPath[rootPathLen + 1]);
        }
        else
        {
            // absPath == rootPath
            return 0;
        }
    }
    // absPath does not start with root path
    return 0;
}

//------------------------------------------------------------------------------
#endif

