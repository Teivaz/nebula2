//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nsystem.h"
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/ndirectory.h"
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
    if (pathName[strlen(pathName)-1] != '/') 
    {
        n_error("nFileServer2::SetAssign: Path '%s' must end with a '/'\n", pathName);
        return false;
    }
        
    // ex. das Assign schon?
    kernelServer->PushCwd(this->assignDir.get());
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (!env) 
    {
        env = (nEnv *) kernelServer->New("nenv", assignName);
        n_assert(env);
    }
    env->SetS(pathName);
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
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Cleanup the path name inplace (replace any backslashes with slashes),
    removes a trailing slash if exists.
*/
void
nFileServer2::CleanupPathName(char* path)
{
    n_assert(path);

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
}

//------------------------------------------------------------------------------
/**
    Expands assign in path to full absolute path, replaces any backslashes
    by slashes, and returns any trainling slash, and makes the path absolute.

    Please note that Nebula does not know the concept of a current working
    directory, thus, all paths MUST be absolute (please note that Nebula
    assigns can be used to create position independent absolute paths).
      
    @param pathName        the path to expand
    @param buf             buffer for result
    @param bufSize         size of the buffer
    @return                result buffer

    history:
     - 30-Jan-2002   peter    created
*/
const char* 
nFileServer2::ManglePath(const char* pathName, char* buf, int bufSize)
{
    char pathBuf[N_MAXPATH];
    char *colon;
    n_strncpy2(pathBuf, pathName, N_MAXPATH);
    buf[0] = 0;

    // check for assigns
    while ((colon = strchr(pathBuf,':')))
    {
        *colon++ = 0;
        if (strlen(pathBuf) > 1)
        {
            const char *replace = this->GetAssign(pathBuf);
            if (replace)
            {
                n_strncpy2(buf, replace, bufSize);
                n_strcat(buf, colon, bufSize);
                n_strncpy2(pathBuf, buf, N_MAXPATH); //copy back for the next round
            }
        }
    }
    
    // no assigns, just do a copy.
    if (0 == buf[0])
    {
        n_strncpy2(buf, pathName, bufSize);
    }
    this->CleanupPathName(buf);
    return buf;
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
    return new nDirectory(this);
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
    return new nFile(this);
}

//------------------------------------------------------------------------------
/**
    Initialize Nebula's home directory assign ("home:").
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

            // "x\y\bin\win32\xxx.exe" -> "x\y\"
            int i;
            char *p;
            for (i=0; i<3; i++) 
            {
                p = strrchr(buf,'\\');
                n_assert(p);
                p[0] = 0;
            }
        // }

        if (strlen(buf) > 0)
        {
            // convert all backslashes to slashes
            char c, *p;
            p = buf;
            while ((c = *p)) 
            {
                if (c == '\\') *p = '/';
                p++;
            }
            // if last char is not a /, append one
            if (buf[strlen(buf)-1] != '/')
            {
                strcat(buf,"/");
            }
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
    #else
    #error nFileServer::initHomeAssign() not implemented!
    #endif
    
    // finally, set the assign
    this->SetAssign("home", buf);
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
    char buf[N_MAXPATH];

    const char *home_dir = this->GetAssign("home");
    n_assert(home_dir);
    n_strncpy2(buf,home_dir,sizeof(buf));

    #ifdef __WIN32__
        strcat(buf,"bin/win32/");
    #elif defined(__LINUX__)
        strcat(buf,"bin/linux/");
    #elif defined(__MACOSX__)
        strcat(buf, "bin/macosx/");
    #else
    #error nFileServer::initBinAssign() not implemented!
    #endif

    this->SetAssign("bin",buf);
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
    HRESULT hr = this->shell32Wrapper.SHGetFolderPath(0,        // hwndOver
                    CSIDL_PERSONAL | CSIDL_FLAG_CREATE,         // nFolder
                    NULL,                                       // hToken
                    0,                                          // dwFlags
                    rawPath);                                   // psxPath
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
    Make any missing directories in path.
*/
bool
nFileServer2::MakePath(const char* dirName)
{
    n_assert(dirName);
    
    nDirectory* dir = this->NewDirectoryObject();
    n_assert(dir);

    // mangle path name
    char mangledPath[N_MAXPATH];
    this->ManglePath(dirName, mangledPath, sizeof(mangledPath));

    // build stack of non-existing dir components
    nPathString path(mangledPath);
    path.ConvertBackslashes();
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
        char mangledFromPath[N_MAXPATH];
        char mangledToPath[N_MAXPATH];
        this->ManglePath(from, mangledFromPath, sizeof(mangledFromPath));
        this->ManglePath(to, mangledToPath, sizeof(mangledToPath));

        // if the target file exists, remove the read/only file attribute
        if (this->FileExists(mangledToPath))
        {
            DWORD fileAttrs = GetFileAttributes(mangledToPath);
            fileAttrs &= ~FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(mangledToPath, fileAttrs);
        }
        return ::CopyFile(mangledFromPath, mangledToPath, FALSE) ? true : false;
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
    char mangledPath[N_MAXPATH];
    this->ManglePath(filename, mangledPath, sizeof(mangledPath));

    #ifdef __WIN32__
        return ::DeleteFile(mangledPath) ? true : false;
    #elif defined(__LINUX__) || defined(__MACOSX__)
        return (0 == unlink(mangledPath)) ? true : false;
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

