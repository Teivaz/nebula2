//------------------------------------------------------------------------------
/**
    @page NebulaToolsnanimtool nanimtool

    nanimtool

    Command line tool for anim manipulation.

    @par -in
        input anim file (nanim2 or nax2 file)
    @par -out
        output anim file (nanim2 or nax2 file)

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "tools/ncmdlineargs.h"
#include "tools/nanimbuilder.h"

//------------------------------------------------------------------------------
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);
    nAnimBuilder anim;
    nAnimBuilder appendanim;

    // get cmd line args
    bool helpArg               = args.GetBoolArg("-help");
    nString inFileArg          = args.GetStringArg("-in", 0);
    nString outFileArg         = args.GetStringArg("-out", 0);

    // show help?
    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
            "nanimtool - Nebula2 anim file tool\n\n"
            "Command line args:\n"
            "------------------\n"
            "-help                 show this help\n"
            "-in [filename]        input anim file (.nanim2 or .nax2 extension)\n"
            "-out [filename]       output anim file (.nanim2 or .nax2 extension)\n");
        return 5;
    }

    // error if no input file given
    if (0 == inFileArg)
    {
        printf("nanimtool error: No input file! (type 'nanimtool -help' for help)\n");
        return 5;
    }

    // startup Nebula
    nKernelServer* kernelServer = new nKernelServer;

    // read input anim
    n_printf("-> loading anim '%s'\n", inFileArg);
    nString filename = inFileArg;
    if (!anim.Load(kernelServer->GetFileServer(), inFileArg.Get()))
    {
        n_printf("nanimtool error: Could not load '%s'\n", inFileArg.Get());
        delete kernelServer;
        return 5;
    }

    // save output anim
    if (outFileArg.IsValid())
    {
        n_printf("-> saving '%s'\n", outFileArg.Get());
        if (!anim.Save(kernelServer->GetFileServer(), outFileArg.Get()))
        {
            n_printf("Error: Could not save '%s'\n", outFileArg.Get());
            delete kernelServer;
            return 5;
        }
    }

    // success
    delete kernelServer;
    return 0;
}
