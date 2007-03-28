//------------------------------------------------------------------------------
//  nattachmentnode_cmds.cc
//  (C) 2004 Megan Fox
//------------------------------------------------------------------------------
#include "scene/nattachmentnode.h"
#include "kernel/npersistserver.h"

static void n_setskinanimator(void* slf, nCmd* cmd);
static void n_getskinanimator(void* slf, nCmd* cmd);
static void n_setjointbyname(void* slf, nCmd* cmd);
static void n_setjointbyindex(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nattachmentnode

    @cppclass
    nAttachmentNode

    @superclass
    ntransformnode

    @classinfo
    Variant of nTransformNode designed to provide a direct link to the transformation
    of a bone in a skeleton.  Create as a child of the target nSkinAnimator, set either
    the bone index or name, and the transform of this node will be the world matrix of
    the bone itself.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setskinanimator_s",  'SSKA', n_setskinanimator);
    cl->AddCmd("s_getskinanimator_v",  'GSKA', n_getskinanimator);
    cl->AddCmd("v_setjointbyname_s",   'SJBN', n_setjointbyname);
    cl->AddCmd("v_setjointbyindex_i",  'SJBI', n_setjointbyindex);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setskinanimator
    @input
    s(RelPath)
    @output
    v
    @info
    Set relative path to a skin animator object.
*/
static void
n_setskinanimator(void* slf, nCmd* cmd)
{
    nAttachmentNode* self = (nAttachmentNode*) slf;
    self->SetSkinAnimator(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getskinanimator
    @input
    v
    @output
    s(RelPath)
    @info
    Get relative path to a skin animator object.
*/
static void
n_getskinanimator(void* slf, nCmd* cmd)
{
    nAttachmentNode* self = (nAttachmentNode*) slf;
    cmd->Out()->SetS(self->GetSkinAnimator());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setjointbyname
    @input
    s(JointName)
    @output
    v
    @info
    Sets the target joint by name
*/
static void
n_setjointbyname(void* slf, nCmd* cmd)
{
    nAttachmentNode* self = (nAttachmentNode*) slf;
    const char *nameStr = cmd->In()->GetS();

    self->SetJointByName(nameStr);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setjointbyindex
    @input
    s(JointIndex)
    @output
    v
    @info
    Sets the target joint by index
*/
static void
n_setjointbyindex(void* slf, nCmd* cmd)
{
    nAttachmentNode* self = (nAttachmentNode*) slf;
    unsigned int jointIndex = cmd->In()->GetI();

    self->SetJointByIndex(jointIndex);
}

//------------------------------------------------------------------------------
/**
*/
bool
nAttachmentNode::SaveCmds(nPersistServer* ps)
{
    if (nTransformNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setskinanimator ---
        if( this->GetSkinAnimator() )
        {
            cmd = ps->GetCmd(this, 'SSKA');
            cmd->In()->SetS(this->GetSkinAnimator());
            ps->PutCmd(cmd);
        }

        if (this->GetJointByIndex() != -1)
        {
            cmd = ps->GetCmd(this, 'SJBI');
            cmd->In()->SetI(this->jointIndex);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
