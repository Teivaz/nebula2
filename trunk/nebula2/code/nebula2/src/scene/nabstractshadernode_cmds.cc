//------------------------------------------------------------------------------
//  nabstractshadernode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nabstractshadernode.h"
#include "kernel/npersistserver.h"

static void n_settexture(void* slf, nCmd* cmd);
static void n_gettexture(void* slf, nCmd* cmd);
static void n_setint(void* slf, nCmd* cmd);
static void n_getint(void* slf, nCmd* cmd);
static void n_setfloat(void* slf, nCmd* cmd);
static void n_getfloat(void* slf, nCmd* cmd);
static void n_setvector(void* slf, nCmd* cmd);
static void n_getvector(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nabstractshadernode

    @cppclass
    nAbstractShaderNode
    
    @superclass
    ntransformnode

    @classinfo
    Parent class for shader related scene node classes. Don't use this
    class directly, instead use its subclasses.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settexture_ss",   'STXT', n_settexture);
    cl->AddCmd("s_gettexture_s",    'GTXT', n_gettexture);
    cl->AddCmd("v_setint_si",       'SINT', n_setint);
    cl->AddCmd("i_getint_s",        'GINT', n_getint);
    cl->AddCmd("v_setfloat_sf",     'SFLT', n_setfloat);
    cl->AddCmd("f_getfloat_s",      'GFLT', n_getfloat);
    cl->AddCmd("v_setvector_sffff", 'SVEC', n_setvector);
    cl->AddCmd("ffff_getvector_s",  'GVEC', n_getvector);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    settexture
    @input
    s(VarName), s(TextureResource)
    @output
    v
    @info
    Set a texture resource shader variable.
*/
static void
n_settexture(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetTexture(nShader2::StringToParameter(s0), s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettexture
    @input
    s(VarName)
    @output
    s(TextureResource)
    @info
    Get a texture resource shader variable.
*/
static void
n_gettexture(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    cmd->Out()->SetS(self->GetTexture(nShader2::StringToParameter(cmd->In()->GetS())));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setint
    @input
    s(VarName), i(IntVal)
    @output
    v
    @info
    Set a integer shader variable.
*/
static void
n_setint(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    int i0 = cmd->In()->GetI();
    self->SetInt(nShader2::StringToParameter(s0), i0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getint
    @input
    s(VarName)
    @output
    i(IntVal)
    @info
    Get a integer shader variable.
*/
static void
n_getint(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    cmd->Out()->SetI(self->GetInt(nShader2::StringToParameter(cmd->In()->GetS())));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfloat
    @input
    s(VarName), f(FloatVal)
    @output
    v
    @info
    Set a float shader variable.
*/
static void
n_setfloat(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    float f0 = cmd->In()->GetF();
    self->SetFloat(nShader2::StringToParameter(s0), f0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfloat
    @input
    s(VarName)
    @output
    f(FloatVal)
    @info
    Get a float shader variable.
*/
static void
n_getfloat(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    cmd->Out()->SetF(self->GetFloat(nShader2::StringToParameter(cmd->In()->GetS())));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvector
    @input
    s(VarName), f(X), f(Y), f(Z), f(W)
    @output
    v
    @info
    Set a 4d vector shader variable.
*/
static void
n_setvector(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetVector(nShader2::StringToParameter(s0), v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvector
    @input
    s(VarName)
    @output
    f(X), f(Y), f(Z), f(W)
    @info
    Get a 4d vector shader variable.
*/
static void
n_getvector(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    vector4 v = self->GetVector(nShader2::StringToParameter(cmd->In()->GetS()));
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
*/
bool
nAbstractShaderNode::SaveCmds(nPersistServer* ps)
{
    if (nTransformNode::SaveCmds(ps))
    {
        nCmd* cmd;
        int i;
        int num;

        //--- settexture ---
        num = this->texNodeArray.Size();
        for (i = 0; i < num; i++)
        {
            TexNode& texNode = this->texNodeArray[i];
            cmd = ps->GetCmd(this, 'STXT');
            cmd->In()->SetS(nShader2::ParameterToString(texNode.shaderParameter));
            cmd->In()->SetS(texNode.texName.Get());
            ps->PutCmd(cmd);
        }

        //--- setint/setfloat/setvector ---
        for (i = 0; i < nShader2::NumParameters; i++)
        {
            const nShaderArg& param = this->shaderParams.GetArg((nShader2::Parameter)i);
            switch (param.GetType())
            {
                case nShaderArg::Void:
                    break;

                case nShaderArg::Int:
                    cmd = ps->GetCmd(this, 'SINT');
                    cmd->In()->SetS(nShader2::ParameterToString((nShader2::Parameter)i));
                    cmd->In()->SetI(param.GetInt());
                    ps->PutCmd(cmd);
                    break;

                case nShaderArg::Float:
                    cmd = ps->GetCmd(this, 'SFLT');
                    cmd->In()->SetS(nShader2::ParameterToString((nShader2::Parameter)i));
                    cmd->In()->SetF(param.GetFloat());
                    ps->PutCmd(cmd);
                    break;

                case nShaderArg::Float4:
                    {
                        cmd = ps->GetCmd(this, 'SVEC');
                        const nFloat4& f = param.GetFloat4();
                        cmd->In()->SetS(nShader2::ParameterToString((nShader2::Parameter)i));
                        cmd->In()->SetF(f.x);
                        cmd->In()->SetF(f.y);
                        cmd->In()->SetF(f.z);
                        cmd->In()->SetF(f.w);
                        ps->PutCmd(cmd);
                    }
                    break;

                case nShaderArg::Texture:
                    // ignore textures as they are already saved above.
                    break;

                default:
                    n_error("nAbstractShaderNode::SaveCmds(): Invalid variable type!\n");
                    break;
            }
        }

        return true;
    }
    return false;
}
