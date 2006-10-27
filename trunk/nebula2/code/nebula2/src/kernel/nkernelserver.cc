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

nKernelServer *nKernelServer::Singleton = 0;

nNebulaUsePackage(nkernel);

//------------------------------------------------------------------------------
/**
    Loads a class and return a pointer to it.

    @param    className   name of the class to be opened
    @return               pointer to class object

     - 08-Oct-98 floh    created
     - 04-Oct-98 floh    char * -> const char *
     - 10-Aug-99 floh    + if class not loaded, now first looks into
                         package tocs before trying to load directly
                         from dll
     - 24-Oct-99 floh    returns zero if class could not be loaded
     - 29-Jul-02 floh    loading from dll's no longer supported, all classes
                         must now be part of a class package!
*/
nClass*
nKernelServer::OpenClass(const char* className)
{
    this->Lock();
    n_assert(className);
    nClass *cl = (nClass*) this->classList.Find(className);
    this->Unlock();
    return cl;
}

//------------------------------------------------------------------------------
/**
    Create a new unnamed Nebula object.

    @param  className   name of class
    @return             pointer to created object or 0


     - 08-Oct-98   floh    created
     - 04-Oct-98   floh    char * -> const char *
*/
nObject*
nKernelServer::NewUnnamedObject(const char* className)
{
    n_assert(className);

    this->Lock();
    nClass *cl = this->OpenClass(className);
    nObject *obj = 0;
    if (cl)
    {
        obj = cl->NewObject();
    }
    this->Unlock();
    return obj;
}

