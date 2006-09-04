//------------------------------------------------------------------------------
//  nvideoserver2_cmds.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "video/nvideoserver2.h"

//static void n_xxx(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nvideoserver

    @superclass
    nroot

    @classinfo
    An abstract video playback server object.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
//    cl->AddCmd("v_xxx_v", 'XXX', n_xxx);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    xxx
    @input
    v
    @output
    v
    @info
    Short description.
*/
//static void
//n_xxx(void* slf, nCmd* /*cmd*/)
//{
//    nVideoServer* self = (nVideoServer*) slf;
//    self->Open();
//}

