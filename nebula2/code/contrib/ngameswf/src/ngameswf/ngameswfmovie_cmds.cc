//------------------------------------------------------------------------------
//  ngameswfmovie_cmds.cc
//  (C) 2004 Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "ngameswf/ngameswfmovie.h"
#include "kernel/npersistserver.h"

static void n_createmovie (void* slf, nCmd* cmd);
static void n_createlibrarymovie (void* slf, nCmd* cmd);
static void n_setbackgroundalpha (void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds ();
    cl->AddCmd ("b_createmovie_s", 'CREA', n_createmovie);
    cl->AddCmd ("b_createlibrarymovie_s", 'CRFL', n_createlibrarymovie);
    cl->AddCmd ("v_setbackgroundalpha_f", 'SETA', n_setbackgroundalpha);
    cl->EndCmds ();
}

//------------------------------------------------------------------------------
/**
*/
static void 
n_createmovie (void* slf, nCmd* cmd)
{
    nGameSwfMovie* self = (nGameSwfMovie*)slf;
    bool ret = self->CreateMovie (cmd->In()->GetS());
    cmd->Out()->SetB(ret);
}

//------------------------------------------------------------------------------
/**
*/
static void
n_createlibrarymovie (void* slf, nCmd* cmd)
{
    nGameSwfMovie* self = (nGameSwfMovie*)slf;
    bool ret = self->CreateLibraryMovie (cmd->In()->GetS());
    cmd->Out()->SetB(ret);
}

//------------------------------------------------------------------------------
/**
*/
static void 
n_setbackgroundalpha (void* slf, nCmd* cmd)
{
    nGameSwfMovie* self = (nGameSwfMovie*)slf;
    self->SetBackgroundAlpha (cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
*/
bool nGameSwfMovie::SaveCmds(nPersistServer* ps)
{
    if(nRoot::SaveCmds (ps))
    {
        return true;
    }

    return false;
}
