//------------------------------------------------------------------------------
//  nmultilayerednode_cmds.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmultilayerednode.h"
#include "kernel/npersistserver.h"

static void n_mlp_setuvstretch(void* slf, nCmd* cmd);
static void n_mlp_setpartindex(void* slf, nCmd* cmd);
static void n_mlp_setdx7uvstretch(void* slf, nCmd* cmd);
static void n_mlp_settexcount(void* slf, nCmd* cmd);


//------------------------------------------------------------------------------
/**
    @scriptclass
    nmultilayerednode

    @cppclass
    nMultiLayeredNode

    @superclass
    nshapenode

    @classinfo
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setuvstretch_if", 'STUS', n_mlp_setuvstretch);
    cl->AddCmd("v_setdx7uvstretch_if", 'SDUS', n_mlp_setdx7uvstretch);
    cl->AddCmd("v_settexcount_i", 'STXC', n_mlp_settexcount);
    cl->AddCmd("v_setpartindex_i", 'STPI', n_mlp_setpartindex);
    cl->EndCmds();
}


static void
n_mlp_setpartindex(void* slf, nCmd* cmd)
{
    nMultiLayeredNode* self = (nMultiLayeredNode*) slf;
    int nr = cmd->In()->GetI();
    self->SetPartIndex(nr);
}

static void
n_mlp_setuvstretch(void* slf, nCmd* cmd)
{
    nMultiLayeredNode* self = (nMultiLayeredNode*) slf;
    int nr = cmd->In()->GetI();
    float val = cmd->In()->GetF();
    self->SetUVStretch(nr,val);
}

static void
n_mlp_setdx7uvstretch(void* slf, nCmd* cmd)
{
    nMultiLayeredNode* self = (nMultiLayeredNode*) slf;
    int nr = cmd->In()->GetI();
    float val = cmd->In()->GetF();
    self->SetDX7UVStretch(nr,val);
}

static void
n_mlp_settexcount(void* slf, nCmd* cmd)
{
    nMultiLayeredNode* self = (nMultiLayeredNode*) slf;
    self->SetTexCount(cmd->In()->GetI());
}


//------------------------------------------------------------------------------
/**
*/
bool
nMultiLayeredNode::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        nCmd* cmd;

        // save part index
        cmd = ps->GetCmd(this, 'STPI');
        cmd->In()->SetI((int) this->partIndex);
        ps->PutCmd(cmd);

        // save count of textures
        cmd = ps->GetCmd(this, 'STXC');
        cmd->In()->SetI((int) this->texCount);
        ps->PutCmd(cmd);

        // save uvStretch-values for DX9
        int i;
        for (i = 0; i < this->texCount; i++)
        {
            cmd = ps->GetCmd(this, 'STUS');
            cmd->In()->SetI((int) i);
            cmd->In()->SetF((float) this->uvStretch[i]);
            ps->PutCmd(cmd);
        };

        // save uvStretch-values for DX7
        for (i = 0; i < this->texCount; i++)
        {
            cmd = ps->GetCmd(this, 'SDUS');
            cmd->In()->SetI((int) i);
            cmd->In()->SetF((float) this->dx7uvStretch[i]);
            ps->PutCmd(cmd);
        };

        return true;
    }
    return false;
}
