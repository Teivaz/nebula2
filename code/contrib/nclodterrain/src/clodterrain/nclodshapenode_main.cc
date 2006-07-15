//------------------------------------------------------------------------------
//  nclodshapenode_main.cc
//  (C) 2004 Gary Haussmann
//------------------------------------------------------------------------------
#include "clodterrain/nclodshapenode.h"
#include "clodterrain/nclodmesh.h"
#include "clodterrain/nclodtile.h"
#include "clodterrain/nclodsplat.h"
#include "kernel/nfileserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nsceneserver.h"
#include "mathlib/bbox.h"

#include "opende/nopendelayer.h"

nNebulaScriptClass(nCLODShapeNode, "nmaterialnode");

class nCLODQuadTreeNode {
public:
    nCLODQuadTreeNode() : tqtdatafile(NULL), renderme(false), collspaceID(0), chunkGeomID(0) {}

    ~nCLODQuadTreeNode() 
    {
        if (this->chunkGeomID)
        {
            nOpende::GeomDestroy(this->chunkGeomID);
        }
        if (meshdata.isvalid())
        {
            if (meshdata->IsValid())
            {
                meshdata->Unload();
            }
            meshdata->Release();
        }
        if (tiledata.isvalid())
        {
            if (tiledata->IsValid())
            {
                tiledata->Unload();
            }
            tiledata->Release();
        }
    }

    /// Request the chunk be loaded
    void LoadChunk()
    {
        n_assert(meshdata.isvalid());
        n_assert(this->chunkGeomID == 0);

        // don't load again if already being loaded
        meshdata->Load(datafile, dataoffset, datasize);
        if (tqtdatafile)
        {
            n_assert(tiledata.isvalid());
            if (!tiledata->IsValid())
                tiledata->Load();
        }
    }

    /// Request the chunk be unloaded
    void UnloadChunk()
    {
        n_assert(meshdata.isvalid());
        n_assert(this->chunkGeomID == 0);
        meshdata->Unload();
        if (tqtdatafile)
        {
            n_assert(tiledata.isvalid());
            if (tiledata->IsValid())
                tiledata->Unload();
        }
    }

    /// Is the chunk valid?
    bool IsValidChunk()
    {
        n_assert(meshdata.isvalid());
        return meshdata->IsValid();
    }

    /// Initialize our child pointers -- do this before calling InitializeFromFile!
    virtual void InitializeChildPointers(nCLODQuadTreeNode *s0,nCLODQuadTreeNode *s1,nCLODQuadTreeNode *s2,nCLODQuadTreeNode *s3)
    {
        children[0] = s0;
        children[1] = s1;
        children[2] = s2;
        children[3] = s3;
    }

    /// Initialize bounding box--only call on the root node, after initializing child pointers
    void InitializeBasics(const bbox3 &mybbox, float hscale, dSpaceID space, nCLODShapeNode::DetailTextureArray *details)
    {
        heightscale = hscale;
        chunkbounds = mybbox;
        collspaceID = space;
        detailTextures = details;

        // calculate new bounds and settings for our children, if any
        if (children[0] != NULL)
        {
            vector3 oldcenter(mybbox.center());
            int childix;
            for (childix = 0; childix < 4; childix++)
            {
                bbox3 newbbox(mybbox);
                // the new bounding box is scaled in half, and maybe offset.
                // we basically have to shift either the min or max coordinate to the
                // center for the x and y axes
                vector3 *xchange = (childix & 1) ? &newbbox.vmin : &newbbox.vmax;
                vector3 *ychange = (childix & 2) ? &newbbox.vmin : &newbbox.vmax;
                xchange->x = oldcenter.x;
                ychange->y = oldcenter.y;
                children[childix]->InitializeBasics(newbbox, hscale, space, details);
            }
        }
    }

    /// read and initialize stuff based on data from the clod file.  Call InitializeBBox before this function
    virtual void InitializeFromFile(nFile *clodfile, int chunkindex, nString const &chunkNamebase)
    {
        // read in neighbor indices
        for (int i=0; i < 4; i++)
            (void)clodfile->GetInt();

        // read in LOD level
        (void)clodfile->GetChar();

        // read in z min/max
        short zmin, zmax;
        zmin = clodfile->GetShort();
        zmax = clodfile->GetShort();

        chunkbounds.vmin.z = zmin * heightscale;
        chunkbounds.vmax.z = zmax * heightscale;

        // read in our file location/size from the file
        datafile = clodfile;
        dataoffset = clodfile->GetInt();
        datasize = clodfile->GetInt();

        // initialize the clodmesh resource
        nString clodmeshname = "/usr/data/";
        clodmeshname += chunkNamebase;
        char buffer[10];
        clodmeshname += itoa(chunkindex, buffer, 10);
        this->meshdata = (nCLODMesh *)nKernelServer::ks->New("nclodmesh", clodmeshname.Get());
        this->meshdata->Configure(GetBounds(), heightscale);

/*      n_printf("clod chunk name %s, bounds (%f, %f, %f) - (%f, %f, %f)\n",
            clodmeshname.Get(),
            chunkbounds.vmin.x, chunkbounds.vmin.y, chunkbounds.vmin.z,
            chunkbounds.vmax.x, chunkbounds.vmax.y, chunkbounds.vmax.z);
            n_printf("clod chunk, offset %d, size %d\n",
            dataoffset, datasize);*/
    }

