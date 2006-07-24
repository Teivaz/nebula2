//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "map/nmap.h"
#include "map/mapquadtree.h"
#include "map/mapblock.h"
#include "map/nmapresourceloader.h"
#include <strstream>
/**
    Automatically infer map position and obtain centre and all that.
*/
MapBlock::MapBlock(nMapNode* m) :
    map(m),
    blockSize(map->GetBlockSize()),
    minD2(n_new_array(float, map->GetNumMipMapLevels())),
    boundingBox(),
    isVisible(false),
    currentLevel(0),
    currentStep(1),
    quadElement(0),
    curIndexBuffer(0),
    curVertex(0)
{
    memset(neighborBlock, 0, 4 * sizeof(MapBlock*));
}

MapBlock::~MapBlock()
{
    if (meshTriStrip.isvalid())
    {
        meshTriStrip->Release();
        meshTriStrip.invalidate();
    }

    // Assume someone else has removed it for us
    if (quadElement)
    {
        n_delete(quadElement);
    }

    if (minD2)
    {
        n_delete_array(minD2);
    }
}

/**
    @brief Initialise vertex buffer
*/
void
MapBlock::Init(const char* resourceLoader, nGfxServer2 * gfx_server, int num, int x, int z)
{
    this->startX = x * (blockSize - 1);
    this->startZ = z * (blockSize - 1);

    nMap* map_data = map->GetMap();

    // Set up bounding box
    boundingBox.set(map_data->GetPoint(this->startX, this->startZ).coord,
                    map_data->GetPoint(this->startX, this->startZ).coord);
    for (int loopZ = 0; loopZ < map->GetBlockSize(); ++loopZ)
    {
        for (int loopX = 0; loopX < map->GetBlockSize(); ++loopX)
        {
            boundingBox.grow(map_data->GetPoint(this->startX + loopX, this->startZ + loopZ).coord);
        }
    }

    // Create a mesh for each node for triangle strips
    n_assert( !meshTriStrip.isvalid() );
    meshTriStrip = map->refGfxServer->NewMesh(0);
    n_assert(meshTriStrip.isvalid());
    meshTriStrip->SetVertexComponents(
        nMesh2::Coord |
        nMesh2::Normal |
        nMesh2::Uv0 |
        nMesh2::Uv1 );
    meshTriStrip->SetUsage( map->GetMeshUsage() );
    int num_vertices = map->GetBlockSize() * map->GetBlockSize();
    meshTriStrip->SetNumVertices(num_vertices);
    // indices needed by the patch
    int num_indices = 6 * map->GetBlockSize() * map->GetBlockSize();
    // indices needed by the crack fixes
    num_indices += (map->GetBlockSize()-1) * (map->GetBlockSize()-1) * 4;
    meshTriStrip->SetNumIndices(num_indices);

    meshTriStrip->SetResourceLoader( resourceLoader );
    meshTriStrip->SetFilename( GenerateResourceLoaderString() );
    meshTriStrip->Load();
}

const char*
MapBlock::GenerateResourceLoaderString()
{
    nString fullName = map->GetFullName();
    std::strstream resourceLoaderStr;
    resourceLoaderStr << fullName.Get()
                      << nMapResourceLoader::SEPARATOR
                      << this->startX
                      << nMapResourceLoader::SEPARATOR
                      << this->startZ
                      << '\0';
    return resourceLoaderStr.str();
}

/**
    Calculates the minimum distance squared (dMin^2) for each mip map
    level in this terrain block.

    For each mip map level, it takes the greatest delta from that level
    to the missing vertices.
*/
void
MapBlock::CalculateMinD2Levels(float c2)
{
    nMap* map_data = map->GetMap();
    // d for level 0 is always 0
    minD2[0] = 0;

    float max_delta = 0;

    float heights[4];

    // Loop thru mip map levels
    for (int level = 1; level < map->GetNumMipMapLevels(); ++level)
    {
        int step = 1 << level;

        // Loop thru quads
        for (int j = this->startZ; j < this->startZ + map->GetBlockSize()-1; j += step)
        {
            for (int i = this->startX; i < this->startX + map->GetBlockSize()-1; i += step)
            {
                heights[0] = map_data->GetPoint(i,        j).coord.y;
                heights[1] = map_data->GetPoint(i,        j + step).coord.y;
                heights[2] = map_data->GetPoint(i + step, j).coord.y;
                heights[3] = map_data->GetPoint(i + step, j + step).coord.y;

                // For each quad, work out maximum delta
                for (int z = 1; z < step; ++z)
                {
                    for (int x = 1; x < step; ++x)
                    {
                        float interp_height = BilinearInterpolate(heights, float(x)/step, float(x)/step);
                        float height = map_data->GetPoint(i + x, j + z).coord.y;
                        float delta = fabsf(height - interp_height);
                        max_delta = n_max(delta, max_delta);
                    }
                }
            }
        }

        // Keep max delta between mip map levels to ensure it increases
        minD2[level] = max_delta*max_delta * c2;
    }
}

