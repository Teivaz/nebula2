#ifndef N_RESOURCE_H
#define N_RESOURCE_H
//------------------------------------------------------------------------------
/**
    @class nResource

    Super class for all resource classes. Resources are mostly loaded
    from a file, can be shared, and unloaded to free runtime space.
    They are managed by a central resource server.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_PATHSTRING_H
#include "util/npathstring.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nResource
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nFileServer2;
class N_PUBLIC nResource : public nRoot
{
public:
    /// general resource types
    enum Type
    {
        NONE,               // not initialized yet
        MESH,               // a mesh object
        TEXTURE,            // a texture object
        SHADER,             // a shader object
        ANIMATION,          // an animation object
        SOUND,              // sound data
    };

    /// constructor
    nResource();
    /// destructor
    virtual ~nResource();
    /// set resource type
    void SetType(Type t);
    /// get resource type
    Type GetType() const;
    /// set absolute path to resource file
    void SetFilename(const char* path);
    /// get absolute filename to resource file
    const char* GetFilename() const;
    /// is resource valid?
    bool IsValid() const;
    /// load the resource (sets the valid flag)
    virtual bool Load();
    /// unload the resource (clears the valid flag)
    virtual void Unload();

    static nKernelServer* kernelServer;

protected:
    nAutoRef<nFileServer2> refFileServer;
    nPathString filename;
    Type type;
    bool valid;
};

//------------------------------------------------------------------------------
#endif
