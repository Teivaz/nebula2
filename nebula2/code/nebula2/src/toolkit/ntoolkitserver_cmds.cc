//------------------------------------------------------------------------------
//  ntoolkitserver_cmds.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "toolkit/ntoolkitserver.h"

static void n_changeshaderparameter(void*, nCmd*);
static void n_dohotloading(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntoolkitserver

    @cppclass
    nToolkitServer

    @superclass
    nroot

    @classinfo
    remote access control
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("s_changeshaderparameter_ssss",  'SMSP', n_changeshaderparameter);
    cl->AddCmd("v_dohotloading_s",              'SMHL', n_dohotloading);
    cl->EndCmds();
}


//------------------------------------------------------------------------------
/**
    @cmd
    changeshaderparameter
    @input
    s(maya shader name)
    s(parameter handler)
    s(parameter ID)
    s(value)
    @output
    s
    @info
    Change a toolkit shader parameter (called by remoteControl)
*/
static void
n_changeshaderparameter(void *o, nCmd *cmd)
{
    nToolkitServer *self = (nToolkitServer*) o;
    nString shaderName = cmd->In()->GetS();
    nString handlerID = cmd->In()->GetS();
    nString parameterID = cmd->In()->GetS();
    nString value = cmd->In()->GetS();
    cmd->Out()->SetS(self->ChangeShaderParameter(shaderName, handlerID, parameterID, value).Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    loadobject
    @input
    s(object path)
    @output
    v
    @info
    Clear any exist objects in the viewer and load new objects.
*/
static void
n_dohotloading(void* o, nCmd *cmd)
{
    nToolkitServer *self = (nToolkitServer*) o;

    nString objPath = cmd->In()->GetS();
    self->DoHotLoading(objPath);
}
