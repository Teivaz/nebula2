#ifndef N_RESOURCESERVER_H
#define N_RESOURCESERVER_H
//------------------------------------------------------------------------------
/**
    Central resource server. Creates and manages resource objects. Resources
    are objects which provide several types of data (or data streams) to 
    the application, and can unload and reload themselves on request.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_RESOURCE_H
#include "resource/nresource.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nResourceServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nResourceServer : public nRoot
{
public:
    /// constructor
    nResourceServer();
    /// destructor
    virtual ~nResourceServer();
    /// find a resource object by its name and type
    virtual nResource* FindResource(const char* rsrcName, nResource::Type rsrcType);
    /// create a resource object
    virtual nResource* NewResource(const char* className, const char* rsrcName, nResource::Type rsrcType);
    /// unload all resources matching type
    virtual void UnloadResources(nResource::Type rsrcType);
    /// reload all resources matching type
    virtual bool ReloadResources(nResource::Type rsrcType);
    /// load a resource bundle (only supported on some architectures)
    virtual bool LoadResourceBundle(const char* filename);
    /// unload current resource bundle and free all associated resource
    virtual void UnloadResourceBundle();

    static nKernelServer* kernelServer;

protected:
    /// generate a valid resource id from a resource path
    char* GetResourceId(const char* rsrcName, char* buf, int bufSize);
    /// find the right resource pool object for a given resource type
    nRoot* GetResourcePool(nResource::Type rsrcType);

    int uniqueId;                       // unique id counter for non-shared resources
    nRef<nRoot> refMeshes;
    nRef<nRoot> refTextures;
    nRef<nRoot> refShaders;
    nRef<nRoot> refAnimations;
    nRef<nRoot> refSounds;
};
//------------------------------------------------------------------------------
#endif
