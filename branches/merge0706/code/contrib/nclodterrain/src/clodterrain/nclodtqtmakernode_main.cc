//------------------------------------------------------------
/* Copyright (C) 2003 Gary Haussmann
 *
 * Clodnodes is released under the terms of the Nebula License, in doc/source/license.txt.
 */
//------------------------------------------------------------


#define N_IMPLEMENTS nCLODTQTMakerNode

#include <math.h>

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "clodterrain/nclodtqtmakernode.h"
#include "il/il.h"
#include "il/ilu.h"

nNebulaScriptClass(nCLODTQTMakerNode, "kernel::nroot");

#define TQTTILE_HEADER_BYTES (8)
#define TQT_TOCENTRYSIZE (3 * sizeof(int))

#define TQT_VERSION (3)

static int tqt_node_count(int depth)
// Return the number of nodes in a fully populated quadtree of the specified depth.
{
    return 0x55555555 & ((1 << depth*2) - 1);
}

static int tqt_node_index(int level, int x, int y)
{
    int base = tqt_node_count(level);   // total node count in all levels above ours.

    // Effective coords within this node's level.
    int col = x;
    int row = y;

    // interleave x and y coordinates here
    int curbit = 0;
    int xy = 0;
    while ((col>0) || (row>0))
    {
        xy |= (col&1) << curbit;
        curbit++;
        xy |= (row&1) << curbit;
        curbit++;
        col >>= 1;
        row >>= 1;
    }

    return base + xy;
//  return tqt_node_count(level) + (y<<level) + x;
}

struct tqt_tile_layer {
    tqt_tile_layer(int tile_dim) : numtiles(tile_dim * tile_dim), tiledim(tile_dim)
    {
        tiles = new ILubyte *[numtiles];
        for (int i=0; i<numtiles; i++) tiles[i] = NULL;
    }
    ~tqt_tile_layer()
    {
        for (int i=0; i<numtiles; i++)
            if (tiles[i]) delete[] tiles[i];
        delete [] tiles;
    }

    void addtile(int x, int y, ILubyte *value)
    {
        n_assert( (x>=0) && (x<tiledim));
        n_assert( (y>=0) && (y<tiledim));
        tiles[x+y*tiledim] = value;
    }

    ILubyte * gettile(int x, int y)
    {
        n_assert( (x>=0) && (x<tiledim));
        n_assert( (y>=0) && (y<tiledim));
        return tiles[x + y * tiledim];
    }

    ILubyte **tiles;
    int numtiles, tiledim;
};


//
// nCLODTQTMakerNODE
//
// constructor
nCLODTQTMakerNode::nCLODTQTMakerNode() : nRoot(),
    m_targetdepth(5), m_tilesize(256), m_outputfilename(NULL),
    m_ref_fs("/sys/servers/file2"), m_validate(false), m_tocpos(-1)
{
    ilInit();
    iluInit();
}

nCLODTQTMakerNode::~nCLODTQTMakerNode()
{
    if (m_outputfilename != NULL)
    {
        n_free( (void *)(m_outputfilename) );
        m_outputfilename = NULL;
    }
}


void nCLODTQTMakerNode::setTQTFilename(const char *chunkfilename)
{
    m_outputfilename = n_strdup(chunkfilename);
}

void nCLODTQTMakerNode::setValidateFlag(bool flagstate)
{
    m_validate = flagstate;
}

void nCLODTQTMakerNode::setTQTParameters(unsigned int depth, unsigned int tilesize)
{
    n_assert(depth > 0);
    n_assert( (tilesize == 2) || (tilesize == 4) || (tilesize == 8) || (tilesize == 16) ||
        (tilesize == 32) || (tilesize == 64) || (tilesize == 128) || (tilesize == 256) ||
        (tilesize == 512) || (tilesize == 1024) );

    m_targetdepth = depth;
    m_tilesize = tilesize;
}

