#ifndef N_MESHCOPYRESOURCELOADER_H
#define N_MESHCOPYRESOURCELOADER_H
//------------------------------------------------------------------------------
/**
    @class nMeshCopyResourceLoader
    @ingroup Resource

    A resourceloader for a meshresource, that is a copy of an other mesh.

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "resource/nresourceloader.h"
#include "gfx2/nmesh2.h"

//------------------------------------------------------------------------------
class nResource;
class nMeshCopyResourceLoader : public nResourceLoader
{
public:
    /// constructor
    nMeshCopyResourceLoader();
    /// destructor
    virtual ~nMeshCopyResourceLoader();
    /// initialize resource
    virtual bool InitResource(const char* sFilename, nResource* callingResource);
    /// Primary load method, the one called by an nResource.
    virtual bool Load(const char* sFilename, nResource* callingResource);
    /// Set Source Mesh
    virtual void SetSourceMesh(nMesh2* srcMesh);
    /// Get Source Mesh
    virtual nMesh2* GetSourceMesh();
    /// checks if mesh was reloaded
    bool HasBeenReloaded();
    /// resets reloaded flag
    void ResetReloadFlag();

private:
    nRef<nMesh2> srcMesh;
    bool reloaded;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshCopyResourceLoader::HasBeenReloaded()
{
    return this->reloaded;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshCopyResourceLoader::ResetReloadFlag()
{
    this->reloaded = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshCopyResourceLoader::SetSourceMesh(nMesh2* srcMesh)
{
    n_assert(srcMesh);
    this->srcMesh.set(srcMesh);
}

//------------------------------------------------------------------------------
/**
*/
inline
nMesh2*
nMeshCopyResourceLoader::GetSourceMesh()
{
    n_assert(this->srcMesh.isvalid());
    return this->srcMesh.get();
}
//------------------------------------------------------------------------------
#endif

