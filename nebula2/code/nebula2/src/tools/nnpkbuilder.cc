//------------------------------------------------------------------------------
//  nnpkbuilder.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nnpkbuilder.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
*/
nNpkBuilder::nNpkBuilder() :
    tocObject(0),
    npkFile(0),
    fileOffset(0),
    dataBlockStart(0),
    dataBlockOffset(0),
    dataSize(0),
    errorCode(NoError)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNpkBuilder::~nNpkBuilder()
{
    n_assert(0 == this->tocObject);
    n_assert(0 == this->npkFile);
}

//------------------------------------------------------------------------------
/**
    Recursively fill the nNpkToc object with the contents of a filesystem
    directory.
*/
bool
nNpkBuilder::GenerateToc(nDirectory* dir, const nString& dirName)
{
    n_assert(dir);
    n_assert(!dirName.IsEmpty());
    n_assert(this->tocObject);
    nFileServer2* fileServer = nFileServer2::Instance();

    // add the directory itself to the tocObject
    nString dirNameLowerCase = dirName;
    dirNameLowerCase.ToLower();
    this->tocObject->BeginDirEntry(dirNameLowerCase.Get());

    // for each directory entry...
    if (!dir->IsEmpty()) do
    {
        // get current directory entry, and convert to lower case
        nString fullEntryName = dir->GetEntryName();
        fullEntryName.ToLower();
        nString entryName = fullEntryName.ExtractFileName();

        // add new entry to toc
        nDirectory::EntryType entryType = dir->GetEntryType();
        if (nDirectory::FILE == entryType)
        {
            // get length of file
            nFile* file = fileServer->NewFileObject();
            n_assert(file);
            int fileLength = 0;
            bool fileOk = false;
            if (file->Open(fullEntryName.Get(), "rb"))
            {
                fileLength = file->GetSize();
                file->Close();
                fileOk = true;
            }
            file->Release();

            if (fileOk)
            {
                this->tocObject->AddFileEntry(entryName.Get(), this->fileOffset, fileLength);
                this->fileOffset += fileLength;
            }
            else
            {
                // FIXME: issue a warning because file could not be opened?
            }
        }
        else if (nDirectory::DIRECTORY == entryType)
        {
            // start a new subdirectory entry
            nDirectory* subDir = fileServer->NewDirectoryObject();
            n_assert(subDir);
            bool subDirOk = false;
            if (subDir->Open(fullEntryName))
            {
                // recurse
                this->GenerateToc(subDir, fullEntryName);
                subDir->Close();
                subDirOk = true;
            }
            n_delete(subDir);
            subDir = 0;

            if (!subDirOk)
            {
                // FIXME: issue a warning?
            }
        }
    }
    while (dir->SetToNextEntry());

    // finish current directory entry
    this->tocObject->EndDirEntry();
    return true;
}

