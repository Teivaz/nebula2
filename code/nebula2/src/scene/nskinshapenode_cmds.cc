//------------------------------------------------------------------------------
//  nskinshapenode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nskinshapenode.h"
#include "kernel/npersistserver.h"

static void n_setskinanimator(void* slf, nCmd* cmd);
static void n_getskinanimator(void* slf, nCmd* cmd);
static void n_beginfragments(void* slf, nCmd* cmd);
static void n_setfraggroupindex(void* slf, nCmd* cmd);
static void n_getfraggroupindex(void* slf, nCmd* cmd);
static void n_beginjointpalette(void* slf, nCmd* cmd);
static void n_setjointindices(void* slf, nCmd* cmd);
static void n_endjointpalette(void* slf, nCmd* cmd);
static void n_endfragments(void* slf, nCmd* cmd);
static void n_getnumfragments(void* slf, nCmd* cmd);
static void n_getjointpalettesize(void* slf, nCmd* cmd);
static void n_getjointindex(void* slf, nCmd* cmd);
static void n_setchar3variationflag(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nskinshapenode    

    @cppclass
    nSkinShapeNode

    @superclass
    nshapenode

    @classinfo
    A shape node which is deformed by an nSkinAnimator object.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setskinanimator_s",           'SSKA', n_setskinanimator);
    cl->AddCmd("s_getskinanimator_v",           'GSKA', n_getskinanimator);
    cl->AddCmd("v_beginfragments_i",            'BGFR', n_beginfragments);
    cl->AddCmd("v_setfraggroupindex_ii",        'SFGI', n_setfraggroupindex);
    cl->AddCmd("i_getfraggroupindex_i",         'GFGI', n_getfraggroupindex);
    cl->AddCmd("v_beginjointpalette_ii",        'BGJP', n_beginjointpalette);
    cl->AddCmd("v_setjointindices_iiiiiiiiii",  'SJID', n_setjointindices);
    cl->AddCmd("v_endjointpalette_i",           'EDJP', n_endjointpalette);
    cl->AddCmd("v_endfragments_v",              'EDFR', n_endfragments);
    cl->AddCmd("i_getnumfragments_v",           'GNFR', n_getnumfragments);
    cl->AddCmd("i_getjointpalettesize_i",       'GJPS', n_getjointpalettesize);
    cl->AddCmd("i_getjointindex_ii",            'GJIX', n_getjointindex);
    cl->AddCmd("v_setchar3variationflag_v",     'SCVF', n_setchar3variationflag);
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
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
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
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    cmd->Out()->SetS(self->GetSkinAnimator());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginfragments
    @input
    i(NumFragments)
    @output
    v
    @info
    Begin defining skin fragments. Skin fragments define their own limited size
    joint palette.
*/
static void
n_beginfragments(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    self->BeginFragments(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfraggroupindex
    @input
    i(FragmentIndex), i(MeshGroupIndex)
    @output
    v
    @info
    Set a mesh group index. This is the portion of the mesh resource
    which this skin fragment renders.
*/
static void
n_setfraggroupindex(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    self->SetFragGroupIndex(i0, i1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfraggroupindex
    @input
    i(FragmentIndex)
    @output
    i(MeshGroupIndex)
    @info
    Get a mesh group index.
*/
static void
n_getfraggroupindex(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    cmd->Out()->SetI(self->GetFragGroupIndex(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginjointpalette
    @input
    i(FragmentIndex), i(JointPaletteSize)
    @output
    v
    @info
    Begin defining the joint palette of a fragment.
*/
static void
n_beginjointpalette(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    self->BeginJointPalette(i0, i1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setjointindices
    @input
    i(FragmentIndex), i(PaletteStartIndex), i...(JointIndices)
    @output
    v
    @info
    Set up to 8 joint indices in a joint palette.
*/
static void
n_setjointindices(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    int fragIndex = cmd->In()->GetI();
    int palIndex  = cmd->In()->GetI();
    int i;
    int ji[8];
    for (i = 0; i < 8; i++)
    {
        ji[i] = cmd->In()->GetI();
    }
    self->SetJointIndices(fragIndex, palIndex, ji[0], ji[1], ji[2], ji[3], ji[4], ji[5], ji[6], ji[7]);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endjointpalette
    @input
    i(FragmentIndex)
    @output
    v
    @info
    Finish defining a fragment's joint palette.
*/
static void
n_endjointpalette(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    self->EndJointPalette(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    endfragments
    @input
    v
    @output
    v
    @info
    Finish defining fragments.
*/
static void
n_endfragments(void* slf, nCmd* /*cmd*/)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    self->EndFragments();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchar3variationflag
    @input
    v
    @output
    v
    @info
    Sets the character3 variation flag
*/
static void
n_setchar3variationflag(void* slf, nCmd* /*cmd*/)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    self->SetChar3VariationFlag(true);
}


//------------------------------------------------------------------------------
/**
    @cmd
    getnumfragments
    @input
    v
    @output
    i(NumFragments)
    @info
    Get number of fragments in skin.
*/
static void
n_getnumfragments(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    cmd->Out()->SetI(self->GetNumFragments());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjointpalettesize
    @input
    i(FragmentIndex)
    @output
    i(JointPaletteSize)
    @info
    Get joint palette size in a fragment.
*/
static void
n_getjointpalettesize(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    cmd->Out()->SetI(self->GetJointPaletteSize(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjointindex
    @input
    i(FragmentIndex), i(PaletteIndex)
    @output
    i(JointIndex)
    @info
    Get the joint index at a fragment's joint palette index.
*/
static void
n_getjointindex(void* slf, nCmd* cmd)
{
    nSkinShapeNode* self = (nSkinShapeNode*) slf;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    cmd->Out()->SetI(self->GetJointIndex(i0, i1));
}

//------------------------------------------------------------------------------
/**
*/
bool
nSkinShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setchar3variationflag ---
        if(this->isChar3AndBoundToVariation)
        {
            cmd = ps->GetCmd(this, 'SCVF');
            ps->PutCmd(cmd);
        };

        //--- setskinanimator ---
        cmd = ps->GetCmd(this, 'SSKA');
        cmd->In()->SetS(this->GetSkinAnimator());
        ps->PutCmd(cmd);

        int numFragments = this->GetNumFragments();
        if (numFragments > 0)
        {
            //--- beginfragments ---
            cmd = ps->GetCmd(this, 'BGFR');
            cmd->In()->SetI(numFragments);
            ps->PutCmd(cmd);

            int fragIndex;
            for (fragIndex = 0; fragIndex < numFragments; fragIndex++)
            {
                //--- setfraggroupindex ---
                cmd = ps->GetCmd(this, 'SFGI');
                cmd->In()->SetI(fragIndex);
                cmd->In()->SetI(this->GetFragGroupIndex(fragIndex));
                ps->PutCmd(cmd);

                int numJoints = this->GetJointPaletteSize(fragIndex);
                if (numJoints > 0)
                {
                    //--- beginjointpalette ---
                    cmd = ps->GetCmd(this, 'BGJP');
                    cmd->In()->SetI(fragIndex);
                    cmd->In()->SetI(numJoints);
                    ps->PutCmd(cmd);

                    int palIndex;
                    for (palIndex = 0; palIndex < numJoints;)
                    {
                        //--- setjointindices ---
                        cmd = ps->GetCmd(this, 'SJID');
                        cmd->In()->SetI(fragIndex);
                        cmd->In()->SetI(palIndex);
                        int lastIndex = palIndex + 8;
                        for (; palIndex < lastIndex; palIndex++)
                        {
                            if (palIndex < numJoints)
                            {
                                cmd->In()->SetI(this->GetJointIndex(fragIndex, palIndex));
                            }
                            else
                            {
                                cmd->In()->SetI(0);
                            }
                        }
                        ps->PutCmd(cmd);
                    }

                    //--- endjointpalette ---
                    cmd = ps->GetCmd(this, 'EDJP');
                    cmd->In()->SetI(fragIndex);
                    ps->PutCmd(cmd);
                }
            }
            
            //--- endfragments ---
            cmd = ps->GetCmd(this, 'EDFR');
            ps->PutCmd(cmd);
        }
        return true;
    }
    return false;
}
