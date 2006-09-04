//------------------------------------------------------------------------------
//  nsetpath.cc
//
//  Command line tool for creating all possible shaders from the shader DB
//  and compile them
//
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <stdio.h>
#include "tools/ncmdlineargs.h"
#include <windows.h>
#include "util/nstream.h"
#include "kernel/nfileserver2.h"
#include "shaderdb/nshaderdefinitionparser.h"
#include "shaderdb/nshaderwriter.h"
#include "shaderdb/nshaderparser.h"

//------------------------------------------------------------------------------
/**
*/
int main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    nString shaderArg       = args.GetStringArg("-fx", 0);
    bool helpArg                = args.GetBoolArg("-help");
    if (helpArg || shaderArg.IsValid() == false)
    {
        printf("ncompileshaders [-fx] [-help]\n"
               "(C) 2006 RadonLabs GmbH\n"
               "Compiles shaders from the shaderdb \n\n"
               "-fx		        - the shader xml file\n"
               "-help           - show this help\n");
        return 5;
    }
    nKernelServer kernelServer;
	nShaderParser parser;

	parser.Initialize(shaderArg);
	printf(parser.Parse().Get());
    return 0;
}