    void InitializeFromTqtFile(nFile *tqtfile, nGfxServer2 *gfx2, int chunkindex, int tilesize, nString const &chunkNamebase)
    {
        // read in tqt tile offset and size
/*      this->tqtdatafile = tqtfile;
        this->tqttilesize = tilesize;
        int texturecount = tqtfile->GetInt();
        n_assert(texturecount == 1);
        tqtdataoffset = tqtfile->GetInt();
        tqtdatasize = tqtfile->GetInt();*/
        this->tqtdatafile = tqtfile;

        // create a texture resource for this chunk
        nString clodtilename = "/usr/data/";
        clodtilename += chunkNamebase;
        char buffer[10];
        clodtilename += itoa(chunkindex, buffer, 10);
        this->tiledata = (nCLODTile *)nKernelServer::ks->New("nclodtile", clodtilename.Get());
        this->tiledata->Configure(tilesize, tqtfile);

        // compute extra splat offset - this will shift the blend texture be 1/2 a texel
        this->splatweightbias = 0.5f/(tilesize/2+1);

        n_printf("made tile %d, size %d\n", chunkindex, tilesize);
    }

    // get the x/y/z boundaries of this chunk
    const bbox3 &GetBounds() const
    {
        return chunkbounds;
    }

    // get the clodmesh
    nCLODMesh *GetChunkMesh() const
    { return meshdata.get(); }

    // returns true if this chunk has children
    bool HasChildren() const
    { return children[0] != NULL; }

    // returns true if we have children and they are loaded
    bool ChildrenValid() const
    {
        if (!HasChildren()) return false;

        // this is false if any children are not loaded
        for (int i=0; i<4; i++)
        {
            if (!children[i]->IsValidChunk())
                return false;
        }
        return true;
    }

    // sets up data to render this chunk
    inline void RenderOn(float mydistance, float mindistance)
    {
        this->renderme = true;
        morphscale = (mydistance/mindistance) - 1.0f;
        if (morphscale < 0.0) morphscale = 0.0f;
        if (morphscale > 1.0) morphscale = 1.0f;
        // add to the collision space
        if ( (this->collspaceID != 0) && (this->IsValidChunk()) && (this->chunkGeomID == 0))
        {
            this->chunkGeomID = nOpende::CreateTriMesh(this->collspaceID, this->meshdata->GetCollisionMesh(),0,0,0);
            //nOpende::GeomTriMeshEnableTC(this->chunkGeomID,dSphereClass, true);
            //nOpende::GeomTriMeshEnableTC(this->chunkGeomID,dBoxClass, true);
            n_printf("made chunk geometry, %d\n", this->chunkGeomID);
        }
    }

    // set up data so that this chunk is NOT rendered
    inline void RenderOff()
    {
        this->renderme = false;
        // remove from the collision space
        if ( this->chunkGeomID != 0)
        {
            n_printf("destroying chunk geometry, %d\n", this->chunkGeomID);
            nOpende::GeomDestroy(this->chunkGeomID);
            this->chunkGeomID = 0;
        }
    }

    // update the chunk state based on proximity to the camera.  Pass in the closest
    // distance this chunk can be before it should split and load higher LOD sub chunks,
    // and the camera position in terrain/object space.  Automatically recurses to sub-chunks
    // as needed
    void UpdateTree(float mindistance, const vector3 &camerapos)
    {
        n_assert(this->IsValidChunk());

        // compute apparent screen space error from the camera's POV
        // find the chunk corner closest to the camera
        vector3 myvector = camerapos - chunkbounds.center();
        vector3 extents = chunkbounds.extents();
        myvector.x = n_max(0,fabsf(myvector.x) - extents.x);
        myvector.y = n_max(0,fabsf(myvector.y) - extents.y);
        myvector.z = n_max(0,fabsf(myvector.z) - extents.z);
        float mydistance = myvector.len();
        n_assert(mydistance >= 0.0f);

/*      n_printf("min lod distance %f, my distance %f, camera pos (%f,%f,%f)\n",
                mindistance, mydistance,
                camerapos.x, camerapos.y, camerapos.z);*/
        // if we have no children, we always render this chunk

        if (!HasChildren())
        {
            RenderOn(mydistance, mindistance);
            return;
        }

        if (mydistance < mindistance)
        {
            // we shouldn't render this chunk, we should render the more-accurate sub-chunks
            // if all our sub-chunks are loaded, we can just recurse to them; otherwise,
            // we have submit load requests to the sub-chunks and render this chunk while we
            // wait for the sub-chunks to load
            if (ChildrenValid())
            {
                // recurse to sub-children.  Since sub-chunk meshs have 1/2 the terrain error
                // that we do, they can be closer to the camera, up to 1/2 the distance we are allowed
                float newmindistance = mindistance * 0.5f;
                RenderOff();
                for (int i=0; i < 4; i++)
                    children[i]->UpdateTree(newmindistance, camerapos);
            }
            else
            {
                // render this chunk while loading in the children
                // normally morphscale would evaluate to <0.0; we clamp it to 0.0
                RenderOn(mydistance, mindistance);
                for (int i=0; i < 4; i++)
                {
                    if (!children[i]->IsValidChunk())
                        children[i]->LoadChunk();
                }
            }
        }
        else
        {
            // this chunk has enough accuracy, so it should be rendered.  We unload children
            // if they are loaded
            RenderOn(mydistance, mindistance);
            if (HasChildren())
            {
                for (int i=0; i < 4; i++)
                {
                    if (children[i]->IsValidChunk())
                        children[i]->PruneTree();
                }
            }
        }

        return;
    }

    // special function to unload this chunk and all lower chunks, as needed
    void PruneTree()
    {
        n_assert(IsValidChunk());
        // unload ourselves and any loaded children
        RenderOff();
        if (HasChildren())
        {
            for (int i=0; i < 4; i++)
            {
                if (children[i]->IsValidChunk())
                    children[i]->PruneTree();
            }
        }   
        this->UnloadChunk();
    }

    // helper class for culling
    class result_info {
        public:
            bool    culled; // true when the volume is not visible
            unsigned char   active_planes;  // one bit per frustum plane
        
            //0x3F=b111111 => 6 planes for view frustum. Front, back and 4 side planes. (Pyramid with top cut off).     
            result_info(bool c = false, unsigned char a = 0x3f) : 
                culled(c), active_planes(a) 
            { }
    };
    
