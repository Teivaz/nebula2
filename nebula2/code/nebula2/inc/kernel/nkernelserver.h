#ifndef N_KERNELSERVER_H
#define N_KERNELSERVER_H
//------------------------------------------------------------------------------
/**
    @class nKernelServer

    The Nebula kernel server class.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_CLASS_H
#include "kernel/nclass.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_STACK_H
#include "util/nstack.h"
#endif

#ifndef N_HASHLIST_H
#include "util/nhashlist.h"
#endif

#ifdef __XBxX__
#include "xbox/nxbwrapper.h"
#endif

#undef N_DEFINES
#define N_DEFINES nKernelServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nTimeServer;
class nPersistServer;
class nHardRefServer;
class nFileServer2;
class nRemoteServer;
class nEnv;
class nLogHandler;

class N_PUBLIC nKernelServer 
{
public:
    /// constructor
    nKernelServer();
    /// destructor
    ~nKernelServer();

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

    /// create a Nebula object, fail hard if no object could be created
    nRoot* New(const char* className, const char* objectName);
    /// create a Nebula object, don't fail when no object could be created
    nRoot* NewNoFail(const char* className, const char* objectName);
    /// create a Nebula object from a persistent object file
    nRoot* Load(const char* path);
    /// lookup a Nebula object in the hierarchie
    nRoot* Lookup(const char* path);

    /// set current working object
    void SetCwd(nRoot* newCwd);
    /// get current working object
    nRoot* GetCwd();
    /// push current working object ob stack, and set new cwd
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
    /// add a code module to the kernel
    void AddModule(const char *, bool (*)(nClass *, nKernelServer *), void (*)(void), void *(*)(void), char *(*)(void));
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
    void Trigger(void);

    /// static pointer to nKernelServer object
    static nKernelServer* ks;

private:
    /// check if a Nebula path is absolute
    bool IsAbsolutePath(const char* path);
    /// create a new Nebula object, and all missing objects leading to it
    nRoot* CheckCreatePath(const char* className, const char* path, bool dieOnError);
    /// load a class and return pointer to it
    nClass* OpenClass(const char* className);
    /// create a new unnamed Nebula object
    nRoot* NewUnnamedObject(const char* className);
    /// determine binary path
    void InitBinPath();

#ifndef N_STATIC
    /// read the global table of contents file
    bool ReadToc();
    /// load a Nebula class package
    bool LoadPackage(const char* packageName);

    class nPckgTocEntry : public nHashNode 
    {
    public:
        /// constructor
        nPckgTocEntry(const char* cl, const char* pk);
        /// get package name
        const char* GetPackageName();

    private:
        char pckgName[N_MAXNAMELEN];
    };

    nHashList tocList;              // list of nPckgTocEntry objects
    nString binDir;                 // path to nkernel.dll including terminating /
#endif

    nFileServer2*   fileServer;     // private pointer to file server
    nPersistServer* persistServer;  // private pointer to persistency server
    nTimeServer*    timeServer;     // private pointer to timeserver
    nRemoteServer*  remoteServer;   // private pointer to remoteserver

    nHardRefServer* hardRefServer;  // private pointer to hardrefserver

    nHashList classList;            // list of nClass objects
    nRoot* root;                    // the root object of the Nebula object hierarchie
    nRoot* cwd;                     // the current working object
    nStack<nRoot*> cwdStack;        // stack of previous cwd's

    nLogHandler* defaultLogHandler; // the default log handler
    nLogHandler* curLogHandler;     // the current log handler

#ifdef __NEBULA_MEM_MANAGER__
    nEnv* varMemAlloc;                  // mem statistics
    nEnv* varMemUsed;
    nEnv* varMemNumAlloc;
#endif
};

//------------------------------------------------------------------------------
/**
*/
#ifndef N_STATIC
inline
nKernelServer::nPckgTocEntry::nPckgTocEntry(const char* cl, const char* pk) :
    nHashNode(cl)
{
    n_strncpy2(this->pckgName, pk, sizeof(this->pckgName));
}
#endif

//------------------------------------------------------------------------------
/**
*/
#ifndef N_STATIC
inline
const char*
nKernelServer::nPckgTocEntry::GetPackageName()
{
    return this->pckgName;
}
#endif

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
