// implementation of heightfielddata class

#define N_IMPLEMENTS HeightFieldData

#include "clodterrain/heightfielddata.h"
#include "kernel/nfileserver2.h"

HeightFieldData::HeightFieldData(float xscale, float yscale, float zscale) 
: m_bitmapxsize(0), m_bitmapysize(0), m_logxsize(0), m_logysize(0),
  m_xscale(xscale), m_yscale(yscale), m_zscale(zscale), 
  m_activations(NULL), m_heights(NULL)
{ }

HeightFieldData::~HeightFieldData()
{
    if (m_activations) { delete[] m_activations; }
    if (m_heights) { delete[] m_heights; }
}

bool HeightFieldData::readBitmap(nString bitmapfilename)
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
    if (iformat != IL_LUMINANCE)
    {
        char *curformat;
        switch(iformat)
        {
        case IL_RGB: curformat = "RGB"; break;
        case IL_RGBA: curformat = "RGBA"; break;
        case IL_BGR: curformat = "BGR"; break;
        case IL_BGRA: curformat = "BGRA"; break;
        case IL_COLOR_INDEX: curformat = "color indexed"; break;
        default: curformat = "unknown"; break;
        }
        n_printf("Input file is %s--converting to greyscale. Some conversion loss may occur!\n", curformat);
        ilConvertImage(IL_LUMINANCE, itype);
    }

    ILint bit_depth = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);

    // Color depth must be 8 or 16 bits.
    if (bit_depth != 8 && bit_depth != 16) {
        n_printf("input file's bit depth is %d, which is not 8 or 16!\n", bit_depth);
        iluDeleteImage(pixelImage);
        return false;
    }

    if (bit_depth == 8)
        m_zscale /= 128.0;

    // make sure image is of size 2^N+1 in both dimensions
    ILuint width  = ilGetInteger(IL_IMAGE_WIDTH);
    ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);

    n_printf("image width = %ld, height = %ld, bit depth = %d\n", width, height, bit_depth);

    ILuint desiredwidth = nextPOW2p1(width);
    ILuint desiredheight = nextPOW2p1(height);
    if ( (desiredwidth != width) || (desiredheight != height) )
    {
        n_printf("image sizes are not 2^N+1, resizing.  Some conversion loss may occur!\n");
        iluScale(desiredwidth, desiredheight, 1);
        width = desiredwidth;
        height = desiredheight;
        n_printf("resized image width = %ld, height = %ld\n", width, height);

    }

    m_bitmapxsize = width;
    m_bitmapysize = height;
    m_logxsize = (unsigned int)(log2(m_bitmapxsize));
    m_logysize = (unsigned int)(log2(m_bitmapysize));
    n_assert(m_bitmapxsize == (1 << m_logxsize)+1u);
    n_assert(m_bitmapysize == (1 << m_logysize)+1u);

    // now dump data into the members
    ILubyte *ildata = ilGetData();
    m_heights = new Sint16[width * height];
    m_activations = new unsigned char [width * height];
    for (unsigned int ix=0; ix < width * height; ix++)
    {
        m_activations[ix] = 0;
        if (bit_depth == 8)
        {
            m_heights[ix] = ildata[ix] * 128;
        } else {
            m_heights[ix] = ildata[ix*2] + 256 * ildata[ix*2+1];
        }
    }

    iluDeleteImage(pixelImage);
    return true;
}

void HeightFieldData::dumpActivationLevels(nString dumpfilename, unsigned int max_activation)
{
    float colorscale = 255.0f/max_activation;
    unsigned char *rgbval = new unsigned char[m_bitmapxsize * m_bitmapysize * 3];
    for (unsigned int ix=0; ix < m_bitmapxsize * m_bitmapysize; ix++)
    {
        unsigned char curval = (unsigned char)(m_activations[ix] * colorscale);
        rgbval[ix*3] = curval;
        rgbval[ix*3+1] = curval;
        rgbval[ix*3+2] = curval;
    }
    ILuint pixelImage = iluGenImage();
    ilBindImage(pixelImage);
    ilTexImage(m_bitmapxsize, m_bitmapysize, 1,
            3, IL_RGB, IL_UNSIGNED_BYTE, rgbval);
    ilSaveImage((char *)dumpfilename.Get());
    iluDeleteImage(pixelImage);
    delete[] rgbval;
}

Sint16 HeightFieldData::height_query(unsigned int level, unsigned int x, unsigned int y, 
                        unsigned int ax, unsigned int ay, 
                        unsigned int rx, unsigned int ry, 
                        unsigned int lx, unsigned int ly)
// Returns the height of the query point (x,y) within the triangle (a, r, l),
// as tesselated to the specified LOD.
//
// Return value is in heightfield discrete coords.  To get meters,
// multiply by hf.vertical_scale.
{
    // If the query is on one of our verts, return that vert's height.
    if ((x == ax && y == ay)
         || (x == rx && y == ry)
         || (x == lx && y == ly))
    {
        return getHeight(x,y);
    }

    // Compute the coordinates of this triangle's base vertex.
    int dx = lx - rx;
    int dy = ly - ry;
    if (iabs(dx) <= 1 && iabs(dy) <= 1) {
        // We've reached the base level.  This is an error condition; we should
        // have gotten a successful test earlier.

        // assert(0);
        n_printf("Error: height_query hit base of heightfield.\n");

        return getHeight(ax, ay);
    }

    // base vert is midway between left and right verts.
    unsigned int    bx = rx + (dx >> 1);
    unsigned int    by = ry + (dy >> 1);

    // compute the length of a side edge.
    float   edge_length_squared = (dx * dx + dy * dy) / 2.f;

    float   sr, sl; // barycentric coords w/r/t the right and left edges.
    sr = ((x - ax) * (rx - ax) + (y - ay) * (ry - ay)) / edge_length_squared;
    sl = ((x - ax) * (lx - ax) + (y - ay) * (ly - ay)) / edge_length_squared;

    unsigned int base_vert_level = getActivation(bx, by);
    if (base_vert_level >= level){
        // The mesh is more tesselated at the desired LOD.  Recurse.
        if (sr >= sl) {
            // Query is in right child triangle.
            return height_query(level, x, y, bx, by, ax, ay, rx, ry);   // base, apex, right
        } else {
            // Query is in left child triangle.
            return height_query(level, x, y, bx, by, lx, ly, ax, ay);   // base, left, apex
        }
    }

    Sint16  az = getHeight(ax, ay);
    Sint16  dr = getHeight(rx, ry) - az;
    Sint16  dl = getHeight(lx, ly) - az;

    // This triangle is as far as the desired LOD goes.  Compute the
    // query's height on the triangle.
    Sint16 retval =  (Sint16)floor(az + sl * dl + sr * dr + 0.5);
    /*n_printf("height_query az=%d, sl=%f sr=%f, dl=%d, dr=%d, retval = %d\n",
        az,sl,sr,dl,dr, retval);*/
    return retval;
}




