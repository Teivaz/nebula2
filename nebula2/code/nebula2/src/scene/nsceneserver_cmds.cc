//------------------------------------------------------------------------------
//  nsceneserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nsceneserver.h"

static void n_setrenderpathfilename(void* slf, nCmd* cmd);
static void n_getrenderpathfilename(void* slf, nCmd* cmd);
static void n_setobeylightlinks(void* slf, nCmd* cmd);
static void n_getobeylightlinks(void* slf, nCmd* cmd);
static void n_setrenderdebug(void* slf, nCmd* cmd);
static void n_getrenderdebug(void* slf, nCmd* cmd);
static void n_setocclusionquery(void* slf, nCmd* cmd);
static void n_getocclusionquery(void* slf, nCmd* cmd);
static void n_setclipplanefencing(void* slf, nCmd* cmd);
static void n_getclipplanefencing(void* slf, nCmd* cmd);

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
    cl->AddCmd("v_setrenderpathfilename_s",  'SRPF', n_setrenderpathfilename);
    cl->AddCmd("s_getrenderpathfilename_v",  'GRPF', n_getrenderpathfilename);
    cl->AddCmd("v_setobeylightlinks_b",      'SOLL', n_setobeylightlinks);
    cl->AddCmd("b_getobeylightlinks_v",      'GOLL', n_getobeylightlinks);
    cl->AddCmd("v_setrenderdebug_b",         'SDBG', n_setrenderdebug);
    cl->AddCmd("b_getrenderdebug_v",         'GDBG', n_getrenderdebug);
    cl->AddCmd("v_setocclusionquery_b",      'SOCQ', n_setocclusionquery);
    cl->AddCmd("b_getocclusionquery_v",      'GOCQ', n_getocclusionquery);
    cl->AddCmd("v_setclipplanefencing_b",    'SCPF', n_setclipplanefencing);
    cl->AddCmd("b_getclipplanefencing_v",    'GCPF', n_getclipplanefencing);
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

//------------------------------------------------------------------------------
/**
    @cmd
    setobeylightlinks
    @input
    b(ObeyLightLinks)
    @output
    v
    @info
    Obey light links, yes or no. If turned off, every shape in the
    scene will be lit by every light. If turned on, the application
    is responsible to generate bidirectional light links between
    render contexts.
*/
static void
n_setobeylightlinks(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    self->SetObeyLightLinks(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getobeylightlinks
    @input
    v
    @output
    b(ObeyLightLinks)
    @info
    Get the obey light links flag.
*/
static void
n_getobeylightlinks(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    cmd->Out()->SetB(self->GetObeyLightLinks());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrenderdebug
    @input
    b(RenderDebug)
    @output
    v
    @info
    Enable/disable debug visualization.
*/
static void
n_setrenderdebug(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    self->SetRenderDebug(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrenderdebug
    @input
    v
    @output
    b(RenderDebug)
    @info
    Get debug visualization flag.
*/
static void
n_getrenderdebug(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    cmd->Out()->SetB(self->GetRenderDebug());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setocclusionquery
    @input
    b(OcclusionQuery)
    @output
    v
    @info
    Enable/disable occlusion query.
*/
static void
n_setocclusionquery(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    self->SetOcclusionQuery(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getocclusionquery
    @input
    v
    @output
    b(OcclusionQuery)
    @info
    Get occlusion query flag.
*/
static void
n_getocclusionquery(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    cmd->Out()->SetB(self->GetOcclusionQuery());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setclipplanefencing
    @input
    b(ClipPlaneFencing)
    @output
    v
    @info
    Enable/disable clip plane fencing for point lights (default is on).
*/
static void
n_setclipplanefencing(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    self->SetClipPlaneFencing(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclipplanefencing
    @input
    v
    @output
    b(ClipPlaneFencing)
    @info
    Get clip plane fencing mode.
*/
static void
n_getclipplanefencing(void* slf, nCmd* cmd)
{
    nSceneServer* self = (nSceneServer*) slf;
    cmd->Out()->SetB(self->GetClipPlaneFencing());
}
