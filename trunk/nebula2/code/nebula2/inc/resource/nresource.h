#ifndef N_RESOURCE_H
#define N_RESOURCE_H
//------------------------------------------------------------------------------
/**
    @class nResource
    @ingroup NebulaResourceSystem

    Super class for all resource classes. Resources are mostly loaded
    from a file, can be shared, and unloaded to free runtime space.
    They are managed by a central resource server.

    The nResource class provides an interface for asynchronous loading
    (loading resources in a background thread). Not all subclasses must
    support asynchronous resource loading though.

    Default behaviour is synchronous loading (Load() returns when the
    resource has been loaded). Asynchronous loading is enabled by
    calling SetAsyncEnabled(true). When async loading is enabled,
    Load() returns immediately with a return value of true but IsValid()
    will return false until the resource has actually been loaded at some
    point in the future.

    To check whether a resource loading request has been issued at
    some point in the past which has not been fullfilled yet, call
    the IsPending() method. IsPending() returns true after an async
    call to Load() and before IsValid() returns true to indicate the
    the resource is available. In all other cases, IsPending() returns 
    false.

    Unloading resources always happend immediately, both in sync and
    async mode.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "util/npathstring.h"
#include "kernel/nautoref.h"
#include "util/nthreadvariable.h"
#include "kernel/nthread.h"
#include "kernel/ndynautoref.h"
#include "resource/nresourceloader.h"


//------------------------------------------------------------------------------
class nFileServer2;
class nFile;
class nResource : public nRoot
{
public:
    /// general resource types
    enum Type
    {
        Bundle        = (1<<0),     // keep in front: a resource bundle (contains other resources)
        Mesh          = (1<<1),     // a mesh object
        Texture       = (1<<2),     // a texture object
        Shader        = (1<<3),     // a shader object
        Animation     = (1<<4),     // an animation object
        SoundResource = (1<<5),     // a shared sound resource (nSoundResource)
        SoundInstance = (1<<6),     // a sound instance (nSound3)
        Font          = (1<<7),     // font data
        Other         = (1<<8),     // something else

        AllResourceTypes = (Bundle | Mesh | Texture | Shader | Animation | SoundResource | SoundInstance | Font | Other),
        InvalidResourceType = (1<<30),
    };

    /// constructor
    nResource();
    /// destructor
    virtual ~nResource();
    /// subclasses must indicate to nResource whether async mode is supported
    virtual bool CanLoadAsync() const;
    /// set resource type
    void SetType(Type t);
    /// get resource type
    Type GetType() const;
    /// set absolute path to resource file
    void SetFilename(const nString& path);
    /// get absolute filename to resource file
    nString GetFilename();
    /// enable/disable async behaviour
    void SetAsyncEnabled(bool b);
    /// get async behaviour
    bool GetAsyncEnabled() const;
    /// set the NOH path to an nResourceLoader
    void SetResourceLoader(const char* resourceLoader);
    /// gets the NOH path to the nResourceLoader
    const char* GetResourceLoader();
    /// is resource valid?
    bool IsValid();
    /// is a resource loading request pending?
    bool IsPending();
    /// issue a load request
    virtual bool Load();
    /// issue a load request from an open file
    virtual bool Load(nFile* file, int offset, int length);
    /// unloads the resource
    virtual void Unload();

protected:
    friend class nResourceServer;

    /// set the valid flag
    void SetValid(bool b);
    /// override in subclasse to perform actual resource loading
    virtual bool LoadResource();
    /// override in subclass to perform actual resource unloading
    virtual void UnloadResource();

    nAutoRef<nFileServer2> refFileServer;
    nDynAutoRef<nResourceLoader> refResourceLoader;

private:
    nAutoRef<nResourceServer> refResourceServer;
    nString filename;
    Type type;
    bool asyncEnabled;
    nNode jobNode;      // for linkage into resource server's loader job list
    nThreadVariable<bool> isValid;
};

//------------------------------------------------------------------------------
/**
    Set the resource type.

    @param  t   resource type
*/
inline
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
inline
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
inline
void
nResource::SetFilename(const nString& path)
{
    this->LockMutex();
    this->filename = path;
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Get the absolute path to the resource file. Returns a copy of the string,
    because in asynchronous mode it is not guaranteed that a string reference
    will remain valid.

    @return     path, or 0 if not initialized yet
*/
inline
nString
nResource::GetFilename()
{
    this->LockMutex();
    nString str = this->filename;
    this->UnlockMutex();
    return str;
}

//------------------------------------------------------------------------------
/**
    Enable/disable asynchronous behaviour.

    @param  b   true to enable async behaviour, false for sync behaviour
*/
inline
void
nResource::SetAsyncEnabled(bool b)
{
    this->asyncEnabled = b;
}

//------------------------------------------------------------------------------
/**
    Get the async behaviour flag.

    @return     true if async loading is enabled
*/
inline
bool
nResource::GetAsyncEnabled() const
{
    return this->asyncEnabled;
}

//------------------------------------------------------------------------------
/**
    Set the valid flag.

    @param  b   new valid flag
*/
inline
void
nResource::SetValid(bool b)
{
    this->isValid = b;
}

//------------------------------------------------------------------------------
/**
    Return whether the resource is valid (loaded) or not (unloaded).

    @return     valid flag
*/
inline
bool
nResource::IsValid()
{
    return this->isValid.Get();
}

//------------------------------------------------------------------------------
/**
    Return whether a yet-unfullfilled resource loading request is pending.

    @return     pending flag (true in async mode between Load() and IsValid() = true)
*/
inline
bool
nResource::IsPending()
{
    return this->jobNode.IsLinked();
}

//------------------------------------------------------------------------------
#endif
