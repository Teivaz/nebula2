//------------------------------------------------------------------------------
//  nnpkfileserver_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nnpkfileserver.h"

static void n_parsedirectory(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nnpkfileserver

    @cppclass
    nNpkFileServer
    
    @superclass
    nfileserver2

    @classinfo
    Transparent filesystem access into npk package files.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("i_parsedirectory_s", 'PRSD', n_parsedirectory);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    parsedirectory

    @input
    s(AbsDirName)

    @output
    i(NumberOfNpkFilesInDir)

    @info
    Parse a directory for npk files, and add the files to the internal
    list of managed files.
*/
static
void
n_parsedirectory(void* slf, nCmd* cmd)
{
    nNpkFileServer* self = (nNpkFileServer*) slf;
    cmd->Out()->SetI(self->ParseDirectory(cmd->In()->GetS()));
}

