//------------------------------------------------------------------------------
/**
    @page NebulaToolsnimagetool nimagetool

    nimagetool

    Nebula image processing tool.

    <dl>
     <dt>-in</dt>
       <dd>filename of input file</dd>
     <dt>-out</dt>
       <dd>filename of output file (extension must be @c .tga or @c .ntx)</dd>
     <dt>-mipmap</dt>
       <dd>generate mipmaps (only for @c .ntx output)</dd>
     <dt>-cubemap</dt>
       <dd>generate a cubemap</dd>
     <dt>-w</dt>
       <dd>target width</dd>
     <dt>-h</dt>
       <dd>target height</dd>
     <dt>-smalleronly</dt>
       <dd>only scale image if target size is smaller</dd>
     <dt>-flip</dt>
       <dd>flip image vertically</dd>
     <dt>-normcubemap</dt>
       <dd>generate a normalization cube map</dd>
     <dt>-specmap</dt>
       <dd>generate a specular map</dd>
    </dl>

    (C) 2003 RadonLabs GmbH
*/
#include <assert.h>
#include "il/il.h"
#include "il/ilu.h"
#include "kernel/nkernelserver.h"
#include "tools/ncmdlineargs.h"
#include "mathlib/vector.h"

const int numImages = 6;
const int posXIndex = 0;
const int negXIndex = 1;
const int posYIndex  = 2;
const int negYIndex  = 3;
const int posZIndex  = 4;
const int negZIndex  = 5;

//------------------------------------------------------------------------------
/**
    Create a new black 1x1 pixel RGB image.
*/
ILuint
CreateImage(bool white)
{
    unsigned char fillPixel[3] = { 0, 0, 0 };
    if (white)
    {
        fillPixel[0] = fillPixel[1] = fillPixel[2] = 0xff;
    }

    ILuint image = iluGenImage();
    ilBindImage(image);
    ilTexImage(1, 1, 1, 3, IL_BGR, IL_UNSIGNED_BYTE, &fillPixel);
    return image;
}

//------------------------------------------------------------------------------
/**
    Convert a vector3 to bgr and write to cube map face
*/
void
writeCubePixel(uchar* data, int width, int x, int y, float vx, float vy, float vz)
{
    int r = int(((vx * 0.5f) + 0.5f) * 255.0f);
    int g = int(((vy * 0.5f) + 0.5f) * 255.0f);
    int b = int(((vz * 0.5f) + 0.5f) * 255.0f);

    uchar* ptr = data + (((width * y) + x) * 3);
    ptr[0] = r;
    ptr[1] = g;
    ptr[2] = b;
}

