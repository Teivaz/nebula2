//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
#include "nxsi/nxsioptions.h"
#include <tools/ncmdlineargs.h>
#include <iostream>
//-----------------------------------------------------------------------------

static const char paramOutputMesh[]   = "--mesh=";
static const char paramOutputAnim[]   = "--anim=";
static const char paramOutputShader[] = "--shader=";
static const char paramOutputScript[] = "--script=";
static const char paramOutputAuto[]   = "--auto";
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
    , shaderFilename("")
    , scriptFilename("")
    , scriptServerName("ntclserver")
    , isOutputAutomatic(false)
    , outputFlags(0)
{
}

//-----------------------------------------------------------------------------

void nXSIOptions::Usage() const
{
    std::cerr   << "\nusage: nxsi <xsiFilename> --<output file type>=<filename> --<options>\n\n" 
                << "output file types:\n"
                << "------------------\n"
                << "mesh (.n3d2 or .nvx2)\n"
                << "anim (.nanim2 or .nax2)\n"
//              << "shader (.fx)\n"
                << "script (the .n2 script that will create the model)\n"
                << "auto (creates all)\n\n"
                << "options:\n"
                << "--------\n"
                << "help (display this help)\n\n"
                << "Example:\n"
                << "nxsi c:\\\\xsi\\\\scenes\\\\cube.xsi --mesh=c:\\nebula2\\data\\cube.n3d2\n"
                << "nxsi c:\\\\xsi\\\\scenes\\\\cube.xsi --auto\n";

    exit(1);
}

void nXSIOptions::Parse(int argc, char* argv[])
{
	nCmdLineArgs cmdLine;
	cmdLine.Initialize(argc, argv);

    // check that we enough arguments
    if (argc <= 2)
    {
        this->Usage();
        return;
    }

    this->xsiFilename = argv[1];
    this->scriptFilename = this->xsiFilename;
    this->scriptFilename.StripExtension();
    this->scriptFilename.Append(".n2");

    // check params
    for (int i = 2; i < argc; ++i)
    {
        const char* currentSwitch = argv[i];
        char* argument;

        if (MatchSwitch(currentSwitch, paramOutputMesh, argument))
        {
            this->meshFilename = argument;
            this->outputFlags |= OUTPUT_MESH;
        }
        else if (MatchSwitch(currentSwitch, paramOutputAnim, argument))
        {
            this->animFilename = argument;
            this->outputFlags |= OUTPUT_ANIM;
        }
/*      else if (MatchSwitch(currentSwitch, paramOutputShader, argument))
        {
            m_shader_filename = argument;
        }*/
        else if (MatchSwitch(currentSwitch, paramOutputScript, argument))
        {
            this->scriptFilename.Set(argument);
            this->outputFlags |= OUTPUT_SCRIPT;
        }
        else if (MatchSwitch(currentSwitch, paramOutputAuto, argument))
        {
            this->isOutputAutomatic = true;
            this->outputFlags |= OUTPUT_MESH | OUTPUT_ANIM | OUTPUT_SCRIPT | OUTPUT_SHADER;
        }
        else if (MatchSwitch(currentSwitch, paramScriptServer, argument))
        {
            this->scriptServerName.Set(argument);
        }
        else if( MatchSwitch(currentSwitch, paramHelp, argument))
        {
            this->Usage();
        }
    }

    if (this->isOutputAutomatic)
    {
        nPathString filename = this->xsiFilename;
        filename.StripExtension();

        this->scriptFilename = filename;
        this->scriptFilename.Append(".n2");

        this->meshFilename = filename;
        this->meshFilename.Append(".n3d2");

        this->animFilename = filename;
        this->animFilename.Append(".nanim2");
    }
}

//-----------------------------------------------------------------------------

bool nXSIOptions::MatchSwitch(const char* currentSwitch, const char* appSwitch) const
{
    char* _;
    return MatchSwitch(currentSwitch, appSwitch, _);
}

bool nXSIOptions::MatchSwitch(const char* currentSwitch, const char* appSwitch, const char* &argument) const
{
    bool isMatch = strncmp(currentSwitch, appSwitch, strlen(appSwitch)) == 0;
    if (isMatch) {
        argument = currentSwitch + strlen(appSwitch);
    }

    return isMatch;
}

//-----------------------------------------------------------------------------
// Eof