//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo.
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "map/nmap.h"
#include "kernel/nkernelserver.h"

nNebulaScriptClass(nMap, "nroot");

//------------------------------------------------------------------------------
/**
*/
nMap::nMap() :
    nRoot(),
    isDataDirty(false),
    // Map variables
    imagePath(),
    gridInterval(1.0f),
    heightMin(0.0f),
    heightMax(256.0f),
    heightScale(1.0f),
    // Derived variables
    mapDimension(0),
    boundingBox(),
    pointData(NULL)
{
    refFileServer = "/sys/servers/file2";
}

//------------------------------------------------------------------------------
/**
*/
nMap::~nMap()
{
}

/**
    @brief Set heightmap file to load terrain from
    Replaces an existing heightmap.
    The heightmap must be square, and if it is n pixels wide,
    then (n-1) / (blockSize-1) should have zero remainder
    (blockSize is from nMapNode).
*/
void nMap::SetHeightMap(const char *name)
{
	imagePath.Set(name);
    isDataDirty = true;
}

/**
    @brief Get heightmap file terrain was loaded from
    @return The filename for the heightmap data.
*/
const char* nMap::GetHeightMap()
{
    //return imagePath.GetFilename();
	return imagePath.Get();
}

/**
    @brief Set the spacing between grid points in metres
    @param spacing The grid spacing in metres.
*/
void nMap::SetGridInterval(float spacing)
{
    n_assert(0.0f < spacing);
    gridInterval = spacing;
    isDataDirty = true;
}

/**
    @brief Get the spacing between grid points in metres
    @return The grid spacing in metres.
*/
float nMap::GetGridInterval() const
{
    return gridInterval;
}

/**
    @brief Set the range of heights
*/
void nMap::SetHeightRange(float min, float max)
{
    n_assert(min < max);
    heightMin = min;
    heightMax = max;
    heightScale = (max - min) / 256.0f;
    isDataDirty = true;
}

/**
    @brief Get the minimum height of the terrain
*/
float nMap::GetHeightRangeMin() const
{
    return heightMin;
}

/**
    @brief Set the maximum height of the terrain
*/
float nMap::GetHeightRangeMax() const
{
    return heightMax;
}
