// Original maya->Nebula 1 exporter copyright 2003 Sane Asylum Studios, Inc.  All Rights Reserved.
// Nebula2 port copyright 2004 Rafael Van Daele-Hunt, but Johannes Kellner deserves credit
// for his 3DSMax->Nebula 2 exporter as well.
//--------------------------------------------------------------------------------
// You may choose to accept and redistribute this program under any of the following licenses:
//   * Nebula License (http://nebuladevice.sourceforge.net/doc/source/license.txt)
//   * MIT License (http://www.opensource.org/licenses/mit-license.php)
//   * GPL License (http://www.gnu.org/copyleft/gpl.html)
//   * LGPL License (http://www.gnu.org/copyleft/lgpl.html)

#include "nmaya/options.h"

static const char szOutputScript[] = "--script=";
static const char szOutputMesh[] = "--mesh=";
static const char szOutputAnim[] = "--anim=";
static const char szAnimStart[] = "--anim-start=";
static const char szAnimEnd[] = "--anim-end=";
static const char szAnimStep[] = "--anim-frame-step=";
static const char szVerbose[] = "--verbose";
static const char szVersion[] = "--version";
static const char szHelp[] = "--help";
static const char szSize[] = "--size=";

static const char szAnalyze[] = "--analyze";

Options::Options() :
    _szMayaInputFilename( "" ),
    _szScriptFilename( "" ),
    _szMeshFilename( "" ),
    _szAnimKeysFilename( "" ),
    _bVerbose( false ),
    _nKeyEveryNFrames( 1 ),
    _nStartFrame( 0 ),      // 0 means beginning of anim
    _nEndFrame( 0 ),        // 0 means end of anim
    _szSizeMult(""),
    _bAnalyze( false )
{
}


bool
Options::matchSwitchP( const char* szSwitch, const char* szProgSwitch ) const
{
    char* _;
    return matchSwitchP( szSwitch, szProgSwitch, _ );
}

bool
Options::matchSwitchP( const char* szSwitch, const char* szProgSwitch, const char* & szArg ) const
{
    bool bMatch = strncmp( szSwitch, szProgSwitch, strlen( szProgSwitch ) ) == 0;
    if ( bMatch )
    {
        szArg = szSwitch + strlen( szProgSwitch );
    }

    return bMatch;
}


void 
Options::usage() const
{
    std::cerr << "usage: nmaya <mayaFilename> --<output file type>=<filename> --<output params>\n\n" 
              << "output file types:\n"
              << "mesh (.n3d2 or .nvx2)\n"
              << "script (the .n2 script that will create the character;\n"
              << "        at the moment, you need one per animation)\n"
              << "anim (.nanim2 or .nax2)\n\n"
              << "notes on output files:\n"
              << "-multiple output files are permitted\n"
              << "-don't forget to include the extension for the output filename\n"
              << "\n"
              << "output parameters (optional and untested):\n"
              << "anim-start=<start frame>\n"
              << "anim-end=<end frame>\n"
              << "anim-frame-step=<step>\n"
              << "size (enter a size multiplikator to resize the model at export)\n"
              << "verbose\n"
              << "version\n"
              << "\n"
              << "Example:\n"
              << "nmaya c:\\\\maya\\\\scenes\\\\bob.mb --mesh=c:\\nebula2\\data\\bob.n3d2\n"
              << "Note that you must double the backslashes for the maya file\n";

    exit( 1 );
}

void
Options::parse( int argc, char* argv[] )
{
    if ( argc > 1 ) 
    {
        _szMayaInputFilename = argv[ 1 ];

        for ( int idxArg=1; idxArg<argc; ++idxArg )
        {
            const char* szSwitch = argv[ idxArg ];

            char* szArg;

            if ( matchSwitchP( szSwitch, szOutputScript, szArg ) ) {
                _szScriptFilename = szArg;
            } else if ( matchSwitchP( szSwitch, szOutputMesh, szArg ) ) {
                _szMeshFilename = szArg;
            } else if ( matchSwitchP( szSwitch, szAnimStep, szArg ) ) {
                _nKeyEveryNFrames = atoi( szArg );
                assert( _nKeyEveryNFrames > 0 );
            } else if ( matchSwitchP( szSwitch, szAnimStart, szArg ) ) {
                _nStartFrame = atoi( szArg );
                assert( _nStartFrame >= 0 );
            } else if ( matchSwitchP( szSwitch, szAnimEnd, szArg ) ) {
                _nEndFrame = atoi( szArg );
            } else if ( matchSwitchP( szSwitch, szOutputAnim, szArg ) ) {
                _szAnimKeysFilename = szArg;
            } else if ( matchSwitchP( szSwitch, szVerbose ) ) {
                _bVerbose = true;
            } else if ( matchSwitchP( szSwitch, szVersion ) ) {
                std::cerr << "nmaya v0.1.0\n";
                exit( 0 );
            } else if( matchSwitchP( szSwitch, szHelp, szArg ) ) {
                this->usage();
            } else if ( matchSwitchP( szSwitch, szAnalyze ) ) {
                _bAnalyze = true;
            } else if ( matchSwitchP( szSwitch, szSize, szArg ) ) {
                _szSizeMult = szArg;
            }
        }
    }
}

