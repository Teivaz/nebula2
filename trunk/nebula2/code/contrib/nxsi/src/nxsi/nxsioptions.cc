//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
#include "nxsi/nxsioptions.h"
#include <tools/ncmdlineargs.h>

//-----------------------------------------------------------------------------

static const char paramOutputMesh[]   = "--mesh=";
static const char paramOutputAnim[]   = "--anim=";
static const char paramOutputScript[] = "--script=";
static const char paramOutputAuto[]   = "--auto";
static const char paramMergeAll[]     = "--mergeall";
static const char paramBinary[]       = "--binary";
static const char paramScriptServer[] = "--scriptserver=";
static const char paramHelp[]         = "--help";

//-----------------------------------------------------------------------------

nXSIOptions::~nXSIOptions()
{
}

nXSIOptions::nXSIOptions()
    : xsiFilename("")
    , meshFilename("")
    , animFilename("")
    , scriptFilename("")
    , scriptServerName("")
    , outputFlags(0)
{
}

//-----------------------------------------------------------------------------

void nXSIOptions::Usage() const
{
    n_printf("usage: nxsi <xsiFilename> --<output type>\n\n"
             "output types:\n"
             "-------------\n"
             "mesh=filename     (merges meshes in one .n3d2 or .nvx2 file)\n"
             "anim=filename     (merges anims in one .nanim2 or .nax2 file)\n"
             "script=filename   (creates the .n2 script that will create the model)\n\n"
             "auto              (creates all and names them automatically)\n"
             "mergeall          (merges all meshes and anims in one file)\n"
             "binary            (creates meshes and anims in binary format)\n\n"
             "scriptserver=name (default: ntclserver)\n"
             "help              (display this help)\n\n"
             "Example:\n"
             "nxsi cube.xsi --mesh=cube.n3d2  (will create only mesh file)\n"
             "nxsi cube.xsi --auto            (will create all automatically)\n\n");

    exit(1);
}

void nXSIOptions::Parse(int argc, char* argv[])
{
    nCmdLineArgs cmdLine;
    cmdLine.Initialize(argc, (const char**)argv);
    this->outputFlags = 0;

    // print one empty line
    n_printf("\n");

    // check that we enough arguments
    if (argc <= 2)
    {
        this->Usage();
        return;
    }

    // get xsi filename
    this->xsiFilename = argv[1];

    // check script server
    this->scriptServerName = cmdLine.GetStringArg(paramScriptServer, "ntclserver");

    // check if help wanted
    if (cmdLine.GetBoolArg(paramHelp))
    {
        this->Usage();
        return;
    }

    // check if merge all requested
    if (cmdLine.GetBoolArg(paramMergeAll))
    {
        this->outputFlags |= OUTPUT_MERGEALL;
    }

    // check if automatic
    if (cmdLine.GetBoolArg(paramOutputAuto))
    {
        this->outputFlags |= OUTPUT_MESH | OUTPUT_ANIM | OUTPUT_SCRIPT | OUTPUT_AUTO;

        nString filename = this->xsiFilename;
        filename.StripExtension();

        this->scriptFilename = filename;
        this->scriptFilename.Append(".n2");

        this->meshFilename = filename;
        this->meshFilename.Append(".n3d2");

        this->animFilename = filename;
        this->animFilename.Append(".nanim2");
    }
    else
    {
        this->outputFlags |= OUTPUT_MERGEALL;

        this->meshFilename = cmdLine.GetStringArg(paramOutputMesh, 0);
        if (this->meshFilename.Get()) this->outputFlags |= OUTPUT_MESH;

        this->animFilename = cmdLine.GetStringArg(paramOutputAnim, 0);
        if (this->animFilename.Get()) this->outputFlags |= OUTPUT_ANIM;

        this->scriptFilename = cmdLine.GetStringArg(paramOutputScript, 0);
        if (this->scriptFilename.Get()) this->outputFlags |= OUTPUT_SCRIPT;
    }

    // check if binary mode requested
    if (cmdLine.GetBoolArg(paramBinary))
    {
        this->outputFlags |= OUTPUT_BINARY;

        this->meshFilename.StripExtension();
        this->meshFilename.Append(".nvx2");

        this->animFilename.StripExtension();
        this->animFilename.Append(".nax2");
    }
}

//-----------------------------------------------------------------------------
// Eof