void nCLODTQTMakerNode::compileTQTFromFile(const char *sourcefilename)
{
    n_assert(m_ref_fs.isvalid());
    nFile *destfile = m_ref_fs->NewFileObject();
    nString bigsrcpath = m_ref_fs->ManglePath(sourcefilename);
    if (!destfile->Open(m_outputfilename, "wb"))
    {
        n_error("nCLODTQTMakerNode::compileTQTFromFile(): Could not open file %s\n",
                m_outputfilename);
        destfile->Release();
    }
    // write out tqt header
    char header[TQTTILE_HEADER_BYTES] = {'t','q','t',0, TQT_VERSION ,0,0,0 };
    destfile->Write(header,8);
    // write out tree depth and tile size
    destfile->PutInt(m_targetdepth);
    destfile->PutInt(m_tilesize);
    m_tocpos = destfile->Tell();
    // write out the TOC
    generateEmptyTOC(*destfile, m_targetdepth);
    tqt_tile_layer *curtiles = new tqt_tile_layer( 1 << (m_targetdepth-1) );
    if ((curtiles = generateTQTLeaves(*destfile, bigsrcpath, curtiles)) &&
         (m_targetdepth > 1) )
    {
        curtiles = generateTQTNodes(*destfile, m_targetdepth-2, curtiles);
    }
    delete curtiles;
    destfile->Close();
    destfile->Release();
}

// generate zero-filled output TOC, to be filled as we generate mesh data
void nCLODTQTMakerNode::generateEmptyTOC(nFile &destfile, int root_level)
// Append an empty table-of-contents for a fully-populated quadtree,
// and rewind the stream to the start of the contents.  Use this to
// make room for TOC at the beginning of the file, while bulk
// vert/index data gets appended after the TOC.
{
    destfile.Seek(m_tocpos, nFile::START);
    unsigned char emptyentry[TQT_TOCENTRYSIZE];
    for (int cix=0; cix < TQT_TOCENTRYSIZE; cix++)
        emptyentry[cix] = 0;

    int chunk_count = tqt_node_count(root_level);   // tqt has a handy function to compute # of nodes in a quadtree
    for (int i = 0; i < chunk_count; i++)
    {
        destfile.Write(emptyentry, TQT_TOCENTRYSIZE);
    }
}

// generate leaf nodes from the source image
tqt_tile_layer * nCLODTQTMakerNode::generateTQTLeaves(nFile &destfile, nString sourcefilename, tqt_tile_layer *tiles)
{
    // grab the image from the file
    ILuint sourceImage = iluGenImage();
    ilBindImage(sourceImage);
    if ( !ilLoadImage( (char*)sourcefilename.Get() ) )
    {
        n_printf("DevIL failed loading image '%s' with '%s'.\n", sourcefilename, iluErrorString(ilGetError()));
        iluDeleteImage(sourceImage);
        return false;
    }


    // make sure image is of size 2^N+1 in both dimensions
    ILint bit_depth = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
    ILint itype = ilGetInteger(IL_IMAGE_TYPE);
    ILint iformat = ilGetInteger(IL_IMAGE_FORMAT);
    ILuint width  = ilGetInteger(IL_IMAGE_WIDTH);
    ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);

    n_printf("image width = %ld, height = %ld, bit depth = %d\n", width, height, bit_depth);

//  n_assert(iformat == IL_RGB);
    n_assert(itype == IL_UNSIGNED_BYTE);

    ILubyte *sourcedata = ilGetData();

    // build a destination image as well
    ILuint destImage = iluGenImage();
    ilBindImage(destImage);
    ilTexImage(m_tilesize, m_tilesize, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);

    // generate the leaf images one by one, using DevIL's blitting function
    int tile_dim = 1 << (m_targetdepth - 1);
    for (int x=0; x < tile_dim; x++)
    for (int y=0; y < tile_dim; y++)
    {
        float y0 = float(y) / tile_dim * height;
        float y1 = float(y+1) / tile_dim * height;
        float x0 = float(x) / tile_dim * width;
        float x1 = float(x+1) / tile_dim * width;

        n_printf("generating tile %d\n", y + x*tile_dim);

        ILubyte *destdata = new ILubyte[m_tilesize * m_tilesize * 3];
        tiles->addtile(x,y, destdata);

        // copy over resampled image
        float xstep = (x1-x0)/m_tilesize;
        float ystep = (y1-y0)/m_tilesize;
        for (unsigned int xoffset=0; xoffset < m_tilesize; xoffset++)
        for (unsigned int yoffset=0; yoffset < m_tilesize; yoffset++)
        {
            float xcoord = x0 + xoffset * xstep;
            float ycoord = y0 + (m_tilesize - 1 - yoffset) * ystep;
            ILubyte *sourcepixel = sourcedata + 3 * ((int)xcoord + ((int)ycoord) * width);
            ILubyte *destpixel = destdata + 3 * (xoffset + yoffset * m_tilesize);
            destpixel[0] = sourcepixel[0];
            destpixel[1] = sourcepixel[1];
            destpixel[2] = sourcepixel[2];
        }

        // write out sample image
        if (ilSetData( (void *)destdata) == IL_FALSE)
        {
            n_printf("error setting tile data\n");
        }

        int tile_offset = tqt_node_index(m_targetdepth-1, x, y);
        pushImage(destfile, destImage, tile_offset);
    }

    iluDeleteImage(sourceImage);
    iluDeleteImage(destImage);

    return tiles;
}