    bool Cull(result_info ri, const plane frustum[6])
    {
        // check this chunk against the clip planes
        if (ri.active_planes) {
            ri = compute_box_visibility(frustum, ri);
            if (ri.culled) {
                // mark as culled, and exit.  No children should be rendered since they're
                // culled also
                cullme = true;
                return true;
            }
        }

        // this node is not culled.  If it is rendered, we do not need to check any children.
        // If it is not rendered, then children will be rendered, so we should check and cull
        // children as well
        cullme = false;
        if (renderme == false)
        {
            n_assert(HasChildren());
            for (int i = 0; i < 4; i++) {           
                children[i]->Cull(ri, frustum);
            }
        }
        
        return true;
    }

    result_info compute_box_visibility(const plane frustum[6], result_info in)
    {
        // Returns a visibility code indicating the culling status of the
        // given axis-aligned box.  The result_info passed in should indicate
        // which planes might cull the box, by setting the corresponding
        // bit in in.active_planes.
        vector3 center = chunkbounds.center();
        vector3 extent = chunkbounds.extents();

        // Check the box against each active frustum plane.
        int bit = 1;
        for (int i = 0; i < 6; i++, bit <<= 1) {
            if ((bit & in.active_planes) == 0) {
                // This plane is already known to not cull the box.
                continue;
            }

            const plane&    p       = frustum[i];
            const vector3 normal    = p.normal();
            
            // Check box against this plane.
            float   d = normal % center + p.d; //Calculate closest distance from center point to plane.
            float   extent_toward_plane = fabsf(extent.x * normal.x)
                + fabsf(extent.y * normal.y)
                + fabsf(extent.z * normal.z);
            if (d < 0) {  
                if (-d > extent_toward_plane) {
                    // Box is culled by plane; it's not visible.
                    return result_info(true, 0);
                } // else this plane is ambiguous so leave it active.
            } else {
                if (d > extent_toward_plane) {
                    // Box is accepted by this plane, so
                    // deactivate it, since neither this
                    // box or any contained part of it can
                    // ever be culled by this plane.
                    in.active_planes &= ~bit;
                    if (in.active_planes == 0) {
                        // This box is definitively inside all the culling
                        // planes, so there's no need to continue.
                        return in;
                    }
                } // else this plane is ambigious so leave it active.
            }
        }

        return in;  // Box not definitively culled.  Return updated active plane flags.
    }

    virtual void RenderTree(nGfxServer2 *gfx, nShader2 *someshader, nCLODQuadTreeNode *tiledchunk)
    {
        // if culled, then we're done
        if (cullme) return;

        // update my tile state in case children need my tile, which may occurs
        // if the child's tile is not yet loaded by the async loader
        if (tiledata.isvalid() && tiledata->IsValid())
        {
            // we are the tiled chunk for all our children
            tiledchunk = this;
        }

        // recurse down and render the chunks as appropriate
        if (renderme)
        {
            n_assert(IsValidChunk());

            // set the morph value for the shader to use and draw the mesh
            n_assert(someshader->IsParameterUsed(nShaderState::Scale));
            someshader->SetFloat(nShaderState::Scale, morphscale);
            // if a tiled chunk exists (either this chunk or an ancestor passed in as
            // the third parameter) map the tile for that chunk.
            if (tiledchunk != NULL)
            {
                n_assert(tiledchunk->tiledata->ToShader(someshader));
                // compute texture transform
                matrix44 drapematrix;
                vector3 chunkextents(tiledchunk->chunkbounds.extents());
                vector3 chunkorigin(tiledchunk->chunkbounds.center()-chunkextents);
                vector3 chunkscale(0.5f/chunkextents.x, 0.5f/chunkextents.y, 1.0f);
                drapematrix.set_translation(-chunkorigin);
                drapematrix.scale(chunkscale);
                n_assert(someshader->IsParameterUsed(nShaderState::TextureTransform0));
                someshader->SetMatrix(nShaderState::TextureTransform0, drapematrix);
            }
            meshdata->DrawMesh(gfx);
        }
        else
        {
            n_assert(HasChildren());
            for (int i=0; i < 4; i++)
            {
                children[i]->RenderTree(gfx, someshader, tiledchunk);
            }
        }
    }

public:
    nRef<nCLODTile> tiledata; // tqt tile for this chunk
    float splatweightbias; // extra texel offset for splat weight textures

protected:
    nRef<nCLODMesh> meshdata; // actual triangle mesh we use to draw stuff
    nFile *datafile; /// file holding the clod data
    int dataoffset, datasize; /// where the chunk is located in the terrain file
    nFile *tqtdatafile; /// tqt file holding the tile data
    bbox3 chunkbounds;   /// used to find the position and apparent screen size of the chunk
    int tqtdatasize, tqttilesize; /// location and size of bytes in the tile data, and pixel size
    float heightscale; // convert from z-coordinate read from file (as shorts) to actual coordinates
    float morphscale;   // morph value, 0-1
    bool renderme;      // flag to indicate this chunk should be rendered
    bool cullme;        // flag indicates we were culled from the view frustum
    dSpaceID collspaceID; // space in which we put terrain chunk geometry
    dGeomID chunkGeomID; // opende id of the geometry for this terrain chunk
    nCLODQuadTreeNode *children[4]; // sub-nodes

    nCLODShapeNode::DetailTextureArray *detailTextures;

private:
};

class nCLODQuadTreeSplatNode : public nCLODQuadTreeNode {
public:
    nCLODQuadTreeSplatNode() : nCLODQuadTreeNode() {}
    ~nCLODQuadTreeSplatNode()
    {
        // splatdata is cleaned when meshdata is cleaned up--hopefully
    }

