//------------------------------------------------------------------------------
//  nsceneserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nsceneserver.h"

static void n_setrenderpathfilename(void* slf, nCmd* cmd);
static void n_getrenderpathfilename(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsceneserver
    
    @cppclass
    nSceneServer
    
    @superclass
    nroot

    @classinfo
    The scene server collects visible scene geometry and renders them in an 
    optimal way by sorting for minimal state switches.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setrenderpathfilename_s", 'SRPF', n_setrenderpathfilename);
    cl->AddCmd("s_getrenderpathfilename_v", 'GRPF', n_getrenderpathfilename);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrenderpathfilename
    @input
    s(RenderPathFilename)
    @output
    v
    @info
    Set the name to the XML file which contains the render path definition.
*/
static void
n_setrenderpathfilename(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    self->SetRenderPathFilename(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrenderpathfilename
    @input
    v
    @output
    s(MeshResource)
    @info
    Get the filename of the renderpath filename.
*/
static void
n_getrenderpathfilename(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    cmd->Out()->SetS(self->GetRenderPathFilename().Get());
}

