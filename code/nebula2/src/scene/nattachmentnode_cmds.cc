//------------------------------------------------------------------------------
//  nattachmentnode_cmds.cc
//  (C) 2004 Megan Fox
//------------------------------------------------------------------------------
#include "scene/nattachmentnode.h"
#include "kernel/npersistserver.h"

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
    cl->AddCmd("v_setjointbyname_s",   'SJBN', n_setjointbyname);
    cl->AddCmd("v_setjointbyindex_i",  'SJBI', n_setjointbyindex);
    cl->EndCmds();
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
        if (this->isJointSet)
        {
            nCmd* cmd;

            cmd = ps->GetCmd(this, 'SJBI');
            cmd->In()->SetI(this->jointIndex);
            ps->PutCmd(cmd);
        }
        return true;
    }
    return false;
}
