//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "map/nbmpfile.h"
#include "map/nmap.h"

/**
    Load height map from image file.
    Set isDirty to false when done.
*/
void
nMap::LoadMap()
{
    if (!isDataDirty)
    {
        return;
    }


    const char * path = GetHeightMap();
    nString mangledPath = this->refFileServer->ManglePath(path);

    // Load heightmap from image
    if( !mangledPath.IsEmpty() )
    {
        if( !LoadFromImage(mangledPath) )
        {
            n_printf("nMap: Could not load height map '%s'\n", mangledPath.Get() );
            n_error("Aborting!\n");
        }
        else
        { // Process result
            CalculateNormals();
        }
    }
    else
    {
        n_printf("nMap: No image path specified\n");
        n_error("Aborting!\n");
    }

    isDataDirty = false;
}

/**
    Read in heightmap info from image file.
*/
bool
nMap::LoadFromImage(const nString& abs_path)
{
    bool retval = false;

    nBmpFile bmp_file;
    if (bmp_file.Open(abs_path.Get(), "rb"))
    {
        // get width and height
        int w = bmp_file.GetWidth();
        int h = bmp_file.GetHeight();
        n_assert(w == h);

        // Initialise map data
        mapDimension = w;
        if (pointData)
        {
            n_delete_array(pointData);
        }
        pointData = n_new_array(MapPoint, mapDimension * mapDimension);

        // allocate a line buffer for pixelformat conversion
        uchar* line_buf = (uchar*)n_malloc(w*3+4);

        // pixelformat of file
        nPixelFormat* file_pf = bmp_file.GetPixelFormat();

        // pixelformat of line buffer
        nPixelFormat line_pf(24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);

        // Load file line by line, pixelformat convert, use red component for height
        ushort min_height = 256;
        ushort max_height = 0;
        int act_y;
        uchar* l;
        line_pf.BeginConv(file_pf);
        while ((l = bmp_file.ReadLine(act_y))) {
            line_pf.Conv(l,line_buf, mapDimension);

            int y = mapDimension-1 - act_y;

            // Read single line
            for (int x = 0; x < mapDimension; ++x)
            {
                ushort highByte = line_buf[x*3 + 1];
                ushort lowByte  = line_buf[x*3 + 2];
                ushort dbl_byte = (highByte<<8) | lowByte;
                dbl_byte >>= 8;

                if (max_height < dbl_byte)
                {
                    max_height = dbl_byte;
                }
                if (min_height > dbl_byte)
                {
                    min_height = dbl_byte;
                }

                float height = heightMin + heightScale*float(dbl_byte);
                pointData[x + y*mapDimension].coord.set(gridInterval * float(x), height, gridInterval * float(y));
            }
        }

        line_pf.EndConv();
        n_free(line_buf);

        bmp_file.Close();

        // Figure bounding box
        boundingBox.vmin.set(0.0f,
                             heightMin + heightScale*float(min_height),
                             0.0f);
        boundingBox.vmax.set(gridInterval * mapDimension,
                             heightMin + heightScale*float(max_height),
                             gridInterval * mapDimension);

        retval = true;
    }

    return retval;
}

/**
    @brief Calculate normals.
    Takes account of grid spacing.

    Uses Sobel filter, hopefully properly.  Two kernels, for x and z:

          X                   Z

        -1   0   1           1   2   1
        -2   0   2           0   0   0
        -1   0   1          -1  -2  -1

*/
void
nMap::CalculateNormals()
{
    vector3 ns, we, normal;

    // Where inner postfix means towards origin and outer means away
    for (int z = 0; z < mapDimension; ++z)
    {
        int z_inner = z + (z != 0 ? -1 : 0);
        int z_outer = z + (z != mapDimension-1 ? 1 : 0);

        for (int x = 0; x < mapDimension; ++x)
        {
            int x_inner = x + ((x != 0) ? -1 : 0);
            int x_outer = x + ((x != mapDimension-1) ? 1 : 0);

            //  0  -X       +X
            // -Z  y00 y10 y20
            //     y01 y11 y21
            // +Z  y02 y12 y22
            float y00 = GetPoint(x_inner, z_inner).coord.y;
            float y01 = GetPoint(x_inner,       z).coord.y;
            float y02 = GetPoint(x_inner, z_outer).coord.y;
            float y10 = GetPoint(      x, z_inner).coord.y;
            float y11 = GetPoint(      x,       z).coord.y;
            float y12 = GetPoint(      x, z_outer).coord.y;
            float y20 = GetPoint(x_outer, z_inner).coord.y;
            float y21 = GetPoint(x_outer,       z).coord.y;
            float y22 = GetPoint(x_outer, z_outer).coord.y;

            // Calculate gradients
            float x_grad = -y00 - 2.0f * y01 - y02 + y20 + 2.0f * y21 + y22;
            x_grad /= 8.0f * gridInterval;

            float z_grad = -y00 - 2.0f * y10 - y20 + y02 + 2.0f * y12 + y22;
            z_grad /= 8.0f * gridInterval;

            float mag = x_grad * x_grad + z_grad * z_grad + 1.0f;
            mag = sqrtf(mag);

            normal.set(-x_grad/mag, 1.0f/mag, -z_grad/mag);

            pointData[x + z*mapDimension].normal = normal;
        }
    }
}
