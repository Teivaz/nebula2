//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------

#include "BombsquadBruce/ccrenderer.h"
#include "kernel/npersistserver.h"

static void n_setscene(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ccrenderer
    
    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setscene_s", 'SSCN', n_setscene);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setscene

    @input
    s

    @output
    v

    @info
    Sets the active scene to attach.
*/
static void n_setscene(void* slf, nCmd* cmd)
{
    CCRenderer* self = (CCRenderer*)slf;
    self->SetScene(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool CCRenderer::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        ps->PutCmd(ps->GetCmd(this, 'SSCX'));
        ps->PutCmd(ps->GetCmd(this, 'SSAM'));
        ps->PutCmd(ps->GetCmd(this, 'SGUI'));
        ps->PutCmd(ps->GetCmd(this, 'SUTG'));

        return true;
    }
    return false;
}

