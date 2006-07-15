//------------------------------------------------------------------------------
//  nmaketerraintexture.cc
//
//  Create a terrain texture from a bt file and a config file.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nct2tools/ngenterraintexture.h"
#include "tools/ncmdlineargs.h"

//------------------------------------------------------------------------------
int
main(int argc, const char** argv)
{
    nKernelServer kernelServer;
    nCmdLineArgs args(argc, argv);

    // get cmd line args
    bool helpArg        = args.GetBoolArg("-help");
    const char* confArg = args.GetStringArg("-conf", 0);

    if (helpArg)
    {
        n_printf("nmaketerraintexture [-help] [-in btfile] [-out tgafile] [-conf configfile] [-size tgasize]\n"
                 "Convert bt (binary terrain) file to tga file.\n"
                 "(C) 2003 RadonLabs GmbH\n\n"
                 "-help     -- display this help\n"
                 "-conf     -- xml config file\n");
        return 5;
    }

    // check args
    if (!confArg)
    {
        n_printf("-conf arg expected!\n");
        return 10;
    }

    // create and configure a GenTerrainTexture object
    nGenTerrainTexture genTexture(&kernelServer);
    genTexture.SetConfigFilename(confArg);
    if (!genTexture.Run())
    {
        n_printf("Generating texture failed with: %s\n", genTexture.GetError());
        return 10;
    }
    return 0;
}
