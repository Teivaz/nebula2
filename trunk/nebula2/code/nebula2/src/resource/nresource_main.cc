#define N_IMPLEMENTS nResource
//------------------------------------------------------------------------------
//  nresource_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "resource/nresource.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nResource, "nroot");

//------------------------------------------------------------------------------
/**
*/
nResource::nResource() :
    refFileServer(kernelServer),
    type(NONE),
    valid(false)
{
    this->refFileServer = "/sys/servers/file2";
}

//------------------------------------------------------------------------------
/**
*/
nResource::~nResource()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
    Set the resource type.

    @param  t   resource type
*/
void
nResource::SetType(Type t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
    Get the resource type.

    @return     resource type
*/
nResource::Type
nResource::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
    Set the absolute path(!) to the resource file. The path can contain
    Nebula-Assigns.

    @param  path    absolute path to the resource file
*/
void
nResource::SetFilename(const char* path)
{
    n_assert(path);
    this->filename = path;
}

//------------------------------------------------------------------------------
/**
    Get the absolute path to the resource file.

    @return     path, or 0 if not initialized yet
*/
const char*
nResource::GetFilename() const
{
    return this->filename.IsEmpty() ? 0 : this->filename.Get();
}

//------------------------------------------------------------------------------
/**
    Return whether the resource is valid (loaded) or not (unloaded).

    @return     valid flag
*/
bool
nResource::IsValid() const
{
    return this->valid;
}

//------------------------------------------------------------------------------
/**
    Load the resource from file, and set the valid flag if successful.
    This method should be modified by subclasses.

    @return     true if resource data successfully loaded
*/
bool
nResource::Load()
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resource data, freeing runtime resources. This method
    should be modified by subclasses.
*/
void
nResource::Unload()
{
    // empty
}

