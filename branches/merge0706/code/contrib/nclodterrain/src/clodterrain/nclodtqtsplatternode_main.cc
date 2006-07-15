//------------------------------------------------------------
/* Copyright (C) 2003 Gary Haussmann
 *
 * Clodnodes is released under the terms of the Nebula License, in doc/source/license.txt.
 */
//------------------------------------------------------------


#define N_IMPLEMENTS nCLODTQTSplatterNode

#include <math.h>

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "clodterrain/nclodtqtsplatternode.h"
#include "clodterrain/tileindexdata.h"
#include "il/il.h"
#include "il/ilu.h"

nNebulaScriptClass(nCLODTQTSplatterNode, "nroot");

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
// nCLODTQTSplatterNODE
//
// constructor
nCLODTQTSplatterNode::nCLODTQTSplatterNode() : nRoot(),
    m_splatdepth(5), m_chunktexsize(256), m_outputfilename(NULL),
    m_ref_fs("/sys/servers/file2"), m_validate(false), m_ref_tiletextures(256,32), 
    m_tocpos(-1), refGfxServer("/sys/servers/gfx")
{
    ilInit();
    iluInit();
}

nCLODTQTSplatterNode::~nCLODTQTSplatterNode()
{
    if (m_outputfilename != NULL)
    {
        n_free( (void *)(m_outputfilename) );
        m_outputfilename = NULL;
    }
}


void nCLODTQTSplatterNode::setTQTFilename(const char *chunkfilename)
{
    m_outputfilename = n_strdup(chunkfilename);
}

void nCLODTQTSplatterNode::setValidateFlag(bool flagstate)
{
    m_validate = flagstate;
}

void nCLODTQTSplatterNode::setTQTParameters(unsigned int splatdepth, unsigned int chunktexsize, unsigned int maxtexindex)
{
    n_assert(splatdepth > 0);
    n_assert( (chunktexsize == 2) || (chunktexsize == 4) || (chunktexsize == 8) || (chunktexsize == 16) ||
        (chunktexsize == 32) || (chunktexsize == 64) || (chunktexsize == 128) || (chunktexsize == 256) ||
        (chunktexsize == 512) || (chunktexsize == 1024) );

    m_splatdepth = splatdepth;
    m_chunktexsize = chunktexsize;
    m_maxtexindex = maxtexindex;
    m_ref_tiletextures.SetFixedSize(m_maxtexindex);
}


void nCLODTQTSplatterNode::setDetailTexture(unsigned int tileindex, const char *tiletexturename)
{
    nString bigsrcpath = m_ref_fs->ManglePath(tiletexturename);  

    // prepare and load the texture
    n_assert(this->refGfxServer.isvalid());
    nTexture2* tex = this->refGfxServer->NewTexture(tiletexturename);
    n_assert(tex);
    if (!tex->IsValid())
    {
        tex->SetFilename(bigsrcpath);
        if (!tex->Load())
        {
            n_printf("nCLODSplatterNode: Error loading texture '%s'\n", bigsrcpath);
            return;
        }
        
        this->m_ref_tiletextures.Set(tileindex, nRef<nTexture2>(tex));
    }
}

void nCLODTQTSplatterNode::compileTQTFromFile(const char *sourcefilename)
{
    n_assert(m_ref_fs.isvalid());
    nFile *destfile = m_ref_fs->NewFileObject();
    nString bigsrcpath = m_ref_fs->ManglePath(sourcefilename);
    if (!destfile->Open(m_outputfilename, "wb"))
    {
        n_error("nCLODTQTSplatterNode::compileTQTFromFile(): Could not open file %s\n",
                m_outputfilename);
        destfile->Release();
        return;
    }
    // write out tqt header
    char header[TQTTILE_HEADER_BYTES] = {'t','q','t',0, TQT_VERSION ,0,0,0 };
    destfile->Write(header,8);
    // write out tree depth and tile size
    destfile->PutInt(m_splatdepth);
    destfile->PutInt(m_chunktexsize);
    m_tocpos = destfile->Tell();
    // write out the TOC
    generateEmptyTOC(*destfile, m_splatdepth);
    tqt_tile_layer *curtiles = new tqt_tile_layer( 1 << (m_splatdepth-2) );
    if ((curtiles = generateTQTLeaves(*destfile, m_splatdepth-2, bigsrcpath , curtiles)) &&
         (m_splatdepth > 2) )
    {
        curtiles = generateTQTNodes(*destfile, m_splatdepth-3, curtiles);
    }
    delete curtiles;

    // generate splat blendmaps
    TileIndexData tilemap;
    if (tilemap.readBitmap(bigsrcpath))
    {
        generateTQTBlendTextures(*destfile, m_splatdepth-1, &tilemap);
    }
    destfile->Close();
    destfile->Release();
}

