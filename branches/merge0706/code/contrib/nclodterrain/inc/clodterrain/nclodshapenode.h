#ifndef N_CLODSHAPENODE_H
#define N_CLODSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nCLODShapeNode
    @ingroup NCLODTerrainContribModule
    @brief Handles loading and rendering of ChunkedLOD terrain chunks
*/
#include "scene/nmaterialnode.h"
#include "gfx2/nmesh2.h"
#include "kernel/ndynautoref.h"
#include "gfx2/ndynamicmesh.h"
#include "mathlib/bbox.h"

#include "opende/nopendespace.h"

class nCLODQuadTreeNode; // for managing terrain chunks

//------------------------------------------------------------------------------
class nCLODShapeNode : public nMaterialNode
{
public:
    /// constructor
    nCLODShapeNode();
    /// destructor
    virtual ~nCLODShapeNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// indicate to scene server that we offer geometry for rendering
    virtual bool HasGeometry() const;
    /// render geometry
    virtual bool RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext);

    /// render some chunk bounding boxes for debugging
    void RenderDebug(nSceneServer *sceneServer, nRenderContext *renderContext);

    /// Scan terrain chunks and request chunk loading/unloading as appropriate, also culls chunks
    virtual void UpdateChunks(nSceneServer *sceneServer, nRenderContext *renderContext);

    /// get the mesh usage flags required by this shape node
    virtual int GetMeshUsage() const;

    /// Set the allowed mesh display error in pixels
    void SetScreenSpaceError(double spaceerror);

    /// Get the allowed mesh display error in pixels
    double GetScreenSpaceError() const;

    /// Get the screen space error for a particular chunk
    double GetChunkError(int chunkindex) const;

    /// set the terrain resource name
    void SetTerrainName(const char* name);

    /// get the terrain resource name
    const char* GetTerrainName() const;

    /// set the tqt resource name
    void SetTqtName(const char* name);

    /// get the tqt resource name
    const char *GetTqtName() const;

    /// start specifying detail textures
    void BeginDetailTextures(unsigned int numdetailtextures);

    /// specify a detail texture
    void SetDetailTexture(unsigned int detailtextureindex, const char *detailtexturename);

    /// finish detail textures
    void EndDetailTextures();

    /**
     * @brief Set scaling of the detail image
     *
     * All detail images are mapped via texture generation, where a vertex (x,y) coordinate is mapped directly into
     * a (u,v) coordinate.  If you have a large distance between points of your heightfield, this can result in a high
     * repetition of the detail texture and very obvious tiling.  If you want to strech out the detail textures to avoid a
     * tiled appearance, reduce this number to 0.1 or maybe 0.01
     */
    void SetDetailScale(double detailscale);

    /// Get current scaling of the detail images
    double GetDetailScale() const;

    /// set the NOH path to the opende space for terrain collision geoms; creates an nOpendeSpace if needed
    void SetCollisionSpace(const char *collisionSpacePath);

    /// get the current NOH path to the collision space used
    const char *GetCollisionSpace();
    
    /// set the NOH path to the terrain's resource loader
    void SetTerrainResourceLoader(const char* resourceLoaderPath);
    
    /// get the mesh's resource loader
    const char* GetTerrainResourceLoader();

    typedef nArray<nDynAutoRef<nTexture2> > DetailTextureArray;

protected:
    /// load terrain mesh resource
    bool LoadTerrain();
    /// load texture quadtree
    bool LoadTqt();

    /// unload terrain resources (both mesh and texture quadtree)
    void UnloadTerrain();


    nString terrainName;
    nString tqtName;
    nDynAutoRef<nResourceLoader> refTerrainResourceLoader;
    nAutoRef<nFileServer2> refFileServer;
    nAutoRef<nGfxServer2> refGfx2;

    // an array of chunks--the quadtree mapping embedded in the chunks
    nCLODQuadTreeNode **quadtreeChunks;
    unsigned int quadtreeDepth;

    // an array of detail textures, used for splatting
    DetailTextureArray detailTextures;

    // open file to terrain--needed since the chunks can't open/close files in another thread
    nFile *terrainFile;

    // open file to texture quadtree file--needed since the chunks can't open/close files in another thread
    nFile *tqtFile;

    nDynamicMesh *dynmesh; // for rendering debug visualization

    // bounding box of the whole terrain
    bbox3 terrainBounds;

    // screen space error
    double screenError;

    // terrain base error (max error of the root chunk)
    float terrainError;

    // reference to the space containing geoms of the terrain.  We get our own space so that
    // the terrain chunks don't try to collide with each other
    nDynAutoRef<nOpendeSpace> refTerrainSpace;

    nRef<nTexture2> refWhiteTexture; // white texture used for base texture writing

    // scaling use on detail textures
    double detailScaling;
};

#endif

