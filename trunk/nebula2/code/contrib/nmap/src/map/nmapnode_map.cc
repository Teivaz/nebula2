#define N_IMPLEMENTS nMap
//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo.
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "map/mapquadtree.h"
#include "map/mapblock.h"
#include "map/nmap.h"
#include "map/nmapnode.h"

/**
    Initialise map vertex buffer.
*/
void nMapNode::ProcessMap()
{
    n_assert(true == isDirty);
    n_assert(true == refMap.isvalid());

    // Get or derive constants
    nMap* map = refMap.get();
    map->LoadMap();
    int map_size = map->GetDimension();

    // Figure out detail scale from detail size
    detailScale = map_size * map->GetGridInterval() / detailSize;

    // Dodgy version of doing log(blockSize)/log(2) using logical shift,
    // could not help it so shoot me
    numMipMapLevels = 1;
    int shifter = blockSize-1;
    while (0 == (shifter & 0x01))
    {
        shifter >>= 1;
        ++numMipMapLevels;
    }

    // Initialise octree
    if (NULL == mapQuadtree)
        mapQuadtree = n_new MapQuadtree(refMap.get());
    // Remove existing
    else
    {
        if (NULL != blockArray)
        {
            for (int j = 0; j < numBlocks; ++j)
            {
                for (int i = 0; i < numBlocks; ++i)
                {
                    MapBlock* block = blockArray[j][i];
                    MapQuadElement* oct_elm = block->GetQuadElement();
                    mapQuadtree->RemElement(oct_elm);

                    n_delete block;
                }

                n_delete[] blockArray[j];
            }
            n_delete[] blockArray;
        }
    }

    // Create blocks
    numBlocks = (map_size - 1)/(blockSize - 1);
    blockArray = n_new MapBlock**[numBlocks];
    for (int j = 0; j < numBlocks; ++j)
    {
        blockArray[j] = n_new MapBlock*[numBlocks];

        for (int i = 0; i < numBlocks; ++i)
        {
            MapBlock* block = n_new MapBlock(this);
            block->Init(refGfxServer.get(), i + j*numBlocks, i, j);

            MapQuadElement* oct_elm = block->GetQuadElement();
            mapQuadtree->AddElement(oct_elm, block->GetBoundingBox().vmin, block->GetBoundingBox().vmax);

            blockArray[j][i] = block;
        }
    }
    mapQuadtree->BalanceTree();

    LinkBlocks();

    // Process terrain blocks
    CalculateMinD2Levels();

    isDirty = false;
}

/**
    Link all the terrain blocks together.
*/
void nMapNode::LinkBlocks()
{
    for (int j = 0; j < numBlocks; ++j)
    {
        for (int i = 0; i < numBlocks; ++i)
        {
            MapBlock* block = blockArray[j][i];

            if (i != numBlocks-1)
            {
                MapBlock* east = blockArray[j][i+1];
                block->SetNeighbor(MapBlock::EAST, east);
                east->SetNeighbor(MapBlock::WEST, block);
            }

            if (j != numBlocks-1)
            {
                MapBlock* south = blockArray[j+1][i];
                block->SetNeighbor(MapBlock::SOUTH, south);
                south->SetNeighbor(MapBlock::NORTH, block);
            }
        }
    }
}

/**
*/
void nMapNode::CalculateMinD2Levels()
{
    float c2 = CalculateCFactor();
    c2 *= c2;

    for (int j = 0; j < numBlocks; ++j)
        for (int i = 0; i < numBlocks; ++i)
        {
            blockArray[j][i]->CalculateMinD2Levels(c2);
        }
}

/**
    @brief Calculate C which is used in precalculating min D.
    It is a factor related to the distance from the camera for a given pixel
    pop to occur.
*/
float nMapNode::CalculateCFactor()
{
    float minx, maxx, miny, maxy, minz, maxz;
	refGfxServer->GetCamera().GetViewVolume(minx, maxx, miny, maxy, minz, maxz);
	//const nCamera2 & cam = refGfxServer->GetCamera();
	//float minz = cam.GetNearPlane();
	//float miny = cam.GetAngleOfView() * 

    int x0, x1, y0, y1;
    //refGfxServer->GetDisplayDesc(x0, y0, x1, y1);
	nDisplayMode2 dm = refGfxServer->GetDisplayMode();
	x0 = 0;
	x1 = dm.GetWidth();
	y0 = 0;
	y1 = dm.GetHeight();
    int v_res = y1 - y0;

    // float A = near_clip_plane / fabs(coord_top_clip_plane)
    float A = minz / fabsf(miny);
    // float T = 2 * tau / vertical screen resolution 
    float T = 2 * pixelError / float(v_res);

    return A/T;
}
