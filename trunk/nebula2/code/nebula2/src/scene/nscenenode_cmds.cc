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
*/
bool 
nSceneNode::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;

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
