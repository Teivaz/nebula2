//------------------------------------------------------------------------------
//  nanimator_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nanimator.h"
#include "kernel/npersistserver.h"

static void n_setchannel(void* slf, nCmd* cmd);
static void n_getchannel(void* slf, nCmd* cmd);
static void n_setlooptype(void* slf, nCmd* cmd);
static void n_getlooptype(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nanimator

    @cppclass
    nAnimator

    @superclass
    nscenenode

    @classinfo
    Base class for all scene node animators.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setchannel_s",  'SCHN', n_setchannel);
    cl->AddCmd("s_getchannel_v",  'GCHN', n_getchannel);
    cl->AddCmd("v_setlooptype_s", 'SLPT', n_setlooptype);
    cl->AddCmd("s_getlooptype_v", 'GLPT', n_getlooptype);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchannel
    @input
    s(ChannelName)
    @output
    v
    @info
    Set name of animation channel which drives this animation
    (e.g. "time").
*/
static
void
n_setchannel(void* slf, nCmd* cmd)
{
    nAnimator* self = (nAnimator*) slf;
    self->SetChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchannel
    @input
    v
    @output
    s(ChannelName)
    @info
    Get name of animation channel which drives this animation.
*/
static
void
n_getchannel(void* slf, nCmd* cmd)
{
    nAnimator* self = (nAnimator*) slf;
    cmd->Out()->SetS(self->GetChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlooptype
    @input
    s(LoopType = "loop", "clamp")
    @output
    v
    @info
    Set the loop type for this animation. Default is "loop".
*/
static
void
n_setlooptype(void* slf, nCmd* cmd)
{
    nAnimator* self = (nAnimator*) slf;
    self->SetLoopType(nAnimLoopType::FromString(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlooptype
    @input
    v
    @output
    s(LoopType = "loop", "clamp")
    @info
    Get the loop type for this animation.
*/
static
void
n_getlooptype(void* slf, nCmd* cmd)
{
    nAnimator* self = (nAnimator*) slf;
    cmd->Out()->SetS(nAnimLoopType::ToString(self->GetLoopType()).Get());
}

//------------------------------------------------------------------------------
/**
*/
bool
nAnimator::SaveCmds(nPersistServer* ps)
{
    if (nSceneNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setchannel ---
        if (this->GetChannel())
        {
            cmd = ps->GetCmd(this, 'SCHN');
            cmd->In()->SetS(this->GetChannel());
            ps->PutCmd(cmd);
        }

        //--- setlooptype ---
        cmd = ps->GetCmd(this, 'SLPT');
        cmd->In()->SetS(nAnimLoopType::ToString(this->GetLoopType()).Get());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
