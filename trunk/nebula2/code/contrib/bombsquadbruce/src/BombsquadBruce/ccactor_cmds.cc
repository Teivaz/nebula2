//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccactor.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
@scriptclass
ccactor

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
CCActor::SaveCmds(nPersistServer* ps)
{
    if (CCRoot::SaveCmds(ps))
    {
      /*
          nCmd* cmd = ps->GetCmd(this, 'XXXX');
                ps->PutCmd(cmd);*/
        

        return true;
    }
    return false;
}