    /// read and initialize stuff based on data from the clod file.  Call InitializeBBox before this function
    virtual void InitializeFromFile(nFile *clodfile, int chunkindex, nString const &chunkNamebase)
    {
        // read in neighbor indices
        for (int i=0; i < 4; i++)
            (void)clodfile->GetInt();

        // read in LOD level
        (void)clodfile->GetChar();

        // read in z min/max
        short zmin, zmax;
        zmin = clodfile->GetShort();
        zmax = clodfile->GetShort();

        chunkbounds.vmin.z = zmin * heightscale;
        chunkbounds.vmax.z = zmax * heightscale;

        // read in our file location/size from the file
        datafile = clodfile;
        dataoffset = clodfile->GetInt();
        datasize = clodfile->GetInt();

        // initialize the clodmesh resource
        nString clodmeshname = "/usr/data/";
        clodmeshname += chunkNamebase;
        char buffer[10];
        clodmeshname += itoa(chunkindex, buffer, 10);
        this->splatdata = (nCLODSplat *)nKernelServer::ks->New("nclodsplat", clodmeshname.Get());
        this->splatdata->Configure(GetBounds(), heightscale);

        // if we alias to meshdata, then we can reuse the meshdata load/unload routines
        this->meshdata = this->splatdata.get();
    }

    void InitializeSplat(nTexture2 *whitetexture)
    {
        this->baseblendtexture = whitetexture;

    }

    virtual void RenderTree(nGfxServer2 *gfx, nShader2 *splatshader, nCLODQuadTreeNode *tiledchunk)
    {
        // if culled, then we're done
        if (cullme) return;

        // update my tile state in case children need my tile, which may occurs
        // if the child's tile is not yet loaded by the async loader
        if (tiledata.isvalid() && tiledata->IsValid())
        {
            // we are the tiled chunk for all our children
            tiledchunk = this;
        }

        // recurse down and render the chunks as appropriate
        if (renderme)
        {
            n_assert(IsValidChunk());

            // if a tiled chunk exists (either this chunk or an ancestor passed in as
            // the third parameter) draw the splats for that chunk
            if (tiledchunk != NULL)
            {
                // compute texture transform
                matrix44 drapematrix, detailmatrix;
                vector3 chunkextents(tiledchunk->GetBounds().extents());
                vector3 chunkorigin(tiledchunk->GetBounds().center()-chunkextents);
                vector3 chunkscale(0.5f/chunkextents.x, 0.5f/chunkextents.y, 1.0f);

                // do extra mapping for blend textures
                if (tiledchunk->tiledata->GetNumTextures() > 1)
                {
                    // re-drape the texture such that we have 129/257/513 texels with a 1/2 texel
                    // offset for the blend texture
                    chunkscale *= 0.5;
                    // chunkextents is only 1/2 the chunk size, compensate with a x2 factor
                    chunkorigin -= chunkextents * 2.0 * tiledchunk->splatweightbias; 
                }
                drapematrix.set_translation(-chunkorigin);
                drapematrix.scale(chunkscale);

                n_assert(splatshader->IsParameterUsed(nShaderState::Scale));
                n_assert(splatshader->IsParameterUsed(nShaderState::TextureTransform0));
                n_assert(splatshader->IsParameterUsed(nShaderState::TextureTransform1));

                splatshader->SetFloat(nShaderState::Scale, morphscale);
                splatshader->SetMatrix(nShaderState::TextureTransform0, drapematrix);

                if (tiledchunk->tiledata->GetNumTextures() > 1)
                {
                    // do splat drawing
                    // set the morph value for the shader to use and draw the mesh
                    for (unsigned int six=0; six < splatdata->GetSplatCount(); six++)
                    {
                        unsigned int splattileindex = splatdata->GetTileIndexForSplat(six);
                        n_assert(tiledchunk->tiledata->ToShader(splatshader,splattileindex));
                        if (detailTextures)
                            splatshader->SetTexture(nShaderState::DiffMap1, (*detailTextures)[splattileindex].get() );

                        if (six==0)
                        {
                            // turn blending off and draw the whole terrain chunk for the first
                            // "splat" as this will set the depth buffer and zero out the whole
                            // chunk so we can additively blend in the rest of the tiles
                            splatshader->SetBool(nShaderState::MatTranslucency, false);
                            splatdata->DrawSplat(gfx,0);
                        }
                        else
                        {
                            // turn on blending and draw only the relevant polygons
                            splatshader->SetBool(nShaderState::MatTranslucency, true);
                            // this line draws the whole chunk, relying on blending to distinguish
                            // between the current terrain texture tile and others.  It should be
                            // guaranteed to generate geometry identical to the first splat, so
                            // there should be no z-fighting, but it blows a lot of fill rate
                            splatdata->DrawSplat(gfx, 0);
                            // The following line will only render the triangles that are needed
                            // to get full coverage for the current tile.  This is typically a
                            // much smaller area than the whole terrain chunk, so it saves fill
                            // rate.  However, by using a triangle list and perhaps different indexing
                            // orders there is sometimes visible z-fighting when using this.
                            //splatdata->DrawSplat(gfx, six+1);
                        }
                    }
                }
                else
                {
                    // just one texture; so render it
                    n_assert(tiledchunk->tiledata->ToShader(splatshader,0));
                    splatshader->SetTexture(nShaderState::DiffMap1, baseblendtexture);
                    splatshader->SetBool(nShaderState::MatTranslucency, false);
                    splatdata->DrawMesh(gfx);
                }
            }
/*            if (splatdata->GetSplatCount() > splatindex)
                splatdata->DrawSplat(gfx,splatindex);*/
        }
        else
        {
            n_assert(HasChildren());
            for (int i=0; i < 4; i++)
            {
                children[i]->RenderTree(gfx, splatshader, tiledchunk);
            }
        }
    }
protected:
    nRef<nCLODSplat> splatdata;
    nTexture2 *baseblendtexture;
};

// prototype for new clod header is 6 octets:
// 4 octets: 'CLOD'
// 2 octets : version number (10)
// 2 octets : tree height (>= 1)
//    to find the # of chunks from tree height, see CalcTreeChunks
// 4 octets: x size (float)
// 4 octets: y size (float)
// 4 octets: z size (float)
// 4 octets: terrain error (float)
// next there are a # of offsets (use GetInt), one per chunk, telling the offset
// in the file where you can find the data for each chunk

