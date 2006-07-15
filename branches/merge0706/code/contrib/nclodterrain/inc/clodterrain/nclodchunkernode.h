//------------------------------------------------------------------------
/* Copyright (C) 2003 Gary Haussmann
 *
 * Re-targeting of Thatcher Ulrich's heightfield chunker code for use in
 * The Nebula Device
 */
//------------------------------------------------------------------------

#ifndef N_CLODCHUNKERNODE_H
#define N_CLODCHUNKERNODE_H

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef MESHGENERATOR_H
#include "clodterrain/meshgenerator.h"
#endif

#ifndef SPLATGENERATOR_H
#include "clodterrain/splatgenerator.h"
#endif

// used internally by the chunkernode
class HeightFieldData;
class nFile;
struct gen_state;

/**
 * @class nCLODChunkerNode
 * @ingroup NCLODTerrainContribModule
 *
 * @classinfo Implements a node that will read in a heightfield and generate
 * the appropriate vertex chunks for use in a ChunkLOD renderer.
 *
 * @author Gary Haussmann
 * 
 * @superclass nRoot
 *
 * @date 05052003
 *
 *
 */
class nCLODChunkerNode : public nRoot
{
public:
    /// constructor
    nCLODChunkerNode();

    /// destructor
    virtual ~nCLODChunkerNode();

    /// persistancy
    virtual bool SaveCmds(nPersistServer *ps);

    /**
         * @brief Specify name of the output file to hold Chunked LOD data.
     */
    virtual void setChunkFilename(const char *chunkfilename);

    /**
     * @brief Specify x,y,z spacings.
     * Use this to specify the scaling factors in the x, y, and z
     * direction.  If not specified, these values all default to
     * 1, or the values in the Heightfield file.
     *
     * @param xspacing distance between grid points in the x direction
     * @param yspacing distance between grid points in the y direction
     * @param zscale amount to scale the z/height values
     */
    void setSpacings(float xspacing, float yspacing, float zscale);

    /**
     * @brief Specify tree depth and maximum error at deepest depth
     *
     * @param depth depth of the quadtree to generate
     * @param maxerror maximum error allowed at the deepest quadtree level
     */
    void setChunkParameters(unsigned int depth, float maxerror);

    /**
     * @brief compile Chunked LOD file from the given filename
     *
     * This function opens the given file, reads in the heightfield data,
     * generates the vertex chunks and writes them out to the
     * output file specified in setChunkFilename.
     */
    virtual void compileChunksFromFile(const char *sourcefilename);

    /**
     * @brief compile Chunked LOD file from the given heightmap
     *
     * This function compile a chunk file from the heightmap which is provided
     * in lieu of a filename.  This function allows you to provide custom
     * heightmaps by subclassing the heightmap class and providing your own initialization
     * and file reading.
     */
    virtual void compileChunksFromHeightField(HeightFieldData *heightmap);

    /**
     * @brief Set the tile index bitmap to use
     *
     * If tile index bitmap is specified, the chunker will partition the triangles
     * in "splats" such that each splat contains no more than N tiles, where you can
     * also specify N.
     */
    virtual void setTileIndexFilename(const char *tileindexfilename);

    /**
     * @brief Set tiles per splat
     *
     * the default value is 4
     */
    virtual void setTilesPerSplat(int splatsize);

    /**
     * @brief Turn validation of saved chunks on or off
     */
    void setValidateFlag(bool newstate);

private:
    /// determines if we re-load the just saved chunks to verify they are correct
    bool m_validate;

    /// determines how many tiles per splat to user
    int m_splatthickness;

    /// spacing of x/y grid
    float m_xspacing, m_yspacing;

    /// scale of the z coordinates in the input file
    float m_zscale;

    /// depth of the quadtree to generate
    unsigned int m_targetdepth;

    /// maximum allowed error at the deepest tree level
    float m_maxerror;

    /// the output filename
    const char *m_outputfilename;

    /// tile index filename
    const char *m_tileindexfilename;

    nAutoRef<nFileServer2> m_ref_fs;

    /// holds the heightfield data
    HeightFieldData *m_heightfield;
    TileIndexData *m_tileindexfield;

    // generates triangle mesh
    MeshGenerator *m_mesher;

    // generate error and activation levels
    void updateActivationLevel(unsigned int ax, unsigned int ay,
                               unsigned int rx, unsigned int ry,
                               unsigned int lx, unsigned int ly);

    void propagateActivationLevel(unsigned int level, unsigned int target_level,
                                unsigned int cx, unsigned int cy);

    unsigned int validateActivationLevels(unsigned int cx, unsigned int cy, unsigned int level);

    // generate zero-filled output TOC, to be filled as we generate mesh data
    void generateEmptyTOC(nFile &destfile, int root_level);

    // generate mesh data for a specific level and all lower tree levels; returns triangle stats
    trianglestats generateAllMeshData(nFile &destfile, int x0, int y0, int log_size, int level);

    // generate mesh data for a specific square of data with the given center, size, and level
    void generateBlockMeshData(nFile &destfile, int cx, int cy, int log_size, int level);

    // generate a single quadrant of mesh data
    void generateQuadrantMeshData(nFile &destfile, gen_state *s, int lx, int ly, int tx, int ty, int rx, int ry, int recursion_level);

    // generate edge "skirt" for a mesh block
    void generateEdgeMeshData(nFile &destfile, int direction, int x0, int y0, int x1, int y1, int level);
};                              

#endif