// generate zero-filled output TOC, to be filled as we generate mesh data
void nCLODTQTSplatterNode::generateEmptyTOC(nFile &destfile, int root_level)
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
tqt_tile_layer * nCLODTQTSplatterNode::generateTQTLeaves(nFile &destfile, int leafdepth, nString sourcefilename, tqt_tile_layer *tiles)
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

/*  if (iformat != IL_RGB)
    {
        n_printf("converting image to RGB type\n");
        ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
        iformat = ilGetInteger(IL_IMAGE_FORMAT);
    }
    n_assert(iformat == IL_RGB);*/
    n_assert(itype == IL_UNSIGNED_BYTE);

    ILubyte *sourcedata = ilGetData();

    // build a destination image as well
    ILuint destImage = iluGenImage();
    ilBindImage(destImage);
    ilTexImage(m_chunktexsize, m_chunktexsize, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);

    // generate the leaf images one by one, using DevIL's blitting function
    int tile_dim = 1 << (leafdepth);
    for (int x=0; x < tile_dim; x++)
    for (int y=0; y < tile_dim; y++)
    {
        float y0 = float(y) / tile_dim * height;
        float y1 = float(y+1) / tile_dim * height;
        float x0 = float(x) / tile_dim * width;
        float x1 = float(x+1) / tile_dim * width;

        n_printf("generating tile %d\n", y + x*tile_dim);

        ILubyte *destdata = new ILubyte[m_chunktexsize * m_chunktexsize * 3];
        tiles->addtile(x,y, destdata);

        // copy over resampled image
        float xstep = (x1-x0)/m_chunktexsize;
        float ystep = (y1-y0)/m_chunktexsize;
        for (unsigned int xoffset=0; xoffset < m_chunktexsize; xoffset++)
        for (unsigned int yoffset=0; yoffset < m_chunktexsize; yoffset++)
        {
            float xcoord = x0 + xoffset * xstep;
            float ycoord = y0 + (m_chunktexsize - 1 - yoffset) * ystep;
            ILubyte sourceindex = sourcedata[((int)xcoord + ((int)ycoord) * width)];
            unsigned char r = sourceindex *15, g = sourceindex*15, b = sourceindex*15;
            // get a color from the source texture if possible
            if (m_ref_tiletextures[sourceindex].isvalid())
            {
                nTexture2 *tex = m_ref_tiletextures[sourceindex].get();
                nTexture2::LockInfo lockinfo;
                tex->Lock(nTexture2::ReadOnly, 0, lockinfo);
                unsigned char *sourcepixel = (unsigned char *)lockinfo.surfPointer;
                int tx = (int)fmod(xcoord*10.0, (double)tex->GetWidth()), ty = (int)fmod(ycoord*10.0, (double)tex->GetHeight());
                sourcepixel = sourcepixel + (tx * tex->GetBytesPerPixel() + ty * lockinfo.surfPitch);
                r = sourcepixel[0];
                g = sourcepixel[1];
                b = sourcepixel[2];
                tex->Unlock(0);
            }
//          ILubyte *sourcepixel = sourcedata + 3 * ((int)xcoord + ((int)ycoord) * width);
            ILubyte *destpixel = destdata + 3 * (xoffset + yoffset * m_chunktexsize);
            destpixel[0] = r;
            destpixel[1] = g;
            destpixel[2] = b;
        }

        // write out sample image
        if (ilSetData( (void *)destdata) == IL_FALSE)
        {
            n_printf("error setting tile data\n");
        }

        int tile_offset = tqt_node_index(leafdepth, x, y);
        pushImage(destfile, destImage, tile_offset);
    }

    iluDeleteImage(sourceImage);
    iluDeleteImage(destImage);

    return tiles;
}