//------------------------------------------------------------------------------
/**
    Create a normalization cube map.
*/
bool
CreateNormCubeMap(ILuint image[numImages], int size)
{
    // configure the 6 images and get pointer to image data
    uchar* dataPtr[numImages];
    int i;
    for (i = 0; i < numImages; i++)
    {
        ilBindImage(image[i]);
        ilTexImage(size, size, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, 0);
        dataPtr[i] = ilGetData();
    }

    int ix, iy;
    vector3 vec;
    float d = 2.0f / float(size);
    for (iy = 0; iy < size; iy++)
    {
        for (ix = 0; ix < size; ix++)
        {
            float y = 1.0f - float(iy) * d;
            float z = 1.0f - float(ix) * d;

            vec.set(1.0f, y, z);
            vec.norm();

            writeCubePixel(dataPtr[posXIndex], size, ix, iy, +vec.x, +vec.y, +vec.z);
            writeCubePixel(dataPtr[negXIndex], size, ix, iy, -vec.x, +vec.y, -vec.z);

            writeCubePixel(dataPtr[posYIndex], size, ix, iy, -vec.z, +vec.x, -vec.y);
            writeCubePixel(dataPtr[negYIndex], size, ix, iy, -vec.z, -vec.x, +vec.y);

            writeCubePixel(dataPtr[posZIndex], size, ix, iy, -vec.z, +vec.y, +vec.x);
            writeCubePixel(dataPtr[negZIndex], size, ix, iy, +vec.z, +vec.y, -vec.x);
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Creates a specular lookup table.
*/
bool
CreateSpecularMap(ILuint image, int width, int height)
{
    // bind and resize empty source image
    ilBindImage(image);
    ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, 0);
    unsigned char* data = ilGetData();

    // fill image data
    int x, y;
    float dx = 1.0f / float(width - 1);
    float dy = 1.0f / float(height - 1);
    double fy = 0.0;
    for (y = 0; y < height; y++, fy += dy)
    {
        double fx = 0.0;
        for (x = 0; x < width; x++, fx += dx)
        {
            uchar* ptr = data + (((width * y) + x) * 4);
            double val = pow(fx, double((height - 1) - y));
            int c = int(val * 255.0f);
            ptr[0] = c;
            ptr[1] = c;
            ptr[2] = c;
            ptr[3] = 255;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Load image from a file.
*/
bool
LoadImage(ILuint image, const char* filename)
{
    assert(filename);

    ilBindImage(image);
    if (!ilLoadImage((const ILstring) filename))
    {
        printf("nimagetool ERROR: could not load image file '%s'\n", filename);
        return false;
    }

    // convert to bgr byte order
    if ((ilGetInteger(IL_IMAGE_FORMAT) != IL_BGR) && (ilGetInteger(IL_IMAGE_FORMAT) != IL_BGRA))
    {
        if (ilGetInteger(IL_IMAGE_FORMAT) == IL_RGB)
        {
            ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
        }
        else
        {
            ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Scale an image. Will not scale if target size is identical to
    current size, or the smallerOnly flag is true, and the target
    size is smaller then the current size.
*/
void
ScaleImage(ILuint image, int width, int height, bool smallerOnly)
{
    ilBindImage(image);
    int curWidth  = ilGetInteger(IL_IMAGE_WIDTH);
    int curHeight = ilGetInteger(IL_IMAGE_HEIGHT);

    // do not scale if sizes are identical
    if ((curWidth == width) && (curHeight == height))
    {
        return;
    }

    // do not scale if target width smaller and smallerOnly is set
    if (smallerOnly && (curWidth < width) && (curHeight < height))
    {
        return;
    }

    // select filter based on current size (use a simple and
    // fast filter if current size is 1, which is the case if
    // the image is still in its default state after creation)
    if ((curWidth == 1) && (curHeight == 1))
    {
        iluImageParameter(ILU_FILTER, ILU_NEAREST);
    }
    else
    {
        iluImageParameter(ILU_FILTER, ILU_BILINEAR);
    }

    // do the actual scale
    iluScale(width, height, 1);
}

//------------------------------------------------------------------------------
/**
    Flip an image vertically.
*/
void
FlipImage(ILuint image)
{
    ilBindImage(image);
    iluFlipImage();
}

//------------------------------------------------------------------------------
/**
    Generate mipmaps for an image.
*/
void
GenerateMipmaps(ILuint image)
{
    ilBindImage(image);
    iluImageParameter(ILU_FILTER, ILU_BILINEAR);
    iluBuildMipmaps();
}

//------------------------------------------------------------------------------
/**
    Save an image as TGA file.
*/
bool
SaveImageTGA(ILuint image, const char* filename)
{
    assert(filename);

    ilBindImage(image);
    ilEnable(IL_FILE_OVERWRITE);
    ILboolean success = ilSave(IL_TGA, (const ILstring) filename);

    if (0 == success)
    {
        printf("nimagetool ERROR: Failed to save image '%s' as tga file!\n", filename);
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Main function.
*/
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    // get cmd line args
    const char* inFileArg[numImages];
    bool helpArg               = args.GetBoolArg("-help");
    inFileArg[0]               = args.GetStringArg("-in", 0);
    const char* outFileArg     = args.GetStringArg("-out", 0);
    inFileArg[posXIndex]       = args.GetStringArg("-inposx", inFileArg[0]);
    inFileArg[negXIndex]       = args.GetStringArg("-innegx", 0);
    inFileArg[posYIndex]       = args.GetStringArg("-inposy", 0);
    inFileArg[negYIndex]       = args.GetStringArg("-innegy", 0);
    inFileArg[posZIndex]       = args.GetStringArg("-inposz", 0);
    inFileArg[negZIndex]       = args.GetStringArg("-innegz", 0);
    bool cubeMapArg            = args.GetBoolArg("-cubemap");
    bool mipMapArg             = args.GetBoolArg("-mipmap");
    int widthArg               = args.GetIntArg("-w", 0);
    int heightArg              = args.GetIntArg("-h", 0);
    bool smallerOnlyArg        = args.GetBoolArg("-smalleronly");
    bool flipArg               = args.GetBoolArg("-flip");
    bool normCubeMapArg        = args.GetBoolArg("-normcubemap");
    bool specMapArg            = args.GetBoolArg("-specmap");
    bool whiteArg              = args.GetBoolArg("-white");

    // show help?
    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
               "nimagetool - Nebula2 image processing tool\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-in [filename]          name of input image file (preferably tga)\n"
               "-out [filename]         name of output image file (.tga extension)\n"
               "-inposx [filename]      optional input file for +x side of cube map\n"
               "-innegx [filename]      optional input file for -x side of cube map\n"
               "-inposy [filename]      optional input file for +y side of cube map\n"
               "-innegy [filename]      optional input file for -y side of cube map\n"
               "-inposz [filename]      optional input file for +z side of cube map\n"
               "-innegz [filename]      optional input file for -z side of cube map\n"
               "-w                      target width in pixels\n"
               "-h                      target height in pixels\n"
               "-smalleronly            only scale if target image is smaller\n"
               "-flip                   flip image vertically\n"
               "-specmap                generate a specular lookup map\n"
               "-white                  fill uninitialized images with white instead of black\n");
        return 5;
    }

    // check for arg errors
    if (0 == outFileArg)
    {
        printf("Error: no output file defined!\n");
        return 10;
    }

    // initialize Nebula runtime
    nKernelServer* kernelServer = new nKernelServer;

    // startup IL and ILU
    ilInit();
    iluInit();
    ilEnable(IL_CONV_PAL);

    // generate 6 empty images
    ILuint images[numImages];
    int i;
    for (i = 0; i < numImages; i++)
    {
        images[i] = CreateImage(whiteArg);
    }

    // generate source images
    bool success = false;
    if (normCubeMapArg)
    {
        n_assert(widthArg == heightArg);
        success = CreateNormCubeMap(images, widthArg);
    }
    else if (specMapArg)
    {
        success = CreateSpecularMap(images[0], widthArg, heightArg);
    }
    else if (cubeMapArg)
    {
        success = true;
        for (i = 0; i < numImages; i++)
        {
            if (inFileArg[i])
            {
                success &= LoadImage(images[i], inFileArg[i]);
            }
        }
    }
    else if (inFileArg[0])
    {
        success = LoadImage(images[0], inFileArg[0]);
    }

    // error during creation?
    if (!success)
    {
        return 10;
    }

    // scale images?
    if ((!normCubeMapArg) && (!specMapArg))
    {
        // If no target width and height is defined, set the size to the size of the first valid
        // image in the image array. This takes care of any leftover 1x1 images in
        // the cube maps (which are then scaled to the correct size)
        int width = widthArg;
        int height = heightArg;
        for (i = 0; i < numImages; i++)
        {
            ilBindImage(images[i]);
            int curWidth  = ilGetInteger(IL_IMAGE_WIDTH);
            int curHeight = ilGetInteger(IL_IMAGE_HEIGHT);
            if ((curWidth > 1) && (curHeight > 1))
            {
                width = curWidth;
                height = curHeight;
                break;
            }
        }

        for (i = 0; i < numImages; i++)
        {
            ScaleImage(images[i], width, height, smallerOnlyArg);
        }
    }

    // flip vertically?
    if (flipArg)
    {
        for (i = 0; i < numImages; i++)
        {
            FlipImage(images[i]);
        }
    }

    // generate mipmaps? this only works for non-cubemaps
    if (mipMapArg && (!normCubeMapArg) && (!cubeMapArg))
    {
        GenerateMipmaps(images[0]);
    }

    // save out target image
    success = false;
    if (cubeMapArg || normCubeMapArg)
    {
        // success = SaveCubeMapNTX(kernelServer, images, outFileArg);
    }
    else
    {
        nString path(outFileArg);
        if (path.CheckExtension("tga"))
        {
            success = SaveImageTGA(images[0], outFileArg);
        }
        else
        {
            printf("nimagetool ERROR: invalid file extension in out filename '%s'\n", outFileArg);
            return 10;
        }
    }

    // delete images
    for (i = 0; i < numImages; i++)
    {
        iluDeleteImage(images[i]);
    }
    ilShutDown();
    delete kernelServer;
    return 0;
}
