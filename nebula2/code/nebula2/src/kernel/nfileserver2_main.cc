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
#include "util/npathstring.h"
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

    this->assignDir = kernelServer->New("nroot", "/sys/share/assigns");
    this->InitHomeAssign();
    this->InitBinAssign();
    this->InitUserAssign();
}

//------------------------------------------------------------------------------
/**

    history:
     - 30-Jan-2002   peter    created
*/
nFileServer2::~nFileServer2()
{
    if (this->assignDir.isvalid())
    {
        this->assignDir->Release();
    }
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
nFileServer2::SetAssign(const char* assignName, const char* pathName)
{
    n_assert(assignName);
    n_assert(pathName);

    // make sure trailing slash exists
    nString pathString = pathName;
    pathString.StripTrailingSlash();
    pathString.Append("/");
        
    // ex. das Assign schon?
    kernelServer->PushCwd(this->assignDir.get());
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (!env) 
    {
        env = (nEnv *) kernelServer->New("nenv", assignName);
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
    @return                the path to which the assign links

    history:
     - 30-Jan-2002   peter    created
*/
const char*
nFileServer2::GetAssign(const char* assignName)
{
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (env) 
    {
        return env->GetS();
    }
    else 
    {
        return "<unknown_assign>";
    }
}

//------------------------------------------------------------------------------
/**
    Cleanup the path name inplace (replace any backslashes with slashes),
    removes a trailing slash if exists.
*/
void nFileServer2::CleanupPathName(nString& str)
{
    // FIXME: include PathString functionality in nString and
    // ditch nPathString completely???
    nPathString pathString(str.Get());
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
    by slashes, and returns any trainling slash, and makes the path absolute.

    Please note that Nebula does not know the concept of a current working
    directory, thus, all paths MUST be absolute (please note that Nebula
    assigns can be used to create position independent absolute paths).
      
    @param pathName        the path to expand
    @return             resulting string

    history:
     - 30-Jan-2002   peter    created
*/
nString
nFileServer2::ManglePath(const char* pathName)
{
    nString pathString = pathName;

    // check for assigns
    int colonIndex;
    while ((colonIndex = pathString.FindChar(':', 0)) > 0)
    {
        // special case: ignore one-caracter "assigns" becayse they are 
        // really DOS drive letters
        if (colonIndex > 1)
        {
            nString assignString = pathString.ExtractRange(0, colonIndex);
            nString postAssignString = pathString.ExtractRange(colonIndex + 1, pathString.Length() - (colonIndex + 1));
            nString replace = this->GetAssign(assignString.Get());
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

    @return          the nDirectory object

    history:
     - 30-Jan-2002   peter    created
*/
nDirectory* 
nFileServer2::NewDirectoryObject()
{
    return new nDirectory;
}

//------------------------------------------------------------------------------
/**
    creates a new nFile object
  
    @return          the nFile object

    history:
     - 30-Jan-2002   peter    created
*/
nFile*
nFileServer2::NewFileObject()
{
    return new nFile;
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
    char buf[N_MAXPATH];
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
        DWORD res = GetModuleFileName(NULL, buf, sizeof(buf));
        if (res == 0) 
        {
            n_error("nFileServer2::InitHomeAssign(): GetModuleFileName() failed!\n");
        }

        nPathString pathToExe(buf);
        pathToExe.ConvertBackslashes();

        // check if executable resides in a win32 directory
        nPathString pathToDir = pathToExe.ExtractLastDirName();
        // converted to lowercase because sometimes the path is in uppercase
        pathToDir.ToLower();
        if (pathToDir == "win32" || pathToDir == "win32d")
        {
            // normal home:bin/win32 directory structure
            // strip bin/win32
            nPathString homePath = pathToExe.ExtractDirName();
            homePath.StripTrailingSlash();
            homePath = homePath.ExtractDirName();
            homePath.StripTrailingSlash();
            homePath = homePath.ExtractDirName();
            this->SetAssign("home", homePath.Get());
            n_printf("home: %s", homePath.Get());
        }
        else
        {
            // not in normal home:bin/win32 directory structure, 
            // use the exe's directory as home path
            nPathString homePath = pathToExe.ExtractDirName();
            this->SetAssign("home", homePath.Get());
            n_printf("home: %s", homePath.Get());
        }
    #elif defined(__LINUX__)
        // under Linux, the NEBULADIR environment variable must be set,
        // otherwise the current working directory will be used
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
        // finally, set the assign
        this->SetAssign("home", buf);

    #elif defined(__MACOSX__)
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
        // finally, set the assign
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
        nPathString pathToExe = buf;
        pathToExe.ConvertBackslashes();
        nPathString binPath = pathToExe.ExtractDirName();
        this->SetAssign("bin", binPath.Get());

    #elif defined(__LINUX__)

        char buf[N_MAXPATH];
    const char *home_dir = this->GetAssign("home");
    n_assert(home_dir);
    n_strncpy2(buf,home_dir,sizeof(buf));
        strcat(buf,"bin/linux/");
        this->SetAssign("bin",buf);

    #elif defined(__MACOSX__)

        char buf[N_MAXPATH];
        const char *home_dir = this->GetAssign("home");
        n_assert(home_dir);
        n_strncpy2(buf,home_dir,sizeof(buf));
        strcat(buf, "bin/macosx/");
        this->SetAssign("bin",buf);

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
#elif __WIN32__
    char rawPath[MAX_PATH];
    HRESULT hr = this->shell32Wrapper.SHGetFolderPath(0,      // hwndOwner
                    CSIDL_PERSONAL | CSIDL_FLAG_CREATE,         // nFolder
                    NULL,                                       // hToken
                    0,                                          // dwFlags
                    rawPath);                                  // pszPath
    n_assert(S_OK == hr);

    nPathString path(rawPath);
    path.ConvertBackslashes();
    path.Append("/");
    this->SetAssign("user", path.Get());
#else
#error "IMPLEMENT ME!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileServer2::FileExists(const char* pathName)
{
    n_assert(pathName);
    nFile* file = this->NewFileObject();
    if (file->Open(pathName, "r"))
    {
        file->Close();
        file->Release();
        return true;
    }
    file->Release();
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileServer2::DirectoryExists(const char* pathName)
{
    n_assert(pathName);
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(pathName))
    {
        dir->Close();
        delete dir;
        return true;
    }
    delete dir;
    return false;
}

//------------------------------------------------------------------------------
/**
    Make any missing directories in path.
*/
bool
nFileServer2::MakePath(const char* dirName)
{
    n_assert(dirName);
    
    nDirectory* dir = this->NewDirectoryObject();
    n_assert(dir);

    // build stack of non-existing dir components
    nPathString path = this->ManglePath(dirName).Get();
    nArray<nPathString> pathStack;
    while ((!path.IsEmpty()) && (!dir->Open(path.Get())))
    {
        pathStack.Append(path);
        path = path.ExtractDirName();
    }
    if (dir->IsOpen())
    {
        dir->Close();
    }
    delete dir;

    // error?
    if (path.IsEmpty())
    {
        return false;
    }

    // create missing directory components
    int i;
    for (i = pathStack.Size() - 1; i >= 0; --i)
    {
        const nPathString& curPath = pathStack[i];
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
*/
bool
nFileServer2::CopyFile(const char* from, const char* to)
{
    n_assert(from && to);

    #ifdef __WIN32__
        // Win32 specific method is more efficient
        nString mangledFromPath = this->ManglePath(from);
        nString mangledToPath   = this->ManglePath(to);

        // if the target file exists, remove the read/only file attribute
        if (this->FileExists(mangledToPath.Get()))
        {
            DWORD fileAttrs = GetFileAttributes(mangledToPath.Get());
            fileAttrs &= ~FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(mangledToPath.Get(), fileAttrs);
        }
        return ::CopyFile(mangledFromPath.Get(), mangledToPath.Get(), FALSE) ? true : false;
    #else
        nFile* fromFile = this->NewFileObject();
        if (!fromFile->Open(from, "rb"))
        {
            n_printf("nFileServer2::Copy(): could not open source file '%s'\n", from);
            fromFile->Release();
            return false;
        }
        nFile* toFile = this->NewFileObject();
        if (!toFile->Open(to, "wb"))
        {
            n_printf("nFileServer2::Copy(): could not open dest file '%s'\n", to);
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
nFileServer2::DeleteFile(const char* filename)
{
    n_assert(filename);
    nString mangledPath = this->ManglePath(filename);

    #ifdef __WIN32__
        return ::DeleteFile(mangledPath.Get()) ? true : false;
    #elif defined(__LINUX__)
        return (0 == unlink(mangledPath.Get())) ? true : false;
    #else
    #error "nFileServer2::DeleteFile() not implemented yet!"
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
nFileServer2::CreateFileNode(const char* name)
{
    n_assert(name);

    nString path = "/sys/share/files/";
    path += name;
    if (kernelServer->Lookup(path.Get()))
    {
        n_error("nFileServer2: file node '%s' exists!", name);
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
nFileServer2::Checksum(const char* filename, uint& crc)
{
    n_assert(filename);
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
    
