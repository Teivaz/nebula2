//------------------------------------------------------------------------
/* Copyright (C) 2003 Gary Haussmann
 *
 * Re-targeting of Thatcher Ulrich's tqt building code for use in
 * The Nebula Device
 */
//------------------------------------------------------------------------

#ifndef N_TQTSPLATTERNODE_H
#define N_TQTSPLATTERNODE_H

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#ifndef N_TILEINDEXDATA_H
#include "clodterrain/tileindexdata.h"
#endif

#ifndef N_GFXSERVER2_H
#include "gfx2/ngfxserver2.h"
#endif


#include "il/il.h"

class nFile;
struct tqt_tile_layer;

/**
 * @class nCLODTQTSplatterNode
 * @ingroup NCLODTerrainContribModule
 *
 * @classinfo Implements a node that will generate the proper blend maps and base textures for splatted terrain.
 *
 * @author Gary Haussmann
 * 
 * @superclass nRoot
 *
 * @date 08032004
 */
class nCLODTQTSplatterNode : public nRoot
{
public:
    /// constructor
    nCLODTQTSplatterNode();

    /// destructor
    virtual ~nCLODTQTSplatterNode();

    /// persistancy
    virtual bool SaveCmds(nPersistServer *ps);

    /**
           * @brief Specify name of the output file to hold TQT data.
     */
    virtual void setTQTFilename(const char *chunkfilename);

    /**
     * @brief Specify tree depth and size of the chunk texture size for each node
     *
     * @param splatdepth depth of the quadtree to generate
     * @param chunktexsize size of the chunk textures (chunktexes are square) to generate at each level
     * @param maxtexindex the highest texture index for which we generate a blend texture
     */
    void setTQTParameters(unsigned int splatdepth, unsigned int chunktexsize, unsigned int maxtexindex);

    /**
     * @brief specify tile textures to use for a given index
     *
     * You can tell the splatter which tile you plan to use for a given index; this
     * is used to generate the low-res base textures.  The base textures are displayed when
     * a chunk is so far from the viewer that using texture splatting and blending is
     * pretty much useless.  At that point, the base texture is displayed instead.
     *
     * If you don't specify a tile texture for a given index, the RGB value from the tile image
     * palette will be used for coloring the base texture.
     */
    void setDetailTexture(unsigned int tileindex, const char *tiletexturename);
    
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
    /// depth of the quadtree to generate; the last layer has splat blend textures
    unsigned int m_splatdepth;

    /// size of the chunk textures
    unsigned int m_chunktexsize;

    /// max texture index for splat blending
    unsigned int m_maxtexindex;

    /// the output filename
    const char *m_outputfilename;

    nAutoRef<nFileServer2> m_ref_fs;

    bool m_validate;

    nArray<nRef<nTexture2> > m_ref_tiletextures;

    // position of the table of contents in the tqt file.
    int m_tocpos;

    nAutoRef<nGfxServer2> refGfxServer;

    // generate zero-filled output TOC, to be filled as we generate mesh data
    void generateEmptyTOC(nFile &destfile, int root_level);

    // generate leaf nodes from the source image
    tqt_tile_layer * generateTQTLeaves(nFile &destfile, int level, nString sourcefilename, tqt_tile_layer *tiles);

    // generate inner node by sampling child nodes
    tqt_tile_layer * generateTQTNodes(nFile &destfile, int level, tqt_tile_layer *oldtiles);
    
    // generate the bottom layer of textures containing splat blend textures
    void generateTQTBlendTextures(nFile &destfile, int level, TileIndexData *tilemap);

    // put a DevIL image into the tqt file at the correct index
    void pushImage(nFile &tqtfile, ILuint imagehandle, int tile_index);

    // put multiple images into a tqt file at a specific chunk index
    void pushImages(nFile &tqtfile, unsigned int numimages, ILuint *imagehandles, int node_index);

    // put multiple blend images into a tqt file at a specific chunk index
    void pushBlendImages(nFile &tqtfile, unsigned int numimages, ILuint *imagehandles, bool *imageemptyflags, int node_index);
};                              

#endif

