#ifndef N_CLODMESH_H
#define N_CLODMESH_H
//------------------------------------------------------------------------------
/**
    @class nCLODMesh
    @ingroup NCLODTerrainContribModule
*/
#include "resource/nresource.h"
#include "gfx2/nmeshgroup.h"
#include "gfx2/nmesh2.h"

#include "opende/nopendelayer.h"

class nGfxServer2;
class nVariableServer;

//------------------------------------------------------------------------------
class nCLODMesh : public nResource
{
public:
    /// constructor
    nCLODMesh();
    /// destructor
    virtual ~nCLODMesh();

    /// configure for scaling of input data
    virtual void Configure(const bbox3 &bounds, float hscale);

    /// draw the silly thing
    void DrawMesh(nGfxServer2 *gfx2) const;

    /// get collision mesh struct
    const dTriMeshDataID GetCollisionMesh() const
    { return this->collisionmesh; }

    /// clod meshes support asynchronous loading
    virtual bool CanLoadAsync() const;

    /// issue a load request from an open file
    virtual bool Load(nFile* file, int offset, int length);

    /// issue an unload request
    virtual void Unload();

protected:
    /// load resource
    virtual bool LoadResource();
    /// unload resource
    virtual void UnloadResource();

    nRef<nMesh2> meshData;      // actual gfxserver-specific mesh instance
    nAutoRef<nGfxServer2> gfx2; // gfx server - we get the mesh from here
    nFile *terrainFile;         // open file containing our terrain data
    unsigned long dataOffset;       // offset in CLOD file where our data resides
    bbox3 boundbox;
    float h_scale;

    dTriMeshDataID collisionmesh;
    dReal *collidevertices;
    int *collideindices;
};

#endif

