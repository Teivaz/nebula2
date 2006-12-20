//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nsystem.h"
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/ndirectory.h"
#include "kernel/ncrc.h"
#include "util/nstring.h"
#ifdef __WIN32__
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#if defined(__MACOSX__)
#include <Carbon/carbon.h>
#endif

nNebulaScriptClass(nFileServer2, "nroot");

nFileServer2* nFileServer2::Singleton = 0;

//------------------------------------------------------------------------------
/**

    history:
     - 30-Jan-2002   peter    created
*/
nFileServer2::nFileServer2() :
    bytesRead(0),
    bytesWritten(0),
    numSeeks(0)
{
    n_assert(0 == Singleton);
    Singleton = this;

    // Initialize assign repository if not already exists. The latter may happen
    // if nKernelServer::ReplaceFileServer() was used to set another
    // standard file server.
    nRoot* assignRoot = kernelServer->Lookup("/sys/share/assigns");
    if (0 == assignRoot)
    {
        this->InitAssigns();
    }
    else
    {
        this->assignDir = assignRoot;
    }
}

//------------------------------------------------------------------------------
/**

    history:
     - 30-Jan-2002   peter    created
*/
nFileServer2::~nFileServer2()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    creates new or modifies existing assign under /sys/share/assigns

    @param assignName      the name of the assign
    @param pathName        the path to which the assign links

    history:
     - 30-Jan-2002   peter    created
*/
bool
nFileServer2::SetAssign(const nString& assignName, const nString& pathName)
{
    // make sure trailing slash exists
    nString pathString = pathName;
    pathString.StripTrailingSlash();
    pathString.Append("/");

    // ex. das Assign schon?
    kernelServer->PushCwd(this->assignDir.get());
    nEnv* env = (nEnv*)this->assignDir->Find(assignName.Get());
    if (!env)
    {
        env = (nEnv*)kernelServer->New("nenv", assignName.Get());
        n_assert(env);
    }
    env->SetS(pathString.Get());
    kernelServer->PopCwd();
    return true;
}

