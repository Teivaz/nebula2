#ifndef N_RESOURCELOADER_H
#define N_RESOURCELOADER_H
//------------------------------------------------------------------------------
/**
    @class nResourceLoader

    @brief contains any custom load function data for an nresource

    In subclassing nResourceLoader, the user is intended to override the
    virtual Load() method with a custom load function to load their
    nResource instances.
    After creating the nResourceLoader, you would store a reference to the
    loader instance on a resource instance, and when that reference is valid
    at Load() time, the resource will use the resource loader's Load()
    method rather than the default.

    (C) 2003 Megan Fox
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nResource;
class nResourceLoader : public nRoot
{
public:
    /// constructor
    nResourceLoader();
    /// destructor
    virtual ~nResourceLoader();

    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// Primary load method, the one called by an nResource.
    virtual bool Load(const char *sFilename, nResource *callingResource);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};
//------------------------------------------------------------------------------
#endif

