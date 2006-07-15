#ifndef N_RESOURCESERVER_H
#define N_RESOURCESERVER_H
//------------------------------------------------------------------------------
/**
    @class nResourceServer
    @ingroup Resource
    @brief Central resource server. Creates and manages resource objects.
    
    Resources are objects which provide several types of data (or data
    streams) to the application, and can unload and reload themselves
    on request.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "resource/nresource.h"
#include "kernel/nref.h"

//------------------------------------------------------------------------------
class nResourceServer : public nRoot
{
public:
    /// constructor
    nResourceServer();
    /// destructor
    virtual ~nResourceServer();
    /// return singleton instance pointer
    static nResourceServer* Instance();
    /// find a resource object by its name and type
    virtual nResource* FindResource(const nString& rsrcName, nResource::Type rsrcType);
    /// create a resource object
    virtual nResource* NewResource(const nString& className, const nString& rsrcName, nResource::Type rsrcType);
    /// unload all resources matching resource type (OR'ed nResource::Type's)
    virtual void UnloadResources(int resTypeMask);
    /// reload all resources matching type
    virtual bool LoadResources(int resTypeMask);
    /// call OnLost() on all resources defined in the mask
    virtual void OnLost(int resTypeMask);
    /// call OnRestored() on all resources defined in the mask
    virtual void OnRestored(int resTypeMask);
    /// return the resource pool directory for a given resource type
    nRoot* GetResourcePool(nResource::Type rsrcType);
    /// generate a valid resource id from a resource path
    nString GetResourceId(const nString& rsrcName);
    /// return number of resources of given type
    int GetNumResources(nResource::Type rsrcType);
    /// get number of bytes of RAM occupied by resource type
    int GetResourceByteSize(nResource::Type rsrcType);

protected:
    friend class nResource;
    static nResourceServer* Singleton;

    /// add a resource to the loader job list
    void AddLoaderJob(nResource* res);
    /// remove a resource from the loader job list
    void RemLoaderJob(nResource* res);
    /// start the loader thread
    void StartLoaderThread();
    /// shutdown the loader thread
    void ShutdownLoaderThread();
    /// the loader thread function
    static int N_THREADPROC LoaderThreadFunc(nThread* thread);
    /// the thread wakeup function
    static void ThreadWakeupFunc(nThread* thread);

    int uniqueId;
    nRef<nRoot> meshPool;
    nRef<nRoot> texPool;
    nRef<nRoot> shdPool;
    nRef<nRoot> animPool;
    nRef<nRoot> sndResPool;
    nRef<nRoot> sndInstPool;
    nRef<nRoot> fontPool;
    nRef<nRoot> bundlePool;
    nRef<nRoot> dbPool;
    nRef<nRoot> otherPool;

    nThreadSafeList jobList;    // list for outstanding background loader jobs
    nThread* loaderThread;      // background thread for handling async resource loading

    nClass* resourceClass;
};

//------------------------------------------------------------------------------
/**
*/
inline
nResourceServer*
nResourceServer::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif
