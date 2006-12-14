//------------------------------------------------------------------------------
//  nbundler.cc
//
//  Nebula2 resource bundler
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "tools/ncmdlineargs.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "tools/nresourcecompiler.h"
#include "kernel/nscriptserver.h"

nNebulaUsePackage(nnebula);

//------------------------------------------------------------------------------
/**
    Open text file with one Nebula2 object name per line, and read it into
    a string array.
*/
bool
loadObjectList(nKernelServer* kernelServer, const nString& filename, nArray<nString>& array)
{
    n_assert(filename.IsValid());
    bool success = false;

    nFileServer2* fileServer = kernelServer->GetFileServer();
    nFile* file = fileServer->NewFileObject();
    if (file->Open(filename, "r"))
    {
        char line[1024];
        while (file->GetS(line, sizeof(line)))
        {
            // strip newline
            char* nl = strpbrk(line, "\n\r");
            if (nl)
            {
                *nl = 0;
            }
            array.Append(line);
        }
        file->Close();
        success = true;
    }
    file->Release();
    return success;
}

//------------------------------------------------------------------------------
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    // get cmd line args
    bool helpArg   = args.GetBoolArg("-help");
    bool binaryArg = args.GetBoolArg("-binary");
    bool waitArg   = args.GetBoolArg("-waitforkey");
    nString inArg                   = args.GetStringArg("-in");
    nString outArg                  = args.GetStringArg("-out");
    nString projDirArg              = args.GetStringArg("-projdir");
    nString scratchDirArg           = args.GetStringArg("-scratchdir", "c:/temp");
    nString scriptServerClassArg    = args.GetStringArg("-scriptserver", "ntclserver");

    // display help?
    if (helpArg)
    {
        n_printf("nbundler.cc [-help] [-in filename] [-out filename]\n"
                 "(C) 2003 RadonLabs GmbH"
                 "Nebula2 resource bundler.\n\n"
                 "-help         -- display this help\n"
                 "-projdir      -- project directory (ends with a slash!)\n"
                 "-scratchdir   -- scratch directory, default is c:/temp\n"
                 "-in           -- filename with list of input Nebula2 objects (1 filename per line)\n"
                 "-out          -- output base filename\n"
                 "-scriptserver -- optional script server class (default is ntclserver)\n"
                 "-binary       -- save Nebula object hierarchy in binary .n2 format\n"
                 "-waitforkey   -- wait for key press when finished\n");
        return 5;
    }

    // check arguments
    if (inArg.IsEmpty())
    {
        n_printf("Error: -in argument expected!\n");
        return 10;
    }
    if (outArg.IsEmpty())
    {
        n_printf("Error: -out argument expected!\n");
        return 10;
    }
    if (projDirArg.IsEmpty())
    {
        n_printf("Error: -projdir argument expected!\n");
        return 10;
    }

    // create and initialize Nebula environment
    nKernelServer kernelServer;
    kernelServer.AddPackage(nnebula);

    nScriptServer* scriptServer = (nScriptServer*)kernelServer.New(scriptServerClassArg.Get(), "/sys/servers/script");
    kernelServer.New("nvariableserver", "/sys/servers/variable");
    kernelServer.New("nanimationserver", "/sys/servers/anim");
    kernelServer.New("nresourceserver", "/sys/servers/resource");
    if (0 == scriptServer)
    {
        n_printf("Error: Could not create script server of class %s!\n", scriptServerClassArg);
        return 10;
    }
    // set proj: assign and run Nebula2 startup script
    if (!kernelServer.GetFileServer()->SetAssign("proj", projDirArg))
    {
        return 10;
    }
    nString result;
    if (!scriptServer->RunScript("home:bin/startup.tcl", result))
    {
        n_printf("Error: Could not execute 'home:bin/startup.tcl'!\n");
        return 10;
    }

    // get list of objects
    nArray<nString> objectArray;
    if (!loadObjectList(&kernelServer, inArg, objectArray))
    {
        n_printf("Error: could not open file %s\n", inArg);
        return 10;
    }

    // create and initialize resource compiler object
    int retval = 0;
    nResourceCompiler resComp;
    resComp.SetPath(nResourceCompiler::BaseFilename, outArg);
    resComp.SetPath(nResourceCompiler::ProjectDirectory, projDirArg);
    resComp.SetPath(nResourceCompiler::ScratchDirectory, scratchDirArg);
    resComp.SetBinaryFlag(binaryArg);
    if (resComp.Open(&kernelServer))
    {
        if (!resComp.Compile(objectArray))
        {
            n_printf("Error: resource compilation failed!\n");
            retval = 10;
        }

        // show summary
        n_printf("\n"
                "Meshes:     %.3f MB\n"
                "Animations: %.3f MB\n"
                "Textures:   %.3f MB\n",
                float(resComp.GetMeshDataSize()) / 1000000.0f,
                float(resComp.GetAnimDataSize()) / 1000000.0f,
                float(resComp.GetTextureDataSize()) / 1000000.0f);

        resComp.Close();
    }
    else
    {
        n_printf("Error: Could not initialize resource compiler object!\n");
        return 10;
    }

    // wait for user input?
    if (waitArg)
    {
        printf("\nPress <Enter> to continue: "); fflush(stdout);
        getc(stdin);
    }

    // all ok
    return retval;
}
