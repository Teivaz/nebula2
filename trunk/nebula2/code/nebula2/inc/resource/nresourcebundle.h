#ifndef N_RESOURCEBUNDLE_H
#define N_RESOURCEBUDNLE_H
//------------------------------------------------------------------------------
/**
    @class nResourceBundle
    @ingroup Resource
    @brief Load a resource bundle from disc and register all contained
    resources.

    (C) 2003 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "kernel/nautoref.h"

class nGfxServer2;
class nAnimationServer;

//------------------------------------------------------------------------------
class nResourceBundle : public nResource
{
public:
    /// constructor
    nResourceBundle();
    /// destructor
    virtual ~nResourceBundle();

protected:
    /// load the resource (sets the valid flag)
    virtual bool LoadResource();
    /// unload the resource (clears the valid flag)
    virtual void UnloadResource();

private:
    /// load a single resource from the resource bundle file
    bool LoadResource(int type, const char* resId, int dataOffset, int dataSize, int flags, nFile* dataFile);
    /// load a mesh resource from the resource bundle file
    bool LoadMesh(const char* resId,  int dataOffset, int dataSize, int flags, nFile* dataFile);
    /// load an animation resource from the resource bundle file
    bool LoadAnimation(const char* resId, int dataOffset, int dataSize, int flags, nFile* dataFile);
    /// load a texture resource from the resource bundle file
    bool LoadTexture(const char* resId, int dataOffset, int dataSize, int flags, nFile* dataFile);

    nAutoRef<nAnimationServer> refAnimServer;
    nArray<nRef<nResource> > bundleResources;  // resource objects associated with this bundle
};
//------------------------------------------------------------------------------
#endif