// Given the tree height, calculate the number of terrain chunks in the quadtree
inline unsigned long CalcTreeChunks(unsigned int treeheight)
{
    n_assert(treeheight > 0);
    n_assert(treeheight < 16);

    return ( ( 1L << (treeheight*2-1) ) - 1L ) & 0x55555555L;
}

//------------------------------------------------------------------------------
/**
*/
nCLODShapeNode::nCLODShapeNode() :
    terrainName(), 
    refFileServer("/sys/servers/file2"),
    refGfx2("/sys/servers/gfx"),
    dynmesh(0),
    quadtreeChunks(NULL), 
    quadtreeDepth(0),
    terrainFile(NULL),
    tqtFile(NULL),
    screenError(2.5),
    detailScaling(0.1)
{
}

//------------------------------------------------------------------------------
/**
*/
nCLODShapeNode::~nCLODShapeNode()
{
    UnloadTerrain();

    n_assert(!this->quadtreeChunks);
    n_assert(!this->dynmesh);

    if (this->terrainFile)
    {
        if (this->terrainFile->IsOpen())
        {
            this->terrainFile->Close();
        }
        this->terrainFile->Release();
    }
    if (this->tqtFile)
    {
        if (this->tqtFile->IsOpen())
        {
            this->tqtFile->Close();
        }
        this->tqtFile->Release();
    }
    if (this->refWhiteTexture.isvalid())
    {
        this->refWhiteTexture->Release();
    }
}

//------------------------------------------------------------------------------
/**
    This method must return the mesh usage flag combination required by
    this shape node class. Subclasses should override this method
    based on their requirements.

    @return     a combination on nMesh2::Usage flags
*/
int
nCLODShapeNode::GetMeshUsage() const
{
    return (nMesh2::WriteOnce | nMesh2::NeedsVertexShader);
}

/// set the screen space error
void nCLODShapeNode::SetScreenSpaceError(double spaceerror)
{
    n_assert(spaceerror > 0.0);

    this->screenError = spaceerror;
}

/// get the current screen space error
double nCLODShapeNode::GetScreenSpaceError() const
{
    return this->screenError;
}

//------------------------------------------------------------------------------
/**
    Unload all currently valid terrain resources.
*/
void
nCLODShapeNode::UnloadTerrain()
{
    // TODO: call unload on all map chunks
    if (this->quadtreeChunks)
    {
        unsigned long numchunks = CalcTreeChunks(this->quadtreeDepth);
        for (unsigned int qix=0; qix < numchunks; qix++)
            delete this->quadtreeChunks[qix];
        delete [] this->quadtreeChunks;
        this->quadtreeChunks = NULL;
    }

    // close the terrain file
    if ( (this->terrainFile) && (this->terrainFile->IsOpen()) )
    {
        this->terrainFile->Close();
    }
    if ( (this->tqtFile) && (this->tqtFile->IsOpen()) )
    {
        this->tqtFile->Close();
    }
    if (this->dynmesh)
    {
        delete this->dynmesh;
        this->dynmesh = NULL;
    }
}

