#define N_IMPLEMENTS nPersistServer
#define N_KERNEL
//-------------------------------------------------------------------
//  npersistserver_cmds.cc
//  (C) 2002 RadonLabs GmbH
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/npersistserver.h"

static void n_setsavelevel(void *, nCmd *);
static void n_getsavelevel(void *, nCmd *);
static void n_setsaverclass(void*, nCmd*);
static void n_getsaverclass(void*, nCmd*);

//-------------------------------------------------------------------
//  CLASS
//  npersistserver
//  SUPERCLASS
//  nroot
//  INFO
//  The nPersistServer object can be found under /sys/servers/file 
//  and offers some commands to influence the way how objects 
//  are saved. All load/save operations of objects run finally through 
//  the fileserver. That's all you need to know about it... 
//  In addition the fileserver provides an interface to define and 
//  resolve assigns. 
//-------------------------------------------------------------------
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setsavelevel_i",  'SSLV', n_setsavelevel);
    cl->AddCmd("i_getsavelevel_v",  'GSLV', n_getsavelevel);
    cl->AddCmd("v_setsaverclass_s", 'SSCL', n_setsaverclass);
    cl->AddCmd("s_getsaverclass_v", 'GSCL', n_getsaverclass);
    cl->EndCmds();
}

//-------------------------------------------------------------------
//  CMD
//  setsavelevel
//  INPUT
//  i (SaveLevel)
//  OUTPUT
//  v
//  INFO
//  Set the save level which filters commands to be saved.
//  The default save level of 0 will let all commands through,
//  a save level of 1 will only let commands with a level of
//  1 or greater through. 
//  This is useful for selectively saving a set of object attributes
//  without having to write several save routines (savegames are
//  an example).
//-------------------------------------------------------------------
static void n_setsavelevel(void *o, nCmd *cmd)
{
    nPersistServer *self = (nPersistServer *) o;
    self->SetSaveLevel(cmd->In()->GetI());
}

//-------------------------------------------------------------------
//  CMD
//  getsavelevel
//  INPUT
//  v
//  OUTPUT
//  i (SaveLevel)
//  INFO
//  Get the current save level.
//-------------------------------------------------------------------
static void n_getsavelevel(void *o, nCmd *cmd)
{
    nPersistServer *self = (nPersistServer *) o;
    cmd->Out()->SetI(self->GetSaveLevel());
}

//-------------------------------------------------------------------
//  CMD
//  setsaverclass
//  INPUT
//  s(SaverClass)
//  OUTPUT
//  v
//  INFO
//  Set name of nScriptServer derived class which should be
//  used for saving objects.
//-------------------------------------------------------------------
static void n_setsaverclass(void* slf, nCmd* cmd)
{
    nPersistServer* self = (nPersistServer*) slf;
    self->SetSaverClass(cmd->In()->GetS());
}

//-------------------------------------------------------------------
//  CMD
//  getsaverclass
//  INPUT
//  v
//  OUTPUT
//  s(SaverClass)
//  INFO
//  Get class name of saver nScriptServer.
//-------------------------------------------------------------------
static void n_getsaverclass(void* slf, nCmd* cmd)
{
    nPersistServer* self = (nPersistServer*) slf;
    cmd->Out()->SetS(self->GetSaverClass());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
