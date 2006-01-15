//------------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#ifndef N_MAPBLOCK_H
#define N_MAPBLOCK_H
//------------------------------------------------------------------------------
/**
    @class MapBlock
    @ingroup NMapContribModule
    @brief Wrapper for a single block on the terrain.

    The terrain data itself is kept on the parent nMapNode object.

    @todo The edge rendering code is particularly messy and would benefit from
    cleanup.

    (C) 2002    ling
*/
#include "map/bbox_qs.h"
#include "scene/nsceneserver.h"
#include "map/nmapnode.h"
#include "gfx2/ngfxserver2.h"

//---------------------------------------------------------------------------
class nGfxServer2;
class nIndexGraph;
class nSceneGraph2;
class nVertexBuffer;
class MapQuadElement;

class MapBlock
{
public:
    /// Neighouring blocks, north, south, west, east
    enum Neighbor
    {
        NORTH,
        SOUTH,
        WEST,
        EAST,
    };

    /// constructor
    MapBlock(nMapNode* map);
    /// destructor
    ~MapBlock();

    void Init(const char* resourceLoader, nGfxServer2* gfx_server, int num, int x, int z);

    void CalculateMinD2Levels(float c2);

    void SelectMipMapLevel(const vector3& camera_pos);
    bool AlignMipMapLevel();
    void Render(nSceneServer * scene_graph);

    void SetVisible(bool set);
    bool GetVisible() const;

    const bbox3& GetBoundingBox() const;

    void SetNeighbor(Neighbor direction, MapBlock* neighbor);
    MapBlock* GetNeighbor(Neighbor direction);

    MapQuadElement* GetQuadElement();

private:
    const char* GenerateResourceLoaderString();
    float BilinearInterpolate(float*, float, float) const;
    int MapVertexToIndex(int x, int z) const;

    void BeginRender();
    void EndRender(nGfxServer2::PrimitiveType primType);
    void RenderTriangle(int a, int b, int c);

    void RenderNorthEdge(bool, bool);
    void RenderSouthEdge(bool, bool);
    void RenderWestEdge(bool, bool);
    void RenderEastEdge(bool, bool);

    /// Owner
    nMapNode* map;

    /// Starting X index
    int startX;
    /// Starting Z index
    int startZ;

    /// Size of block
    int blockSize;

    /// Array of neighbouring blocks
    MapBlock* neighborBlock[4];

    // Array of minimum distance squared where mipmap would be used
    float* minD2;

    /// Block bounds
    bbox3_qs boundingBox;

    /// Visibility
    bool isVisible;
    /// Current mip map level
    int currentLevel;
    /// Current render step
    int currentStep;

    /// Associated quadtree element
    MapQuadElement* quadElement;

    // Triangle strip
    nRef<nMesh2> meshTriStrip;
    ushort * curIndexBuffer;
    /// Current vertex index
    int curVertex;

};

//---------------------------------------------------------------------------
/**
*/
inline
void
MapBlock::RenderTriangle(int a, int b, int c)
{
    curIndexBuffer[curVertex++] = a;
    curIndexBuffer[curVertex++] = b;
    curIndexBuffer[curVertex++] = c;
}

//---------------------------------------------------------------------------
/**
    @brief Select appropriate mip map level given camera position
    Also sets visible to true.
*/
inline 
void 
MapBlock::SelectMipMapLevel(const vector3& camera_pos)
{
    isVisible = true;

    // Get distance from camera to terrain block centre
    float dx = boundingBox.xm - camera_pos.x;
    float dy = boundingBox.ym - camera_pos.y;
    float dz = boundingBox.zm - camera_pos.z;
    float dist_sqr = dx*dx + dy*dy + dz*dz;

    // Iterate and select appropriate level, going backwards, probably more calculations
    currentLevel = map->GetNumMipMapLevels()-1;
    while (minD2[currentLevel] > dist_sqr)
        --currentLevel;
}

//---------------------------------------------------------------------------
/**
    @brief Align mip map level with neighbours.
    @return True if the block had to align.
*/
inline 
bool 
MapBlock::AlignMipMapLevel()
{
    bool align = false;

    for (int i = 0; i < 4; ++i)
    {
        MapBlock* neighbor = neighborBlock[i];
        if (NULL == neighbor || false == neighbor->isVisible)
            continue;

        if (neighbor->currentLevel+1 < currentLevel)
        {
            currentLevel = neighbor->currentLevel+1;
            align = true;
        }
    }

    return align;
}

//---------------------------------------------------------------------------
/**
*/
inline 
void 
MapBlock::SetVisible(bool set)
{
    isVisible = set;
}

//---------------------------------------------------------------------------
/**
*/
inline 
bool 
MapBlock::GetVisible() const
{
    return isVisible;
}

//---------------------------------------------------------------------------
/**
*/
inline 
const bbox3& 
MapBlock::GetBoundingBox() const
{
    return boundingBox;
}

//---------------------------------------------------------------------------
/**
*/
inline
void
MapBlock::SetNeighbor(MapBlock::Neighbor direction, MapBlock* neighbor)
{
    neighborBlock[direction] = neighbor;
}

//---------------------------------------------------------------------------
/**
*/
inline MapBlock* MapBlock::GetNeighbor(MapBlock::Neighbor direction)
{
    return neighborBlock[direction];
}

//---------------------------------------------------------------------------
/**
*/
inline
int
MapBlock::MapVertexToIndex(int x, int z) const
{
    return x + z * blockSize;
}

//---------------------------------------------------------------------------
/**
    @brief Begin data input to index buffer.
*/
inline
void
MapBlock::BeginRender()
{
    n_assert(NULL == curIndexBuffer);
    curIndexBuffer = meshTriStrip->LockIndices();
    curVertex = 0;
}

//---------------------------------------------------------------------------
/**
    @brief Render index buffer.
*/
inline
void
MapBlock::EndRender(nGfxServer2::PrimitiveType primType)
{
    n_assert(NULL != curIndexBuffer);
    meshTriStrip->UnlockIndices();

    if (curVertex > 2)
    {
        map->refGfxServer->SetMesh(meshTriStrip, meshTriStrip);
        map->refGfxServer->SetVertexRange(0, meshTriStrip->GetNumVertices());
        map->refGfxServer->SetIndexRange(0, meshTriStrip->GetNumIndices());
        map->refGfxServer->DrawIndexedNS(primType);
    }

    curIndexBuffer = NULL;
    curVertex = 0;
}

//---------------------------------------------------------------------------

#endif
