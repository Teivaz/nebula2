//------------------------------------------------------------------------------
//  nblendshapenode_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nblendshapenode.h"
#include "kernel/npersistserver.h"

static void n_setmeshat(void* slf, nCmd* cmd);
static void n_getmeshat(void* slf, nCmd* cmd);
static void n_getnumshapes(void* slf, nCmd* cmd);
static void n_setgroupindex(void* slf, nCmd* cmd);
static void n_getgroupindex(void* slf, nCmd* cmd);
static void n_setlocalboxat(void* slf, nCmd* cmd);
static void n_getlocalboxat(void* slf, nCmd* cmd);
static void n_setweightat(void* slf, nCmd* cmd);
static void n_getweightat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nblendshapenode

    @cppclass
    nBlendShapeNode
    
    @superclass
    nshapenode

    @classinfo
    Class for interpolating between up to 8 similar meshes.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();

    cl->AddCmd("v_setmeshat_is",            'SMSA', n_setmeshat);
    cl->AddCmd("s_getmeshat_i",             'GMSA', n_getmeshat);
    cl->AddCmd("i_getnumshapes_v",          'GNMS', n_getnumshapes);
    cl->AddCmd("v_setgroupindex_i",         'SGRI', n_setgroupindex);
    cl->AddCmd("i_getgroupindex_v",         'GGRI', n_getgroupindex);
    cl->AddCmd("v_setlocalboxat_iffffff",   'SLBA', n_setlocalboxat);
    cl->AddCmd("ffffff_getlocalboxat_i",    'GLBA', n_getlocalboxat);
    cl->AddCmd("v_setweightat_if",          'SWEA', n_setweightat);
    cl->AddCmd("f_getweightat_i",           'GWEA', n_getweightat);

    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmeshat
    @input
    i(index)
    s(MeshResource)
    @output
    v
    @info
    Set the name of the mesh resource for the specified index.
    The mesh at stream 0 must provide the index buffer for all meshes.
*/
static void
n_setmeshat(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    int index = cmd->In()->GetI();
    const char* meshName = cmd->In()->GetS();
    self->SetMeshAt(index, meshName);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmeshat
    @input
    i(index)
    @output
    s(MeshResource)
    @info
    Get the name of the mesh resource for the specified index.
*/
static void
n_getmeshat(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    cmd->Out()->SetS(self->GetMeshAt(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumshapes
    @input
    v
    @output
    i(NumShapes)
    @info
    Get the number of shapes.
*/
static void
n_getnumshapes(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    cmd->Out()->SetI(self->GetNumShapes());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setgroupindex
    @input
    i(GroupIndex)
    @output
    v
    @info
    Set the mesh group index
*/
static void
n_setgroupindex(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    self->SetGroupIndex(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getgroupindex
    @input
    v
    @output
    i(GroupIndex)
    @info
    Get the mesh group index
*/
static void
n_getgroupindex(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    cmd->Out()->SetI(self->GetGroupIndex());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlocalboxat
    @input
    i(shapeIndex), f(midX), f(midY), f(midZ), f(extentX), f(extentY), f(extentZ)
    @output
    v
    @info
    Define the local bounding box for a shape. Shape node compute their bounding
    box automatically at load time. This method can be used to define
    bounding boxes for other nodes. This may be useful for higher level
    code like gameframeworks. Nebula itself only uses bounding boxes
    defined on shape nodes.
*/
static void
n_setlocalboxat(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    vector3 mid;
    vector3 ext;
    int shapeIndex = cmd->In()->GetI();
    mid.x = cmd->In()->GetF();
    mid.y = cmd->In()->GetF();
    mid.z = cmd->In()->GetF();
    ext.x = cmd->In()->GetF();
    ext.y = cmd->In()->GetF();
    ext.z = cmd->In()->GetF();
    bbox3 box(mid, ext);
    self->SetLocalBoxAt(shapeIndex, box);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlocalboxat
    @input
    i(shapeIndex)
    @output
    f(midX), f(midY), f(midZ), f(extentX), f(extentY), f(extentZ)
    @info
    Return the local bounding box for a shape.
*/
static void
n_getlocalboxat(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    int shapeIndex = cmd->Out()->GetI();
    const bbox3& box = self->GetLocalBoxAt(shapeIndex);
    const vector3& mid = box.center();
    const vector3& ext = box.extents();
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
    setweightat
    @input
    i(shapeIndex), f(weight)
    @output
    v
    @info
    Set the weight of a particular shape.
*/
static void
n_setweightat(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    int shapeIndex = cmd->In()->GetI();
    self->SetWeightAt(shapeIndex, cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getweightat
    @input
    i(shapeIndex)
    @output
    f(weight)
    @info
    Return the weight for a particular shape.
*/
static void
n_getweightat(void* slf, nCmd* cmd)
{
    nBlendShapeNode* self = (nBlendShapeNode*) slf;
    int shapeIndex = cmd->In()->GetI();
    cmd->Out()->SetF(self->GetWeightAt(shapeIndex));
}

//------------------------------------------------------------------------------
/**
*/
bool
nBlendShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nMaterialNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setgroupindexat ---
        cmd = ps->GetCmd(this, 'SGRI');
        cmd->In()->SetI(this->GetGroupIndex());
        ps->PutCmd(cmd);

        int shapeIndex;
        for (shapeIndex = 0; shapeIndex < this->GetNumShapes(); ++shapeIndex)
        {
            //--- setmeshat ---
            cmd = ps->GetCmd(this, 'SMSA');
            cmd->In()->SetI(shapeIndex);
            cmd->In()->SetS(this->GetMeshAt(shapeIndex));
            ps->PutCmd(cmd);

            //--- setlocalboxat ---
            const bbox3& box = this->GetLocalBoxAt(shapeIndex);
            vector3 nullVec;
            if (!(box.vmin.isequal(nullVec, 0.000001f) &&
                box.vmax.isequal(nullVec, 0.000001f)))
            {
                cmd = ps->GetCmd(this, 'SLBA');
                cmd->In()->SetI(shapeIndex);
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

            //--- setweightat ---
            cmd = ps->GetCmd(this, 'SWEA');
            cmd->In()->SetI(shapeIndex);
            cmd->In()->SetF(this->GetWeightAt(shapeIndex));
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}

