//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo.
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
//#include "gfx/nindexbuffer.h"
#include "scene/nsceneserver.h"
//#include "gfx/nvertexbuffer.h"
#include "map/nmapnode.h"
#include "map/nmap.h"

nNebulaScriptClass(nMapNode, "nmaterialnode");

//------------------------------------------------------------------------------
/**
*/
nMapNode::nMapNode() :
    detailSize(1.0f),
    detailScale(0.0f),
    isDirty(false),
    numMipMapLevels(0),
    numBlocks(0),
    blockArray(),
    mapQuadtree(NULL),
    renderDebug(false),
    meshUsage(nMesh2::WriteOnce)
{
    refGfxServer = "/sys/servers/gfx";
    sprintf(edgebuf_name, "%08xe", (unsigned int)this);
    sprintf(stripbuf_name, "%08xs", (unsigned int)this);
}

//------------------------------------------------------------------------------
/**
*/
nMapNode::~nMapNode()
{	
}

/**
*/
bool nMapNode::LoadResources()
{
    nMaterialNode::LoadResources();
    if (true == this->refMap.isvalid())
        refMap->LoadMap();
	return true;
}


/**
    If this replaces an existing heightmap,
    it forces a reload immediately to ensure sim runs without a hitch.
*/
void nMapNode::SetMapPath(const char* name)
{
    refMap = name;
    isDirty = true;
}

/**
    @return The filename for the heightmap data.
*/
const char* nMapNode::GetMapPath()
{
    return refMap.getname();
}

/**
    @brief Set the terrain block size in vertices.
    This is used to divide into the heightmap where:
    
        dimensions = block_size * num_blocks - (num_blocks - 1)

    Or:

        num_blocks = (dimensions - 1) / (block_size - 1)

    It is far easier to keep blockSize as 2^n+1.
        
    @param size A value of 2^n+1
*/
void nMapNode::SetBlockSize(int size)
{
    n_assert(0 < size);
    n_assert(size%2);
    blockSize = size;
    isDirty = true;
}

void nMapNode::SetError(int error)
{
    pixelError = error;
    isDirty = true;
}

int nMapNode::GetError() const
{
    return pixelError;
}

/**
    @param size Size of the texture detail in metres.
*/
void nMapNode::SetDetailSize(float size)
{
    n_assert(0.0f < size);
    detailSize = size;
    isDirty = true;
}

/**
    @return Size of a single detail texture.
*/
float nMapNode::GetDetailSize()
{
    return detailSize;
}

//------------------------------------------------------------------------------
/**
    This method returns the current mesh usage flag combination.

    @return     a combination on nMesh2::Usage flags
*/
int
nMapNode::GetMeshUsage() const
{
    return meshUsage;
}

//------------------------------------------------------------------------------
/**
    Specifies the mesh usage flag combination required by
    this shape node class.
*/
void
nMapNode::SetMeshUsage(int usage)
{
    meshUsage = usage;
}