/**
    Obtain the interpolated height given four heights, where
    h[0] = top left
    h[1] = top right
    h[2] = bottom left
    h[3] = bottom right
*/
float MapBlock::BilinearInterpolate(float* h, float x, float z) const
{
    float top = h[0]*(1.0f - x) + x*h[1];
    float bottom = h[2]*(1.0f - x) + x*h[3];

    return top*(1.0f - z) + z*bottom;
}

/**
    Render block
*/
void
MapBlock::Render(nSceneServer * scene_graph)
{
    BeginRender();
    ushort * pIndex = curIndexBuffer;

    currentStep = 1 << currentLevel;

    // Check which neighbours require aligning,
    // could probably optimise this chunk to be obscure
    bool align_north = NULL != neighborBlock[NORTH] && neighborBlock[NORTH]->isVisible && neighborBlock[NORTH]->currentLevel > currentLevel;
    bool align_south = NULL != neighborBlock[SOUTH] && neighborBlock[SOUTH]->isVisible && neighborBlock[SOUTH]->currentLevel > currentLevel;
    bool align_west = NULL != neighborBlock[WEST] && neighborBlock[WEST]->isVisible && neighborBlock[WEST]->currentLevel > currentLevel;
    bool align_east = NULL != neighborBlock[EAST] && neighborBlock[EAST]->isVisible && neighborBlock[EAST]->currentLevel > currentLevel;

    int start_x = 0;
    int end_x =  blockSize-1;
    int start_z = 0;
    int end_z = blockSize-1;

    if (true == align_north)
    {
        start_z += currentStep;
    }
    if (true == align_south)
    {
        end_z -= currentStep;
    }
    if (true == align_west)
    {
        start_x += currentStep;
    }
    if (true == align_east)
    {
        end_x -= currentStep;
    }

    // Strip diagonals towards se, i.e. trailing diagonals
    bool downwards = true;
    for (int x = start_x; x < end_x; x += currentStep)
    {
        // Walk down
        if (true == downwards)
        {
            for (int z = start_z; z <= end_z; z += currentStep)
            {
                pIndex[curVertex++] = MapVertexToIndex(x+currentStep, z);
                pIndex[curVertex++] = MapVertexToIndex(x, z);
            }
        }
        // Walk up, add two degenerate triangles at start and each of strip
        // to generate correctly ordered vertices
        else
        {
            pIndex[curVertex++] = MapVertexToIndex(x, end_z);
            pIndex[curVertex++] = MapVertexToIndex(x, end_z);
            for (int z = end_z; z >= start_z; z -= currentStep)
            {
                pIndex[curVertex++] = MapVertexToIndex(x, z);
                pIndex[curVertex++] = MapVertexToIndex(x+currentStep, z);
            }
            pIndex[curVertex++] = MapVertexToIndex(x+currentStep, start_z);
            pIndex[curVertex++] = MapVertexToIndex(x+currentStep, start_z);
        }

        downwards = !downwards;
    }
    EndRender(nGfxServer2::TriangleStrip);

    BeginRender();
    if (true == align_west)
    {
        RenderWestEdge(align_north, align_south);
    }
    if (true == align_east)
    {
        RenderEastEdge(align_north, align_south);
    }
    if (true == align_north)
    {
        RenderNorthEdge(align_west, align_east);
    }
    if (true == align_south)
    {
        RenderSouthEdge(align_west, align_east);
    }
    EndRender(nGfxServer2::TriangleList);
}

/**
    Render triangle fans starting from northwest corners
*/
void
MapBlock::RenderNorthEdge(bool align_west, bool align_east)
{
    int vertices[5];

    int dbl_step = currentStep << 1;

    int z = 0;
    int end_x =  map->GetBlockSize()-1;
    int last_x = end_x - dbl_step;

    for (int x = 0; x < end_x; x += dbl_step)
    {
        vertices[0] = MapVertexToIndex(x, z);
        vertices[1] = MapVertexToIndex(x, z + currentStep);
        vertices[2] = MapVertexToIndex(x + currentStep, z + currentStep);
        vertices[3] = MapVertexToIndex(x + dbl_step, z + currentStep);
        vertices[4] = MapVertexToIndex(x + dbl_step, z);

        // Render weird blob
        if (true == align_east && last_x == x)
        {
            RenderTriangle(vertices[0], vertices[2], vertices[4]);
        }
        // Or normal
        else
        {
            RenderTriangle(vertices[0], vertices[3], vertices[4]);
            RenderTriangle(vertices[0], vertices[2], vertices[3]);
        }

        // Omit last triangle
        if (true == align_west && 0 == x)
        {
            continue;
        }

        RenderTriangle(vertices[0], vertices[1], vertices[2]);
    }
}

