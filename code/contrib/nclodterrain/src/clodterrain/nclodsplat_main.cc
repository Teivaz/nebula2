//------------------------------------------------------------------------------
//  nclodsplat_main.cc
//  (C) 2004 Gary Haussmann
//------------------------------------------------------------------------------
#include "clodterrain/nclodsplat.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfile.h"
#include "gfx2/ngfxserver2.h"

nNebulaClass(nCLODSplat, "nresource");

//------------------------------------------------------------------------------
/**
*/
nCLODSplat::nCLODSplat() :
    nCLODMesh()
{
    this->SetType(nResource::Other);
    this->SetAsyncEnabled(true);
}

//------------------------------------------------------------------------------
/**
*/
nCLODSplat::~nCLODSplat()
{
}

void nCLODSplat::Configure(const bbox3 &bounds, float hscale)
{
    this->boundbox = bounds;
    this->h_scale = hscale;
}

/// draw the silly thing
void nCLODSplat::DrawSplat(nGfxServer2 *gfx2, int splatindex) const
{
    n_assert(meshData.isvalid());

    // render the mesh in normal mode (always at stream 0)
    gfx2->SetMesh(this->meshData.get(), this->meshData.get());

    // map from splatindex to a specific group
/*    int mappedsplatindex = -1;
    for (unsigned int iix=0; iix < splatcount; iix++)
    {
        if (splattile[iix] == splatindex)
        {
            // ok, this is the one
            mappedsplatindex = iix+1;
        }
    }*/
    int mappedsplatindex = splatindex;

    n_assert(mappedsplatindex >= 0);

    // set the vertex and index range
    const nMeshGroup& curGroup = this->meshData->Group(mappedsplatindex);
    gfx2->SetVertexRange(curGroup.GetFirstVertex(), curGroup.GetNumVertices());
    gfx2->SetIndexRange(curGroup.GetFirstIndex(), curGroup.GetNumIndices());
    // strip or triangle list?
    if (mappedsplatindex == 0)
        gfx2->DrawIndexedNS(nGfxServer2::TriangleStrip);
    else
        gfx2->DrawIndexedNS(nGfxServer2::TriangleList);
}


/// clod meshes support asynchronous loading
bool nCLODSplat::CanLoadAsync() const
{
    return true;
}

/// issue a load request from an open file
bool nCLODSplat::Load(nFile* file, int offset, int length)
{
    // TODO: fix async loader
    if (this->IsValid())
        return true;

//  n_assert(!this->IsValid());
//  n_assert(!this->IsPending());
//  n_assert(!meshData.isvalid());



    // set our file and offset, and then issue a normal load
    this->terrainFile = file;
    this->dataOffset = offset;

    // get a mesh and configure it
    if (!meshData.isvalid())
    {
        nString meshname = this->GetName();
        meshname += "_m";
        meshData = gfx2->NewMesh(meshname.Get());
    }
    if (!this->collisionmesh)
    {
        this->collisionmesh = nOpende::GeomTriMeshDataCreate();
    }

    n_assert(meshData.isvalid());

    return this->nResource::Load();
}


/// issue an unload request
void nCLODSplat::Unload()
{
    this->nResource::Unload();
}


