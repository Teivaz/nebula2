//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbactor.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
@scriptclass
bbactor

@superclass
nroot

@classinfo
A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
	clazz->BeginCmds();
	clazz->EndCmds();
}
//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
BBActor::SaveCmds(nPersistServer* ps)
{
    if (BBRoot::SaveCmds(ps))
    {
      /*
          nCmd* cmd = ps->GetCmd(this, 'XXXX');
                ps->PutCmd(cmd);*/
        

        return true;
    }
    return false;
}

