#define N_IMPLEMENTS nKernelServer
#define N_KERNEL
//------------------------------------------------------------------------------
//  nkernelserver.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/npersistserver.h"
#include "kernel/ntimeserver.h"
#include "kernel/nhardrefserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nremoteserver.h"
#include "kernel/ndefaultloghandler.h"

nKernelServer *nKernelServer::ks = 0;

// from nmemory.cc
#ifdef __NEBULA_MEM_MANAGER__
extern int n_memallocated;
extern int n_memused;
extern int n_memnumalloc;
#endif

// on static-linking platforms, a global n_addmodules() function must exist,
// which adds all class modules to the kernel.
#ifdef N_STATIC
extern "C" void n_addmodules(nKernelServer*);
#endif

//------------------------------------------------------------------------------
/**
    Loads a class and return a pointer to it.
    First it is checked if the class is already loaded, if so the pointer to 
    this class is returned, otherwise first a loading from package is tried
    than loading from the dll itself.

    @param    className   name of the class to be opened
    @return               pointer to class object

    08-Oct-98 floh    created
    04-Oct-98 floh    char * -> const char *
    10-Aug-99 floh    + if class not loaded, now first looks into
                      package tocs before trying to load directly
                      from dll
    24-Oct-99 floh    returns zero if class could not be loaded
    29-Jul-02 floh    loading from dll's no longer supported, all classes
                      must now be part of a class package!                      
*/
nClass*
nKernelServer::OpenClass(const char* className)
{
    n_assert(className);

    // class already loaded?
    nClass *cl = (nClass*) this->classList.Find(className);

    // handle systems which don't support dynamic class loading
#ifndef N_STATIC
    if (cl) 
    {
        return cl;
    }
    
    // try loading from package (only on systems which support
    // dynamic loading)
    nPckgTocEntry *pte = (nPckgTocEntry*) this->tocList.Find(className);
    if (pte) 
    {
        if (this->LoadPackage(pte->GetPackageName())) 
        {
            cl = (nClass*) this->classList.Find(className);
            if (cl) 
            {
                return cl;
            }
        }
    }
#endif

    return cl;
}

