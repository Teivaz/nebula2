//------------------------------------------------------------------------------
//  nnpkfileserver_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nnpkfileserver.h"
#include "file/nnpkfilewrapper.h"
#include "file/nnpktocentry.h"
#include "file/nnpkfile.h"
#include "file/nnpkdirectory.h"
#include "tools/nnpkbuilder.h"

nNebulaScriptClass(nNpkFileServer, "nfileserver2");

//------------------------------------------------------------------------------
/**
*/
nNpkFileServer::nNpkFileServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNpkFileServer::~nNpkFileServer()
{
    // delete npk file wrappers
    nNpkFileWrapper* cur;
    while ((cur = (nNpkFileWrapper*) this->npkFiles.RemHead()))
    {
        n_delete(cur);
    }
}

//------------------------------------------------------------------------------
/**
*/
nFile*
nNpkFileServer::NewFileObject()
{
    return n_new(nNpkFile);
}

//------------------------------------------------------------------------------
/**
*/
nDirectory*
nNpkFileServer::NewDirectoryObject()
{
    return n_new(nNpkDirectory);
}

//------------------------------------------------------------------------------
/**
    Check the extension of a pathname.
*/
bool
nNpkFileServer::CheckExtension(const char* path, const char* ext)
{
    n_assert(path);
    const char* dot = strrchr(path, '.');
    if (dot)
    {
        if (strcmp(dot + 1, ext) == 0)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Parses a single npk file and adds it to the list of npk files.
*/
bool
nNpkFileServer::ParseNpkFile(const nString& path)
{
    n_assert(!path.IsEmpty());
    if (this->FileExists(path.Get()))
    {
        nString dirname  = path.ExtractDirName();
        nString absDirPath = this->ManglePath(dirname.Get());
        nString absFilePath = this->ManglePath(path.Get());
        if (this->AddNpkFile(absDirPath, absFilePath))
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Scan through a directory for npk files and create a nNpkFileWrapper
    object for each npk file found, and open the npk files. Returns
    the number of npk files found.
*/
int
nNpkFileServer::ParseDirectory(const nString& dirName, const nString& extension)
{
    n_assert(!dirName.IsEmpty());
    n_assert(!extension.IsEmpty());

    nString absPath = this->ManglePath(dirName.Get());

    // scan directory for npk files...
    int numNpks = 0;
    nDirectory* dir = nFileServer2::NewDirectoryObject();
    n_assert(dir);
    if (dir->Open(absPath.Get()))
    {
        if (!dir->IsEmpty()) do
        {
            const char* entryName = dir->GetEntryName();
            nDirectory::EntryType entryType = dir->GetEntryType();
            if ((entryType == nDirectory::FILE) && this->CheckExtension(entryName, extension.Get()))
            {
                n_printf("*** Reading npk file '%s'\n", entryName);
                if (this->AddNpkFile(absPath, entryName))
                {
                    numNpks++;
                }
            }
        } while (dir->SetToNextEntry());
        dir->Close();
    }
    else
    {
        n_printf("nNpkFileServer::ParseDirectory(): could not open dir '%s'\n", absPath.Get());
    }
    n_delete(dir);
    return numNpks;
}

//------------------------------------------------------------------------------
/**
    Parse a single npk file, creates a nNpkFileWrapper object, adds it the
    the npkFiles list and opens it.
*/
bool
nNpkFileServer::AddNpkFile(const nString& rootPath, const nString& absFilename)
{
    n_assert(!rootPath.IsEmpty());
    n_assert(!absFilename.IsEmpty());

    // first check if file wrapper already exists, and delete it
    nNpkFileWrapper* curFileWrapper = (nNpkFileWrapper*) this->npkFiles.GetHead();
    while (curFileWrapper)
    {
        nNpkFileWrapper* nextFileWrapper = (nNpkFileWrapper*) curFileWrapper->GetSucc();
        if (curFileWrapper->GetAbsPath() == absFilename)
        {
            curFileWrapper->Remove();
            n_delete(curFileWrapper);
        }
        curFileWrapper = nextFileWrapper;
    }

    // create new file wrapper
    nNpkFileWrapper* fileWrapper = n_new(nNpkFileWrapper());
    n_assert(fileWrapper);
    if (fileWrapper->Open(this, rootPath.Get(), absFilename.Get()))
    {
        this->npkFiles.AddTail(fileWrapper);
        return true;
    }
    else
    {
        n_delete(fileWrapper);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Find a nTocEntry by filename. The filename must be absolute.
*/
nNpkTocEntry*
nNpkFileServer::FindTocEntry(const char* absPath)
{
    nNpkFileWrapper* curWrapper;
    for (curWrapper = (nNpkFileWrapper*) this->npkFiles.GetHead(); 
         curWrapper; 
         curWrapper = (nNpkFileWrapper*) curWrapper->GetSucc())
    {
        nNpkToc& toc = curWrapper->GetTocObject();
        nNpkTocEntry* tocEntry = toc.FindEntry(absPath);
        if (tocEntry)
        {
            return tocEntry;
        }
    }
    // not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    Pack a directory into new NPK file. The directory will *NOT* be removed
    after the operation.
    NOTE: if the "noRootName" argument is set, there will be no directory
    name stored in the npk file for the toplevel directory, instead, the
    current filename of the npk file (without extension) will be used
    for the toplevel directory's name when reading from the npk file. Use
    with care, the default should be false.

    @param  rootPath        path to directory where src directory is located
    @param  dirName         single-component-directory name inside root dir
    @param  npkName         filename of npk file
    @param  noTopLevelName  do not fill out the toplevel directory name when packing
*/
bool
nNpkFileServer::Pack(const nString& rootPath, const nString& dirName, const nString& npkName, bool noTopLevelName)
{
    nNpkBuilder npkBuilder;
    return npkBuilder.Pack(rootPath, dirName, npkName, noTopLevelName);
}

//------------------------------------------------------------------------------
/**
    This releases all NPK file wrappers whose filename matches a given
    pattern.
*/
void
nNpkFileServer::ReleaseNpkFiles(const nString& pattern)
{
    nNpkFileWrapper* cur = 0;
    nNpkFileWrapper* next = 0;
    if (cur = (nNpkFileWrapper*) this->npkFiles.GetHead()) do
    {
        next = (nNpkFileWrapper*) cur->GetSucc();
        if (cur->GetAbsPath().MatchPattern(pattern))
        {
            cur->Remove();
            n_delete(cur);
        }
    }
    while (cur = next);
}
