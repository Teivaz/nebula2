//------------------------------------------------------------------------------
//  nscenenode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nscenenode.h"
#include "kernel/npersistserver.h"

static void n_addanimator(void* slf, nCmd* cmd);
static void n_removeanimator(void* slf, nCmd* cmd);
static void n_getnumanimators(void* slf, nCmd* cmd);
static void n_getanimatorat(void* slf, nCmd* cmd);
static void n_loadresources(void* slf, nCmd* cmd);
static void n_setlocalbox(void* slf, nCmd* cmd);
static void n_getlocalbox(void* slf, nCmd* cmd);
static void n_setrenderpri(void* slf, nCmd* cmd);
static void n_getrenderpri(void* slf, nCmd* cmd);
static void n_addhints(void* slf, nCmd* cmd);
static void n_clearhints(void* slf, nCmd* cmd);
static void n_gethints(void* slf, nCmd* cmd);
static void n_hashints(void* slf, nCmd* cmd);
static void n_setintattr(void* slf, nCmd* cmd);
static void n_getintattr(void* slf, nCmd* cmd);
static void n_setfloatattr(void* slf, nCmd* cmd);
static void n_getfloatattr(void* slf, nCmd* cmd);
static void n_setboolattr(void* slf, nCmd* cmd);
static void n_getboolattr(void* slf, nCmd* cmd);
static void n_setstringattr(void* slf, nCmd* cmd);
static void n_getstringattr(void* slf, nCmd* cmd);
static void n_setvector3attr(void* slf, nCmd* cmd);
static void n_getvector3attr(void* slf, nCmd* cmd);
static void n_setvector4attr(void* slf, nCmd* cmd);
static void n_getvector4attr(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nscenenode

    @cppclass
    nSceneNode

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
    cl->AddCmd("v_removeanimator_s",    'REMA', n_removeanimator);
    cl->AddCmd("i_getnumanimators_v",   'GNMA', n_getnumanimators);
    cl->AddCmd("s_getanimatorat_i",     'GAAT', n_getanimatorat);
    cl->AddCmd("b_loadresources_v",     'LORE', n_loadresources);
    cl->AddCmd("v_setlocalbox_ffffff",  'SLCB', n_setlocalbox);
    cl->AddCmd("ffffff_getlocalbox_v",  'GLCB', n_getlocalbox);
    cl->AddCmd("v_setrenderpri_i",      'SRPR', n_setrenderpri);
    cl->AddCmd("i_getrenderpri_v",      'GRPR', n_getrenderpri);
    cl->AddCmd("v_addhints_i",          'ADDH', n_addhints);
    cl->AddCmd("v_clearhints_i",        'CLRH', n_clearhints);
    cl->AddCmd("i_gethints_v",          'GETH', n_gethints);
    cl->AddCmd("b_hashints_i",          'HASH', n_hashints);
    cl->AddCmd("v_setintattr_si",       'SINA', n_setintattr);
    cl->AddCmd("i_getintattr_s",        'GINA', n_getintattr);
    cl->AddCmd("v_setfloatattr_sf",     'SFLA', n_setfloatattr);
    cl->AddCmd("f_getfloatattr_s",      'GFLA', n_getfloatattr);
    cl->AddCmd("v_setboolattr_sb",      'SBOA', n_setboolattr);
    cl->AddCmd("b_getboolattr_s",       'GBOA', n_getboolattr);
    cl->AddCmd("v_setstringattr_ss",    'SSTA', n_setstringattr);
    cl->AddCmd("s_getstringattr_s",     'GSTA', n_getstringattr);
    cl->AddCmd("v_setvector3attr_sfff", 'SV3A', n_setvector3attr);
    cl->AddCmd("fff_getvector3attr_s",  'GV3A', n_getvector3attr);
    cl->AddCmd("v_setvector4attr_sffff", 'SV4A', n_setvector4attr);
    cl->AddCmd("ffff_getvector4attr_s",  'GV4A', n_getvector4attr);
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
    removeanimator
    @input
    s(AnimatorPath)
    @output
    v
    @info
    Removes an animator from the node's animator list
*/
static void
n_removeanimator(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    self->RemoveAnimator(cmd->In()->GetS());
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
    Define the local bounding box. Shape nodes compute their bounding
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
    v
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
    @cmd
    addhints
    @input
    i(HintMask)
    @output
    v
    @info
    Add one or more hint flags.
*/
static void
n_addhints(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    self->AddHints(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    clearhints
    @input
    i(HintMask)
    @output
    v
    @info
    Clear one or more hint flags.
*/
static void
n_clearhints(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    self->ClearHints(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gethints
    @input
    v
    @output
    i(HintMask)
    @info
    Get all currently set hint flags.
*/
static void
n_gethints(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetI(self->GetHints());
}

//------------------------------------------------------------------------------
/**
    @cmd
    hashints
    @input
    i(HintMask)
    @output
    b(Success)
    @info
    Return true if all hint flags in the mask are set.
*/
static void
n_hashints(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetB(self->HasHints(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setintattr
    @input
    s(Name), i(Value)
    @output
    v
    @info
    Set an int attribute on the node.
*/
static void
n_setintattr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    nString name = cmd->In()->GetS();
    int val = cmd->In()->GetI();
    self->SetIntAttr(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getintattr
    @input
    s(Name)
    @output
    i(Value)
    @info
    Get an int attribute from the node.
*/
static void
n_getintattr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetI(self->GetIntAttr(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfloatattr
    @input
    s(Name), f(Value)
    @output
    v
    @info
    Set a float attribute on the node.
*/
static void
n_setfloatattr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    nString name = cmd->In()->GetS();
    float val = cmd->In()->GetF();
    self->SetFloatAttr(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfloatattr
    @input
    s(Name)
    @output
    f(Value)
    @info
    Get a float attribute from the node.
*/
static void
n_getfloatattr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetF(self->GetFloatAttr(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setboolattr
    @input
    s(Name), b(Value)
    @output
    v
    @info
    Set a bool attribute on the node.
*/
static void
n_setboolattr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    nString name = cmd->In()->GetS();
    bool val = cmd->In()->GetB();
    self->SetBoolAttr(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getboolattr
    @input
    s(Name)
    @output
    b(Value)
    @info
    Get a bool attribute from the node.
*/
static void
n_getboolattr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetB(self->GetBoolAttr(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstringattr
    @input
    s(Name), s(Value)
    @output
    v
    @info
    Set a string attribute on the node.
*/
static void
n_setstringattr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    nString name = cmd->In()->GetS();
    nString val = cmd->In()->GetS();
    self->SetStringAttr(name, val);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstringattr
    @input
    s(Name)
    @output
    S(Value)
    @info
    Get a string attribute from the node.
*/
static void
n_getstringattr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    cmd->Out()->SetS(self->GetStringAttr(cmd->In()->GetS()).Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvector3attr
    @input
    s(Name), f(X), f(Y), f(Z)
    @output
    v
    @info
    Set a vector3 attribute on the node.
*/
static void
n_setvector3attr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    nString name = cmd->In()->GetS();
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->SetVector3Attr(name, v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvector3attr
    @input
    s(Name)
    @output
    f(X), f(Y), f(Z)
    @info
    Get a vector3 attribute from the node.
*/
static void
n_getvector3attr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    const vector3& v = self->GetVector3Attr(cmd->In()->GetS());
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvector4attr
    @input
    s(Name), f(X), f(Y), f(Z), f(W)
    @output
    v
    @info
    Set a vector4 attribute on the node.
*/
static void
n_setvector4attr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    nString name = cmd->In()->GetS();
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetVector4Attr(name, v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvector4attr
    @input
    s(Name)
    @output
    f(X), f(Y), f(Z), f(W)
    @info
    Get a vector4 attribute from the node.
*/
static void
n_getvector4attr(void* slf, nCmd* cmd)
{
    nSceneNode* self = (nSceneNode*) slf;
    const vector4& v = self->GetVector4Attr(cmd->In()->GetS());
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
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

        //--- addhints ---
        if (0 != this->GetHints())
        {
            cmd = ps->GetCmd(this, 'ADDH');
            cmd->In()->SetI(this->GetHints());
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

        //--- custom attributes ---
        num = this->attrs.Size();
        for (i = 0; i < num; i++)
        {
            const nAttr& attr = this->attrs[i];
            switch (attr.GetType())
            {
                case nAttr::Int:
                    cmd = ps->GetCmd(this, 'SINA');
                    cmd->In()->SetS(attr.GetName().Get());
                    cmd->In()->SetI(attr.GetInt());
                    ps->PutCmd(cmd);
                    break;

                case nAttr::Float:
                    cmd = ps->GetCmd(this, 'SFLA');
                    cmd->In()->SetS(attr.GetName().Get());
                    cmd->In()->SetF(attr.GetFloat());
                    ps->PutCmd(cmd);
                    break;

                case nAttr::Bool:
                    cmd = ps->GetCmd(this, 'SBOA');
                    cmd->In()->SetS(attr.GetName().Get());
                    cmd->In()->SetB(attr.GetBool());
                    ps->PutCmd(cmd);
                    break;

                case nAttr::String:
                    cmd = ps->GetCmd(this, 'SSTA');
                    cmd->In()->SetS(attr.GetName().Get());
                    cmd->In()->SetS(attr.GetString());
                    ps->PutCmd(cmd);
                    break;

                case nAttr::Vector3:
                    cmd = ps->GetCmd(this, 'SV3A');
                    cmd->In()->SetS(attr.GetName().Get());
                    cmd->In()->SetF(attr.GetVector3().x);
                    cmd->In()->SetF(attr.GetVector3().y);
                    cmd->In()->SetF(attr.GetVector3().z);
                    ps->PutCmd(cmd);
                    break;

                case nAttr::Vector4:
                    cmd = ps->GetCmd(this, 'SV4A');
                    cmd->In()->SetS(attr.GetName().Get());
                    cmd->In()->SetF(attr.GetVector4().x);
                    cmd->In()->SetF(attr.GetVector4().y);
                    cmd->In()->SetF(attr.GetVector4().z);
                    cmd->In()->SetF(attr.GetVector4().w);
                    ps->PutCmd(cmd);
                    break;
            }
        }
        return true;
    }
    return false;
}
