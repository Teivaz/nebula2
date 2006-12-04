//------------------------------------------------------------------------------
//  nchopbtfile.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nct2tools/nbtfile.h"
#include "tools/ncmdlineargs.h"
#include "util/nfixedarray.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
int 
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    // get cmd line args
    bool helpArg = args.GetBoolArg("-help");
    nString inArg = args.GetStringArg("-in", 0);
    nString outArg = args.GetStringArg("-out", "out.bt");
    int xArg = args.GetIntArg("-x", 0);
    int yArg = args.GetIntArg("-y", 0);
    int wArg = args.GetIntArg("-w", 0);
    int hArg = args.GetIntArg("-h", 0);
    bool infoArg = args.GetBoolArg("-info");

    if (helpArg)
    {
        printf("nchopbtfile [-help] [-in btfile] [-out btfile] [-x] [-y] [-w] [-h] [-info]\n"
               "Chop a piece out of a bt file.\n"
               "(C) 2004 RadonLabs GmbH\n"
               "-help   -- show this help\n"
               "-in     -- name of input file\n"
               "-out    -- name of output file\n"
               "-x      -- x coordinate\n"
               "-y      -- y coordinate\n"
               "-w      -- width\n"
               "-h      -- height\n"
               "-info   -- just display info about input file\n");
        return 5;
    }

    // check args
    if (inArg.IsEmpty())
    {
        printf("-in arg expected!");
        return 10;
    }
    if (!infoArg && ((wArg <= 0) || (hArg <= 0)))
    {
        printf("-w and -h must be greater 0!\n");
        return 10;
    }

    nKernelServer kernelServer;
    nBtFile btFile(&kernelServer);
    if (!btFile.Open(inArg.Get()))
    {
        printf("Could not open -in file '%s'\n", inArg.Get());
        return 10;
    }

    // print info
    vector3 size = btFile.GetSize();
    const vector3& heixelSize = btFile.GetHeixelSize();
    printf("dimension of file in meters: %f, %f, %f\n", size.x, size.y, size.z);
    printf("size of one heixel: %f, %f, %f\n", heixelSize.x, heixelSize.y, heixelSize.z);
    printf("num columns: %d\n", btFile.GetNumColumns());
    printf("num rows: %d\n", btFile.GetNumRows());
    printf("is float data: %s\n", btFile.IsFloatData() ? "yes" : "no");
    printf("data size: %d\n", btFile.GetDataSize());

    if (btFile.GetDataSize() != 2)
    {
        printf("Error: Data must be in 'short' format\n");
        return 10;
    }

    if (!infoArg)
    {
        // write out target file
        nFile* file = nFileServer2::Instance()->NewFileObject();
        if (file->Open(outArg, "wb"))
        {
            // write header
            char hdr[256] = { 0 };
            file->Write(hdr, sizeof(hdr));
            file->Seek(0, nFile::START);
            file->Write("binterr1.1", 10);
            file->PutInt(wArg);                 // Columns
            file->PutInt(hArg);                 // Rows
            file->PutShort(2);                  // Data size (short)
            file->PutShort(0);                  // Floating-point flag (no floats)
            file->PutShort(0);                  // Projection (ignored)
            file->PutShort(0);                  // UTM zone (ignored)
            file->PutShort(0);                  // Datum (ignored)
            file->PutDouble(heixelSize.x * wArg);   // Left extent (ignored)
            file->PutDouble(0.0);               // Right extent (ignored)
            file->PutDouble(0.0);               // Bottom extent (ignored)
            file->PutDouble(heixelSize.z * hArg);   // Top extent (ignored)
            file->PutShort(0);                  // projection
            file->PutFloat(heixelSize.y);       // vertical scale
            file->Seek(256, nFile::START);      // skip rest of header

            // write data
            // read data, col-wise
            nFixedArray<short> array(hArg);
            int x;
            for (x = 0; x < wArg; x++)
            {
                int y;
                for (y = 0; y < hArg; y++)
                {
                    int realX = x + xArg;
                    int realY = y + yArg;
                    short h = btFile.GetDirectShortValueAt(realY, realX);
                    array[y] = h;
                }
                file->Write(&array[0], hArg * sizeof(short));
            }
            file->Close();
        }
        else
        {
            printf("Could not open output file '%s'\n", outArg);
            file->Release();
            return 10;
        }
        file->Release();
    }
    btFile.Close();
    return 0;
}
