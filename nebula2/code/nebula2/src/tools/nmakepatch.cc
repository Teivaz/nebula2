//------------------------------------------------------------------------------
//  nmakepatch.cc
//
//  Compares the files in 2 directories, and creates a diff directory
//  with the files which differ between the 2 directories. Uses CRC
//  checksum to decide whether a file has changed.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "tools/ncmdlineargs.h"
#include "kernel/ndirectory.h"
#include "kernel/nfile.h"
#include "util/nstring.h"

nString NewDirPath;
nString OldDirPath;
nString DiffDirPath;
nFile* BatchFile = 0;
bool Different = false;

//------------------------------------------------------------------------------
/**
    Handle a single file. This checks if the file exists in oldDir,
    and if yes does a CRC check whether the 2 files are identical. If they
    are not, or the file does not exist, copy the file into DiffDir.
*/
bool
HandleDiffFile(const nString& filename)
{
    nFileServer2* fileServer = nFileServer2::Instance();

    // build corresponding filename in old dir
    nString oldFileName = filename.Substitute(NewDirPath.Get(), OldDirPath.Get());

    bool copyFile = false;
    if (fileServer->FileExists(oldFileName.Get()))
    {
        // file exists, do a crc check...
        uint newCrc = 0;
        uint oldCrc = 0;
        bool newSuccess = fileServer->Checksum(filename.Get(), newCrc);
        bool oldSuccess = fileServer->Checksum(oldFileName.Get(), oldCrc);
        if (newSuccess && oldSuccess)
        {
            if (newCrc == oldCrc)
            {
                printf("Identical: %s\n", filename.Get());
            }
            else
            {
                printf("Different: %s\n", filename.Get());
                copyFile = true;
            }
        }
        else
        {
            printf("ERROR: FAILED TO OBTAIN CHECKSUM FROM FILES '%s' and '%s'!\n", filename.Get(), oldFileName.Get());
            return false;
        }
    }
    else
    {
        // file does not exist in old dir
        printf("New: %s\n", filename.Get());
        copyFile = true;
    }

    if (copyFile)
    {
        // copy new file to difference directory
        nString diffFileName = filename.Substitute(NewDirPath.Get(), DiffDirPath.Get());
        nString diffFileDir = diffFileName.ExtractDirName();
        fileServer->MakePath(diffFileDir.Get());
        fileServer->CopyFile(filename.Get(), diffFileName.Get());
        Different = true;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Check whether a complete directory tree no longer exists and writes
    a statement to the cleanup batch file. Returns false if directory
    no longer exists.
*/
bool
CheckCleanupDirectory(const nString& dirName)
{
    nFileServer2* fileServer = nFileServer2::Instance();

    // build corresponding filename in new dir
    nString newDirName = dirName.Substitute(OldDirPath.Get(), NewDirPath.Get());
    if (!fileServer->DirectoryExists(newDirName.Get()))
    {
        // directory no longer exists...
        nString relativeDirName = dirName.Substitute(OldDirPath.Get(), "").Substitute("/", "\\").TrimLeft("\\");
        char buf[N_MAXPATH];
        snprintf(buf, sizeof(buf), "if EXIST %%1\\%s (\necho Remove directory %%1\\%s\nrmdir /S /Q %%1\\%s )\n", relativeDirName.Get(), relativeDirName.Get(), relativeDirName.Get());
        BatchFile->PutS(buf);
        Different = true;
        printf("Remove Directory: %s\n", relativeDirName.Get());
        return false;
    }
    // directiry still exists
    return true;
}

//------------------------------------------------------------------------------
/**
    Check if the given file also exists in the NewDir, and if no, creates
    an entry in the BatchFile (which will contain delete statements for
    files and directories that are obsolete.
*/
void
HandleCleanupFile(const nString& filename)
{
    nFileServer2* fileServer = nFileServer2::Instance();

    // build corresponding filename in new dir
    nString newFileName = filename.Substitute(OldDirPath.Get(), NewDirPath.Get());

    // check if file does not exist in new dir
    if (!fileServer->FileExists(newFileName.Get()))
    {
        // add an entry to the batch file
        nString relativeFilename = filename.Substitute(OldDirPath.Get(), "").Substitute("/", "\\").TrimLeft("\\");
        char buf[N_MAXPATH];
        snprintf(buf, sizeof(buf), "if EXIST %%1\\%s (\necho Delete %%1\\%s\ndel /F /Q %%1\\%s )\n", relativeFilename.Get(), relativeFilename.Get(), relativeFilename.Get());
        BatchFile->PutS(buf);
        Different = true;
        printf("Delete: %s\n", relativeFilename.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Recursively walk the given directory and check differences between files.
    If cleanupMode is false, the difference directory will be created, dirName
    must point to the "NewDir".
    If cleanupMode is false, the old directory will be walked through, and
    BatchFile will be filled with delete statements for obsolete files.
*/
void
WalkDirectory(const nString& dirName, bool cleanupMode)
{
    nDirectory* dir = nFileServer2::Instance()->NewDirectoryObject();
    if (dir->Open(dirName.Get()))
    {
        // check if directory still event exists
        if (CheckCleanupDirectory(dirName))
        {
            if (dir->SetToFirstEntry()) do
            {
                if (dir->GetEntryType() == nDirectory::FILE)
                {
                    // handle one file
                    nString fileName = dir->GetEntryName();
                    if (cleanupMode)
                    {
                        HandleCleanupFile(fileName);
                    }
                    else
                    {
                        HandleDiffFile(fileName);
                    }
                }
                else if (dir->GetEntryType() == nDirectory::DIRECTORY)
                {
                    // recurse into subdirectory
                    nString subDirName = dir->GetEntryName();
                    WalkDirectory(subDirName, cleanupMode);
                }
            }
            while (dir->SetToNextEntry());
        }
        dir->Close();
    }
    delete dir;
}

//------------------------------------------------------------------------------
/**
    13-Jun-04   creates a file cleanup.bat, which contains delete statements
                for files that no longer exist in the new release
*/
int
main(int argc, const char** argv)
{
    int retval = 0;
    nCmdLineArgs args(argc, argv);
    nKernelServer kernelServer;

    bool helpArg = args.GetBoolArg("-help");
    OldDirPath  = args.GetStringArg("-olddir", 0);
    NewDirPath  = args.GetStringArg("-newdir", 0);
    DiffDirPath = args.GetStringArg("-diffdir", 0);

    if (helpArg)
    {
        printf("(C) 2004 RadonLabs GmbH\n"
               "nmakepatch - create difference of 2 directories\n\n"
               "-help       show this help\n"
               "-olddir     path to 'old' directory\n"
               "-newdir     path to 'new' directory\n"
               "-diffdir    path to difference directory (must not exist, will be created)\n\n"
               "Return values:\n"
               "0:      difference between directories exists\n"
               "5:      directories are identical\n"
               ">5:     a serious error\n");
        return 5;
    }

    // check args
    if (0 == OldDirPath)
    {
        printf("Error: -olddir arg expected!\n");
        return 10;
    }
    if (0 == NewDirPath)
    {
        printf("Error: -newdir arg expected!\n");
        return 10;
    }
    if (0 == DiffDirPath)
    {
        printf("Error: -diffdir arg expected!\n");
        return 10;
    }

    nFileServer2* fileServer = nFileServer2::Instance();
    if (!fileServer->DirectoryExists(OldDirPath))
    {
        printf("Error: old dir '%s' does not exist!\n", OldDirPath);
        return 10;
    }
    if (!fileServer->DirectoryExists(NewDirPath))
    {
        printf("Error: new dir '%s' does not exist!\n", NewDirPath);
        return 10;
    }
    if (fileServer->DirectoryExists(DiffDirPath))
    {
        printf("Error: diff dir '%s' already exists, please delete!\n", DiffDirPath);
        return 10;
    }

    // create difference dir path
    bool success = fileServer->MakePath(DiffDirPath);
    if (!success)
    {
        printf("Error: failed to create difference dir '%s'!\n", DiffDirPath);
        return 10;
    }

    // pass 1: build difference directory
    WalkDirectory(NewDirPath, false);

    // pass 2: build cleanup batch file (contains obsolete files)
    nString batchFilename = DiffDirPath;
    batchFilename.Append("/cleanup.bat");
    BatchFile = nFileServer2::Instance()->NewFileObject();
    bool batchFileOpened = BatchFile->Open(batchFilename.Get(), "w");
    n_assert(batchFileOpened);
    BatchFile->PutS("@echo off\n");
    BatchFile->PutS("echo Execute cleanup operations...\n");
    WalkDirectory(OldDirPath, true);
    BatchFile->PutS("echo Done cleaning up.\n");
    BatchFile->Close();
    BatchFile->Release();
    BatchFile = 0;

    // return 5 if directories are identical
    if (Different)
    {
        return 0;
    }
    else
    {
        return 5;
    }
}