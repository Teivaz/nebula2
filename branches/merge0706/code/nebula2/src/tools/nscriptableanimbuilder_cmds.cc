//------------------------------------------------------------------------------
//  nscriptableanimbuilder_cmds.cc
//
//  (C)2005 Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "tools/nscriptableanimbuilder.h"
#include "kernel/nfileserver2.h"

static void n_saveanim(void* slf, nCmd* cmd);

static void n_setlooptype(void* slf, nCmd* cmd);

static void n_begingroup(void* slf, nCmd* cmd);
static void n_addcurve(void* slf, nCmd* cmd);
static void n_addkey(void* slf, nCmd* cmd);
static void n_endgroup(void* slf, nCmd* cmd);

static void n_getnumgroups(void* slf, nCmd* cmd);
static void n_optimize(void* slf, nCmd* cmd);
static void n_clear(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();

    cl->AddCmd("b_saveanim_s",     'SASA', n_saveanim);
    cl->AddCmd("v_setlooptype_is", 'SETL', n_setlooptype);
    cl->AddCmd("i_begingroup_v",   'BEGG', n_begingroup);
    cl->AddCmd("i_addcurve_i",     'ADDC', n_addcurve);
    cl->AddCmd("v_addkey_iiiffff", 'ADDK', n_addkey);
    cl->AddCmd("v_endgroup_v",     'ENDG', n_endgroup);
    cl->AddCmd("i_getnumgroups_v", 'GTNG', n_getnumgroups);
    cl->AddCmd("i_optimize_v",     'OPTI', n_optimize);
    cl->AddCmd("v_clear_v",        'CLER', n_clear);

    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_saveanim(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);
    //void* server = cmd->In()->GetO();
    const char* filename = cmd->In()->GetS();
    //nFileServer2*   fileServer = static_cast<nFileServer2*>(server);

    bool ret = self->nAnimBuilder::Save(nFileServer2::Instance(), filename);

    cmd->Out()->SetB(ret);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_setlooptype(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);

    int groupIndex = cmd->In()->GetI();
    const char* type = cmd->In()->GetS();

    self->SetLoopType(groupIndex, type);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_begingroup(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);

    int groupIndex = self->BeginGroup();
    cmd->Out()->SetI(groupIndex);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addcurve(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);

    int index = cmd->In()->GetI();

    int curveIndex = self->AddCurve(index);

    cmd->Out()->SetI(curveIndex);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addkey(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);

    int groupIndex = cmd->In()->GetI();
    int curveIndex = cmd->In()->GetI();
    int keyIndex   = cmd->In()->GetI();
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    float w = cmd->In()->GetF();

    self->AddKey(groupIndex, curveIndex, keyIndex, x, y, z, w);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_endgroup(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);

    self->EndGroup();
}

//------------------------------------------------------------------------------
/**
*/
static
void n_getnumgroups(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);

    int numGroups = self->GetNumGroups();
    cmd->Out()->SetI(numGroups);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_optimize(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);

    int ret = self->Optimize();
    cmd->Out()->SetI(ret);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_clear(void* slf, nCmd* cmd)
{
    nScriptableAnimBuilder* self = static_cast<nScriptableAnimBuilder*>(slf);

    self->Clear();
}