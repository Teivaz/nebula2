
//------------------------------------------------------------------------------
//  nresource_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "resource/nresource.h"
#include "resource/nresourceserver.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nResource, "nroot");

//------------------------------------------------------------------------------
/**
*/
nResource::nResource() :
    refResourceServer("/sys/servers/resource"),
    refFileServer("/sys/servers/file2"),
    type(InvalidResourceType),
    asyncEnabled(false),
    isValid(false),
    jobNode(this)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nResource::~nResource()
{
    n_assert(!this->IsValid());
    
    // if we are pending for an async load, we must remove ourselves 
    // from the loader job list.
    if (this->refResourceServer.isvalid())
    {
        this->refResourceServer->RemLoaderJob(this);
    }
}

//------------------------------------------------------------------------------
/**
    Sets a resource loader for the nResource

    @param  resourceLoaderPath    nResourceLoader instance
*/
void
nResource::SetResourceLoader(const char* resourceLoaderPath)
{
    this->refResourceLoader = resourceLoaderPath;
}

//------------------------------------------------------------------------------
/**
    Gets the currently-set resource loader

    @return    the currently-set nResourceLoader
*/
const char*
nResource::GetResourceLoader()
{
    return this->refResourceLoader.getname();
}

//------------------------------------------------------------------------------
/**
    Subclasses must override this method to indicate to the nResource class
    whether they support asynchronous resource loading. Default is false.
*/
bool
nResource::CanLoadAsync() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Request to load a resource in synchronous or asynchronous mode. Will
    care about multithreading issues and invoke the LoadResource() method
    which should be overriden by subclasses to implement the actual loading.
    Subclasses must indicate with the CanLoadAsync() method whether they
    support asynchronous loading or not.

    NOTE: in asynchronous mode, the method will return true although the
    resource data is not available yet. Use the IsValid() method to 
    check when the resource data is available.

    @return     true if all ok, 
*/
bool
nResource::Load()
{
    // if we are already valid, do nothing
    if (this->IsValid())
    {
        return true;
    }

    #ifndef __NEBULA_NO_THREADS__
    if (this->GetAsyncEnabled() && this->CanLoadAsync())
    {
        // if a load request is already pending, do nothing
        if (this->IsPending())
        {
            return true;
        }
        else
        {
            // otherwise, add to resource server's loader jobs
            this->refResourceServer->AddLoaderJob(this);
            return true;
        }
    }
    else
    #endif
    {
        // the synchronous case is simply
        return this->LoadResource();
    }
}

//------------------------------------------------------------------------------
/**
    Loads the resource from within an open file.
    This method should be overriden by subclasses.

    @return     true if resource data successfully loaded
*/
bool
nResource::Load(nFile* /*file*/, int /*offset*/, int /*length*/)
{
    n_error("nResource::Load(file, offset, length) not implemented yet!");
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resource data, freeing runtime resources. This method will call
    the protected virtual UnloadResources() method which should be overriden
    by subclasses.
    This method works in sync and async mode and care about the multithreading
    issues before and after calling LoadResources();
*/
void
nResource::Unload()
{
    // remove from loader list, if pending for async load
    if (this->refResourceServer.isvalid())
    {
        this->refResourceServer->RemLoaderJob(this);
    }
    if (this->IsValid())
    {
        this->UnloadResource();
    }
}

//------------------------------------------------------------------------------
/**
    Subclasses must override this method and implement the actual
    resource loading code here. The method may get called from a thread
    (if the class returns true in the CanLoadAsync() method and the
    resource object works in async mode).
*/
bool
nResource::LoadResource()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Subclasses must override this method and implement the resource unloading.
    This method will always run in the main thread.
*/
void
nResource::UnloadResource()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Return an estimated byte size of the resource data. This is only
    used for statistics.
*/
int
nResource::GetByteSize()
{
    return 0;
}



