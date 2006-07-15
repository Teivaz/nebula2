//
// implementation for tileindexdata class


#include "il/ilu.h"
#include "clodterrain/tileindexdata.h"
#include "clodterrain/heightfielddata.h"

const int TileIndexData::maxtilebits = 8;
const int TileIndexData::maxtileindex = 1 << TileIndexData::maxtilebits;

TileIndexData::~TileIndexData()
{
    if (m_tileindices)
    {
         delete [] m_tileindices;
         m_tileindices = NULL;
    }

    if (m_tilepalette)
    {
        delete [] m_tilepalette;
        m_tilepalette = NULL;
    }
}

bool TileIndexData::readBitmap(nString bitmapfilename)
{
        // try to read in the image file
    ilInit();
    iluInit();
    ILuint pixelImage = iluGenImage();
    ilBindImage(pixelImage);

    if (!ilLoadImage((char*) bitmapfilename.Get()))
    {
        n_printf("DevIL failed loading image '%s' with '%s'.\n", bitmapfilename, iluErrorString(ilGetError()));
        iluDeleteImage(pixelImage);
        return false;
    }

    // make sure image is greyscale
    ILint iformat = ilGetInteger(IL_IMAGE_FORMAT);
    ILint itype = ilGetInteger(IL_IMAGE_TYPE);
    n_printf("Input file format %x, type %x\n", iformat, itype);
    if (iformat != IL_COLOR_INDEX)
    {
        char *curformat;
        switch(iformat)
        {
            case IL_RGB: curformat = "RGB"; break;
            case IL_RGBA: curformat = "RGBA"; break;
            case IL_BGR: curformat = "BGR"; break;
            case IL_BGRA: curformat = "BGRA"; break;
            case IL_LUMINANCE: curformat = "greyscale"; break;
            default: curformat = "unknown"; break;
        }
        n_printf("Input file is %s, needs to be color indexed for splatting\n", curformat);
        iluDeleteImage(pixelImage);
        return false;
    }

    ILint bit_depth = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);

    // Color depth must be 4 or 8 bits.
    if (bit_depth != 4 && bit_depth != 8) {
        n_printf("input tileindex file's bit depth is %d, which is not 4 or 8!\n", bit_depth);
        iluDeleteImage(pixelImage);
        return false;
    }

    // make sure image is of size 2^N in both dimensions
    ILuint width  = ilGetInteger(IL_IMAGE_WIDTH);
    ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);

    n_printf("image width = %ld, height = %ld, bit depth = %d\n", width, height, bit_depth);

    ILuint desiredwidth = nextPOW2p1(width);
    ILuint desiredheight = nextPOW2p1(height);
    if ( (desiredwidth != width) || (desiredheight != height) )
    {
        n_printf("image sizes are not 2^N+1!  Tileindex load failed\n");
        iluDeleteImage(pixelImage);
        return false;
    }

    m_bitmapxsize = width;
    m_bitmapysize = height;
    m_logxsize = (unsigned int)(log2(m_bitmapxsize));
    m_logysize = (unsigned int)(log2(m_bitmapysize));
    n_assert(m_bitmapxsize == (1 << m_logxsize) + 1u );
    n_assert(m_bitmapysize == (1 << m_logysize) + 1u );

    // now dump data into the members
    ILubyte *ildata = ilGetData();
    m_tileindices = new tileindex[width * height];
    for (unsigned int ix=0; ix < width * height; ix++)
    {
        m_tileindices[ix] = ildata[ix];
    }

    // copy over palette
    ILint iPaletteType = ilGetInteger(IL_PALETTE_TYPE);
    ILint iPaletteNumCols= ilGetInteger(IL_PALETTE_NUM_COLS);
    ILint iPaletteBPP = ilGetInteger(IL_PALETTE_BPP);
    m_tilepalette = new unsigned char[maxtileindex*3];
    unsigned int colormapping[3];
    bool getmapping = true;

    // start w/ a synthesized palette - the real palette overwrites entries
    for (int pix=0; pix < maxtileindex; pix++)
    {
        const int pixvalue = ( (255 * pix)/iPaletteNumCols ) % 256;
        m_tilepalette[pix*3 + 0] = pixvalue;
        m_tilepalette[pix*3 + 1] = pixvalue;
        m_tilepalette[pix*3 + 2] = pixvalue;
    }
    switch (iPaletteType)
    {
    case IL_PAL_RGB24: // normal palette mapping
           colormapping[0] = 0;
           colormapping[1] = 1;
           colormapping[2] = 2;
           break;
    case IL_PAL_BGR32:
            colormapping[0] = 2;
            colormapping[1] = 1;
            colormapping[2] = 0;
            break;
    default:
        n_printf("unknown palette type, synthesizing greyscale palette\n");
        getmapping = false;
    }
    if (getmapping)
    {
        ILubyte *ilpalette = ilGetPalette();
        for (int pix=0; pix < iPaletteNumCols; pix++)
        {
            for (int pe=0; pe<3; pe++)
                m_tilepalette[pix*3+pe] = ilpalette[pix*iPaletteBPP + colormapping[pe]];
        }
    }

    iluDeleteImage(pixelImage);
    return true;
}

/// get the number of different tile indices for a given triangle
unsigned int TileIndexData::countTileIndices(int ax, int ay,
                              int rx, int ry,
                              int lx, int ly)
{
    bool markbuffer[maxtileindex];

    return specifyTileIndices(ax,ay,rx,ry,lx,ly, markbuffer);
}

inline int isignum(int x)
{
    if (x < 0)
    { return -1;}
    else if (x > 0)
    { return 1;}
    else
    { return 0;}
}


