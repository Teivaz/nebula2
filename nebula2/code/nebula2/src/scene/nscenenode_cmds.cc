#define N_IMPLEMENTS nSceneNode
//------------------------------------------------------------------------------
//  nscenenode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nscenenode.h"
#include "kernel/npersistserver.h"

static void n_addanimator(void* slf, nCmd* cmd);
static void n_getnumanimators(void* slf, nCmd* cmd);
static void n_getanimatorat(void* slf, nCmd* cmd);
static void n_loadresources(void* slf, nCmd* cmd);
static void n_setlocalbox(void* slf, nCmd* cmd);
static void n_getlocalbox(void* slf, nCmd* cmd);
static void n_setrenderpri(void* slf, nCmd* cmd);
static void n_getrenderpri(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nscenenode
    
    @superclass
    nroot

    @classinfo
    Parent class of scene graph nodes. nSceneNode objects can form complex
    hierarchies and cross-hierarchy dependencies.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addanimator_s",       'ADDA', n_addanimator);
    cl->AddCmd("i_getnumanimators_v",   'GNMA', n_getnumanimators);
    cl->AddCmd("s_getanimatorat_i",     'GAAT', n_getanimatorat);
    cl->AddCmd("b_loadresources_v",     'LORE', n_loadresources);
    cl->AddCmd("v_setlocalbox_ffffff",  'SLCB', n_setlocalbox);
    cl->AddCmd("ffffff_getlocalbox_v",  'GLCB', n_getlocalbox);
    cl->AddCmd("v_setrenderpri_i",      'SRPR', n_setrenderpri);
    cl->AddCmd("i_getrenderpri_v",      'GRPR', n_getrenderpri);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addanimator
    @input
    s(AnimatorPath)
    @output
    v
    @info
    Defines the relative path to an animator object, which is called
    to manipulate this object at render time.
*/
static void
n_addanimator(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    self->AddAnimator(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumanimators
    @input
    v
    @output
    i(NumAnimators)
    @info
    Returns the number of animators attached to this object.
*/
static void
n_getnumanimators(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetI(self->GetNumAnimators());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getanimatorat
    @input
    i(Index)
    @output
    s(AnimatorPath)
    @info
    Returns relative path to animator at given index.
*/
static void
n_getanimatorat(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetS(self->GetAnimatorAt(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    loadresources
    @input
    v
    @output
    b(status)
    @info
    Loads resources and returns status
*/
static void
n_loadresources(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetB(self->LoadResources());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlocalbox
    @input
    f(midX), f(midY), f(midZ), f(extentX), f(extentY), f(extentZ)
    @output
    v
    @info
    Define the local bounding box. Shape node compute their bounding
    box automatically at load time. This method can be used to define
    bounding boxes for other nodes. This may be useful for higher level
    code like gameframeworks. Nebula itself only uses bounding boxes
    defined on shape nodes.
*/
static void
n_setlocalbox(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    vector3 mid;
    vector3 ext;
    mid.x = cmd->In()->GetF();
    mid.y = cmd->In()->GetF();
    mid.z = cmd->In()->GetF();
    ext.x = cmd->In()->GetF();
    ext.y = cmd->In()->GetF();
    ext.z = cmd->In()->GetF();
    bbox3 box(mid, ext);
    self->SetLocalBox(box);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlocalbox
    @input
    v
    @output
    f(midX), f(midY), f(midZ), f(extentX), f(extentY), f(extentZ)
    @info
    Return the local bounding box.
*/
static void
n_getlocalbox(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    const vector3& mid = self->GetLocalBox().center();
    const vector3& ext = self->GetLocalBox().extents();
    cmd->Out()->SetF(mid.x);
    cmd->Out()->SetF(mid.y);
    cmd->Out()->SetF(mid.z);
    cmd->Out()->SetF(ext.x);
    cmd->Out()->SetF(ext.y);
    cmd->Out()->SetF(ext.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrenderpri
    @input
    i(RenderPri)
    @output
    v
    @info
    Sets the render priority of the node. Should be between -127 and +127.
*/
static void
n_setrenderpri(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    self->SetRenderPri(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrenderpri
    @input
    @output
    i(RenderPri)
    @info
    Gets the render priority of the node.
*/
static void
n_getrenderpri(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetI(self->GetRenderPri());
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSceneNode::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setrenderpri ---
        if (this->GetRenderPri() != 0)
        {
            cmd = ps->GetCmd(this, 'SRPR');
            cmd->In()->SetI(this->GetRenderPri());
            ps->PutCmd(cmd);
        }

        //--- setlocalbox ---
        const bbox3& box = this->GetLocalBox();
        vector3 nullVec;
        if (!(box.vmin.isequal(nullVec, 0.000001f) &&
              box.vmax.isequal(nullVec, 0.000001f)))
        {
            cmd = ps->GetCmd(this, 'SLCB');
            const vector3& mid = box.center();
            const vector3& ext = box.extents();
            cmd->In()->SetF(mid.x);
            cmd->In()->SetF(mid.y);
            cmd->In()->SetF(mid.z);
            cmd->In()->SetF(ext.x);
            cmd->In()->SetF(ext.y);
            cmd->In()->SetF(ext.z);
            ps->PutCmd(cmd);
        }

        //--- addanimator ---
        int i;
        int num = this->GetNumAnimators();
        for (i = 0; i < num; i++)
        {
            cmd = ps->GetCmd(this, 'ADDA');
            cmd->In()->SetS(this->GetAnimatorAt(i));
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
