//------------------------------------------------------------------------------
//  nmultilayerednode_cmds.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmultilayerednode.h"
#include "kernel/npersistserver.h"

static void n_setuvstretch(void* slf, nCmd* cmd);
static void n_setspecintensity(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmultilayerednode

    @cppclass
    nMultiLayeredNode

    @superclass
    nshapenode

    @classinfo
    A specialized shape node for multilayered material rendering.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setuvstretch_if", 'STUS', n_setuvstretch);
    cl->AddCmd("v_setspecintensity_if", 'SSPI', n_setspecintensity);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuvstretch
    @input
    i(LayerIndex),f(StretchFactor)
    @output
    v
    @info
    Set the UV stretch factor for a layer
*/
static void
n_setuvstretch(void* slf, nCmd* cmd)
{
    nMultiLayeredNode* self = (nMultiLayeredNode*) slf;
    int layer = cmd->In()->GetI();
    float val = cmd->In()->GetF();
    self->SetUVStretch(layer, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setspecintensity
    @input
    i(LayerIndex),f(SpecIntensity)
    @output
    v
    @info
    Set the specular intensity for a layer.
*/
static void
n_setspecintensity(void* slf, nCmd* cmd)
{
    nMultiLayeredNode* self = (nMultiLayeredNode*) slf;
    int layer = cmd->In()->GetI();
    float val = cmd->In()->GetF();
    self->SetSpecIntensity(layer, val);
}

//------------------------------------------------------------------------------
/**
*/
bool
nMultiLayeredNode::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        int i;
        for (i = 0; i < MaxLayers; i++)
        {
            nCmd* cmd;

            //--- setuvstretch ---
            cmd = ps->GetCmd(this, 'STUS');
            cmd->In()->SetI(i);
            cmd->In()->SetF(this->uvStretch[i]);
            ps->PutCmd(cmd);

            //--- setspecintensity ---
            cmd = ps->GetCmd(this, 'SSPI');
            cmd->In()->SetI(i);
            cmd->In()->SetF(this->specIntensity[i]);
            ps->PutCmd(cmd);
        }
        return true;
    }
    return false;
}