//------------------------------------------------------------------------------
/**
    Load new terrain, release old one if valid.
*/
bool
nCLODShapeNode::LoadTerrain()
{
    // make us a small white texture if needed
    if (!refWhiteTexture.isvalid())
    {
        refWhiteTexture = (nTexture2 *)refGfx2->NewTexture("nclodterrain_whitetexture");
        if (!refWhiteTexture->IsValid())
        {
            refWhiteTexture->SetUsage(nTexture2::CreateEmpty);
            refWhiteTexture->SetType(nTexture2::TEXTURE_2D);
            refWhiteTexture->SetWidth(8);
            refWhiteTexture->SetHeight(8);
            refWhiteTexture->SetFormat(nTexture2::A1R5G5B5);
            refWhiteTexture->Load();

            struct nTexture2::LockInfo tilesurf;
            if (refWhiteTexture->Lock(nTexture2::WriteOnly, 0, tilesurf))
            {
                unsigned short *surface = (unsigned short *)tilesurf.surfPointer;
                for (unsigned int pixbyte=0; pixbyte < 8*8; pixbyte++)
                    surface[pixbyte] = 0xffff;
                refWhiteTexture->Unlock(0);
            }
        }
    }

    if ((this->quadtreeChunks == NULL) && (!this->terrainName.IsEmpty()))
    {
        // allocate terrain file if needed
        if (!this->terrainFile)
        {
            this->terrainFile = this->refFileServer->NewFileObject();
        }

        // construct some sort of resource name base for terrain chunks
        nString chunkNamebase = this->terrainName.Get();
//        chunkNamebase += "_terr"; 

        // open the terrain file
        if (!this->terrainFile->Open(chunkNamebase.Get(), "rb"))
        {
            n_error("nCLODShapeNode::LoadTerrain(): Could not open file %s\n",
                    chunkNamebase.Get());
            return false;
        }

        // read in header data
        const int headersize = 4;
        // nFile::GetS saves space for a null at the end, so we pad the buffer by one byte
        char header[headersize]; 
        this->terrainFile->Read(header, headersize); 

        short clodversion;

        // verify header data, read in tree version and depth
        if ( (header[0] == 'C') && (header[1] == 'L') && (header[2] == 'O') && (header[3] == 'D') )
        {
            short treedepth;
            clodversion = this->terrainFile->GetShort();
            treedepth = this->terrainFile->GetShort();

            n_printf("clod version found = %d\n", clodversion);
            n_printf("tree depth = %d\n", treedepth);

            n_assert(clodversion == 10 || clodversion == 11);
            n_assert(treedepth > 0);

            this->quadtreeDepth = static_cast<unsigned int>(treedepth);
        }
        else
        {
            n_error("Bad header in clod file %s\n", chunkNamebase);
            return false;
        }

        // read in the terrain x/y/z size
        float x,y,z,zscale;
        x = this->terrainFile->GetFloat();
        y = this->terrainFile->GetFloat();
        z = 0.0; // we read in z bounds from the chunk header
        zscale = this->terrainFile->GetFloat();
        vector3 center(x*0.5f,y*0.5f,z*0.5f), extents(x*0.5f,y*0.5f,z*0.5f);
        this->terrainBounds = bbox3(center,extents);

        // read in the terrain base error (error of the deepest chunk)
        this->terrainError = this->terrainFile->GetFloat();

        // figure out how many chunks we have and allocate them
        unsigned long numchunks = CalcTreeChunks(this->quadtreeDepth);
        this->quadtreeChunks = new nCLODQuadTreeNode *[numchunks];

        // setup the chunks; first setup chunk pointers
        unsigned int chunkix;
        for (chunkix=0; chunkix < numchunks; chunkix++)
        {
            if (clodversion == 10)
                this->quadtreeChunks[chunkix] = new nCLODQuadTreeNode;
            else
            {
                nCLODQuadTreeSplatNode *newsplat = new nCLODQuadTreeSplatNode;
                newsplat->InitializeSplat(this->refWhiteTexture.get());
                this->quadtreeChunks[chunkix] = newsplat;
            }
        }

        for (chunkix=0; chunkix < numchunks; chunkix++)
        {
            // chunks in the bottom tree layer contains null child pointers; the rest of the
            // chunks contain pointers to other chunks in the array; their children
            nCLODQuadTreeNode *thischunk = this->quadtreeChunks[chunkix];
            unsigned int subchunkindex = chunkix * 4 + 1;

            if (subchunkindex >= numchunks)
            {
                thischunk->InitializeChildPointers(NULL, NULL, NULL, NULL);
            }
            else
            {
                // the four children are arranged in a row further down the array
                nCLODQuadTreeNode **subchunks = this->quadtreeChunks + subchunkindex;
                thischunk->InitializeChildPointers(subchunks[0], subchunks[1], subchunks[2], subchunks[3]);
            }
        }

        this->SetLocalBox(this->terrainBounds);

        // setup chunk bounding boxes
        dSpaceID terrspace = 0;
        if (this->refTerrainSpace.isvalid())
        {
            terrspace = this->refTerrainSpace->id();
            n_assert(terrspace != 0);
        }
        DetailTextureArray *detailtex = NULL;
        if (this->detailTextures.Size() > 0)
            detailtex = & (this->detailTextures);
        this->quadtreeChunks[0]->InitializeBasics(this->terrainBounds, zscale, terrspace, detailtex);

        // setup the chunks; next we read in chunk info from the file
        for (chunkix=0; chunkix < numchunks; chunkix++)
        {
            // what chunk is next in the file?
            int filechunkindex = this->terrainFile->GetInt();

            // read in the data for that chunk
            this->quadtreeChunks[filechunkindex]->InitializeFromFile(this->terrainFile, filechunkindex, chunkNamebase);
        }

    }

    if (this->dynmesh == NULL)
        this->dynmesh = new nDynamicMesh();
    
    return true;
}

