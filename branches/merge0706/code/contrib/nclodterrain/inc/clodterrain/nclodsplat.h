#ifndef N_CLODSPLAT_H
#define N_CLODSPLAT_H
//------------------------------------------------------------------------------
/**
    @class nCLODSplat
    @ingroup NCLODTerrainContribModule
 */
#include "resource/nresource.h"
#include "gfx2/nmeshgroup.h"
#include "gfx2/nmesh2.h"

#ifndef N_CLODMESH_H
#include "clodterrain/nclodmesh.h"
#endif

#include "opende/nopendelayer.h"

class nGfxServer2;
class nVariableServer;

//------------------------------------------------------------------------------
class nCLODSplat : public nCLODMesh
{
public:
    /// constructor
    nCLODSplat();
    /// destructor
    virtual ~nCLODSplat();

    /// configure for scaling of input data
    void Configure(const bbox3 &bounds, float hscale);

    /// how many splats?
    unsigned int GetSplatCount() const { return splatcount; } 

    /// clod meshes support asynchronous loading
    virtual bool nCLODSplat::CanLoadAsync() const;

    /// which tile is used on this splat?
    unsigned int GetTileIndexForSplat(unsigned int splatindex) const
    { return splattile[splatindex]; }

    /// draw a splat
    void DrawSplat(nGfxServer2 *gfx2, int splatindex) const;

    /// issue a load request from an open file
    virtual bool Load(nFile* file, int offset, int length);

    /// issue an unload request
    virtual void Unload();

protected:
    /// load resource
    virtual bool LoadResource();
    /// unload resource
    virtual void UnloadResource();

    // index ranges and tile types for the various splats
    nArray<int> splatindexstart;
    nArray<int> splatindexcount;
    nArray<int> splattile;
    unsigned int splatcount;
};

#endif

