#define N_IMPLEMENTS nNpkFileServer
//------------------------------------------------------------------------------
//  nnpkfileserver_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nnpkfileserver.h"
#include "file/nnpkfilewrapper.h"
#include "file/nnpktocentry.h"
#include "file/nnpkfile.h"
#include "file/nnpkdirectory.h"

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
    while (cur = (nNpkFileWrapper*) this->npkFiles.RemHead())
    {
        n_delete cur;
    }
}

//------------------------------------------------------------------------------
/**
*/
nFile*
nNpkFileServer::NewFileObject()
{
    return n_new nNpkFile(this);
}

//------------------------------------------------------------------------------
/**
*/
nDirectory*
nNpkFileServer::NewDirectoryObject()
{
    return n_new nNpkDirectory(this);
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
    Scan through a directory for npk files and create a nNpkFileWrapper
    object for each npk file found, and open the npk files. Returns
    the number of npk files found.
*/
int
nNpkFileServer::ParseDirectory(const char* dirName)
{
    // absolutize directory name
    char absPath[N_MAXPATH];
    this->ManglePath(dirName, absPath, sizeof(absPath));

    // scan directory for npk files...
    int numNpks = 0;
    nDirectory* dir = nFileServer2::NewDirectoryObject();
    n_assert(dir);
    if (dir->Open(absPath))
    {
        if (!dir->IsEmpty()) do
        {
            const char* entryName = dir->GetEntryName();
            nDirectory::nEntryType entryType = dir->GetEntryType();
            if ((entryType == nDirectory::FILE) && this->CheckExtension(entryName, "npk"))
            {
                n_printf("*** Reading npk file '%s'\n", entryName);
                if (this->ParseNpkFile(absPath, entryName))
                {
                    numNpks++;
                }
            }
        } while (dir->SetToNextEntry());
        dir->Close();
    }
    else
    {
        n_printf("nNpkFileServer::ParseDirectory(): could not open dir '%s'\n", absPath);
    }
    n_delete dir;
    return numNpks;
}

//------------------------------------------------------------------------------
/**
    Parse a single npk file, creates a nNpkFileWrapper object, adds it the
    the npkFiles list and opens it.
*/
bool
nNpkFileServer::ParseNpkFile(const char* rootPath, const char* absFilename)
{
    nNpkFileWrapper* fileWrapper = n_new nNpkFileWrapper();
    n_assert(fileWrapper);
    if (fileWrapper->Open(this, rootPath, absFilename))
    {
        this->npkFiles.AddTail(fileWrapper);
        return true;
    }
    else
    {
        n_delete fileWrapper;
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

