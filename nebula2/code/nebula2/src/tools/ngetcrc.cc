//------------------------------------------------------------------------------
//  ngetcrc.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfileserver2.h"
#include "tools/ncmdlineargs.h"

//------------------------------------------------------------------------------
/**
    Returns the CRC checksum of file.
*/
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    bool helpArg        = args.GetBoolArg("-help");
    const char* fileArg = args.GetStringArg("-file", 0);
    const char* cmpArg  = args.GetStringArg("-cmp", 0);
    
    if (helpArg)
    {
        printf("(C) 2004 RadonLabs GmbH\n"
               "ngetcrc - Compute crc checksum for a file\n"
               "Command line args:\n"
               "------------------\n"
               "-help           show this help\n"
               "-file           the file name\n"
               "-cmp            compare crc with this file, return 0 if identical, 5 if not\n");
        return 5;
    }

    nKernelServer kernelServer;
    nFileServer2* fileServer = kernelServer.GetFileServer();
    uint crc0 = 0;
    uint crc1 = 0;
    if (fileArg)
    {
        bool success = fileServer->Checksum(fileArg, crc0);
        if (success)
        {
            printf("0x%lx\n", crc0);
        }
        else
        {
            printf("Error: Failed to open file '%s'!\n", fileArg);
            return 10;
        }
    }
    else
    {
        printf("Error: Filename argument expected!\n");
        return 10;
    }
    if (cmpArg)
    {
        bool success = fileServer->Checksum(cmpArg, crc1);
        if (success)
        {
            printf("0x%lx\n", crc1);
        }
        else
        {
            printf("Error: Failed to open file '%s'!\n", fileArg);
            return 10;
        } 
        if (crc0 == crc1)
        {
            printf("Files are identical\n");
            return 0;
        }
        else
        {
            printf("Files are different\n");
            return 5;
        }
    }
    return 0;
}