//------------------------------------------------------------------------------
/**
    Lookup an object by the given path and return a pointer to it.
    Check whether object described by path exists, returns
    pointer or NULL.

    @param  path    the path of the object to be found
    @return         pointer to object, or 0

     - 08-Oct-98   floh    created
     - 04-Oct-98   floh    char * -> const char *
     - 11-Dec-01   floh    bugfix: returned 'cwd' if "" string given,
                           now return 0
*/
nRoot*
nKernelServer::Lookup(const char* path)
{
    n_assert(path);
    this->Lock();
    nRoot* cur = 0;

    // check for empty string
    if (path[0])
    {
        char* nextPathComponent;
        char strBuf[N_MAXPATH];

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
    }

    this->Unlock();
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

     - 08-Oct-98   floh    created
     - 04-Oct-98   floh    char * -> const char *
     - 01-Dec-98   floh    if object exists, increase ref count
     - 24-Oct-99   floh    don't break on problems, instead return NULL
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
                child = (nRoot *) this->NewUnnamedObject("nroot");
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

     - 08-Oct-98   floh    created
     - 04-Nov-98   floh    + nFileServer Object
     - 07-Dec-98   floh    + nTimeServer Object
                           + nFileServer Object is now called
                           "/sys/servers/file"
     - 15-Jan-99   floh    + there is no nRoot::Init() anymore.
     - 22-Feb-99   floh    + char_buf initialising
     - 26-Feb-99   floh    + initialising MemHandler
     - 11-May-99   floh    + loads by default the Nebula-Package-DLL
     - 25-May-99   floh    + logmsg redirection
     - 26-May-99   floh    + num_objects, var_memnumalloc
     - 10-Aug-99   floh    + HashTable size for Classlist and toc_list
                           set to 128
                           + doesn't load the nclasses package overall
                           but reads in TOC-Files
     - 20-Jan-00   floh    + no SetScriptServer() anymore
*/
nKernelServer::nKernelServer() :
    varMemHighWaterSize(0),
    varMemTotalSize(0),
    varMemTotalCount(0),
    fileServer(0),
    persistServer(0),
    timeServer(0),
    remoteServer(0),
    hardRefServer(0),
    root(0),
    cwd(0),
    classList(64),
    defaultLogHandler(0),
    curLogHandler(0)
{
    // initialize static kernelserver pointer
    n_assert(0 == Singleton);
    Singleton = this;

    // initialize the debug memory system
#ifdef __WIN32__
    n_dbgmeminit();
#endif

    // set the default log handler
    #ifdef __XBxX__
        this->defaultLogHandler = n_new(nLogHandler);
    #else
        this->defaultLogHandler = n_new(nDefaultLogHandler);
    #endif
    this->SetLogHandler(this->defaultLogHandler);

    // initialize the kernel package classes
    this->AddPackage(nkernel);

    // create hard ref server
    this->hardRefServer = n_new(nHardRefServer);
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

    // create memory status variables
    this->varMemHighWaterSize = (nEnv*) this->New("nenv", "/sys/var/mem_highwatersize");
    this->varMemTotalSize     = (nEnv*) this->New("nenv", "/sys/var/mem_totalsize");
    this->varMemTotalCount    = (nEnv*) this->New("nenv", "/sys/var/mem_totalcount");
}

//------------------------------------------------------------------------------
/**
    nKernelServer destructor.

     - 08-Oct-98   floh    created
     - 07-Dec-98   floh    + TimeServer
     - 26-Feb-99   floh    + kills MemHandler
     - 25-May-99   floh    + Stdout redirection
     - 07-Feb-00   floh    + more detailed shutdown messages
     - 15-Sep-04   floh    + reset log handler at beginning of destructor
*/
nKernelServer::~nKernelServer()
{
    this->Lock();
    this->SetLogHandler(0);

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

    // kill object hierarchy
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
            if (actClass->GetRefCount() == 0)
            {
                numZeroRefs++;
                actClass->Remove();
                n_delete(actClass);
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
            n_printf("%s: refcount %d\n", actClass->GetName(), actClass->GetRefCount());
        }
        n_error("nKernelServer: Refcount errors occured during cleanup, check log for details!\n");
    }

    // kill the nHardRefServer
    n_delete(this->hardRefServer);
    this->hardRefServer = 0;

    // delete default log handler
    n_delete(this->defaultLogHandler);
    this->defaultLogHandler = 0;

    // reset static kernelserver pointer
    n_assert(0 != Singleton);
    Singleton = 0;

    this->Unlock();
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
    this->Lock();
    if (this->curLogHandler)
    {
        if (this->curLogHandler->IsOpen())
        {
            this->curLogHandler->Close();
        }
        this->curLogHandler = 0;
    }
    if (logHandler)
    {
        this->curLogHandler = logHandler;
        this->curLogHandler->Open();
    }
    else
    {
        this->curLogHandler = this->defaultLogHandler;
        this->curLogHandler->Open();
    }
    this->Unlock();
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
    va_list argList;
    va_start(argList, str);
    if (this->curLogHandler)
    {
        this->Lock();
        this->curLogHandler->Print(str, argList);
        this->Unlock();
    }
    else
    {
        vprintf(str, argList);
    }
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
    Show an important message which may require user interaction. The
    actual behavior of this method depends on the actual log handler
    class (under Win32, a message box could be shown to the user).
*/
void
nKernelServer::Message(const char* str, ...)
{
    va_list argList;
    va_start(argList, str);
    if (this->curLogHandler)
    {
        this->Lock();
        this->curLogHandler->Message(str, argList);
        this->Unlock();
    }
    else
    {
        vprintf(str, argList);
    }
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
    Show an error message which may require user interaction. The
    actual behavior of this method depends on the actual log handler
    class (under Win32, a message box could be shown to the user).
*/
void
nKernelServer::Error(const char* str, ...)
{
    va_list argList;
    va_start(argList, str);
    if (this->curLogHandler)
    {
        this->Lock();
        this->curLogHandler->Error(str, argList);
        this->Unlock();
    }
    else
    {
        vprintf(str, argList);
    }
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
    this->Lock();
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
    this->Unlock();
}

//------------------------------------------------------------------------------
/**
    Remove class object from kernel.

    @param  cl  Pointer to class to be removed

     - 17-May-00   floh    created
*/
void
nKernelServer::RemClass(nClass *cl)
{
    this->Lock();
    n_assert(cl);

    nClass *superClass = cl->GetSuperClass();
    n_assert(superClass);
    superClass->RemSubClass(cl);
    this->ReleaseClass(superClass);
    this->Unlock();
}

//------------------------------------------------------------------------------
/**
    Return pointer to class object defined by className. If the class
    is not loaded, 0 is returned

    @param  className   Name of the class
    @return             pointer to class object or 0

     - 08-Oct-98   floh    created
     - 04-Oct-98   floh    char * -> const char *
*/
nClass*
nKernelServer::FindClass(const char* className)
{
    n_assert(className);
    this->Lock();
    nClass *cl = (nClass *) this->classList.Find(className);
    this->Unlock();
    return cl;
}

//------------------------------------------------------------------------------
/**
    Create a class object by name, this increments the refcount of the class
    object.

     - 08-Oct-98   floh    created
     - 04-Oct-98   floh    char * -> const char *
*/
nClass*
nKernelServer::CreateClass(const char* className)
{
    n_assert(className);
    this->Lock();
    nClass *cl = this->OpenClass(className);
    if (cl)
    {
        cl->AddRef();
    }
    this->Unlock();
    return cl;
}

//------------------------------------------------------------------------------
/**
    Decrement the ref count of a given class. Currently, the class object
    will not be unloaded when it's refcount reaches 0.

    @param  cl  pointer to class object

     - 08-Oct-98   floh    created
*/
void
nKernelServer::ReleaseClass(nClass* cl)
{
    n_assert(cl);
    this->Lock();
    cl->Release();
    this->Unlock();
}

//------------------------------------------------------------------------------
/**
    Create a Nebula object given a class name and a path in the
    Nebula object hierarchy. This method will abort the Nebula app with
    a fatal error if the object couldn't be created.

    @param  className   Name of the object
    @param  path        Path where to create the new object in the hierarchy
    @return             pointer to class object

     - 08-Oct-98   floh    created
     - 04-Oct-98   floh    char * -> const char *
     - 15-Jul-99   floh    uses Link() on object
     - 29-Jul-99   floh    Link() killed
     - 24-Oct-99   floh    throws a fatal error if object could not
                           be created
     - 04-Oct-00   floh    + keep pointer to last created object
*/
nRoot*
nKernelServer::New(const char* className, const char* path)
{
    n_assert(className && path);
    this->Lock();
    nRoot *o = this->CheckCreatePath(className, path, true);
    n_assert(o);
    this->Unlock();
    return o;
}

//------------------------------------------------------------------------------
/**
    Create a Nebula object given a class name. This method will abort the
    Nebula app with a fatal error if the object couldn't be created.

    @param  className   Class name of the object
    @return             Pointer to new object
*/
nObject*
nKernelServer::New(const char* className)
{
    n_assert(className);
    this->Lock();
    nObject *obj = this->NewUnnamedObject(className);
    if (!obj)
    {
        n_error("nKernelServer: Couldn't create object of class '%s'.\n", className);
    }
    this->Unlock();
    return obj;
}

//------------------------------------------------------------------------------
/**
    Same as nKernelServer::New(), but doesn't fail if the object can't be
    created, instead, a 0 pointer is returned.

    @param className    Class name of the object
    @param path         Path where to create the new object in the hierarchy

     - 24-Oct-99   floh    created
     - 30-Jan-05   kims    removed assertion even when it failed to create a object.
                           null pointer should be done on the caller side.
*/
nRoot*
nKernelServer::NewNoFail(const char* className, const char *path)
{
    n_assert(className && path);
    this->Lock();
    nRoot *o = this->CheckCreatePath(className, path, false);
    if (!o)
    {
        n_printf("nKernelServer: Couldn't create object of class '%s'.\n", className);
    }
    this->Unlock();
    return o;
}

//------------------------------------------------------------------------------
/**
    Same as nKernelServer::New(), but doesn't fail if the object can't be
    created, instead, a null pointer is returned.

    @param className    Class name of the object
*/
nObject*
nKernelServer::NewNoFail(const char* className)
{
    n_assert(className);
    this->Lock();
    nObject *obj = this->NewUnnamedObject(className);
    if (!obj)
    {
        n_printf("nKernelServer: Couldn't create object of class '%s'.\n", className);
    }
    n_assert(obj);
    this->Unlock();
    return obj;
}

//------------------------------------------------------------------------------
/*
    Create a Nebula object from a persistent object file. The created
    object's name is derived from the path name (for nRoot-derived objects).

    @param  path    path of persistent object file in host filesystem
    @return         pointer to created object, or 0

     - 08-Oct-98   floh    created
     - 04-Oct-98   floh    char * -> const char *
     - 11-Nov-98   floh    implementiert
*/
nObject*
nKernelServer::Load(const char* path)
{
    n_assert(path);
    this->Lock();
    nObject* obj = this->persistServer->LoadObject(path, 0);
    this->Unlock();
    return obj;
}

//------------------------------------------------------------------------------
/*
    Create an nRoot object from a persistent object file with a given name.

    @param  path    path of persistent object file in host filesystem
    @param  name    object name
    @return         pointer to created object, or 0

    -08-Oct-98   floh    created
    -04-Oct-98   floh    char * -> const char *
    -11-Nov-98   floh    implementiert
*/
nRoot*
nKernelServer::LoadAs(const char* path, const char* name)
{
    n_assert(path && name);
    this->Lock();
    nObject* obj = this->persistServer->LoadObject(path, name);
    n_assert(obj->IsA("nroot"));
    this->Unlock();
    return (nRoot *)obj;
}

//------------------------------------------------------------------------------
/**
    Set the current working object.

    @param  o   pointer to new current working object

     - 08-Oct-98   floh    created
     - 13-May-99   floh    + if NULL pointer given, set root object
*/
void
nKernelServer::SetCwd(nRoot* o)
{
    this->Lock();
    if (o)
    {
        this->cwd = o;
    }
    else
    {
        this->cwd = this->root;
    }
    this->Unlock();
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

     - 28-Sep-00   floh    created
*/
void
nKernelServer::PushCwd(nRoot* o)
{
    this->Lock();
    this->cwdStack.Push(this->cwd);
    if (o)
    {
        this->cwd = o;
    }
    else
    {
        this->cwd = this->root;
    }
    this->Unlock();
}

//------------------------------------------------------------------------------
/**
    Pop working object from stack, and set as new working object.
    Return previous working object.

     - 28-Sep-00   floh    created
*/
nRoot*
nKernelServer::PopCwd()
{
    this->Lock();
    nRoot *prevCwd = this->cwd;
    this->cwd = this->cwdStack.Pop();
    this->Unlock();
    return prevCwd;
}

//------------------------------------------------------------------------------
/**
    Update mem status variables from mem manager.

     - 13-May-99   floh    created
*/
void nKernelServer::Trigger()
{
    // get memory statistics...
#ifdef __WIN32__
    nMemoryStats memStats = n_dbgmemgetstats();
    this->varMemHighWaterSize->SetI(memStats.highWaterSize);
    this->varMemTotalSize->SetI(memStats.totalSize);
    this->varMemTotalCount->SetI(memStats.totalCount);
#endif
}

//------------------------------------------------------------------------------
/**
    Add an extension class package to the kernel. The provided function
    should call nKernelServer::AddModule() for each class in the package.
*/
void
nKernelServer::AddPackage(void(*_func)())
{
    _func();
}

//------------------------------------------------------------------------------
/**
    Add a new class package module to the class list. Normally called
    from the n_init() function of a class package.
*/
void
nKernelServer::AddModule(const char *name,
                         bool (*_init_func)(nClass *, nKernelServer *),
                         void *(*_new_func)())
{
    this->Lock();
    nClass *cl = (nClass *) this->classList.Find(name);
    if (!cl)
    {
        cl = n_new(nClass(name, this, _init_func, _new_func));
        this->classList.AddTail(cl);
    }
    this->Unlock();
}

//------------------------------------------------------------------------------
/**
    Set an alternative file server. Release the previous file server.
*/
void
nKernelServer::ReplaceFileServer(const char* className)
{
    n_assert(className);

    this->Lock();
    if (this->fileServer)
    {
        this->fileServer->Release();
        this->fileServer = 0;
    }
    this->fileServer = (nFileServer2*) this->New(className, "/sys/servers/file2");
    n_assert(this->fileServer);
    this->Unlock();
}
