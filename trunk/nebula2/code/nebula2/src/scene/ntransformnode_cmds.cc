//------------------------------------------------------------------------------
//  ntransformnode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntransformnode.h"
#include "kernel/npersistserver.h"

static void n_setactive(void* slf, nCmd* cmd);
static void n_getactive(void* slf, nCmd* cmd);
static void n_setlockviewer(void* slf, nCmd* cmd);
static void n_getlockviewer(void* slf, nCmd* cmd);
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
static void n_setrotatepivot(void* slf, nCmd* cmd);
static void n_getrotatepivot(void* slf, nCmd* cmd);
static void n_setscalepivot(void* slf, nCmd* cmd);
static void n_getscalepivot(void* slf, nCmd* cmd);
static void n_hasrotatepivot(void* slf, nCmd* cmd);
static void n_hasscalepivot(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntransformnode

    @cppclass
    nTransformNode

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
    cl->AddCmd("v_setactive_b",         'SACT', n_setactive);
    cl->AddCmd("b_getactive_v",         'GACT', n_getactive);
    cl->AddCmd("v_setlockviewer_b",     'SLKV', n_setlockviewer);
    cl->AddCmd("b_getlockviewer_v",     'GLKV', n_getlockviewer);
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
    cl->AddCmd("v_setscalepivot_fff",   'SSCP', n_setscalepivot);
    cl->AddCmd("fff_getscalepivot_v",   'GSCP', n_getscalepivot);
    cl->AddCmd("v_setrotatepivot_fff",  'SRTP', n_setrotatepivot);
    cl->AddCmd("fff_getrotatepivot_v",  'GRTP', n_getrotatepivot);
    cl->AddCmd("b_hasscalepivot_v",     'HSCP', n_hasscalepivot);
    cl->AddCmd("b_hasrotatepivot_v",    'HRTP', n_hasrotatepivot);
    cl->EndCmds();
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
    v
    @output
    b(Active)
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
    setlockviewer
    @input
    b(LockViewer)
    @output
    v
    @info
    Set to true if this node's position is locked to the viewer.
*/
static void
n_setlockviewer(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    self->SetLockViewer(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlockviewer
    @input
    @output
    b(LockViewer)
    @info
    Return lock viewer flag.
*/
static void
n_getlockviewer(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    cmd->Out()->SetB(self->GetLockViewer());
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
    @cmd
    setscalepivot
    @input
    f(X), f(Y), f(Z)
    @output
    v
    @info
    Set the optional pivot point for scaling.
*/
static void
n_setscalepivot(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->SetScalePivot(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscalepivot
    @input
    v
    @output
    f(X), f(Y), f(Z)
    @info
    Get the optional pivot point for rotations.
*/
static void
n_getscalepivot(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    const vector3& v = self->GetScalePivot();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrotatepivot
    @input
    f(X), f(Y), f(Z)
    @output
    v
    @info
    Set the optional pivot point for rotations.
*/
static void
n_setrotatepivot(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->SetRotatePivot(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrotatepivot
    @input
    v
    @output
    f(X), f(Y), f(Z)
    @info
    Get the optional pivot point for rotations.
*/
static void
n_getrotatepivot(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    const vector3& v = self->GetRotatePivot();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    hasscalepivot
    @input
    v
    @output
    b(HasScalePivotFlag)
    @info
    Returns true if a scale pivot point has been set.
*/
static void
n_hasscalepivot(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    cmd->Out()->SetB(self->HasScalePivot());
}

//------------------------------------------------------------------------------
/**
    @cmd
    hasrotatepivot
    @input
    v
    @output
    b(HasRotatePivotFlag)
    @info
    Returns true if a rotate pivot point has been set.
*/
static void
n_hasrotatepivot(void* slf, nCmd* cmd)
{
    nTransformNode* self = (nTransformNode*) slf;
    cmd->Out()->SetB(self->HasRotatePivot());
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
        static const vector3 oneVec(1.0f, 1.0f, 1.0f);

        //--- setactive ---
        if (!this->GetActive())
        {
            cmd = ps->GetCmd(this, 'SACT');
            cmd->In()->SetB(this->GetActive());
            ps->PutCmd(cmd);
        }

        //--- setlockviewer ---
        if (this->GetLockViewer())
        {
            cmd = ps->GetCmd(this, 'SLKV');
            cmd->In()->SetB(this->GetLockViewer());
            ps->PutCmd(cmd);
        }

        //--- setposition ---
        const vector3& pos = this->tform.gettranslation();
        if (!pos.isequal(vector3::zero, 0.0f))
        {
            cmd = ps->GetCmd(this, 'SPOS');
            cmd->In()->SetF(pos.x);
            cmd->In()->SetF(pos.y);
            cmd->In()->SetF(pos.z);
            ps->PutCmd(cmd);
        }

        //--- seteuler/setquat ---
        if (this->tform.iseulerrotation())
        {
            const vector3& euler = this->tform.geteulerrotation();
            if (!euler.isequal(vector3::zero, 0.0f))
            {
                cmd = ps->GetCmd(this, 'SEUL');
                cmd->In()->SetF(n_rad2deg(euler.x));
                cmd->In()->SetF(n_rad2deg(euler.y));
                cmd->In()->SetF(n_rad2deg(euler.z));
                ps->PutCmd(cmd);
            }
        }
        else
        {
            const quaternion& quat = this->tform.getquatrotation();
            if (!quat.isequal(quaternion::identity, 0.0f))
            {
                cmd = ps->GetCmd(this, 'SQUT');
                cmd->In()->SetF(quat.x);
                cmd->In()->SetF(quat.y);
                cmd->In()->SetF(quat.z);
                cmd->In()->SetF(quat.w);
                ps->PutCmd(cmd);
            }
        }

        //--- setscale ---
        const vector3& scale = this->tform.getscale();
        if (!scale.isequal(oneVec, 0.0f))
        {
            cmd = ps->GetCmd(this, 'SSCL');
            cmd->In()->SetF(scale.x);
            cmd->In()->SetF(scale.y);
            cmd->In()->SetF(scale.z);
            ps->PutCmd(cmd);
        }

        //--- setrotatepivot ---
        if (this->HasRotatePivot())
        {
            const vector3& rotatePivot = this->GetRotatePivot();
            if (!rotatePivot.isequal(vector3::zero, 0.0f))
            {
                cmd = ps->GetCmd(this, 'SRTP');
                cmd->In()->SetF(rotatePivot.x);
                cmd->In()->SetF(rotatePivot.y);
                cmd->In()->SetF(rotatePivot.z);
                ps->PutCmd(cmd);
            }
        }

        //--- setscalepivot ---
        if (this->HasScalePivot())
        {
            const vector3& scalePivot = this->GetScalePivot();
            if (!scalePivot.isequal(vector3::zero, 0.0f))
            {
                cmd = ps->GetCmd(this, 'SSCP');
                cmd->In()->SetF(scalePivot.x);
                cmd->In()->SetF(scalePivot.y);
                cmd->In()->SetF(scalePivot.z);
                ps->PutCmd(cmd);
            }
        }

        return true;
    }
    return false;
}