bool
nCLODShapeNode::LoadTqt()
{
    // initialize texture quadtree if appropriate.  The quadtree must already be loaded
    // based on the terrain mesh file and we must know the name of the tqt file
    if ( (this->quadtreeChunks) && (!this->tqtName.IsEmpty()) )
    {
        // allocate tqt file if needed
        if (!this->tqtFile)
        {
            this->tqtFile = this->refFileServer->NewFileObject();
        }

        // construct some sort of resource name base for terrain chunks
        nString tqtNamebase = this->tqtName.Get();

        // open the terrain file
        if (!this->tqtFile->Open(tqtNamebase.Get(), "rb"))
        {
            n_error("nCLODShapeNode::LoadTqt(): Could not open file %s\n",
                    tqtNamebase.Get());
            return false;
        }

        // read in header and verify
        // read in header data
        const int headersize = 4;
        char header[headersize]; 
        this->tqtFile->Read(header, headersize); 

        unsigned int tqtversion, tqttreedepth, tilesize;

        // verify header data, read in tree version and depth
        if ( (header[0] == 't') && (header[1] == 'q') && (header[2] == 't') && (header[3] == 0) )
        {
            tqtversion = (unsigned int)this->tqtFile->GetInt();
            tqttreedepth  = (unsigned int)this->tqtFile->GetInt();
            tilesize   = (unsigned int)this->tqtFile->GetInt();

            n_printf("tqt version found = %d\n", tqtversion);
            n_printf("tree depth = %d\n", tqttreedepth);
            n_printf("tile size = %d\n", tilesize);

            n_assert(tqtversion > 2);
            n_assert(tqttreedepth <= this->quadtreeDepth);
        }
        else
        {
            n_error("Bad header in tqt file %s\n", tqtNamebase);
            return false;
        }

        unsigned long numtqtchunks = CalcTreeChunks(tqttreedepth);

        n_assert(this->refGfx2.isvalid());

        // load up the tile info
        for (unsigned int tix=0; tix < numtqtchunks; tix++)
        {
            this->quadtreeChunks[tix]->InitializeFromTqtFile(this->tqtFile, this->refGfx2.get(), tix, tilesize, tqtNamebase);
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nCLODShapeNode::LoadResources()
{
    if (nMaterialNode::LoadResources())
    {
        if ( this->LoadTerrain() && this->LoadTqt() )
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nCLODShapeNode::UnloadResources()
{
    nMaterialNode::UnloadResources();
    this->UnloadTerrain();
}

//------------------------------------------------------------------------------
/**
    Indicate to scene server that we provide geometry
*/
bool
nCLODShapeNode::HasGeometry() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Update geometry, set as current mesh in the gfx server and
    call nGfxServer2::DrawIndexed().
*/
bool
nCLODShapeNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);
    nGfxServer2* gfx = this->refGfx2.get();


    // TODO call geometry manipulators!

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    UpdateChunks(sceneServer, renderContext);

    // draw root node if it is loaded
    nCLODQuadTreeNode *rootchunk = this->quadtreeChunks[0];
    nShader2 *terrainshader = gfx->GetShader();
    // set texture transform to drape the texture over the terrain
    n_assert(terrainshader->IsParameterUsed(nShaderState::TextureTransform0));
    n_assert(terrainshader->IsParameterUsed(nShaderState::TextureTransform1));
    matrix44 drapematrix, detailmatrix;
    vector3 terrextents(this->terrainBounds.extents());
    vector3 texturescale(0.5f/terrextents.x, 0.5f/terrextents.y, 1.0f);
    drapematrix.scale(texturescale);
    terrainshader->SetMatrix(nShaderState::TextureTransform0, drapematrix);
    detailmatrix.scale(vector3(static_cast<float>(detailScaling), static_cast<float>(detailScaling), static_cast<float>(detailScaling)));
    terrainshader->SetMatrix(nShaderState::TextureTransform1, detailmatrix);

    if (rootchunk->IsValidChunk())
    {
        rootchunk->RenderTree(gfx, terrainshader, NULL);
    }

    // remove the mesh from the gfx thingy
    gfx->SetMesh(0, 0);
    
//  RenderDebug(sceneServer, renderContext);

    return true;
}

/// Scan terrain chunks and request chunk loading/unloading as appropriate
void nCLODShapeNode::UpdateChunks(nSceneServer *sceneServer, nRenderContext *renderContext)
{
    nCLODQuadTreeNode *rootnode = this->quadtreeChunks[0];

    // get camera setting from the gfx server, and the camera position.
    // from these two items (Camera parameters and position) we can calculate
    // the apparent screen error of each terrain chunk
    nGfxServer2* gfx = this->refGfx2.get();

    // collect all the camera and view properties that will be needed
    nCamera2 const &currentcamera = gfx->GetCamera();
    nDisplayMode2 const &currentdisplaymode = gfx->GetDisplayMode();
    matrix44 const &modeltransform = sceneServer->GetModelTransform();
    matrix44 const &invmodelviewmatrix = gfx->GetTransform(nGfxServer2::InvModelView);
    vector3 camerapos = invmodelviewmatrix.pos_component();

    // calculate the closest distance for the root terrain chunk.  Each level down will
    // be allowed 1/2 the distance of the level above it
    double disp_h = currentdisplaymode.GetHeight();
    double aov = currentcamera.GetAngleOfView();
    double tan_half_aov = tan(aov*0.5 * 3.14159265/180.0);
    n_assert(tan_half_aov > 0.0);
    n_assert(disp_h > 0);
    double rooterror = this->terrainError * (1<<(this->quadtreeDepth-1));
    float mindistance = (float) ((disp_h /* * this->terrainError */ * rooterror) / (2.0 * this->screenError * tan_half_aov));
    n_assert(mindistance > 0.0);

    // walk the quadtree and load/unload chunks as needed
    if (!rootnode->IsValidChunk())
    {
        rootnode->LoadChunk();
        rootnode->RenderOn(mindistance,mindistance);
        n_sleep(0.01);
        return;
    }
    else
    {
        rootnode->UpdateTree(mindistance, camerapos);
    }

    // now cull chunks. first, calculate the view frustum planes
    //Setup clipplanes in cameraspace.
    float minx, maxx, miny, maxy, minz, maxz;    
    currentcamera.GetViewVolume(minx, maxx, miny, maxy, minz, maxz);                                                    
    vector4 clipPlanes[6];
    // The setup of the planes here are a bit mucky:
    //      0 - front
    //      1 - back
    //      2 - left
    //      3 - right
    //      4 - top
    //      5 - bottom
    float angle_h = atanf(maxx / minz);
    float cos_h = cosf(angle_h);
    float sin_h = sinf(angle_h);
    float angle_v = atanf(maxy / minz);
    float cos_v = cosf(angle_v);
    float sin_v = sinf(angle_v);        

    // D = - dot(p.normal, point_in_plane)
    clipPlanes[0].set(0.0f, 0.0f, -1.0f, 0.0f);
    clipPlanes[0].w = -( -1.0f * -minz);
    clipPlanes[1].set(0.0f, 0.0f, 1.0f, 0.0f);
    clipPlanes[1].w = -( 1.0f * -maxz);
    clipPlanes[2].set(cos_h, 0.0f, -sin_h, 0.0f);
    clipPlanes[3].set(-cos_h, 0.0f, -sin_h, 0.0f);
    clipPlanes[4].set(0.0f, -cos_v, -sin_v, 0.0f);
    clipPlanes[5].set(0.0f, cos_v, -sin_v, 0.0f);
                    
    //Transform clipplanes from cameraspace to 
    //(terrain) objectspace.  The plane/vector4 representation requires that we
    // do this via transpose() instread of invert_simple()
    matrix44 inv_viewer = gfx->GetTransform(nGfxServer2::ModelView);
    inv_viewer.transpose();
    
    // Would have used plane but vector4 can be transformed
    plane cullplanes[6];
    for (int i = 0; i < 6; ++i)
    {
        clipPlanes[i] = inv_viewer * clipPlanes[i];
        cullplanes[i].a = clipPlanes[i].x;
        cullplanes[i].b = clipPlanes[i].y;
        cullplanes[i].c = clipPlanes[i].z;
        cullplanes[i].d = clipPlanes[i].w;
    }
    
    // now cull the nodes
//  n_printf("culling terrain quadtree\n");
    nCLODQuadTreeNode::result_info clipstatus;  // default construction is "not clipped by any frustum plane"
    rootnode->Cull(clipstatus, cullplanes);
}

/// set the NOH path to the opende space for terrain collision geoms; creates an nOpendeSpace if needed
void nCLODShapeNode::SetCollisionSpace(const char *collisionSpacePath)
{
    this->UnloadTerrain();

    // create the space if needed
    if (!kernelServer->Lookup(collisionSpacePath))
    {
        kernelServer->New("nopendesimplespace",collisionSpacePath);
        this->refTerrainSpace = collisionSpacePath;
        this->refTerrainSpace->Create("none");
    }
    else
        this->refTerrainSpace = collisionSpacePath;

    n_assert(this->refTerrainSpace.isvalid());
}
    
/// get the current NOH path to the collision space used
const char *nCLODShapeNode::GetCollisionSpace()
{
    if (this->refTerrainSpace.isvalid())
        return this->refTerrainSpace.getname();
    else
        return NULL;
}
    

/// render some chunk bounding boxes for debugging
void nCLODShapeNode::RenderDebug(nSceneServer *sceneServer, nRenderContext *renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);
    nGfxServer2* gfx = this->refGfx2.get();

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
    if (!this->dynmesh->IsValid())
    {
        this->dynmesh->Initialize(nGfxServer2::LineList, nMesh2::Coord, 0, true);
    }
    n_assert(this->dynmesh->IsValid());

    // draw bounding box around the root node
    nDynamicMesh *dmesh = this->dynmesh;

    float *vp;
    int vcount, icount;
    unsigned short *ip;
    dmesh->BeginIndexed(vp, ip, vcount, icount);
    bbox3 rootbox = this->quadtreeChunks[0]->GetBounds();
    if ( (vcount > 8) && (icount > 12) )
    {
        for (int vindex=0; vindex < 8; vindex++)
        {
            float *curvertex = vp + vindex * 3;
            curvertex[0] = rootbox.vmin.x + (rootbox.vmax.x-rootbox.vmin.x) * (vindex & 1);
            curvertex[1] = rootbox.vmin.y + (rootbox.vmax.y-rootbox.vmin.y) * (vindex & 2)/2;
            curvertex[2] = rootbox.vmin.z + (rootbox.vmax.z-rootbox.vmin.z) * (vindex & 4)/4;
        }

        unsigned short indices[24] = { 0,1, 1,3, 3,2, 2,0, 0,4, 1,5, 2,6, 3,7, 4,5, 5,7, 7,6, 6,4 };
        for (int iindex=0; iindex < 24; iindex++)
            ip[iindex] = indices[iindex];
        dmesh->EndIndexed(8,24);
    }
    else
        dmesh->EndIndexed(0,0);
}



//------------------------------------------------------------------------------
/**
    Set the resource name. The mesh resource name consists of the
    filename of the mesh, and optionally a mesh group name, delimited by '|'.
    If no group name exists, the first group in the mesh file will be used.
*/
void
nCLODShapeNode::SetTerrainName(const char* name)
{
    n_assert(name);
    this->UnloadTerrain();

    this->terrainName = name;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nCLODShapeNode::GetTerrainName() const
{
    return this->terrainName.IsEmpty() ? 0 : this->terrainName.Get();
}

void
nCLODShapeNode::SetTqtName(const char* name)
{
    n_assert(name);
    this->UnloadTerrain();

    this->tqtName = name;
}

/// get the tqt resource name
const char *nCLODShapeNode::GetTqtName() const
{
    return this->tqtName.IsEmpty() ? 0 : this->tqtName.Get();
}

/// start specifying detail textures
void nCLODShapeNode::BeginDetailTextures(unsigned int numdetailtextures)
{
    // trash the current textures
    detailTextures.Clear();
    detailTextures.SetFixedSize(numdetailtextures);
}

/// specify a detail texture
void nCLODShapeNode::SetDetailTexture(unsigned int detailtextureindex, const char *detailtexturename)
{
    n_assert( (detailtextureindex >= 0) && (detailtextureindex < (unsigned int)detailTextures.Size()) );
    nTexture2 *thisdetailtexture = refGfx2->NewTexture(detailtexturename);
    thisdetailtexture->SetFilename(detailtexturename);
    detailTextures[detailtextureindex] = thisdetailtexture;
}

/// finish detail textures
void nCLODShapeNode::EndDetailTextures()
{
    // we need textures in every slot!
    for (int tix=0; tix < detailTextures.Size(); tix++)
    {
        n_assert(detailTextures[tix].isvalid());
        detailTextures[tix]->Load();
    }
}


/**
 * @brief Set scaling of the detail image
 *
 * All detail images are mapped via texture generation, where a vertex (x,y) coordinate is mapped directly into
 * a (u,v) coordinate.  If you have a large distance between points of your heightfield, this can result in a high
 * repetition of the detail texture and very obvious tiling.  If you want to strech out the detail textures to avoid a
 * tiled appearance, reduce this number to 0.1 or maybe 0.01
 */
void nCLODShapeNode::SetDetailScale(double detailscale)
{
    this->detailScaling = detailscale;
}

/// Get current scaling of the detail images
double nCLODShapeNode::GetDetailScale() const
{
    return this->detailScaling;
}

//------------------------------------------------------------------------------
/**
    Set the resource loader used to load the terrain data.  If this is NULL, then
    the default mesh loader will be instantiated automatically.
*/
void
nCLODShapeNode::SetTerrainResourceLoader(const char* resourceLoaderPath)
{
    this->refTerrainResourceLoader = resourceLoaderPath;
}

//------------------------------------------------------------------------------
/**
    Get the terrain resource loader.
    
    @return resource loader name or null when there is no resource loader
*/
const char *
nCLODShapeNode::GetTerrainResourceLoader()
{
    if (this->refTerrainResourceLoader.isvalid())
        return this->refTerrainResourceLoader.getname();
    else
        return 0;
}

/// Get the screen space error for a particular chunk
double nCLODShapeNode::GetChunkError(int chunkindex) const
{
    return 1.0;
}