//------------------------------------------------------------------------------
/**
    Recursively write a toc entry (recurses if the toc entry is a
    directory entry.
*/
bool
nNpkBuilder::WriteTocEntry(nNpkTocEntry* tocEntry, bool noTopLevelName)
{
    n_assert(this->npkFile);
    n_assert(tocEntry);

    nNpkTocEntry::Type entryType = tocEntry->GetType();
    nString entryName;
    if (noTopLevelName)
    {
        entryName = "<noname>";
    }
    else
    {
        entryName = tocEntry->GetName();
    }
    int entryNameLen = entryName.Length();
    int entryFileOffset = tocEntry->GetFileOffset();
    int entryFileLength = tocEntry->GetFileLength();

    if (nNpkTocEntry::DIR == entryType)
    {
        // write directory entry, and recurse
        int blockLen = sizeof(short) + entryNameLen;
        this->npkFile->PutInt('DIR_');
        this->npkFile->PutInt(blockLen);
        this->npkFile->PutShort(entryNameLen);
        this->npkFile->Write(entryName.Get(), entryNameLen);

        nNpkTocEntry* curSubEntry = tocEntry->GetFirstEntry();
        while (curSubEntry)
        {
            this->WriteTocEntry(curSubEntry, false);
            curSubEntry = tocEntry->GetNextEntry(curSubEntry);
        }

        // write a final directory end marker
        this->npkFile->PutInt('DEND');
        this->npkFile->PutInt(0);
    }
    else
    {
        // write a file entry
        int blockLen = 2 * sizeof(int) + sizeof(short) + entryNameLen;
        this->npkFile->PutInt('FILE');
        this->npkFile->PutInt(blockLen);
        this->npkFile->PutInt(entryFileOffset);
        this->npkFile->PutInt(entryFileLength);
        this->npkFile->PutShort(entryNameLen);
        this->npkFile->Write(entryName.Get(), entryNameLen);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Recursively write the table of contents to the file.
*/
bool
nNpkBuilder::WriteToc(bool noTopLevelName)
{
    n_assert(this->tocObject);
    n_assert(this->npkFile);

    // write header
    this->npkFile->PutInt('NPK0');     // magic number
    this->npkFile->PutInt(4);          // block len
    this->npkFile->PutInt(0);          // the data offset (fixed later)

    // recursively write toc entries
    return this->WriteTocEntry(this->tocObject->GetRootEntry(), noTopLevelName);
}

//------------------------------------------------------------------------------
/**
    Write the data for an entry into the file, and recurse on directory
    entries.
*/
bool
nNpkBuilder::WriteEntryData(nNpkTocEntry* tocEntry)
{
    n_assert(this->npkFile);
    n_assert(tocEntry);

    nFileServer2* fileServer = nFileServer2::Instance();
    nNpkTocEntry::Type entryType = tocEntry->GetType();
    const char* entryName = tocEntry->GetName();
    int entryFileOffset = tocEntry->GetFileOffset();
    int entryFileLength = tocEntry->GetFileLength();

    if (nNpkTocEntry::DIR == entryType)
    {
        // a dir entry, just recurse
        nNpkTocEntry* curSubEntry = tocEntry->GetFirstEntry();
        while (curSubEntry)
        {
            if (!this->WriteEntryData(curSubEntry))
            {
                return false;
            }
            curSubEntry = tocEntry->GetNextEntry(curSubEntry);
        }
    }
    else if (nNpkTocEntry::FILE == entryType)
    {
        // make sure the file is still consistent with the toc data
        n_assert(this->npkFile->Tell() == (this->dataBlockOffset + entryFileOffset));

        // get the full source path name
        nString fileName = tocEntry->GetFullName();

        // read source file data
        nFile* srcFile = fileServer->NewFileObject();
        n_assert(srcFile);
        if (srcFile->Open(fileName, "rb"))
        {
            // allocate buffer for file and file contents
            char* buffer = n_new_array(char, entryFileLength);
            int bytesRead = srcFile->Read(buffer, entryFileLength);
            n_assert(bytesRead == entryFileLength);
            srcFile->Close();

            // write buffer to target file
            int bytesWritten = this->npkFile->Write(buffer, entryFileLength);
            n_assert(bytesWritten == entryFileLength);
            n_delete(buffer);

            this->dataSize += entryFileLength;
        }
        else
        {
            n_error("nNpkBuilder::WriteEntryData(): failed to open src file '%s'!", fileName);
        }
        srcFile->Release();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Write the data block to the file, and fix the data start offset in
    the file header...
*/
bool
nNpkBuilder::WriteData()
{
    n_assert(this->npkFile);
    n_assert(this->tocObject);

    this->dataBlockStart = this->npkFile->Tell();
    this->dataBlockOffset = this->dataBlockStart + 8;
    this->npkFile->PutInt('DATA');
    int dataSizeOffset = this->npkFile->Tell();
    this->npkFile->PutInt(0);    // fix later...

    this->dataSize = 0;
    if (!this->WriteEntryData(this->tocObject->GetRootEntry()))
    {
        return false;
    }

    // fix block lengths
    this->npkFile->Seek(8, nFile::START);
    this->npkFile->PutInt(this->dataBlockStart);

    this->npkFile->Seek(dataSizeOffset, nFile::START);
    this->npkFile->PutInt(this->dataSize);
    this->npkFile->Seek(0, nFile::END);

    return true;
}

//------------------------------------------------------------------------------
/**
    Parse a directory and create a new NPK file from it.

    @param  rootPath        path to the directory where directory to pack is located
    @param  dirName         directory name inside rootPath (only one path component!)
    @param  npkName         full pathname to output npk file
    @param  noTopLevelName  if true, no root directory name will be stored in the file,
                            so that the npk file's name (without extension) serves as the
                            name of the root directory (use with care, default should be
                            false)
    @return                 true, if all ok, false on error, call GetError() for details
*/
bool
nNpkBuilder::Pack(const nString& rootPath, const nString& dirName, const nString& npkName, bool noTopLevelName)
{
    n_assert(!rootPath.IsEmpty());
    n_assert(!dirName.IsEmpty());
    n_assert(!npkName.IsEmpty());
    n_assert(0 == this->npkFile);
    bool success = false;

    this->SetError(NoError);

    nFileServer2* fileServer = nFileServer2::Instance();

    // open source directory
    nString absDirName = rootPath;
    absDirName.Append("/");
    absDirName.Append(dirName);
    nDirectory* dir = fileServer->NewDirectoryObject();
    if (!dir->Open(absDirName))
    {
        this->SetError(CannotOpenSourceDirectory);
        n_delete(dir);
        return success;
    }

    // open target file
    this->npkFile = fileServer->NewFileObject();
    if (!this->npkFile->Open(npkName.Get(), "w"))
    {
        this->SetError(CannotOpenNpkFile);
        n_delete(dir);
        this->npkFile->Release();
        this->npkFile = 0;
        return success;
    }

    // create table of contents
    this->tocObject = n_new(nNpkToc);
    this->tocObject->SetRootPath(rootPath.Get());

    this->fileOffset = 0;
    if (this->GenerateToc(dir, dirName))
    {
        // write header and toc file...
        if (this->WriteToc(noTopLevelName))
        {
            // write data block
            if (this->WriteData())
            {
                // all done
                success = true;
            }
        }
    }

    // cleanup
    n_delete(this->tocObject);
    this->tocObject = 0;

    this->npkFile->Release();
    this->npkFile = 0;

    dir->Close();
    n_delete(dir);
    return success;
}
