#ifndef N_KERNELSERVER_H
#define N_KERNELSERVER_H
//------------------------------------------------------------------------------
/**
    @class nKernelServer

    @brief The central Nebula kernel server class.

    Every Nebula2 application needs exactly one kernel server object which
    persists throughout the lifetime of the application.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nclass.h"
#include "kernel/nroot.h"
#include "util/nstack.h"
#include "util/nhashlist.h"
#include "kernel/nmutex.h"
#include "kernel/natomtable.h"

#ifdef __WIN32__
#include "kernel/nwin32wrapper.h"
#endif

#ifdef __XBxX__
#include "xbox/nxbwrapper.h"
#endif

//------------------------------------------------------------------------------
class nTimeServer;
class nPersistServer;
class nHardRefServer;
class nFileServer2;
class nRemoteServer;
class nEnv;
class nLogHandler;

class nKernelServer
{
public:
    /// constructor
    nKernelServer();
    /// destructor
    ~nKernelServer();
    /// return pointer to single kernel server instance
    static nKernelServer* Instance();
    /// take the kernel lock (to make the kernel multithreading safe)
    void Lock();
    /// release the kernel lock
    void Unlock();

    /// add a class object to the kernel
    void AddClass(const char* superClassName, nClass* cl);
    /// close and remove a class from the kernel
    void RemClass(nClass* cl);
    /// create a class object
    nClass* CreateClass(const char* className);
    /// release a class object
    void ReleaseClass(nClass* cl);
    /// find a loaded class object by name
    nClass* FindClass(const char* className);
    /// return the list of classes
    const nHashList * GetClassList() const;

    /// create a Nebula object, fail hard if no object could be created
    nRoot* New(const char* className, const char* objectName);
    /// create a Nebula object, fail hard if no object could be created
    nObject* New(const char* className);
    /// create a Nebula object, don't fail when no object could be created
    nRoot* NewNoFail(const char* className, const char* objectName);
    /// create a Nebula object, don't fail when no object could be created
    nObject* NewNoFail(const char* className);
    /// create a Nebula object from a persistent object file
    nObject* Load(const char* path);
    /// create a Nebula object from a persistent object file with given name
    nRoot* LoadAs(const char* path, const char* objName);
    /// lookup a Nebula object in the hierarchy
    nRoot* Lookup(const char* path);

    /// set current working object
    void SetCwd(nRoot* newCwd);
    /// get current working object
    nRoot* GetCwd();
    /// push current working object on stack, and set new cwd
    void PushCwd(nRoot* newCwd);
    /// restore previous working object
    nRoot* PopCwd();

    /// install a log handler (see details for ownership)
    void SetLogHandler(nLogHandler* logHandler);
    /// get pointer to current log handler object
    nLogHandler* GetLogHandler() const;
    /// print a log message
    void __cdecl Print(const char* msg, ...);
    /// show an important user message which may require user ack
    void __cdecl Message(const char* msg, ...);
    /// show an error message which may require user ack
    void __cdecl Error(const char* msg, ...);

    /// set an alternative file server
    void ReplaceFileServer(const char* className);
    /// add an extension class package to the kernel
    void AddPackage(void (*)());
    /// add a code module to the kernel
    void AddModule(const char *, bool (*_init_func)(nClass *, nKernelServer *), void* (*_new_func)());
    /// get pointer to hard ref server
    nHardRefServer* GetHardRefServer() const;
    /// get pointer to file server
    nPersistServer* GetPersistServer() const;
    /// get pointer to file server
    nFileServer2* GetFileServer() const;
    /// get pointer to remote server
    nRemoteServer* GetRemoteServer() const;
    /// get pointer to time server
    nTimeServer* GetTimeServer() const;
    /// optionally call to update memory useage variables
    void Trigger();

private:
    static nKernelServer* Singleton;

    /// check if a Nebula path is absolute
    bool IsAbsolutePath(const char* path);
    /// create a new Nebula object, and all missing objects leading to it
    nRoot* CheckCreatePath(const char* className, const char* path, bool dieOnError);
    /// load a class and return pointer to it
    nClass* OpenClass(const char* className);
    /// create a new unnamed Nebula object
    nObject* NewUnnamedObject(const char* className);

    nFileServer2*   fileServer;     // private pointer to file server
    nPersistServer* persistServer;  // private pointer to persistency server
    nTimeServer*    timeServer;     // private pointer to timeserver
    nRemoteServer*  remoteServer;   // private pointer to remoteserver

    nHardRefServer* hardRefServer;  // private pointer to hardrefserver

    nHashList classList;            // list of nClass objects
    nRoot* root;                    // the root object of the Nebula object hierarchy
    nRoot* cwd;                     // the current working object
    nStack<nRoot*> cwdStack;        // stack of previous cwd's

    nLogHandler* defaultLogHandler; // the default log handler
    nLogHandler* curLogHandler;     // the current log handler

    nEnv* varMemHighWaterSize;              // mem statistics
    nEnv* varMemTotalSize;
    nEnv* varMemTotalCount;

    nMutex mutex;                   // the kernel lock mutex

    #ifdef __WIN32__
    nWin32Wrapper win32Wrapper;
    #endif

    // nAtomTable atomTable;           // the global atom table
};

//------------------------------------------------------------------------------
/**
*/
inline
nKernelServer*
nKernelServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nKernelServer::Lock()
{
    this->mutex.Lock();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nKernelServer::Unlock()
{
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nHashList*
nKernelServer::GetClassList() const
{
    return &(this->classList);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nKernelServer::IsAbsolutePath(const char* path)
{
    n_assert(path);
    return (path[0] == '/');
}

//------------------------------------------------------------------------------
/**
*/
inline
nHardRefServer*
nKernelServer::GetHardRefServer() const
{
    return this->hardRefServer;
}

//------------------------------------------------------------------------------
/**
*/
inline
nPersistServer*
nKernelServer::GetPersistServer() const
{
    return this->persistServer;
}

//------------------------------------------------------------------------------
/**
*/
inline
nFileServer2*
nKernelServer::GetFileServer() const
{
    return this->fileServer;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRemoteServer*
nKernelServer::GetRemoteServer() const
{
    return this->remoteServer;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTimeServer*
nKernelServer::GetTimeServer() const
{
    return this->timeServer;
}

//--------------------------------------------------------------------
#endif