//------------------------------------------------------------------------------
/**
    Load in terrainr data.
*/
bool nCLODSplat::LoadResource()
{
    // hmmm, already loaded.  happens in the async case where a job
    // is removed from the queue and then immediately re-added to the queue
    // before it is unloaded (or before it is fully loaded)
    if (this->IsValid())
        return true;

    n_assert(!this->IsValid());
    n_assert(gfx2.isvalid());
    n_assert(this->collisionmesh != NULL);

    // loop counters
    int vix;
    unsigned int iix, six;

    //
    // read in data from the terrain file
    //

    // position the file at the appropriate spot
    this->terrainFile->Seek(this->dataOffset, nFile::START);

    // read in the vertex count
    unsigned short vertexcount = this->terrainFile->GetShort();

    // there are 4 shorts per vertex: x,y,z, and morph amount
    unsigned int valuecount = vertexcount * 4;
    short *vertexdatabuffer = new short[valuecount];
    this->terrainFile->Read(vertexdatabuffer, valuecount * sizeof(short) );

    // read in the index count; each index is one short
    unsigned int dummy = this->terrainFile->GetInt();  // should be a dummy 0
    n_assert(dummy == 0);

    unsigned int indexcount = this->terrainFile->GetInt();
    n_assert(indexcount > 0);

    // read in the splat data
    this->splatcount = this->terrainFile->GetInt();

    this->splatindexstart.SetFixedSize(splatcount);
    this->splatindexcount.SetFixedSize(splatcount);
    this->splattile.SetFixedSize(splatcount);

    unsigned int totalindexcount = indexcount;
    for (six=0; six < splatcount; six++)
    {
        this->splattile[six] = this->terrainFile->GetInt();
        unsigned int splatsize = this->terrainFile->GetInt();

        this->splatindexstart[six] = totalindexcount;
        this->splatindexcount[six] = splatsize;

        totalindexcount += splatsize;
    }

    // read in original triangle strip index data, and the splat index data
    ushort *indexdatabuffer = new ushort[totalindexcount];
    this->terrainFile->Read(indexdatabuffer, totalindexcount * sizeof(ushort) );

    //
    // construct the renderable mesh
    //

    n_assert(meshData.isvalid());
    meshData->SetNumGroups(1 + splatcount);
    meshData->SetNumVertices(vertexcount);
    meshData->SetNumIndices(totalindexcount);
    meshData->SetUsage(nMesh2::WriteOnce | nMesh2::NeedsVertexShader);

    // we will give each vertex one uv0 texture coord, and stick the morph value
    // in texture coordinate 'u'
    meshData->SetVertexComponents(nMesh2::Coord | nMesh2::Uv0);

    // make an empty mesh and stuff the vertex data into the mesh
    meshData->Load();

    float *verts = meshData->LockVertices();
    n_assert(verts != NULL);
    vector3 boxcenter = this->boundbox.center();
    vector3 extents = this->boundbox.extents();
    float sx = extents.x / (1<<14),
          sy = extents.y / (1<<14),
          sz = extents.z / (1<<14);
    for (vix=0; vix < vertexcount; vix++)
    {
        // here we have to scale and shift the x,y,z coordinates to keep them
        // in the bounding box
        verts[0] = vertexdatabuffer[vix * 4+0] * sx + boxcenter.x;
        verts[1] = vertexdatabuffer[vix * 4+1] * sy + boxcenter.y;
        verts[2] = vertexdatabuffer[vix * 4+2] * this->h_scale;
        verts[3] = vertexdatabuffer[vix * 4+3] * this->h_scale; // morph value
        verts[4] = 0.0;
        // skip to the next vertex
        verts += 5;
    }
    meshData->UnlockVertices();

    // stuff the index data into the mesh
    ushort *indices = meshData->LockIndices();
    n_assert(indices);
    for (iix=0; iix < totalindexcount; iix++)
    {
        indices[iix] = indexdatabuffer[iix];
    }
    meshData->UnlockIndices();

    // setup the base group info
    nMeshGroup &meshgroup = meshData->Group(0);
    meshgroup.SetFirstVertex(0);
    meshgroup.SetNumVertices(vertexcount);
    meshgroup.SetFirstIndex(0);
    meshgroup.SetNumIndices(indexcount);
    meshgroup.SetBoundingBox(this->boundbox);

    // setup the splat group info
    for (six=0; six < splatcount; six++)
    {
        nMeshGroup &splatgroup = meshData->Group(six+1);
        splatgroup.SetFirstVertex(0);
        splatgroup.SetNumVertices(vertexcount);
        splatgroup.SetFirstIndex(splatindexstart[six]);
        splatgroup.SetNumIndices(splatindexcount[six]);
        splatgroup.SetBoundingBox(this->boundbox);
    }

    //
    // create the collision mesh
    //
#if 1
    // count the # of non-degenerate triangles
    int realfacecount = 0;
    for (iix=0; iix < indexcount-2; iix++)
    {
        if ( (indexdatabuffer[iix] != indexdatabuffer[iix+1]) &&
             (indexdatabuffer[iix+1] != indexdatabuffer[iix+2]) )
             realfacecount++;
    }

    int realindexcount = realfacecount * 3;
    this->collidevertices = new dReal[vertexcount*3];
    this->collideindices = new int[realindexcount];

    // build vertex and index data
    dReal *collideverts = this->collidevertices;
    for (vix=0; vix < vertexcount; vix++)
    {
        // here we have to scale and shift the x,y,z coordinates to keep them
        // in the bounding box
        collideverts[0] = vertexdatabuffer[vix * 4+0] * sx + boxcenter.x;
        collideverts[1] = vertexdatabuffer[vix * 4+1] * sy + boxcenter.y;
        collideverts[2] = vertexdatabuffer[vix * 4+2] * this->h_scale;
        // skip to the next vertex
        collideverts += 3;
    }
    int realiix = 0;
    for (iix=0; iix < indexcount-2; iix++)
    {
        if ( (indexdatabuffer[iix] != indexdatabuffer[iix+1]) &&
             (indexdatabuffer[iix+1] != indexdatabuffer[iix+2]) )
        {
            // when converting from tristrip to triangles, we need to reverse
            // the winding of every other triangle
            if (iix & 1)
            {
                collideindices[realiix++] = indexdatabuffer[iix];
                collideindices[realiix++] = indexdatabuffer[iix+2];
                collideindices[realiix++] = indexdatabuffer[iix+1];
            }
            else
            {
                collideindices[realiix++] = indexdatabuffer[iix];
                collideindices[realiix++] = indexdatabuffer[iix+1];
                collideindices[realiix++] = indexdatabuffer[iix+2];
            }
            n_assert(realiix <= realindexcount);
        }
    }
    n_assert(realiix == realindexcount);

    nOpende::GeomTriMeshDataBuildSingle(this->collisionmesh,
        this->collidevertices, 3 * sizeof(dReal), vertexcount, 
        this->collideindices,  realindexcount, 3 * sizeof(int));

    delete [] vertexdatabuffer;
    delete [] indexdatabuffer;
    n_printf("loaded clod chunk %s at offset %d.  %d vertices, %d indices, %d collision triangles\n", 
        this->GetName(),
        this->dataOffset,
        vertexcount,
        indexcount,
        realfacecount
        );
#endif

//    n_printf("loaded clod chunk %s\n", this->GetName());
    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload everything.
*/
void nCLODSplat::UnloadResource()
{
    n_assert(meshData.isvalid());
    n_assert(this->IsValid());
//    n_assert(this->collisionmesh);

    // destroy the collision stuff
    if (this->collisionmesh)
    {
        nOpende::GeomTriMeshDataDestroy(this->collisionmesh);
        delete [] this->collidevertices;
        delete [] this->collideindices;
        this->collisionmesh = NULL;
        this->collidevertices = NULL;
        this->collideindices = NULL;
    }

    // unload the mesh!
    meshData->Unload();

//  n_printf("unloading clod chunk %s\n", this->GetName());

    this->SetState(Unloaded);
}
