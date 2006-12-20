//------------------------------------------------------------------------------
//  nblendshapeanimator_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nblendshapeanimator.h"
#include "kernel/npersistserver.h"

static void n_setanimation(void* slf, nCmd* cmd);
static void n_getanimation(void* slf, nCmd* cmd);
static void n_setanimationgroup(void* slf, nCmd* cmd);
static void n_getanimationgroup(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nblendshapeanimator
    @cppclass
    nBlendShapeAnimator
    @superclass
    nanimator
    @classinfo
    Animate a blendshape node.
*/

void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setanimation_s",      'SANI', n_setanimation);
    cl->AddCmd("s_getanimation_v",      'GANI', n_getanimation);
    cl->AddCmd("v_setanimationgroup_i", 'SANG', n_setanimationgroup);
    cl->AddCmd("i_getanimationgroup_v", 'GANG', n_getanimationgroup);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimation
    @input
    s(Animation File)
    @output
    v
    @info
    Set the name of the animation resource.
*/
static void
n_setanimation(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    self->SetAnimation(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getanimation
    @input
    v
    @output
    s(Animation File)
    @info
    Get the name of the animation resource.
*/
static void
n_getanimation(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    cmd->Out()->SetS(self->GetAnimation().Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimationgroup
    @input
    i(GroupIndxe)
    @output
    v
    @info
    Set the group of the animation to be used.
*/
static void
n_setanimationgroup(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    self->SetAnimationGroup(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getanimationgroup
    @input
    v
    @output
    i(GroupIndex)
    @info
    Get the index of the animation group used.
*/
static void
n_getanimationgroup(void* slf, nCmd* cmd)
{
    nBlendShapeAnimator* self = (nBlendShapeAnimator*) slf;
    cmd->Out()->SetI(self->GetAnimationGroup());
}

//------------------------------------------------------------------------------
/**
*/
bool
nBlendShapeAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setanimation ---
        cmd = ps->GetCmd(this, 'SANI');
        cmd->In()->SetS(this->GetAnimation().Get());
        ps->PutCmd(cmd);

        //--- setanimationgroup ---
        cmd = ps->GetCmd(this, 'SANG');
        cmd->In()->SetI(this->GetAnimationGroup());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