// generate inner node by sampling child nodes
tqt_tile_layer * nCLODTQTMakerNode::generateTQTNodes(nFile &destfile, int level, tqt_tile_layer *oldtiles)
{
    // here we take four adjacent images and subsample them by 2 to produce a final
    // image that is half the size
    ILuint destimage = iluGenImage();
    ILuint sourceimage = iluGenImage();

    ilBindImage(sourceimage);
    ilTexImage(m_tilesize, m_tilesize, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);

    int tile_dim = 1 << level;
    tqt_tile_layer *newtiles = new tqt_tile_layer( tile_dim );

    // iterate over all the images for this level
    for (int x=0; x < tile_dim; x++)
    for (int y=0; y < tile_dim; y++)
    {
        // assemble these images into the destination image, which is twice as big to
        // hold all the data...
        ilBindImage(destimage);
        ilTexImage(m_tilesize*2, m_tilesize*2, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);

        // get the four sub-images needed to composite together for this image
        for (int xoffset=0; xoffset < 2; xoffset++)
        for (int yoffset=0; yoffset < 2; yoffset++)
        {
            int subtilex = x*2+xoffset;
            int subtiley = y*2+yoffset;
            ILubyte *curtile = oldtiles->gettile(subtilex, subtiley);
            int sub_index = tqt_node_index(level+1, subtilex, subtiley);
            n_printf("mapping tile (%d,%d), index %d, value %x\n", subtilex, subtiley, sub_index, curtile);
            ilBindImage(sourceimage);
            ilSetData((void *)curtile);

            ilBindImage(destimage);
            ilBlit(sourceimage, m_tilesize * xoffset, m_tilesize * yoffset, 0,
                                0,0,0,
                                m_tilesize, m_tilesize,1);
        }

        // scale the image down by 1/2 to get the right size
        ilBindImage(destimage);
        iluScale(m_tilesize, m_tilesize, 1);

        ILubyte *newtile = new ILubyte [3 * m_tilesize * m_tilesize];
        ILubyte *imagecopy = ilGetData();
        for (unsigned int i=0; i < 3 * m_tilesize * m_tilesize; i++)
        {
            newtile[i] = imagecopy[i];
        }
        newtiles->addtile(x,y,newtile);

        // dump the image out
        int tile_offset = tqt_node_index(level, x, y);
        pushImage(destfile, destimage, tile_offset);
    }

    iluDeleteImage(destimage);
    iluDeleteImage(sourceimage);

    delete oldtiles;

    if (level > 0)
        return generateTQTNodes(destfile, level-1, newtiles);

    return newtiles;
}

// put a DevIL image into the tqt file at the correct index
void nCLODTQTMakerNode::pushImage(nFile &tqtfile, ILuint imagehandle, int tile_index)
{
    n_printf("writing image %d\n", tile_index);

    ilBindImage(imagehandle);
    ilSave(IL_BMP, "testimg.bmp");

    ILubyte *imagecopy = ilGetData();

    tqtfile.Seek(0, nFile::END);
    int texture_pos = tqtfile.Tell();
    int image_size = 4 * m_tilesize * m_tilesize;
    tqtfile.PutInt(0);
    tqtfile.PutInt(image_size);
    unsigned char *imgdata = new unsigned char [image_size];

    for (unsigned int i=0; i < m_tilesize * m_tilesize; i++)
    {
        imgdata[i*4+3] = 255;
        imgdata[i*4+0] = imagecopy[i*3];
        imgdata[i*4+1] = imagecopy[i*3+1];
        imgdata[i*4+2] = imagecopy[i*3+2];
    }

    tqtfile.Write(imgdata, image_size);

    // Rewind to the TOC and  put the offset of this tile in there
    tqtfile.Seek(m_tocpos + TQT_TOCENTRYSIZE * tile_index, nFile::START);
    n_printf("texture pos %d, size %d\n", texture_pos, image_size);
    tqtfile.PutInt(1);
    tqtfile.PutInt(texture_pos);
    tqtfile.PutInt(image_size);

    delete [] imgdata;
}