//------------------------------------------------------------------------------
/**
    Create a new unnamed Nebula object.
    
    @param  className   name of class
    @return             pointer to created object or 0


    08-Oct-98   floh    created
    04-Oct-98   floh    char * -> const char *
*/
nRoot*
nKernelServer::NewUnnamedObject(const char* className)
{
    n_assert(className);

    nClass *cl = this->OpenClass(className);
    if (cl)
    {
        return cl->NewObject();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Lookup an object by the given path and return a pointer to it.
    Check whether object described by path exists, returns
    pointer or NULL.

    @param  path    the path of the object to be found
    @return         pointer to object, or 0

    08-Oct-98   floh    created
    04-Oct-98   floh    char * -> const char *
    11-Dec-01   floh    bugfix: returned 'cwd' if "" string given,
                        now return 0
*/
nRoot*
nKernelServer::Lookup(const char* path)
{
    n_assert(path);

    nRoot* cur;
    char* nextPathComponent;
    char strBuf[N_MAXPATH];

    // check for empty string
    if (0 == path[0])
    {
        return 0;
    }

    // copy path to scratch buffer
    char *str = strBuf;
    n_strncpy2(strBuf, path, sizeof(strBuf));
    
    if (this->IsAbsolutePath(str)) 
    {
        cur = this->root;
    }
    else
    {
        cur = this->cwd;
    }

    while ((nextPathComponent = strtok(str, "/")) && cur) 
    {
        if (str) 
        {
            str = NULL;
        }
        cur = cur->Find(nextPathComponent);
    }
    return (nRoot *) cur;
}                    

//------------------------------------------------------------------------------
/**
    Create a new object and create all missing objects in the hierarchy.
    Create object described by path, fill up missing path components
    with nRoot objects.

    @param  className   The name of the object
    @param  path        Path where the object should be created
    @param  dieOnError  Flag on true creates a n_error message "Aborting\n", on false doesn't

    08-Oct-98   floh    created
    04-Oct-98   floh    char * -> const char *
    01-Dec-98   floh    if object exists, increase ref count
    24-Oct-99   floh    don't break on problems, instead return NULL
*/
nRoot*
nKernelServer::CheckCreatePath(const char* className, const char *path, bool dieOnError)
{
    n_assert(className);
    n_assert(path);

    nRoot* parent = 0;
    nRoot* child  = 0;
    char* curPathComponent;
    char* nextPathComponent;
    char strBuf[N_MAXPATH];

    // copy path to scratch buffer
    n_strncpy2(strBuf, path, sizeof(strBuf)); 

    // get parent of new object
    if (this->IsAbsolutePath(strBuf)) 
    {
        parent = this->root;
    }
    else
    {
        parent = this->cwd;
    }

    curPathComponent = strtok(strBuf, "/");
    if (curPathComponent) 
    {
        // for each directory path component
        while ((nextPathComponent = strtok(NULL, "/"))) 
        {
            child = parent->Find(curPathComponent);
            if (!child) 
            {
                // subdir doesn't exist, fill up
                child = this->NewUnnamedObject("nroot");
                if (child) 
                {
                    child->SetName(curPathComponent);
                    parent->AddTail(child);
                    child->Initialize();
                } 
                else 
                {
                    if (dieOnError) n_error("nKernelServer: Couldn't create object '%s' in path '%s'.\n", curPathComponent, path);
                    else            n_printf("nKernelServer: Couldn't create object '%s' in path '%s'.\n", curPathComponent, path);
                    return 0;
                }
            }
            parent = child;
            curPathComponent = nextPathComponent;
        }

        // curPathComponent is now name of last path component
        child = parent->Find(curPathComponent);
        if (!child) 
        {
            // create and link object
            child = (nRoot*) this->NewUnnamedObject(className);
            if (child) 
            {
                child->SetName(curPathComponent);
                parent->AddTail(child);
                child->Initialize();
            } 
            else 
            {
                if (dieOnError) n_error("nKernelServer: Couldn't create object '%s' of class '%s'.\n", path, className);
                else            n_printf("nKernelServer: Couldn't create object '%s' of class '%s'.\n", path, className);
                return 0;
            }
        }
    } 
    else 
    {
        if (dieOnError) n_error("nKernelServer: Empty name for new object of class '%s'!\n", className);
        else            n_printf("nKernelServer: Empty name for new object of class '%s'!\n", className);
        return 0;
    }
    return child;
}

//------------------------------------------------------------------------------
/**
    nKernelServer constructor.

    08-Oct-98   floh    created
    04-Nov-98   floh    + nFileServer Object
    07-Dec-98   floh    + nTimeServer Object
                        + nFileServer Object is now called 
                        "/sys/servers/file"
    15-Jan-99   floh    + there is no nRoot::Init() anymore.
    22-Feb-99   floh    + char_buf initialising
    26-Feb-99   floh    + initialising MemHandler
    11-May-99   floh    + loads by default the Nebula-Package-DLL
    25-May-99   floh    + logmsg redirection
    26-May-99   floh    + num_objects, var_memnumalloc
    10-Aug-99   floh    + HashTable size for Classlist and toc_list 
                        set to 128
                        + doesn't load the nclasses package overall 
                        but reads in TOC-Files
    20-Jan-00   floh    + no SetScriptServer() anymore
*/
nKernelServer::nKernelServer() :
    #ifndef N_STATIC
    tocList(16),
    #endif
    classList(64),
    fileServer(0),
    persistServer(0),
    remoteServer(0),
    timeServer(0)
{
    // initialize static kernelserver pointer
    ks = this;

    // set the default log handler
    #ifdef __XBxX__
        this->defaultLogHandler = new nLogHandler;
    #else
        this->defaultLogHandler = new nDefaultLogHandler;
    #endif
    this->SetLogHandler(this->defaultLogHandler);

    #ifdef N_STATIC
        // initialize class modules on static link systems
        n_addmodules(this);
    #else
        // on dynamic systems read toc files
        this->InitBinPath();
        this->ReadToc();
    #endif

    // create hard ref server
    this->hardRefServer = n_new nHardRefServer;
    n_assert(this->hardRefServer);

    // create root object
    this->root = (nRoot *) this->NewUnnamedObject("nroot");
    n_assert(this->root);
    this->root->SetName("/");
    this->cwd = this->root;

    // create necessary servers 
    this->fileServer    = (nFileServer2*)   this->New("nfileserver2",   "/sys/servers/file2");
    this->persistServer = (nPersistServer*) this->New("npersistserver", "/sys/servers/persist");
    this->remoteServer  = (nRemoteServer*)  this->New("nremoteserver",  "/sys/servers/remote");
    this->timeServer    = (nTimeServer*)    this->New("ntimeserver",    "/sys/servers/time");

    #ifdef __NEBULA_MEM_MANAGER__
        // create status variables
        this->varMemAlloc = (nEnv *) this->New("nenv","/sys/var/mem_alloc");
        this->varMemUsed  = (nEnv *) this->New("nenv","/sys/var/mem_used");
        this->varMemNumAlloc = (nEnv *) this->New("nenv","/sys/var/mem_num");
    #endif
}

//------------------------------------------------------------------------------
/**
    nKernelServer destructor.

    08-Oct-98   floh    created
    07-Dec-98   floh    + TimeServer
    26-Feb-99   floh    + kills MemHandler
    25-May-99   floh    + Stdout redirection
    07-Feb-00   floh    + more detailed shutdown messages
*/
nKernelServer::~nKernelServer(void)
{
    // kill time and file server
    if (this->timeServer) 
    {
        this->timeServer->Release();
        this->timeServer = 0;
    }
    if (this->persistServer) 
    {
        this->persistServer->Release();
        this->persistServer = 0;
    }
    if (this->fileServer)
    {
        this->fileServer->Release();
        this->fileServer = 0;
    }
    if (this->remoteServer)
    {
        this->remoteServer->Release();
        this->remoteServer = 0;
    }

    // kill object hierarchie
    this->root->Release();

    // kill class list
    // ===============
    // Class objects must be released in inheritance order,
    // subclasses first, then parent class. Do multiple
    // runs over the class list and release classes
    // with a ref count of 0, until list is empty or
    // no classes with a ref count of 0 zero exists
    // (that's a fatal error, there are still objects
    // around of that class).
    bool isEmpty;
    long numZeroRefs = 1;
    while ((!(isEmpty = this->classList.IsEmpty())) && (numZeroRefs > 0)) 
    {
        numZeroRefs = 0;
        nClass *actClass = (nClass*) this->classList.GetHead();
        nClass *nextClass;
        do 
        {
            nextClass = (nClass*) actClass->GetSucc();
            if (actClass->GetRef() == 0) 
            {
                numZeroRefs++;
                actClass->Remove();
                n_delete actClass;
            }
            actClass = nextClass;
        } while (actClass);
    }
    if (!isEmpty) 
    {
        n_printf("~nKernelServer(): ref_count error cleaning up class list!\n");
        n_printf("Offending classes:\n");
        nClass* actClass;
        for (actClass = (nClass*) this->classList.GetHead();
             actClass;
             actClass = (nClass*) actClass->GetSucc())
        {
            n_printf("%s: refcount %d\n", actClass->GetName(), actClass->GetRef());
        }
        n_error("nKernelServer: Refcount errors occured during cleanup, check log for details!\n");
    }

    // kill the nHardRefServer
    n_delete this->hardRefServer;
    this->hardRefServer = 0;
    
    // kill package toc list (only on dynamic link systems)
    #ifndef N_STATIC
        nPckgTocEntry *pe;
        while ((pe = (nPckgTocEntry *) this->tocList.RemHead())) 
        {
            n_delete pe;
        }
    #endif
    
    // delete default log handler
    delete this->defaultLogHandler;
    this->defaultLogHandler = 0;
    this->curLogHandler = 0;

    // reset static kernelserver pointer
    ks = 0;
}

//------------------------------------------------------------------------------
/**
    Set new log handler. NOTE: the nKernelServer does not gain ownership of
    the log handler, it just uses it. Whoever sets a log handler is responsible
    to cleanly release it, and must make sure that the kernel server is
    not left with an invalid pointer by set a zero log handler.

    @param  logHandler  pointer to a new nLogHandler object, or 0 to 
                        restore the default log handler
*/
void
nKernelServer::SetLogHandler(nLogHandler* logHandler)
{
    if (logHandler)
    {
        this->curLogHandler = logHandler;
    }
    else
    {
        this->curLogHandler = this->defaultLogHandler;
    }
}

//------------------------------------------------------------------------------
/**
    Get pointer to current log handler object. This method can be used
    to obtain a pointer to the log line buffer object of the log handler.
    Please note that support for a line buffer is optional!
*/
nLogHandler*
nKernelServer::GetLogHandler() const
{
    return this->curLogHandler;
}

//------------------------------------------------------------------------------
/**
    Print a new text message to the log. This always works like a normal
    printf, it will never halt the program for user interaction.
*/
void
nKernelServer::Print(const char* str, ...)
{
    n_assert(this->curLogHandler);
    va_list argList;
    va_start(argList, str);
    this->curLogHandler->Print(str, argList);
    va_end(argList);    
}

//------------------------------------------------------------------------------
/**
    Show an important message which may require user interaction. The
    actual behaviour of this method depends on the actual log handler
    class (under Win32, a message box could be shown to the user).
*/
void
nKernelServer::Message(const char* str, ...)
{
    n_assert(this->curLogHandler);
    va_list argList;
    va_start(argList, str);
    this->curLogHandler->Message(str, argList);
    va_end(argList);    
}

//------------------------------------------------------------------------------
/**
    Show an error message which may require user interaction. The
    actual behaviour of this method depends on the actual log handler
    class (under Win32, a message box could be shown to the user).
*/
void
nKernelServer::Error(const char* str, ...)
{
    n_assert(this->curLogHandler);
    va_list argList;
    va_start(argList, str);
    this->curLogHandler->Error(str, argList);
    va_end(argList);    
}

//------------------------------------------------------------------------------
/**
    Add a class object to the kernel.

    @param  superClassName   name of super class
    @param  cl               class object to add

  - 17-May-00   floh    created
*/
void 
nKernelServer::AddClass(const char *superClassName, nClass *cl)
{
    n_assert(superClassName);
    n_assert(cl);

    nClass *superClass = this->OpenClass(superClassName);
    if (superClass) 
    {
        superClass->AddSubClass(cl);
    }
    else 
    {
        n_error("nKernelServer::AddClass(): Could not open super class '%s'\n", superClassName);
    }
}

//------------------------------------------------------------------------------
/**
    Remove class object from kernel.

    @param  cl  Pointer to class to be removed

    17-May-00   floh    created
*/
void 
nKernelServer::RemClass(nClass *cl)
{
    n_assert(cl);

    nClass *superClass = cl->GetSuperClass();
    n_assert(superClass);
    superClass->RemSubClass(cl);
    this->ReleaseClass(superClass);
}

//------------------------------------------------------------------------------
/**
    Return pointer to class object defined by className. If the class
    is not loaded, 0 is returned

    @param  className   Name of the class
    @return             pointer to class object or 0

    08-Oct-98   floh    created
    04-Oct-98   floh    char * -> const char *
*/
nClass*
nKernelServer::FindClass(const char* className)
{
    n_assert(className);
    nClass *cl = (nClass *) this->classList.Find(className);
    return cl;
}

//------------------------------------------------------------------------------
/**
    Create a class object by name, this increments the refcount of the class 
    object.

    08-Oct-98   floh    created
    04-Oct-98   floh    char * -> const char *
*/
nClass*
nKernelServer::CreateClass(const char* className)
{
    n_assert(className);
    nClass *cl = this->OpenClass(className);
    if (cl) 
    {
        cl->AddRef();
    }
    return cl;
}

//------------------------------------------------------------------------------
/**
    Decrement the ref count of a given class. Currently, the class object
    will not be unloaded when it's refcount reaches 0.

    @param  cl  pointer to class object

    08-Oct-98   floh    created
*/
void 
nKernelServer::ReleaseClass(nClass* cl)
{
    n_assert(cl);
    cl->RemRef();
}

//------------------------------------------------------------------------------
/**
    Create a Nebula object given a class name and a path in the 
    Nebula object hierarchie. This method will abort the Nebula app with
    a fatal error if the object couldn't be created.

    @param  className   Name of the object
    @param  path        Path where to create the new object in the hierarchy
    @return             pointer to class object

    08-Oct-98   floh    created
    04-Oct-98   floh    char * -> const char *
    15-Jul-99   floh    uses Link() on object
    29-Jul-99   floh    Link() killed
    24-Oct-99   floh    throws a fatal error if object could not 
                        be created
    04-Oct-00   floh    + keep pointer to last created object
*/
nRoot*
nKernelServer::New(const char* className, const char* path)
{
    n_assert(className && path);
    nRoot *o = this->CheckCreatePath(className, path, true);
    n_assert(o);
    return o;
}

//------------------------------------------------------------------------------
/**
    Same as nKernelServer::New(), but doesn't fail if the object can't be
    created, instead, a 0 pointer is returned.

    @param className    Name of the object
    @param path         Path where to create the new object in the hierarchy

    24-Oct-99   floh    created
*/
nRoot*
nKernelServer::NewNoFail(const char* className, const char *path)
{
    n_assert(className && path);
    nRoot *o = this->CheckCreatePath(className, path, false);
    n_assert(o);
    return o;
}

//------------------------------------------------------------------------------
/*
    Create a Nebula object from a persistent object file.

    @param  path    path of persistent object file in host filesystem
    @return         pointer to created object, or 0

    08-Oct-98   floh    created
    04-Oct-98   floh    char * -> const char *
    11-Nov-98   floh    implementiert
*/
nRoot*
nKernelServer::Load(const char* path)
{
    n_assert(path);
    return this->persistServer->LoadObject(path);
}

//------------------------------------------------------------------------------
/**
    Set the current working object.

    @param  o   pointer to new current working object

    08-Oct-98   floh    created
    13-May-99   floh    + if NULL pointer given, set root object
*/
void
nKernelServer::SetCwd(nRoot* o)
{
    if (o) 
    {
        this->cwd = o;
    }
    else   
    {
        this->cwd = this->root;
    }
}

//------------------------------------------------------------------------------
/**
    Get the current working object.

    @return     pointer to current working object

  - 08-Oct-98   floh    created
*/
nRoot*
nKernelServer::GetCwd()
{
    return this->cwd;
}    

//------------------------------------------------------------------------------
/**
    Push current working object on a stack, and set new current working object.

    @param  o   pointer to new current working object

    28-Sep-00   floh    created
*/
void 
nKernelServer::PushCwd(nRoot* o)
{
    n_assert(o);
    this->cwdStack.Push(this->cwd);
    this->cwd = o;
}

//------------------------------------------------------------------------------
/**
    Pop working object from stack, and set as new working object.
    Return previous working object.

    28-Sep-00   floh    created
*/
nRoot*
nKernelServer::PopCwd()
{
    nRoot *prevCwd = this->cwd;
    this->cwd = this->cwdStack.Pop();
    return prevCwd;
}

//------------------------------------------------------------------------------
/**
    Update mem status variables from mem manager.

    13-May-99   floh    created
*/
void nKernelServer::Trigger(void)
{
    #ifdef __NEBULA_MEM_MANAGER__
        this->varMemAlloc->SetI(n_memallocated);
        this->varMemUsed->SetI(n_memused);
        this->varMemNumAlloc->SetI(n_memnumalloc);
    #endif
}

//------------------------------------------------------------------------------
/**
    Load a class package dll. This method is only supported on system
    with dynamic linking.
*/
#ifndef N_STATIC
bool
nKernelServer::LoadPackage(const char*name)
{
    void *image = n_dllopen(name);
    bool retval = false;
    if (image) 
    {
        bool (*addmodules_func)(nKernelServer *) = (bool (*)(nKernelServer *)) n_dllsymbol(image,"n_addmodules");
        if (addmodules_func) 
        {
            addmodules_func(ks);
            return true;
        }
    } 
    else 
    {
        n_printf("nKernelServer: could not load package '%s'!\n",name);
    }
    return retval;
}
#endif

//------------------------------------------------------------------------------
/**
    Read the global classes.toc file, which contains the class-to-dll
    mappings.

    Format:
    $dllname
    %classname
    %classname
    $dllname
    %classname
    %classname
*/  
#ifndef N_STATIC
bool 
nKernelServer::ReadToc()
{
    nString fileName(this->binDir);
    fileName += "classes.toc";
    FILE *fp = fopen(fileName.Get(), "r");
    if (fp) 
    {
        char line[256];
        char dllName[N_MAXNAMELEN] = { 0 };
        while (fgets(line, sizeof(line), fp)) 
        {
            // clip illegal chars
            char *nl = strpbrk(line, "# \t\n");
            if (nl) 
            {
                *nl = 0;
            }

            // dllname?
            if (line[0] == '$') 
            {
                n_strncpy2(dllName, &(line[1]), sizeof(dllName));
            } 
            else if (line[0] == '%') 
            {
                n_assert(dllName[0]);                    
                nPckgTocEntry *toc = n_new nPckgTocEntry(&(line[1]), dllName);
                this->tocList.AddTail(toc);
            }
        }
        fclose(fp);
        return true;
    }
    else
    {
        n_printf("Could not open file Nebula toc file!\n");
        return false;
    }
}
#endif

//------------------------------------------------------------------------------
/**
    Add a new class package module to the class list. Normally called
    from the n_init() function of a class package.
*/
void 
nKernelServer::AddModule(const char *name,
                         bool (*_init_func)(nClass *, nKernelServer *),
                         void (*_fini_func)(void),
                         void *(*_new_func)(void),
                         char *(*_version_func)(void))
{
    nClass *cl = (nClass *) this->classList.Find(name);
    if (!cl) 
    {
        cl = n_new nClass(name, this, _init_func, _fini_func, _new_func);
        this->classList.AddTail(cl);
    }
}

//------------------------------------------------------------------------------
/**
    Set an alternative file server. Release the previous file server.
*/
void
nKernelServer::ReplaceFileServer(const char* className)
{
    n_assert(className);

    if (this->fileServer)
    {
        this->fileServer->Release();
        this->fileServer = 0;
    }

    this->fileServer = (nFileServer2*) this->New(className, "/sys/servers/file2");
    n_assert(this->fileServer);
}

//------------------------------------------------------------------------------
/**
*/
#ifndef N_STATIC
void
nKernelServer::InitBinPath()
{
    char buf[N_MAXPATH];

    #if __WIN32__
    
        // Win32: try to find the nkernel.dll module handle's filename
        // and cut off the last 2 directories
        HMODULE hmod = GetModuleHandle("nkernel.dll");
        DWORD res = GetModuleFileName(hmod,buf,sizeof(buf));
        if (res == 0) 
        {
            n_printf("nKernelServer::InitBinDir(): GetModuleFileName() failed!\n");
        }

        char c, *p = strrchr(buf, '\\');
        p[1] = 0;
        // convert all backslashes to slashes
        p = buf;
        while ((c = *p)) 
        {
            if (c == '\\') *p = '/';
            p++;
        }
    
    #else
    #error nKernelServer::InitBinDir() not implemented!
    #endif

    this->binDir = buf;
}
#endif