/// given triangle, marks array elements to be 1 if the triangle contains any tiles of that index
// entries of tile indices not in the triangle are marked with 0.
// note that the array passed in should be at least TileIndexData::maxtileindex bytes long!
// @return the # of tiles contained by the triangle
unsigned int TileIndexData::specifyTileIndices(int ax, int ay,
                                int rx, int ry,
                                int lx, int ly,
                                bool *markarray)
{
    // zero out the array and recursive call
    for (int idx=0; idx < maxtileindex; idx++)
        markarray[idx] = false;

    tileindices_recursive(ax,ay,rx,ry,lx,ly, markarray);

    // special case to pull in the tiles one pixel beyond the diagonal; since linear
    // interpolation of texel occurs along the u and v axes, it turns out that tiles
    // one pixel beyond the diagonal can influence the interpolation value.
    if ( ((ax == rx) && (rx == lx)) ||
         ((ay == ry) && (ry == ly)) )
    {
        // degenerate case
    }
    else if ( (ax == rx) ||  (ax == lx) )
    {
        // type 1, where the line from rx/ry to lx/ly is the diagonal
        int newrx = rx + isignum(rx-ax);
        int newry = ry + isignum(ry-ay);
        int newlx = lx + isignum(lx-ax);
        int newly = ly + isignum(ly-ay);
        int newdx = isignum(newlx-newrx);
        int newdy = isignum(newly-newry);
        while (newrx != newlx)
        {
            int diagtile = getTileIndexClipped(newrx,newry);
            if (diagtile >= 0) markarray[diagtile] = true;
            newrx += newdx;
            newry += newdy;
        }
        n_assert(newry == newly);
    }
    else
    {
        // type 2, where from a->r and a->l are both diagonals
        int newax = ax + isignum(2 * ax - lx - rx);
        int neway = ay + isignum(2 * ay - ly - ry);
        int newrx = rx + isignum(rx - lx);
        int newry = ry + isignum(ry - ly);
        int newlx = lx + isignum(lx - rx);
        int newly = ly + isignum(ly - ry);
        int newax2 = newax, neway2 = neway;
        int rdx = isignum(newrx-newax), rdy = isignum(newry-neway);
        int ldx = isignum(newlx-newax), ldy = isignum(newly-neway);
        while (newax != newrx)
        {
            int diagtile1 = getTileIndexClipped(newax,neway);
            int diagtile2 = getTileIndexClipped(newax2, neway2);
            if (diagtile1 >= 0) markarray[diagtile1] = true;
            if (diagtile2 >= 0) markarray[diagtile2] = true;
            newax += rdx;
            neway += rdy;
            newax2 += ldx;
            neway2 += ldy;
        }
        n_assert(neway == newry);
        n_assert(newax2 == newlx);
        n_assert(neway2 == newly);

    }

    // count # of tileindices and return that
    unsigned int numindices = 0;
    for (int ix=0; ix < maxtileindex; ix++)
    {
        numindices += markarray[ix];
    }

    return numindices;
}

void TileIndexData::tileindices_recursive(int ax, int ay,
                                   int rx, int ry,
                                   int lx, int ly,
                                   bool *markarray)
{
    int dx = lx - rx;
    int dy = ly - ry;

    // if we're at the bottom, the triangle consists of three pixels;
    // just add the tile indices of those three pixels to the array
    // Compute the coordinates of this triangle's base vertex.
    if (iabs(dx) <= 1 && iabs(dy) <= 1) {
        // grab the tiles for each corner of the triangle
        int t1 = getTileIndex(ax,ay);
        int t2 = getTileIndex(rx,ry);
        int t3 = getTileIndex(lx,ly);
        n_assert((t1 >= 0) && (t1 < maxtileindex));
        n_assert((t2 >= 0) && (t2 < maxtileindex));
        n_assert((t3 >= 0) && (t3 < maxtileindex));
        markarray[t1] = true;
        markarray[t2] = true;
        markarray[t3] = true;
        return;
    }

    // otherwise, divide into two smaller triangles and add all the
    // tile indices of those two triangles together

    // base vert is midway between left and right verts.
    unsigned int    bx = rx + (dx >> 1);
    unsigned int    by = ry + (dy >> 1);

    tileindices_recursive(bx,by, ax,ay, rx,ry, markarray);
    tileindices_recursive(bx,by, lx,ly, ax,ay, markarray);  // hmmm, optimized tail call?
}


/// given a rectangle and a tile index, marks user bitmap such that spots with that tile index are true
// all other parts of the array are marked with false.  Markbitmap is row-major (x varies, then y)
// note that the user array should be of size (x2-x1)*(y1-y2) elements!
// @return the # of spots marked with true
unsigned int TileIndexData::makeTileMap(int x1, int y1,
                         int x2, int y2,
                         unsigned int targetindex,
                         bool *markbitmap)
{
    unsigned int markrowsize = (x2-x1);
    unsigned int markcount = 0;
    for (int ix=0; ix < (x2-x1); ix++)
    {
        for (int iy=0; iy < (y2-y1); iy++)
        {
            // what tile is it?
            unsigned int tilevalue = getTileIndex(ix+x1, iy+y1);
            if (tilevalue == targetindex)
            {
                markcount++;
                markbitmap[ix + iy * markrowsize] = true;
            }
            else
                markbitmap[ix + iy * markrowsize] = false;
        }
    }
    return markcount;
}


/// give the default RGB color for a give tile index
// result is placed in the 3-character array 'rgb'
void TileIndexData::getRGB(unsigned int tileindex, unsigned char *rgb)
{
    n_assert(tileindex >= 0);
    n_assert(tileindex < maxtileindex);

    for (int i=0; i<3; i++)
        rgb[i] = m_tilepalette[tileindex*3+i];
}

