//------------------------------------------------------------------------------
//  nbumprev -file "path"
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "tools/ncmdlineargs.h"

//------------------------------------------------------------------------------
//  Creates or updates a C++ file of the format:
//
//  int Revision = XXX; const char* BuildTime = __TIMESTAMP__;
int
main(int argc, const char** argv)
{
    nKernelServer kernelServer;
    nFileServer2* fileServer = kernelServer.GetFileServer();
    nCmdLineArgs args(argc, argv);

    // parse args
    const char* filename = args.GetStringArg("-file", "rev.cc");

    // create file if it doesn't exist yet
    if (!fileServer->FileExists(filename))
    {
        nFile* newFile = fileServer->NewFileObject();
        if (newFile->Open(filename, "w"))
        {
            newFile->PutS("int Revision = 0; const char* BuildTime = __TIMESTAMP__;\n");
            newFile->Close();
        }
        newFile->Release();
    }

    int newRevision = 0;
    nFile* readFile = fileServer->NewFileObject();
    if (readFile->Open(filename, "r"))
    {
        char line[1024];
        readFile->GetS(line, sizeof(line));
        readFile->Close();

        nString tokenize = line;
        nArray<nString> tokens;
        tokenize.Tokenize(" \n\r;\"*=", tokens);
        n_assert(tokens.Size() == 7);

        const nString& revisionString = tokens[2];
        newRevision = atoi(revisionString.Get()) + 1;
    }
    else
    {
        n_printf("Could not open %s for reading!", filename);
    }
    readFile->Release();
    readFile = 0;

    nFile* writeFile = fileServer->NewFileObject();
    if (writeFile->Open(filename, "w"))
    {
        char buf[1024];
        sprintf(buf, "int Revision = %d; const char* BuildTime = __TIMESTAMP__;\n", newRevision);
        writeFile->PutS(buf);
        writeFile->Close();
    }
    else
    {
        n_printf("Could not open %s for writing!", filename);
    }
    writeFile->Release();
    writeFile = 0;
    return 0;
}