// generate inner node by sampling child nodes
tqt_tile_layer * nCLODTQTSplatterNode::generateTQTNodes(nFile &destfile, int level, tqt_tile_layer *oldtiles)
{
    // here we take four adjacent images and subsample them by 2 to produce a final
    // image that is half the size
    ILuint destimage = iluGenImage();
    ILuint sourceimage = iluGenImage();

    ilBindImage(sourceimage);
    ilTexImage(m_chunktexsize, m_chunktexsize, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);

    int tile_dim = 1 << level;
    tqt_tile_layer *newtiles = new tqt_tile_layer( tile_dim );

    // iterate over all the images for this level
    for (int x=0; x < tile_dim; x++)
    for (int y=0; y < tile_dim; y++)
    {
        // assemble these images into the destination image, which is twice as big to
        // hold all the data...
        ilBindImage(destimage);
        ilTexImage(m_chunktexsize*2, m_chunktexsize*2, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
    
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
            ilBlit(sourceimage, m_chunktexsize * xoffset, m_chunktexsize * yoffset, 0,
                                0,0,0,
                                m_chunktexsize, m_chunktexsize,1);
        }

        // scale the image down by 1/2 to get the right size
        ilBindImage(destimage);
        iluScale(m_chunktexsize, m_chunktexsize, 1);

        ILubyte *newtile = new ILubyte [3 * m_chunktexsize * m_chunktexsize];
        ILubyte *imagecopy = ilGetData();
        for (unsigned int i=0; i < 3 * m_chunktexsize * m_chunktexsize; i++)
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

void nCLODTQTSplatterNode::generateTQTBlendTextures(nFile &destfile, int level, TileIndexData *tilemap)
{
    unsigned int blendtex_dim = 1 << level;
    unsigned int blendtex_stride = (tilemap->getXSize() / blendtex_dim);
    unsigned int blendtex_size = blendtex_stride+1; // 1 pixel overlap between chunks
    n_assert(blendtex_size <= m_chunktexsize);
    n_assert(blendtex_stride*2 == m_chunktexsize);
    n_assert( (blendtex_stride * blendtex_dim + 1) == tilemap->getXSize() );
    
    bool *blendmarks = new bool[blendtex_size * blendtex_size];

    ILuint *destImages = new ILuint[m_maxtexindex];
    bool *useempty = new bool[m_maxtexindex];
    ilGenImages(m_maxtexindex, destImages);
        
    for (unsigned int x=0; x < blendtex_dim; x++)
    for (unsigned int y=0; y < blendtex_dim; y++)
    {
        unsigned int y0 = y * blendtex_stride;
        unsigned int y1 = y0 + blendtex_size;
        unsigned int x0 = x * blendtex_stride;
        unsigned int x1 = x0 + blendtex_size;
        
        n_printf("generating blend textures for chunk %d\n", y + x*blendtex_dim);

        // dump index data into this array
        /// given a rectangle and a tile index, marks user bitmap such that spots with that tile index are 1
        // all other parts of the array are marked with 0
        // note that the user array should be of size (x2-x1)*(y1-y2) bytes!
        // @return the # of spots marked with 1
        ILubyte *destdata = new ILubyte[m_chunktexsize * m_chunktexsize * 3];
        for (unsigned int tileindex=0; tileindex < m_maxtexindex; tileindex++)
        {
            unsigned int nummarks = tilemap->makeTileMap(x0,y0,x1,y1, tileindex, blendmarks);

            // no marks? use the empty map, don't generate a new one
            if (nummarks == 0)
            {
                useempty[tileindex] = true;
                continue;
            }
            else
                useempty[tileindex] = false;

            // generate image from the markarray
            ilBindImage(destImages[tileindex]);
            ilTexImage(m_chunktexsize, m_chunktexsize, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
            for (unsigned int ix=0; ix < blendtex_size; ix++)
                for (unsigned int iy=0; iy < blendtex_size; iy++)
                {
                    ILubyte *destpixel = destdata + 3 * (ix + (m_chunktexsize-1-iy) * m_chunktexsize);
                    if (blendmarks[ix + iy * blendtex_size])
                    {
                        destpixel[0] = 255;
                        destpixel[1] = 255;
                        destpixel[2] = 255;
                    }
                    else
                    {
                        destpixel[0] = destpixel[1] = destpixel[2] = 0;
                    }
                }

            // write out sample image
            if (ilSetData( (void *)destdata) == IL_FALSE)
            {
                n_printf("error setting blend map data\n");
            }
        }
        delete [] destdata;
        unsigned int node_index = tqt_node_index(level, x, y);
        pushBlendImages(destfile, m_maxtexindex, destImages, useempty, node_index);
    }
    
    ilDeleteImages(m_maxtexindex, destImages);
    delete [] destImages;
    delete [] useempty;
}

// put a DevIL image into the tqt file at the correct index
void nCLODTQTSplatterNode::pushImage(nFile &tqtfile, ILuint imagehandle, int tile_index)
{
    n_printf("writing image %d\n", tile_index);

    ilBindImage(imagehandle);
//    char buffer[100];
//    sprintf(buffer,"testimg%d.bmp", tile_index);
//    ilSave(IL_BMP, buffer);

    ILubyte *imagecopy = ilGetData();

    tqtfile.Seek(0, nFile::END);
    int texture_pos = tqtfile.Tell();
    int image_size = 4 * m_chunktexsize * m_chunktexsize;
    unsigned char *imgdata = new unsigned char [image_size];

    tqtfile.PutInt(0);
    tqtfile.PutInt(image_size);

    for (unsigned int i=0; i < m_chunktexsize * m_chunktexsize; i++)
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

// put multiple images into a tqt file at a specific chunk index
void nCLODTQTSplatterNode::pushImages(nFile &tqtfile, unsigned int numimages, ILuint *imagehandles, int node_index)
{
    n_printf("writing %d images for chunk %d\n", numimages, node_index);

    int image_size = 4 * m_chunktexsize * m_chunktexsize;
    unsigned char *imgdata = new unsigned char [image_size];
    
//  ilSave(IL_BMP, "testimg.bmp");

    tqtfile.Seek(0, nFile::END);
    unsigned int texture_pos = tqtfile.Tell();

    for (unsigned int imageindex=0; imageindex < numimages; imageindex++)
    {
        ilBindImage(imagehandles[imageindex]);
        char buffer[100];
        sprintf(buffer,"testimg%d-%d.bmp", node_index, imageindex);
//        ilSave(IL_BMP, buffer);

        tqtfile.PutInt(imageindex);
        tqtfile.PutInt(image_size);

        ILubyte *imagecopy = ilGetData();

        for (unsigned int i=0; i < m_chunktexsize * m_chunktexsize; i++)
        {
            imgdata[i*4+3] = 255;
            imgdata[i*4+0] = imagecopy[i*3];
            imgdata[i*4+1] = imagecopy[i*3+1];
            imgdata[i*4+2] = imagecopy[i*3+2];
        }

        tqtfile.Write(imgdata, image_size);
    }
    // Rewind to the TOC and  put the offset of this tile in there
    tqtfile.Seek(m_tocpos + TQT_TOCENTRYSIZE * node_index, nFile::START);
    n_printf("texture pos %d, size %d\n", texture_pos, image_size);
    tqtfile.PutInt(numimages);
    tqtfile.PutInt(texture_pos);
    tqtfile.PutInt(image_size*numimages);

    
    delete [] imgdata;
}


// put multiple images into a tqt file at a specific chunk index
void nCLODTQTSplatterNode::pushBlendImages(nFile &tqtfile, unsigned int numimages, ILuint *imagehandles, bool *emptyimageflag, int node_index)
{
    n_printf("writing %d images for chunk %d\n", numimages, node_index);

    int image_size = (m_chunktexsize * m_chunktexsize);
    unsigned char *imgdata = new unsigned char [image_size];
    
//  ilSave(IL_BMP, "testimg.bmp");

    tqtfile.Seek(0, nFile::END);
    unsigned int texture_pos = tqtfile.Tell();

    for (unsigned int imageindex=0; imageindex < numimages; imageindex++)
    {
        tqtfile.PutInt(imageindex);

        // an empty image?
        if (emptyimageflag[imageindex])
        {
            tqtfile.PutInt(0);
            continue;
        }
        else
            tqtfile.PutInt(image_size);

        ilBindImage(imagehandles[imageindex]);
        char buffer[100];
        sprintf(buffer,"testimg%d-%d.bmp", node_index, imageindex);
//        ilSave(IL_BMP, buffer);

        ILubyte *imagecopy = ilGetData();

//        int curtexel=0; // we write out a chunk header every 16 texels
        ILubyte curbyteval = 0, *curbyte = imgdata;
        for (unsigned int i=0; i < m_chunktexsize * m_chunktexsize; i++)
        {
            // write out a chunk header if needed
/*            if (curtexel % 16 == 0)
            {
                // color_0 = white
                *curbyte++ = 0xff;
                *curbyte++ = 0xff;
                // color_1 = black
                *curbyte++ = 0;
                *curbyte++ = 0;
            }*/

/*            if (imagecopy[i*3] & 1)
                curbyteval |= 1;
            if (curtexel % 4 == 3)
            {
                *curbyte++ = curbyteval;
                curbyteval = 0;
            }
            curbyteval <<= 2;
            curtexel++;*/
            *curbyte++ = imagecopy[i*3];
        }

        tqtfile.Write(imgdata, image_size);
    }
    // Rewind to the TOC and  put the offset of this tile in there
    tqtfile.Seek(m_tocpos + TQT_TOCENTRYSIZE * node_index, nFile::START);
    n_printf("texture pos %d, size %d\n", texture_pos, image_size);
    tqtfile.PutInt(numimages);
    tqtfile.PutInt(texture_pos);
    tqtfile.PutInt(image_size*numimages);

    
    delete [] imgdata;
}
