//------------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#ifndef N_MAPNODE_H
#define N_MAPNODE_H
//------------------------------------------------------------------------------
/**
    @class nMapNode
    @ingroup NMapContribModule
    @brief Implementation of terrain.

    This particular implementation is designed to be not transformed.

    It implements block LOD using geo mipmapping as outlined by Willem Boer.

    @todo Possible detail to do in the future is to create a composite texture
    node.  This texture node would load a very large texture and break down
    into smaller patches instead of using one enormous texture which is only
    partially displayed.  This might share code with similar functionality
    in nCTerrain.

    Known bugs:
    
    @bug The detail texture appears to go purple at a certain view distance
    and/or angle.

    @bug In OpenGL, it appears to not clean up after itself properly and leaves
    the pixel shader in a state which affects the next dynamic vertex buffer
    user.

    @bug Not more than one of these nodes can be rendered at a time.

    (C) 2002    ling
*/

#include "scene/nscenenode.h"
#include "scene/nshapenode.h"
#include "kernel/ndynautoref.h"

//---------------------------------------------------------------------------
class nIndexBuffer;
class nVertexBuffer;
class nMap;
class MapBlock;
class MapQuadtree;

class nMapNode : public nMaterialNode
{
public:
    /// constructor
    nMapNode();
    /// destructor
    virtual ~nMapNode();
    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);
    /// Preload resources
    virtual bool LoadResources();

    virtual bool HasGeometry() const;
    /// update internal state and render
    virtual bool RenderGeometry(nSceneServer * scene_graph, nRenderContext * renderContext);

    /// get the mesh usage flags required by this node
    int GetMeshUsage() const;
    /// override the default mesh usage for this node
    void SetMeshUsage(int);

    /// sets the identifier of an nMapResourceLoader to be used by MapBlocks
    void SetResourceLoader( const char* );
    /// gets the identifier of an nMapResourceLoader to be used by MapBlocks
    const char* GetResourceLoader() const;

    void SetMapPath(const char*);
    const char* GetMapPath();
    nMap* GetMap();

    void SetBlockSize(int size);
    int GetBlockSize() const;
    void SetError(int error);
    int GetError() const;
    /// Set number of times to repeat detail texture
    virtual void SetDetailSize(float);
    /// Get number of times detail texture is repeated
    virtual float GetDetailSize();
    int GetNumMipMapLevels() const;
    float GetDetailScale() const;

    /// Occlusion debug method
    void SetUpdateOcclusion(bool set);
    /// Debug method to visualise what is going on
    virtual int Visualize();

    nAutoRef<nGfxServer2> refGfxServer;

private:
    // Map loading methods
    void ProcessMap();
    void LinkBlocks();
    void CalculateMinD2Levels();
    float CalculateCFactor();
    void DeleteBlocks();

    /// Map data source
    nAutoRef<nMap> refMap;
    /// Block size
    int blockSize;
    /// Pixel error
    int pixelError;
    /// Detail texture size
    float detailSize;
    /// Calculated detail texture scaling
    float detailScale;

    /// Current data is processed, false is parameters have changed
    bool isDirty;
    /// Number of levels
    int numMipMapLevels;
    /// Number of blocks along an axis
    int numBlocks;
    /// List of map blocks
    MapBlock*** blockArray;
    /// Occlusion quadtree
    MapQuadtree* mapQuadtree;
    /// Debug method
    bool renderDebug;
    /// used for nMesh2::SetUsage:
    int meshUsage;
    /// the identifier of an nMapResourceLoader to be used by MapBlocks
    const char* resourceLoader;
};

//---------------------------------------------------------------------------
/**
    @brief Get the map data source.
*/
inline
nMap*
nMapNode::GetMap()
{
    return refMap;
}

//---------------------------------------------------------------------------
/**
    @brief Get the terrain block size.
    @return The grid spacing in metres.
*/
inline
int
nMapNode::GetBlockSize() const
{
    return blockSize;
}

//---------------------------------------------------------------------------
/**
    @brief Get calculated number of mip map levels
*/
inline
int
nMapNode::GetNumMipMapLevels() const
{
    return numMipMapLevels;
}

//---------------------------------------------------------------------------
/**
    @brief Get calculated detail scale multiplier.
*/
inline
float
nMapNode::GetDetailScale() const
{
    return detailScale;
}

//---------------------------------------------------------------------------

#endif
