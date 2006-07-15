//------------------------------------------------------------------------------
//  ntransformcurveanimator_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntransformcurveanimator.h"
#include "kernel/npersistserver.h"

static void n_setanimation(void* slf, nCmd* cmd);
static void n_getanimation(void* slf, nCmd* cmd);
static void n_setanimationgroup(void* slf, nCmd* cmd);
static void n_getanimationgroup(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntransformcurveanimator

    @cppclass
    nTransformCurveAnimator

    @superclass
    nanimator

    @classinfo
    Animate attributes of a transform node using a nAnimation.
*/

void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setanimation_s",      'SANI', n_setanimation);
    cl->AddCmd("s_getanimation_v",      'GANI', n_getanimation);
    cl->AddCmd("v_setanimationgroup_i", 'SAGR', n_setanimationgroup);
    cl->AddCmd("i_getanimationgroup_v", 'GAGR', n_getanimationgroup);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimation
    @input
    s(Animation file)
    @output
    v
    @info
    Set the name of the animation resource.
*/
static void
n_setanimation(void* slf, nCmd* cmd)
{
    nTransformCurveAnimator* self = (nTransformCurveAnimator*) slf;
    self->SetAnimation(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getanimation
    @input
    v
    @output
    s(Animtion file)
    @info
    Get the name of the animation resource.
*/
static void
n_getanimation(void* slf, nCmd* cmd)
{
    nTransformCurveAnimator* self = (nTransformCurveAnimator*) slf;
    cmd->Out()->SetS(self->GetAnimation());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimationgroup
    @input
    i(Animation group)
    @output
    v
    @info
    Set the group in the animation resource.
*/
static void
n_setanimationgroup(void* slf, nCmd* cmd)
{
    nTransformCurveAnimator* self = (nTransformCurveAnimator*) slf;
    self->SetAnimationGroup(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getanimationgroup
    @input
    v
    @output
    i(Animation group)
    @info
    Get the group in the animation resource.
*/
static void
n_getanimationgroup(void* slf, nCmd* cmd)
{
    nTransformCurveAnimator* self = (nTransformCurveAnimator*) slf;
    cmd->Out()->SetI(self->GetAnimationGroup());
}

//------------------------------------------------------------------------------
/**
*/
bool
nTransformCurveAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setanimation ---
        cmd = ps->GetCmd(this, 'SANI');
        cmd->In()->SetS(this->GetAnimation());
        ps->PutCmd(cmd);

        //--- setanimationgroup ---
        cmd = ps->GetCmd(this, 'SAGR');
        cmd->In()->SetI(this->GetAnimationGroup());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