/**
    Render triangle fans starting from southwest corners
*/
void
MapBlock::RenderSouthEdge(bool align_west, bool align_east)
{
    int vertices[5];

    int dbl_step = currentStep << 1;

    int z = map->GetBlockSize()-1 - currentStep;
    int end_x =  map->GetBlockSize()-1;
    int last_x = end_x - dbl_step;

    for (int x = 0; x < end_x; x += dbl_step)
    {
        vertices[0] = MapVertexToIndex(x + dbl_step, z + currentStep);
        vertices[1] = MapVertexToIndex(x + dbl_step, z);
        vertices[2] = MapVertexToIndex(x + currentStep, z);
        vertices[3] = MapVertexToIndex(x, z);
        vertices[4] = MapVertexToIndex(x, z + currentStep);

        // Render weird blob
        if (true == align_west && 0 == x)
        {
            RenderTriangle(vertices[0], vertices[2], vertices[4]);
        }
        // Or normal
        else
        {
            RenderTriangle(vertices[0], vertices[3], vertices[4]);
            RenderTriangle(vertices[0], vertices[2], vertices[3]);
        }

        // Omit last triangle
        if (true == align_east && last_x == x)
        {
            continue;
        }

        RenderTriangle(vertices[0], vertices[1], vertices[2]);
    }
}

/**
    Render triangle fans starting from northwest corners
*/
void
MapBlock::RenderWestEdge(bool align_north, bool align_south)
{
    int vertices[5];

    int west_step = currentStep << 1;

    int x = 0;
    int end_z = map->GetBlockSize()-1;
    int last_z = end_z - west_step;

    for (int z = 0; z < end_z; z += west_step)
    {
        vertices[0] = MapVertexToIndex(x, z);
        vertices[1] = MapVertexToIndex(x, z + west_step);
        vertices[2] = MapVertexToIndex(x + currentStep, z + west_step);
        vertices[3] = MapVertexToIndex(x + currentStep, z + currentStep);
        vertices[4] = MapVertexToIndex(x + currentStep, z);

        // Render weird blob
        if (true == align_south && last_z == z)
        {
            RenderTriangle(vertices[0], vertices[1], vertices[3]);
        }
        // Or normal
        else
        {
            RenderTriangle(vertices[0], vertices[1], vertices[2]);
            RenderTriangle(vertices[0], vertices[2], vertices[3]);
        }

        // Omit last triangle
        if (true == align_north && 0 == z)
        {
            continue;
        }

        RenderTriangle(vertices[0], vertices[3], vertices[4]);
    }
}

/**
    Render triangle fans starting from southeast corners
*/
void
MapBlock::RenderEastEdge(bool align_north, bool align_south)
{
    int vertices[5];

    int east_step = currentStep << 1;

    int x =  map->GetBlockSize()-1 - currentStep;
    int end_z = map->GetBlockSize()-1;
    int last_z = end_z - east_step;

    for (int z = 0; z < end_z; z += east_step)
    {
        vertices[0] = MapVertexToIndex(x + currentStep, z + east_step);
        vertices[1] = MapVertexToIndex(x + currentStep, z);
        vertices[2] = MapVertexToIndex(x, z);
        vertices[3] = MapVertexToIndex(x, z + currentStep);
        vertices[4] = MapVertexToIndex(x, z + east_step);

        // Render weird blob
        if (true == align_north && 0 == z)
        {
            RenderTriangle(vertices[0], vertices[1], vertices[3]);
        }
        // Or normal
        else
        {
            RenderTriangle(vertices[0], vertices[1], vertices[2]);
            RenderTriangle(vertices[0], vertices[2], vertices[3]);
        }

        // Omit last triangle
        if (true == align_south && last_z == z)
        {
            continue;
        }

        RenderTriangle(vertices[0], vertices[3], vertices[4]);
    }
}

MapQuadElement*
MapBlock::GetQuadElement()
{
    if (!quadElement)
    {
        quadElement = n_new(MapQuadElement(this));
    }

    return quadElement;
}
