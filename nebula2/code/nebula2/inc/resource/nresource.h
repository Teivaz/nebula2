#ifndef N_RESOURCE_H
#define N_RESOURCE_H
//------------------------------------------------------------------------------
/**
    @class nResource
    @ingroup Resource
    @brief A superclass of all resource related subclasses such as nMesh,
    nFont2, nTexture2 and so on.

    See @ref Resource for more details.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "util/nthreadvariable.h"
#include "kernel/nthread.h"
#include "kernel/ndynautoref.h"
#include "resource/nresourceloader.h"


//------------------------------------------------------------------------------
class nFileServer2;
class nFile;
class nResourceServer;
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

    /// resource states
    enum State
    {
        Unloaded,       // currently not loaded
        Valid,          // currently valid
        Lost,           // currently not available
        Empty,          // available, but contains no data
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
    /// is a resource loading request pending?
    bool IsPending();
    /// issue a load request
    virtual bool Load();
    /// issue a load request from an open file, FIXME: this kinda sucks, Floh.
    virtual bool Load(nFile* file, int offset, int length);
    /// unloads the resource
    virtual void Unload();
    /// set the valid flag
    void SetState(State s);
    /// get current state, this can be Lost, Restored, Valid
    State GetState();
    /// convenience method, returns true when GetState() == Valid
    bool IsValid();
    /// convenience method, returns true when GetState() == Unloaded
    bool IsUnloaded();
    /// convenience method, returns GetState() != Unloaded
    bool IsLoaded();
    /// convenience method, returns true when GetState() == Lost
    bool IsLost();
    /// convenience method, returns true when GetState() == Empty
    bool IsEmpty();
    /// get an estimated byte size of the resource data (for memory statistics)
    virtual int GetByteSize();
    /// get the unique id of this resource
    uint GetUniqueId() const;

protected:
    friend class nResourceServer;

    /// override in subclasse to perform actual resource loading
    virtual bool LoadResource();
    /// override in subclass to perform actual resource unloading
    virtual void UnloadResource();
    /// called when contained resource may become lost
    virtual void OnLost();
    /// called when contained resource may be restored
    virtual void OnRestored();

    nDynAutoRef<nResourceLoader> refResourceLoader;

private:
    static uint uniqueIdCounter;

    nAutoRef<nResourceServer> refResourceServer;
    nString filename;
    Type type;
    bool asyncEnabled;
    nNode jobNode;      // for linkage into resource server's loader job list
    nThreadVariable<State> state;
    uint uniqueId;
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
    Set the current state, this can be Unloaded, Lost, Restored, Valid.

    @param  s   new state
*/
inline
void
nResource::SetState(State s)
{
    this->state = s;
}

//------------------------------------------------------------------------------
/**
    Return the current state.

    @return     current state
*/
inline
nResource::State
nResource::GetState()
{
    return this->state.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nResource::IsValid()
{
    return this->GetState() == Valid;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nResource::IsUnloaded()
{
    return this->GetState() == Unloaded;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nResource::IsLoaded()
{
    return this->GetState() != Unloaded;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nResource::IsEmpty()
{
    return this->GetState() == Empty;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nResource::IsLost()
{
    return this->GetState() == Lost;
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
/**
    Returns the unique id of this resource object. You should use the unique
    id to check whether 2 resources are identical instead of comparing their
    pointers.
*/
inline
uint
nResource::GetUniqueId() const
{
    return this->uniqueId;
}

//------------------------------------------------------------------------------
#endif
