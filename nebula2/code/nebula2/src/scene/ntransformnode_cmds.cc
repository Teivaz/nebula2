#define N_IMPLEMENTS nTransformNode
//------------------------------------------------------------------------------
//  ntransformnode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntransformnode.h"
#include "kernel/npersistserver.h"

static void n_setviewspace(void* slf, nCmd* cmd);
static void n_getviewspace(void* slf, nCmd* cmd);
static void n_setactive(void* slf, nCmd* cmd);
static void n_getactive(void* slf, nCmd* cmd);
static void n_setposition(void* slf, nCmd* cmd);
static void n_getposition(void* slf, nCmd* cmd);
static void n_setposx(void* slf, nCmd* cmd);
static void n_setposy(void* slf, nCmd* cmd);
static void n_setposz(void* slf, nCmd* cmd);
static void n_seteuler(void* slf, nCmd* cmd);
static void n_geteuler(void* slf, nCmd* cmd);
static void n_seteulerx(void* slf, nCmd* cmd);
static void n_seteulery(void* slf, nCmd* cmd);
static void n_seteulerz(void* slf, nCmd* cmd);
static void n_setquat(void* slf, nCmd* cmd);
static void n_getquat(void* slf, nCmd* cmd);
static void n_setscale(void* slf, nCmd* cmd);
static void n_getscale(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntransformnode
    
    @superclass
    nscenenode

    @classinfo
    Parent class of scene graph nodes. nSceneNode objects can form complex
    hierarchies and cross-hierarchy dependencies.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setviewspace_b",      'SVWS', n_setviewspace);
    cl->AddCmd("b_getviewspace_v",      'GVWS', n_getviewspace);
    cl->AddCmd("v_setactive_b",         'SACT', n_setactive);
    cl->AddCmd("b_getactive_v",         'GACT', n_getactive);
    cl->AddCmd("v_setposition_fff",     'SPOS', n_setposition);
    cl->AddCmd("v_setposx_f",           'SPOX', n_setposx);
    cl->AddCmd("v_setposy_f",           'SPOY', n_setposy);
    cl->AddCmd("v_setposz_f",           'SPOZ', n_setposz);
    cl->AddCmd("fff_getposition_v",     'GPOS', n_getposition);
    cl->AddCmd("v_seteuler_fff",        'SEUL', n_seteuler);
    cl->AddCmd("v_seteulerx_f",         'SEUX', n_seteulerx);
    cl->AddCmd("v_seteulery_f",         'SEUY', n_seteulery);
    cl->AddCmd("v_seteulerz_f",         'SEUZ', n_seteulerz);
    cl->AddCmd("fff_geteuler_v",        'GEUL', n_geteuler);
    cl->AddCmd("v_setquat_ffff",        'SQUT', n_setquat);
    cl->AddCmd("ffff_getquat_v",        'GQUT', n_getquat);
    cl->AddCmd("v_setscale_fff",        'SSCL', n_setscale);
    cl->AddCmd("fff_getscale_v",        'GSCL', n_getscale);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setviewspace
    @input
    b(ViewSpace)
    @output
    v
    @info
    Set to true if this node lives in view space, default is false 
    (hierarchy space).
*/
static void
n_setviewspace(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    self->SetViewSpace(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getviewspace
    @input
    v
    @output
    b(ViewSpace)
    @info
    Return view space flag.
*/
static void
n_getviewspace(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    cmd->Out()->SetB(self->GetViewSpace());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setactive
    @input
    b(Active)
    @output
    v
    @info
    Set to true if this node is active (default is true).
*/
static void
n_setactive(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    self->SetActive(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getactive
    @input
    b(Active)
    @output
    v
    @info
    Return active flag.
*/
static void
n_getactive(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    cmd->Out()->SetB(self->GetActive());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setposition
    @input
    f(X), f(Y), f(Z)
    @output
    v
    @info
    Set position of node.
*/
static void
n_setposition(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->SetPosition(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setposx
    @input
    f(X)
    @output
    v
    @info
    Set x position of node.
*/
static void
n_setposx(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v = self->GetPosition();
    v.x = cmd->In()->GetF();
    self->SetPosition(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setposy
    @input
    f(Y)
    @output
    v
    @info
    Set y position of node.
*/
static void
n_setposy(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v = self->GetPosition();
    v.y = cmd->In()->GetF();
    self->SetPosition(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setposz
    @input
    f(Z)
    @output
    v
    @info
    Set z position of node.
*/
static void
n_setposz(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v = self->GetPosition();
    v.z = cmd->In()->GetF();
    self->SetPosition(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getposition
    @input
    v
    @output
    f(X), f(Y), f(Z)
    @info
    Get position of node.
*/
static void
n_getposition(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    const vector3& v = self->GetPosition();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    seteuler
    @input
    f(X), f(Y), f(Z)
    @output
    v
    @info
    Set euler orientation of the node as euler. Units are degree (not rad)

*/
static void
n_seteuler(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v;
    v.x = n_deg2rad(cmd->In()->GetF());
    v.y = n_deg2rad(cmd->In()->GetF());
    v.z = n_deg2rad(cmd->In()->GetF());
    self->SetEuler(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    seteulerx
    @input
    f(X)
    @output
    v
    @info
    Set X euler angle.
*/
static void
n_seteulerx(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v = self->GetEuler();
    v.x = n_deg2rad(cmd->In()->GetF());
    self->SetEuler(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    seteulery
    @input
    f(Y)
    @output
    v
    @info
    Set Y euler angle.
*/
static void
n_seteulery(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v = self->GetEuler();
    v.y = n_deg2rad(cmd->In()->GetF());
    self->SetEuler(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    seteulerz
    @input
    f(Z)
    @output
    v
    @info
    Set Z euler angle.
*/
static void
n_seteulerz(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v = self->GetEuler();
    v.z = n_deg2rad(cmd->In()->GetF());
    self->SetEuler(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    geteuler
    @input
    v
    @output
    f(X), f(Y), f(Z)
    @info
    Get the euler orientation of the node.
*/
static void
n_geteuler(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    const vector3& v = self->GetEuler();
    cmd->Out()->SetF(n_rad2deg(v.x));
    cmd->Out()->SetF(n_rad2deg(v.y));
    cmd->Out()->SetF(n_rad2deg(v.z));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setquat
    @input
    f(X), f(Y), f(Z), f(W)
    @output
    v
    @info
    Set the quaternion orientation of the node.
*/
static void
n_setquat(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    quaternion q;
    q.x = cmd->In()->GetF();
    q.y = cmd->In()->GetF();
    q.z = cmd->In()->GetF();
    q.w = cmd->In()->GetF();
    self->SetQuat(q);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getquat
    @input
    v
    @output
    f(X), f(Y), f(Z), f(W)
    @info
    Get quaternion orientation of the node.
*/
static void
n_getquat(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    const quaternion& q = self->GetQuat();
    cmd->Out()->SetF(q.x);
    cmd->Out()->SetF(q.y);
    cmd->Out()->SetF(q.z);
    cmd->Out()->SetF(q.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setscale
    @input
    f(X), f(Y), f(Z)
    @output
    v
    @info
    Set the scale of the node. Default is (1, 1, 1).
*/
static void
n_setscale(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->SetScale(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscale
    @input
    f(X), f(Y), f(Z)
    @output
    v
    @info
    Get the scale of the node.
*/
static void
n_getscale(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    const vector3& v = self->GetScale();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
*/
bool
nTransformNode::SaveCmds(nPersistServer* ps)
{
    if (nSceneNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setviewspace ---
        cmd = ps->GetCmd(this, 'SVWS');
        cmd->In()->SetB(this->GetViewSpace());
        ps->PutCmd(cmd);

        //--- setactive ---
        cmd = ps->GetCmd(this, 'SACT');
        cmd->In()->SetB(this->GetActive());
        ps->PutCmd(cmd);

        //--- setposition ---
        cmd = ps->GetCmd(this, 'SPOS');
        cmd->In()->SetF(this->pos.x);
        cmd->In()->SetF(this->pos.y);
        cmd->In()->SetF(this->pos.z);
        ps->PutCmd(cmd);

        //--- seteuler/setquat ---
        if (this->CheckFlags(USEQUAT))
        {
            cmd = ps->GetCmd(this, 'SQUT');
            cmd->In()->SetF(this->quat.x);
            cmd->In()->SetF(this->quat.y);
            cmd->In()->SetF(this->quat.z);
            cmd->In()->SetF(this->quat.w);
            ps->PutCmd(cmd);
        }
        else
        {
            cmd = ps->GetCmd(this, 'SEUL');
            cmd->In()->SetF(this->euler.x);
            cmd->In()->SetF(this->euler.y);
            cmd->In()->SetF(this->euler.z);
            ps->PutCmd(cmd);
        }

        //--- setscale ---
        cmd = ps->GetCmd(this, 'SSCL');
        cmd->In()->SetF(this->scale.x);
        cmd->In()->SetF(this->scale.y);
        cmd->In()->SetF(this->scale.z);
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
