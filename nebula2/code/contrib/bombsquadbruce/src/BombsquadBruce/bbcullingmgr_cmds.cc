//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbcullingmgr.h"
#include "kernel/npersistserver.h"

//static void n_setmap(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    bbcullingmgr

    @superclass
    nroot

    @classinfo
    Determines which objects are visible from a given perspective.

*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    //clazz->AddCmd("v_setmap_s",             'STRN', n_setmap);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
@cmd
setmap

@input
s

@output
v

@info
Tells the world which map (nMap) to use
*/
//static void n_setmap(void* slf, nCmd* cmd)
//{
//    BBWorld* self = (BBWorld*)slf;
//    self->SetMap(cmd->In()->GetS());
//}