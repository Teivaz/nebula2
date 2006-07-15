//------------------------------------------------------------------------------
//  nbttotga.cc
//
//  Converts a bt (binary terrain) file to a tga greyscale image or a 
//  tga normal map.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nct2tools/nbtfile.h"
#include "nct2tools/ntgafile.h"
#include "tools/ncmdlineargs.h"

//------------------------------------------------------------------------------
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    // get cmd line args
    bool helpArg        = args.GetBoolArg("-help");
    const char* inArg   = args.GetStringArg("-in", 0);
    const char* outArg  = args.GetStringArg("-out", 0);
    int sizeArg         = args.GetIntArg("-size", 256);
    bool normalMapArg   = args.GetBoolArg("-nmap");
    float vscaleArg     = args.GetFloatArg("-vscale", 10000.0f);

    if (helpArg)
    {
        printf("nbttotga [-help] [-in btfile] [-out tgafile] [-size tgasize] [-nmap]\n"
               "Convert bt (binary terrain) file to tga file.\n"
               "(C) 2003 RadonLabs GmbH\n\n"
               "-help     -- display this help\n"
               "-in       -- name of input bt file\n"
               "-out      -- name of output tga file\n"
               "-size     -- size of tga file\n"
               "-nmap     -- generate normal map (otherwise height map)\n"
               "-vscale   -- vertical scale (default is 10000)\n");
        return 5;
    }

    // check args
    if (!inArg)
    {
        printf("-in arg expected!\n");
        return 10;
    }
    if (!outArg)
    {
        printf("-out arg expected!\n");
        return 10;
    }

    nKernelServer kernelServer;

    // open the bt file
    nBtFile btFile(&kernelServer);
    if (!btFile.Open(inArg))
    {
        printf("Could not open bt file '%s' for reading.\n", inArg);
        return 10;
    }
    
    // open the tga file
    nTgaFile tgaFile(&kernelServer);
    tgaFile.SetWidth(sizeArg);
    tgaFile.SetHeight(sizeArg);
    if (!tgaFile.OpenWrite(outArg))
    {
        printf("Could not open tga file for writing '%s'.\n", outArg);
        return 10;
    }

    uint* imgData = n_new_array(uint, sizeArg * sizeArg);

    // conversion loop
    vector3 btSize = btFile.GetSize();
    float dx = btSize.x / float(sizeArg);
    float dz = btSize.z / float(sizeArg);
    int iz;
    float fz = 0.0f;
    for (iz = 0; iz < sizeArg; iz++, fz+=dz)
    {
        int ix;
        float fx = 0.0f;
        for (ix = 0; ix < sizeArg; ix++, fx+=dx)
        {
            if (normalMapArg)
            {
                vector3 n = btFile.SampleNormal(fx, fz);
                int inx = int(((n.x + 1.0f) * 0.5f) * 255.0f);
                int iny = int(((n.y + 1.0f) * 0.5f) * 255.0f);
                int inz = int(((n.z + 1.0f) * 0.5f) * 255.0f);
                uint argb = N_ARGB(255, inx, inz, iny);
                imgData[iz * sizeArg + ix] = argb;
            }
            else
            {
                float h = btFile.SampleHeight(fx, fz);
                int ih = int((h / vscaleArg) * 255.0f);
                ih = n_iclamp(ih, 0, 255);
                uint argb = N_ARGB(255, ih, ih, ih);
                imgData[iz * sizeArg + ix] = argb;
            }
        }
    }

    // write image data to tga file
    tgaFile.WriteChunk(0, 0, sizeArg, sizeArg, (char*) imgData);

    // close and exit
    n_delete_array(imgData);
    tgaFile.Close();
    btFile.Close();

    return 0;
}
