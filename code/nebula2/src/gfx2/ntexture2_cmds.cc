//------------------------------------------------------------------------------
//  ntexture2_cmds.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/ntexture2.h"
#include "kernel/npersistserver.h"

static void n_savetexturetofile(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntexture2

    @cppclass
    ntexture2

    @superclass
    nresource

    @classinfo
    base texture class
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_savetexturetofile_s",   'STTF', n_savetexturetofile);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    savetexturetofile
    @input
    s(filename)
    @output
    b(success)
    @info
    Saves the texture to a file with the given filename
*/
static void
n_savetexturetofile(void* slf, nCmd* cmd)
{
    nTexture2* self = (nTexture2*) slf;
    cmd->Out()->SetB(self->SaveTextureToFile(cmd->In()->GetS(), nTexture2::JPG));
}
