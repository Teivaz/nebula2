#define N_IMPLEMENTS nResourceServer
//------------------------------------------------------------------------------
//  nresourceserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "resource/nresourceserver.h"
#include "resource/nresource.h"

nNebulaClass(nResourceServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nResourceServer::nResourceServer() :
    uniqueId(0)
{
    this->refMeshes        = kernelServer->New("nroot", "/sys/share/rsrc/meshes");
    this->refTextures      = kernelServer->New("nroot", "/sys/share/rsrc/textures");
    this->refShaders       = kernelServer->New("nroot", "/sys/share/rsrc/shaders");
    this->refAnimations    = kernelServer->New("nroot", "/sys/share/rsrc/anims");
    this->refSounds        = kernelServer->New("nroot", "/sys/share/rsrc/sounds");
}

//------------------------------------------------------------------------------
/**
*/
nResourceServer::~nResourceServer()
{
    // unload any existing resources
    this->UnloadResources(nResource::MESH);
    this->UnloadResources(nResource::TEXTURE);
    this->UnloadResources(nResource::SHADER);
    this->UnloadResources(nResource::ANIMATION);
    this->UnloadResources(nResource::SOUND);
}

//------------------------------------------------------------------------------
/**
    Create a resource id from a resource name. The resource name is usually
    just the filename of the resource file. The method strips off the last
    32 characters from the resource name, and replaces any invalid characters
    with underscores. It is valid to provide a 0-rsrcName for unshared resources.
    A unique rsrc identifier string will then be created.

    @param  rsrcName    pointer to a resource name (usually a file path), or 0
    @param  buf         pointer to a char buffer
    @param  bufSize     size of char buffer
    @return             a pointer to buf, which contains the result
*/
char*
nResourceServer::GetResourceId(const char* rsrcName, char* buf, int bufSize)
{
    n_assert(buf);
    n_assert(bufSize >= N_MAXNAMELEN);

    if (!rsrcName)
    {
        sprintf(buf, "unique%d", this->uniqueId++);
    }
    else
    {
        int len = strlen(rsrcName) + 1;
        int offset = len - N_MAXNAMELEN;
        if (offset < 0)
        {
            offset = 0;
        }

        // copy string and replace illegal characters, this also copies the terminating 0
        char c;
        const char* from = &(rsrcName[offset]);
        char* to   = buf;
        while ((c = *from++))
        {
            if (('.' == c) || (c == '/') || (c == ':') || (c == '\\'))
            {
                *to++ = '_';
            }
            else
            {
                *to++ = c;
            }
        }
        *to = 0;
    }
    return buf;
}

//------------------------------------------------------------------------------
/**
    Find the right resource root object for a given resource type.

    @param  rsrcType    the resource type
    @return             the root object
*/
nRoot*
nResourceServer::GetResourcePool(nResource::Type rsrcType)
{
    switch(rsrcType)
    {
        case nResource::MESH:       return this->refMeshes.get();
        case nResource::TEXTURE:    return this->refTextures.get();
        case nResource::SHADER:     return this->refShaders.get();
        case nResource::ANIMATION:  return this->refAnimations.get();
        case nResource::SOUND:      return this->refSounds.get();
        default:                    n_assert(false);
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Find a resource object by resource type and name.

    @param  rsrcName    the rsrc name
    @param  rsrcType    resource type
    @return             pointer to resource object, or 0 if not found
*/
nResource*
nResourceServer::FindResource(const char* rsrcName, nResource::Type rsrcType)
{
    n_assert(rsrcName);
    n_assert(nResource::NONE != rsrcType);

    char rsrcId[N_MAXNAMELEN];

    this->GetResourceId(rsrcName, rsrcId, sizeof(rsrcId));
    nRoot* rsrcPool = this->GetResourcePool(rsrcType);
    n_assert(rsrcPool);

    return (nResource*) rsrcPool->Find(rsrcId);
}

//------------------------------------------------------------------------------
/**
    Create a new possible shared resource object. Bumps refcount on an 
    existing resource object. Pass a zero rsrcName if a (non-shared) resource 
    should be created.

    @param  className   the Nebula class name
    @param  rsrcName    the rsrc name (for resource sharing), can be 0
    @param  rsrcType    resource type
    @return             pointer to resource object
*/
nResource*
nResourceServer::NewResource(const char* className, const char* rsrcName, nResource::Type rsrcType)
{
    n_assert(className);
    n_assert(nResource::NONE != rsrcType);

    char rsrcId[N_MAXNAMELEN];
    this->GetResourceId(rsrcName, rsrcId, sizeof(rsrcId));
    nRoot* rsrcPool = this->GetResourcePool(rsrcType);
    n_assert(rsrcPool);

    // see if resource exist
    nResource* obj = (nResource*) rsrcPool->Find(rsrcId);
    if (obj)
    {
        // exists, bump refcount and return
        obj->AddRef();
    }
    else
    {
        // create new resource object
        kernelServer->PushCwd(rsrcPool);
        obj = (nResource*) kernelServer->New(className, rsrcId);
        kernelServer->PopCwd();
        n_assert(obj);
    }
    return obj;
}

//------------------------------------------------------------------------------
/**
    Unload all resource matching the given resource type.

    @param  rsrcType    a resource type
*/
void
nResourceServer::UnloadResources(nResource::Type rsrcType)
{
    nRoot* rsrcPool = this->GetResourcePool(rsrcType);
    n_assert(rsrcPool);

    nResource* rsrc;
    for (rsrc = (nResource*) rsrcPool->GetHead(); rsrc; rsrc = (nResource*) rsrc->GetSucc())
    {
        rsrc->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Reload all resources matching the given resource type. Returns false
    if any of the resources didn't load correctly.

    @param  rsrcType    a resource type
    @return             true if all resources loaded correctly
*/
bool
nResourceServer::ReloadResources(nResource::Type rsrcType)
{
    nRoot* rsrcPool = this->GetResourcePool(rsrcType);
    n_assert(rsrcPool);

    bool retval = true;
    nResource* rsrc;
    for (rsrc = (nResource*) rsrcPool->GetHead(); rsrc; rsrc = (nResource*) rsrc->GetSucc())
    {
        retval &= rsrc->Load();
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Load a resource bundle file. This method is meant to be derived
    by platform specific subclasses for platforms where all resources
    can be packed into a single file.
*/
bool
nResourceServer::LoadResourceBundle(const char* /* filename */)
{
    // empty
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the current resource bundle.
*/
void
nResourceServer::UnloadResourceBundle()
{
    // empty
}

//------------------------------------------------------------------------------