//------------------------------------------------------------------------------
/**
    queries existing assign under /sys/share/assigns

    @param assignName      the name of the assign
    @return                the path to which the assign links, or NULL if
                           assign is undefined
    history:
    - 30-Jan-2002   peter    created
*/
nString
nFileServer2::GetAssign(const nString& assignName)
{
    nEnv* env = (nEnv*)this->assignDir->Find(assignName.Get());
    if (env)
    {
        return env->GetS();
    }
    n_printf("Assign '%s' not defined!\n", assignName.Get());
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Remove all existing assigns and setup base assigns.
*/
void
nFileServer2::ResetAssigns()
{
}

//------------------------------------------------------------------------------
/**
    Returns a cleaned up path name (replaces backslashes with slashes,
    and removes trailing slash if exists.
*/
void
nFileServer2::CleanupPathName(nString& str)
{
    nString pathString(str.Get());
    pathString.ConvertBackslashes();
    pathString.StripTrailingSlash();
    str = pathString.Get();

/*    n_assert(path);

    char* ptr = path;
    char c;

#ifdef __XBxX__
    // on xbox replace slashes with backslashes
    while (c = *ptr)
    {
        if (c == '/')
        {
            *ptr = '\\';
        }
        else
        {
            *ptr = c;
        }
        ptr++;
    }
    // remove trailing slash
    if ((ptr > path) && (*(--ptr) == '\\'))
    {
        *ptr = 0;
    }
#else
    // on all other systems replace backslashes with slashes
    while ((c = *ptr))
    {
        if (c == '\\')
        {
            *ptr = '/';
        }
        else
        {
            *ptr = c;
        }
        ptr++;
    }
    // remove trailing slash
    if ((ptr > path) && (*(--ptr) == '/'))
    {
        *ptr = 0;
    }
#endif
*/
}

//------------------------------------------------------------------------------
/**
    Expands assign in path to full absolute path, replaces any backslashes
    by slashes, removes any trailing slash, and makes the path absolute.

    Please note that Nebula does not know the concept of a current working
    directory, thus, all paths MUST be absolute (please note that Nebula
    assigns can be used to create position independent absolute paths).

    @param pathName   the path to expand
    @return           resulting string

    history:
    - 30-Jan-2002   peter    created
*/
nString
nFileServer2::ManglePath(const nString& pathName)
{
    nString pathString = pathName;

    // check for assigns
    int colonIndex;
    while ((colonIndex = pathString.FindCharIndex(':', 0)) > 0)
    {
        // special case: ignore one character "assigns" because they are
        // really DOS drive letters
        if (colonIndex > 1)
        {
            nString assignString = pathString.ExtractRange(0, colonIndex);
            nString postAssignString = pathString.ExtractRange(colonIndex + 1, pathString.Length() - (colonIndex + 1));
            nString replace = this->GetAssign(assignString);
            if (!replace.IsEmpty())
            {
                replace.Append(postAssignString);
            }
            pathString = replace;
        }
        else break;
    }
    this->CleanupPathName(pathString);
    return pathString;
}

//------------------------------------------------------------------------------
/**
    creates a new nDirectory object

    @return    the nDirectory object

    history:
    - 30-Jan-2002   peter    created
*/
nDirectory*
nFileServer2::NewDirectoryObject() const
{
    nDirectory* result = n_new(nDirectory);
    n_assert(result != 0);
    return result;
}

//------------------------------------------------------------------------------
/**
    creates a new nFile object

    @return          the nFile object

    history:
    - 30-Jan-2002   peter    created
*/
nFile*
nFileServer2::NewFileObject() const
{
    nFile* result = n_new(nFile);
    n_assert(result != 0);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
nFileServer2::InitAssigns()
{
    n_assert(!this->assignDir.isvalid());
    this->assignDir = kernelServer->New("nroot", "/sys/share/assigns");
    this->InitHomeAssign();
    this->InitBinAssign();
    this->InitUserAssign();
    this->InitTempAssign();
}

//------------------------------------------------------------------------------
/**
    Initialize Nebula's home directory assign ("home:").

    - 14-May-04  floh   Win32: if parent directory not "win32", use the executable's
                        directory as home:
*/
void
nFileServer2::InitHomeAssign()
{
#ifdef __XBxX__
    this->SetAssign("home", "d:/");
#else
    #if __WIN32__
        // Win32: Check for the NEBULADIR environment variable first,
        // then try to find the nkernel.dll module handle's filename
        // and cut off the last 2 directories
        //
        // *** NOTE BY FLOH ***
        // Checking for a NEBULADIR env variable is a bad idea because it may
        // lead to hard to find end-user problems if several shipped Nebula
        // application exist on the machine and the user has defined
        // the NEBULADIR variable (for instance because the user happens
        // to be a Nebula developer). This happened when we shipped Nomads
        // and people had the Nebula SDK installed which required a
        // NOMADS_HOME variable :(
        /*
        char* s = getenv("NEBULADIR");
        if (s)
        {
            n_strncpy2(buf,s,sizeof(buf));
        }
        else
        {
        */

        // use the executable's directory to locate the home directory
        char buf[N_MAXPATH];
        DWORD res = GetModuleFileName(NULL, buf, sizeof(buf));
        if (res == 0)
        {
            n_error("nFileServer2::InitHomeAssign(): GetModuleFileName() failed!\n");
        }

        nString pathToExe(buf);
        pathToExe.ConvertBackslashes();

        // check if executable resides in a win32 directory
        nString pathToDir = pathToExe.ExtractLastDirName();
        // converted to lowercase because sometimes the path is in uppercase
        pathToDir.ToLower();
        if (pathToDir == "win32" || pathToDir == "win32d")
        {
            // normal home:bin/win32 directory structure
            // strip bin/win32
            nString homePath = pathToExe.ExtractDirName();
            homePath.StripTrailingSlash();
            homePath = homePath.ExtractDirName();
            homePath.StripTrailingSlash();
            homePath = homePath.ExtractDirName();
            this->SetAssign("home", homePath);
        }
        else
        {
            // not in normal home:bin/win32 directory structure,
            // use the exe's directory as home path
            nString homePath = pathToExe.ExtractDirName();
            this->SetAssign("home", homePath);
        }
    #elif defined(__LINUX__)
        // under Linux, the NEBULADIR environment variable must be set,
        // otherwise the current working directory will be used
        char buf[N_MAXPATH];
        char *s = getenv("NEBULADIR");
        if (s)
        {
            n_strncpy2(buf,s,sizeof(buf));
        }
        else
        {
            n_error("Env variable NEBULADIR not set! Aborting.");
        }
        // if last char is not a /, append one
        if ((strlen(buf) > 0) && (buf[strlen(buf)-1] != '/'))
        {
            strcat(buf,"/");
        }
        this->SetAssign("home", buf);
    #elif defined(__MACOSX__)
        char buf[N_MAXPATH];
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef bundleURL = CFBundleCopyBundleURL(mainBundle);
        FSRef bundleFSRef;
        CFURLGetFSRef(bundleURL, &bundleFSRef);
        FSRefMakePath(&bundleFSRef, (unsigned char*)buf, N_MAXPATH);
        // if last char is not a /, append one
        if ((strlen(buf) > 0) && (buf[strlen(buf)-1] != '/'))
        {
            strcat(buf,"/");
        }
        this->SetAssign("home", buf);
    #else
    #error nFileServer::initHomeAssign() not implemented!
    #endif
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nFileServer2::InitBinAssign()
{
#ifdef __XBxX__
    this->SetAssign("bin", "d:/");
#else
    #ifdef __WIN32__
        // use the executable's directory to locate the bin directory
        char buf[N_MAXPATH];
        DWORD res = GetModuleFileName(NULL, buf, sizeof(buf));
        if (res == 0)
        {
            n_error("nFileServer2::InitHomeAssign(): GetModuleFileName() failed!\n");
        }
        nString pathToExe = buf;
        pathToExe.ConvertBackslashes();
        nString binPath = pathToExe.ExtractDirName();
        this->SetAssign("bin", binPath);

    #elif defined(__LINUX__)

        nString home_dir = this->GetAssign("home");
        home_dir += "bin/linux/";
        this->SetAssign("bin", home_dir);

    #elif defined(__MACOSX__)

        nString home_dir = this->GetAssign("home");
        home_dir += "bin/macosx/";
        this->SetAssign("bin", home_dir);

    #else
    #error nFileServer::initBinAssign() not implemented!
    #endif

#endif
}

//------------------------------------------------------------------------------
/**
    Initialize the user assign. This is where the application should
    save any type of data, like save games or config options, since
    applications may not have write access to the home: directory (which is by
    tradition the application directory.

    On the Xbox, the user assign points to the application's
    hard disk partition.

    On Windows, the user assign points to CSIDL_PERSONAL.

    On Unix, the user assign should point to the user's home
    directory.
*/
void
nFileServer2::InitUserAssign()
{
#ifdef __XBxX__
    this->SetAssign("user", "d:/");
#elif defined(__WIN32__)
    char rawPath[MAX_PATH];
    HRESULT hr = nWin32Wrapper::Instance()->SHGetFolderPath(0,      // hwndOwner
                    CSIDL_PERSONAL | CSIDL_FLAG_CREATE,       // nFolder
                    NULL,                                     // hToken
                    0,                                        // dwFlags
                    rawPath);                                 // pszPath
    n_assert(S_OK == hr);

    nString path(rawPath);
    path.ConvertBackslashes();
    path.Append("/");
    this->SetAssign("user", path);
#elif defined(__LINUX__) || defined(__MACOSX__)
    nString path(getenv("HOME"));
    path.Append("/");
    this->SetAssign("user", path);
#else
#error "IMPLEMENT ME!"
#endif
}

//------------------------------------------------------------------------------
/**
    Initialize the standard temp: assign.
*/
void
nFileServer2::InitTempAssign()
{
#ifdef __XBxXX__
    this->SetAssign("temp", "d:/");
#elif __WIN32__
    char rawPath[MAX_PATH];
    DWORD numChars = GetTempPath(sizeof(rawPath), rawPath);
    n_assert(numChars > 0);

    nString path(rawPath);
    path.ConvertBackslashes();
    path.Append("/");
    this->SetAssign("temp", path);
#elif defined(__LINUX__) || defined(__MACOSX__)
    const char * tmpDir = getenv("TMPDIR");
    if (NULL == tmpDir)
    {
        tmpDir = "/tmp";
    }
    nString path(tmpDir);
    path.Append("/");
    this->SetAssign("user", path);
#else
#error "IMPLEMENT ME!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileServer2::FileExists(const nString& pathName) const
{
    n_assert(pathName != 0);
    bool result = false;
    nFile* file = this->NewFileObject();
    if (file->Exists(pathName))
    {
        result = true;
    }
    file->Release();
    return result;
}

//------------------------------------------------------------------------------
/**
*/
int
nFileServer2::GetFileSize(const nString& pathName) const
{
    n_assert(pathName.IsValid());
    int size = 0;
    nFile* file = this->NewFileObject();
    if (file->Open(pathName, "r"))
    {
        size = file->GetSize();
        file->Close();
    }
    file->Release();
    return size;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileServer2::DirectoryExists(const nString& pathName) const
{
    n_assert(pathName != 0);
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(pathName))
    {
        dir->Close();
        n_delete(dir);
        return true;
    }
    n_delete(dir);
    return false;
}

//------------------------------------------------------------------------------
/**
    Make any missing directories in path.
*/
bool
nFileServer2::MakePath(const nString& dirName)
{
    nDirectory* dir = this->NewDirectoryObject();
    n_assert(dir);

    // build stack of non-existing dir components
    nString path = this->ManglePath(dirName).Get();
    nArray<nString> pathStack;
    while (!path.IsEmpty() && !dir->Open(path))
    {
        pathStack.Append(path);
        nString nextPath = path.ExtractDirName().TrimRight("/\\");
        if (nextPath == path)
        {
            // no more slashes in path, stop loop
            break;
        }
        path = nextPath;
    }
    if (dir->IsOpen())
    {
        dir->Close();
    }
    n_delete(dir);

    // error?
    if (path.IsEmpty())
    {
        return false;
    }

    // create missing directory components
    int i;
    for (i = pathStack.Size() - 1; i >= 0; --i)
    {
        const nString& curPath = pathStack[i];
        #ifdef __WIN32__
            int err = _mkdir(curPath.Get());
        #else
            int err = mkdir(curPath.Get(), S_IRWXU|S_IRWXG);
        #endif
        if (-1 == err)
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Copy a file.
    FIXME: the Non-Win32 version reads the entire file is into RAM!

    - 09-Mar-04     floh    Win32 version now removes write protection on
                            target file before copying.
    - 17-Oct-05     floh    Win32 version now uses nFile so that the source
                            can also live in an NPK archive
*/
bool
nFileServer2::CopyFile(const nString& from, const nString& to)
{
    #ifdef __WIN32__
        // copy the file through Nebula2 file routines, this makes
        // sure that we can also copy from archive files
        bool success = false;

        // if the target file exists, remove the read/only file attribute
        if (this->FileExists(to))
        {
            DWORD fileAttrs = GetFileAttributes(to.Get());
            fileAttrs &= ~FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(to.Get(), fileAttrs);
        }
        nFile* fromFile = this->NewFileObject();
        if (fromFile->Open(from, "rb"))
        {
            nFile* toFile = this->NewFileObject();
            if (toFile->Open(to, "wb"))
            {
                int bufSize = fromFile->GetSize();
                void* buf = n_malloc(bufSize);
                int bytesRead = fromFile->Read(buf, bufSize);
                n_assert(bytesRead == bufSize);
                int bytesWritten = toFile->Write(buf, bufSize);
                n_assert(bytesWritten == bufSize);
                n_free(buf);
                toFile->Close();
                success = true;
            }
            else
            {
                n_error("nFileServer2::CopyFile(%s, %s): could not open target file!", from.Get(), to.Get());
            }
            toFile->Release();
            fromFile->Close();
        }
        else
        {
            n_error("nFileServer2::CopyFile(%s, %s): could not open source file!", from.Get(), to.Get());
        }
        fromFile->Release();
        return success;
    #else
        nFile* fromFile = this->NewFileObject();
        if (!fromFile->Open(from, "rb"))
        {
            n_printf("nFileServer2::Copy(): could not open source file '%s'\n", from.Get());
            fromFile->Release();
            return false;
        }
        nFile* toFile = this->NewFileObject();
        if (!toFile->Open(to, "wb"))
        {
            n_printf("nFileServer2::Copy(): could not open dest file '%s'\n", to.Get());
            fromFile->Close();
            fromFile->Release();
            toFile->Release();
            return false;
        }

        int size = fromFile->GetSize();
        n_assert(size > 0);
        void* buffer = n_malloc(size);
        n_assert(buffer);
        int numRead = fromFile->Read(buffer, size);
        n_assert(numRead == size);
        int numWritten = toFile->Write(buffer, size);
        n_assert(numWritten == size);
        n_free(buffer);

        fromFile->Close();
        toFile->Close();
        fromFile->Release();
        toFile->Release();
        return true;
    #endif
}

//------------------------------------------------------------------------------
/**
    Delete a file.
*/
bool
nFileServer2::DeleteFile(const nString& filename)
{
    nString mangledPath = this->ManglePath(filename);

    #ifdef __WIN32__
        return ::DeleteFile(mangledPath.Get()) ? true : false;
    #elif defined(__LINUX__) || defined(__MACOSX__)
        return (0 == unlink(mangledPath.Get())) ? true : false;
    #else
    #error "nFileServer2::DeleteFile() not implemented yet!"
    #endif
}

//------------------------------------------------------------------------------
/**
    Delete an empty directory.
*/
bool
nFileServer2::DeleteDirectory(const nString& dirName)
{
    nString mangledPath = this->ManglePath(dirName);

    #ifdef __WIN32__
    return ::RemoveDirectory(mangledPath.Get()) ? true : false;
    #elif defined(__LINUX__) || defined(__MACOSX__)
    return (rmdir(mangledPath.Get()) == 0) ? true : false;
    #else
    #error "nFileServer2::DeleteDirectory() not implemented yet!"
    #endif
}

//------------------------------------------------------------------------------
/**
    Create a filenode and return its full path name. A filenode is a
    nFile object wrapped into a nRoot subclass. It offers access to filesystem
    functionality for scripting languages which don't offer access to
    the host filesystem (like MicroTcl).
*/
nFileNode*
nFileServer2::CreateFileNode(const nString& name)
{
    nString path = "/sys/share/files/";
    path += name;
    if (kernelServer->Lookup(path.Get()))
    {
        n_error("nFileServer2: file node '%s' exists!", name.Get());
        return 0;
    }
    nFileNode* fileNode = (nFileNode*) kernelServer->New("nfilenode", path.Get());
    return fileNode;
}

//------------------------------------------------------------------------------
/**
    Compute the CRC checksum for a file.
    FIXME: the current implementation loads the entire file into memory.

    @param  filename    [in]    a Nebula filename
    @param  crc         [out]   the computed CRC checksum
    @return             true if all ok, false if file could not be opened
*/
bool
nFileServer2::Checksum(const nString& filename, uint& crc)
{
    crc = 0;
    bool success = false;
    nFile* file = this->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "rb"))
    {
        // read file into RAM buffer
        int numBytes = file->GetSize();
        uchar* buf = (uchar*) n_malloc(numBytes);
        n_assert(buf);

        int numRead = file->Read(buf, numBytes);
        n_assert(numRead == numBytes);

        // compute CRC
        nCRC crcSummer;
        crc = crcSummer.Checksum(buf, numBytes);

        // free and close everything
        n_free(buf);
        file->Close();
        success = true;
    }
    file->Release();
    return success;
}

//------------------------------------------------------------------------------
/**
    Set the read-only status of a file.
*/
void
nFileServer2::SetFileReadOnly(const nString& filename, bool readOnly)
{
    nString mangledPath = this->ManglePath(filename);
#ifdef __WIN32__
    DWORD fileAttrs = GetFileAttributes(mangledPath.Get());
    if (readOnly)
    {
        fileAttrs |= FILE_ATTRIBUTE_READONLY;
    }
    else
    {
        fileAttrs &= ~FILE_ATTRIBUTE_READONLY;
    }
    SetFileAttributes(mangledPath.Get(), fileAttrs);
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct stat s;
    if (stat(mangledPath.Get(), &s) != -1)
    {
        mode_t mode = s.st_mode;
        if (readOnly)
        {
            // Remove all write flags...
            mode &= ~S_IWUSR;
            mode &= ~S_IWGRP;
            mode &= ~S_IWOTH;
        }
        else
        {
            mode |= S_IWUSR;
        }
        chmod(mangledPath.Get(), mode);
    }
#else
#error "nFileServer2::SetFileReadOnly() not implemented yet!"
#endif
}

//------------------------------------------------------------------------------
/**
    Get the read-only status of a file. If the file does not exist,
    the routine returns false.
*/
bool
nFileServer2::IsFileReadOnly(const nString& filename)
{
    nString mangledPath = this->ManglePath(filename);
#ifdef __WIN32__
    DWORD fileAttrs = GetFileAttributes(mangledPath.Get());
    return (fileAttrs & FILE_ATTRIBUTE_READONLY);
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct stat s;
    if (stat(mangledPath.Get(), &s) == -1)
    {
        return false;
    }
    if (s.st_uid == geteuid() && (s.st_mode & S_IRWXU))
    {
        return true;
    }
    else if (s.st_gid == getegid() && (s.st_mode & S_IRWXG))
    {
        return true;
    }
    return false;
#else
#error "nFileServer2::IsFileReadOnly() not implemented yet!"
#endif
}

//------------------------------------------------------------------------------
/**
    List all files in a directory, ignores subdirecories.
*/
nArray<nString>
nFileServer2::ListFiles(const nString& dirName)
{
    nArray<nString> fileList;
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(dirName))
    {
        if (dir->SetToFirstEntry()) do
        {
            if (dir->GetEntryType() == nDirectory::FILE)
            {
                fileList.Append(dir->GetEntryName());
            }
        }
        while (dir->SetToNextEntry());
        dir->Close();
    }
    n_delete(dir);
    return fileList;
}

//------------------------------------------------------------------------------
/**
    List all files in a directory matching given pattern, ignores subdirectories.
*/
nArray<nString>
nFileServer2::ListMatchingFiles(const nString& dirName, const nString& pattern)
{
    nArray<nString> fileList;
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(dirName))
    {
        if (dir->SetToFirstEntry()) do
        {
            if ((dir->GetEntryType() == nDirectory::FILE) && dir->GetEntryName().MatchPattern(pattern))
            {
                fileList.Append(dir->GetEntryName());
            }
        }
        while (dir->SetToNextEntry());
        dir->Close();
    }
    n_delete(dir);
    return fileList;
}

//------------------------------------------------------------------------------
/**
    List all subdirectories in a directory, ignores files.
*/
nArray<nString>
nFileServer2::ListDirectories(const nString& dirName)
{
    nArray<nString> dirList;
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(dirName))
    {
        if (dir->SetToFirstEntry()) do
        {
            if (dir->GetEntryType() == nDirectory::DIRECTORY)
            {
                dirList.Append(dir->GetEntryName());
            }
        }
        while (dir->SetToNextEntry());
        dir->Close();
    }
    n_delete(dir);
    return dirList;
}

//------------------------------------------------------------------------------
/**
    List all subdirectories in a directory matching the given pattern, ignores files.
*/
nArray<nString>
nFileServer2::ListMatchingDirectories(const nString& dirName, const nString& pattern)
{
    nArray<nString> dirList;
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(dirName))
    {
        if (dir->SetToFirstEntry()) do
        {
            if ((dir->GetEntryType() == nDirectory::DIRECTORY) && dir->GetEntryName().MatchPattern(pattern))
            {
                dirList.Append(dir->GetEntryName());
            }
        }
        while (dir->SetToNextEntry());
        dir->Close();
    }
    n_delete(dir);
    return dirList;
}

//------------------------------------------------------------------------------
/**
    Returns the timestamp when the file was last written to. If the file
    doesn't exist, a default nFileTime object will be returned.
*/
nFileTime
nFileServer2::GetFileWriteTime(const nString& pathName)
{
    nFileTime fileTime;
    nFile* file = this->NewFileObject();
    if (file->Open(pathName, "r"))
    {
        fileTime = file->GetLastWriteTime();
        file->Close();
    }
    file->Release();
    return fileTime;
}
