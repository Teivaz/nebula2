//------------------------------------------------------------------------
/* Copyright (C) 2003 Gary Haussmann
 *
 * Re-targeting of Thatcher Ulrich's tqt building code for use in
 * The Nebula Device
 */
//------------------------------------------------------------------------

#ifndef N_TQTMAKERNODE_H
#define N_TQTMAKERNODE_H

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#include "il/il.h"

class nFile;
struct tqt_tile_layer;

/**
 * @class nTQTMakerNode
 * @ingroup NCLODTerrainContribModule
 *
 * @classinfo Implements a node that will read in an image and generate
 * a quadtree of images at various resolutions.
 *
 * @author Gary Haussmann
 * 
 * @superclass nRoot
 *
 * @date 11102003
 */
class nCLODTQTMakerNode : public nRoot
{
public:
    /// constructor
    nCLODTQTMakerNode();

    /// destructor
    virtual ~nCLODTQTMakerNode();

    /// persistancy
    virtual bool SaveCmds(nPersistServer *ps);

    /**
         * @brief Specify name of the output file to hold TQT data.
     */
    virtual void setTQTFilename(const char *chunkfilename);

    /**
     * @brief Specify tree depth and size of the tiles for each node
     *
     * @param depth depth of the quadtree to generate
     * @param tilesize size of the tiles (tiles are square) to generate at each level
     */
    void setTQTParameters(unsigned int depth, unsigned int tilesize);

    /**
     * @brief compile TQT file from the given filename
     *
     * This function opens the given file, reads in the image data,
     * generates the TQT node images and write them out to the previously
         * specified tqt-file.
     */
    virtual void compileTQTFromFile(const char *sourcefilename);

    virtual void setValidateFlag(bool flagstate);

private:
    /// depth of the quadtree to generate
    unsigned int m_targetdepth;

    /// size of the tiles
    unsigned int m_tilesize;

    /// the output filename
    const char *m_outputfilename;

    nAutoRef<nFileServer2> m_ref_fs;

    bool m_validate;

    // position of the table of contents in the tqt file.
    int m_tocpos;

    // generate zero-filled output TOC, to be filled as we generate mesh data
    void generateEmptyTOC(nFile &destfile, int root_level);

    // generate leaf nodes from the source image
    tqt_tile_layer * generateTQTLeaves(nFile &destfile, nString sourcefilename, tqt_tile_layer *tiles);

    // generate inner node by sampling child nodes
    tqt_tile_layer * generateTQTNodes(nFile &destfile, int level, tqt_tile_layer *oldtiles);

    // put a DevIL image into the tqt file at the correct index
    void pushImage(nFile &tqtfile, ILuint imagehandle, int tile_index);

};                              

#endif

