//------------------------------------------------------------------------------
//  nnpkfilewrapper.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nnpkfilewrapper.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
/**
*/
nNpkFileWrapper::nNpkFileWrapper() :
    isOpen(false),
    dataOffset(0),
    binaryFile(0),
    asciiFile(0)
{

}

//------------------------------------------------------------------------------
/**
*/
nNpkFileWrapper::~nNpkFileWrapper()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open the npk file, parse the table of contents, and create the open
    file handles for binary and ascii read mode.

    @param  fs                  pointer to nFileServer2
    @param  rootPath            root path (where npk file is located)
    @param  absFilename         absolute filename of npk file
*/
bool
nNpkFileWrapper::Open(nFileServer2* fs, const char* rootPath, const char* absFilename)
{
    n_assert(!this->isOpen);
    n_assert(fs);
    n_assert(rootPath);
    n_assert(absFilename);

    // build an absolute filename
    this->absPath = absFilename;

    this->binaryFile = fs->nFileServer2::NewFileObject();
    this->asciiFile  = fs->nFileServer2::NewFileObject();
    n_assert(this->binaryFile);
    n_assert(this->asciiFile);

    if (!this->binaryFile->Open(this->absPath, "rb"))
    {
        n_printf("nNpkFileWrapper: could not open file '%s' in read binary mode!\n", this->absPath.Get());
        this->binaryFile->Release();
        this->asciiFile->Release();
        this->binaryFile = 0;
        this->asciiFile  = 0;
        return false;
    }

    if (!this->asciiFile->Open(this->absPath.Get(), "r"))
    {
        n_printf("nNpkFileWrapper: could not open file '%s' in ascii binary mode!\n", this->absPath.Get());
        this->binaryFile->Close();
        this->binaryFile->Release();
        this->asciiFile->Release();
        this->binaryFile = 0;
        this->asciiFile  = 0;
        return false;
    }

    bool res = this->ParseToc(rootPath);
    if (!res)
    {
        n_printf("nNpkFileWrapper: could not parse table of contents in file '%s'!\n", this->absPath.Get());
        this->binaryFile->Close();
        this->asciiFile->Close();
        this->binaryFile->Release();
        this->asciiFile->Release();
        this->binaryFile = 0;
        this->asciiFile = 0;
        return 0;
    }

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the file handles.
*/
void
nNpkFileWrapper::Close()
{
    n_assert(this->isOpen);
    n_assert(this->binaryFile && this->binaryFile->IsOpen());
    n_assert(this->asciiFile && this->asciiFile->IsOpen());

    this->binaryFile->Close();
    this->asciiFile->Close();
    this->binaryFile->Release();
    this->asciiFile->Release();
    this->binaryFile = 0;
    this->asciiFile = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Get pointer to binary file object.
*/
nFile*
nNpkFileWrapper::GetBinaryFile() const
{
    n_assert(this->isOpen);
    n_assert(this->binaryFile && this->binaryFile->IsOpen());
    return this->binaryFile;
}

//------------------------------------------------------------------------------
/**
    Get pointer to ascii file object.
*/
nFile*
nNpkFileWrapper::GetAsciiFile() const
{
    n_assert(this->isOpen);
    n_assert(this->asciiFile && this->asciiFile->IsOpen());
    return this->asciiFile;
}

//------------------------------------------------------------------------------
/**
    Get toc object reference.
*/
nNpkToc&
nNpkFileWrapper::GetTocObject()
{
    return this->toc;
}

//------------------------------------------------------------------------------
/**
    Read npk file header, make sure its a npk file, and initialize dataOffset
    member.
*/
bool
nNpkFileWrapper::ReadHeader(nFile* file)
{
    int magic = file->GetInt();
    if (magic != 'NPK0')
    {
        return false;
    }
    file->GetInt(); // read blocklen
    int dataBlockStart = file->GetInt();

    this->dataOffset = dataBlockStart + 8;
    return true;
}

//------------------------------------------------------------------------------
/**
    Read a toc entries and add to nNpkToc object.
*/
bool
nNpkFileWrapper::ReadTocEntries(nFile* file)
{
    char nameBuf[N_MAXPATH];

    bool insideToc = true;
    while (insideToc)
    {
        // read next fourcc code and block len
        int fourcc = file->GetInt();
        file->GetInt();

        if ('DIR_' == fourcc)
        {
            // a directory entry
            short dirNameLen = file->GetShort();
            file->Read(nameBuf, dirNameLen);
            nameBuf[dirNameLen] = 0;

            // placeholder root directory name?
            // if yes, replace the directory name
            // with the actual NPK file's name
            if (0 == strcmp(nameBuf, "<noname>"))
            {
                nString absolutePath = this->absPath;
                nString newName = absolutePath.ExtractFileName();
                newName.StripExtension();
                newName.ToLower();
                n_strncpy2(nameBuf, newName.Get(), sizeof(nameBuf));
            }
            nNpkTocEntry* newEntry = this->toc.BeginDirEntry(nameBuf);
            newEntry->SetFileWrapper(this);

//            n_printf("=> dir '%s' entered\n", nameBuf);
        }
        else if ('DEND' == fourcc)
        {
            // a directory end marker
            this->toc.EndDirEntry();
//            n_printf("<= dir\n");
        }
        else if ('FILE' == fourcc)
        {
            // a file
            int fileOffset = file->GetInt();
            int fileLength = file->GetInt();
            short fileNameLen = file->GetShort();
            file->Read(nameBuf, fileNameLen);
            nameBuf[fileNameLen] = 0;

            // add raw data offset to fileOffset
            fileOffset += this->dataOffset;
            nNpkTocEntry* newEntry = this->toc.AddFileEntry(nameBuf, fileOffset, fileLength);
            newEntry->SetFileWrapper(this);

//            n_printf("=> file '%s'\n", nameBuf);
        }
        else
        {
            // end of toc
            insideToc = false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Make sure this is a valid npk file, initialize the toc object, and
    the dataOffset member.
*/
bool
nNpkFileWrapper::ParseToc(const char* rootPath)
{
    // initialize toc's rootpath
    this->toc.SetRootPath(rootPath);

    // read header
    if (!this->ReadHeader(this->binaryFile))
    {
        n_printf("Error reading header in npk file '%s'\n", this->absPath.Get());
        return false;
    }

    // read root toc entry (this will recursively parse the entire toc)
    if (!this->ReadTocEntries(this->binaryFile))
    {
        n_printf("Error parsing table of contents in npk file '%s'\n", this->absPath.Get());
        return false;
    }
    return true;
}



