//-------------------------------------------------------------------
//  kernel/nenv_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/nenv.h"
#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/ncmdproto.h"

static void n_gettype(void *, nCmd *);
static void n_geti(void *, nCmd *);
static void n_getf(void *, nCmd *);
static void n_getb(void *, nCmd *);
static void n_gets(void *, nCmd *);
static void n_geto(void *, nCmd *);
static void n_getf4(void*, nCmd*);
static void n_seti(void *, nCmd *);
static void n_setf(void *, nCmd *);
static void n_setb(void *, nCmd *);
static void n_sets(void *, nCmd *);
static void n_seto(void *, nCmd *);
static void n_setf4(void*, nCmd*);

//-------------------------------------------------------------------
/**
    @scriptclass
    nenv

    @cppclass
    nEnv

    @superclass
    nroot

    @classinfo
    The nenv class is similar to an environment variable.
    Just create, set the type and value, and query it
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("s_gettype_v",   'GTYP', n_gettype);
    cl->AddCmd("i_geti_v",      'GETI', n_geti);
    cl->AddCmd("f_getf_v",      'GETF', n_getf);
    cl->AddCmd("b_getb_v",      'GETB', n_getb);
    cl->AddCmd("s_gets_v",      'GETS', n_gets);
    cl->AddCmd("o_geto_v",      'GETO', n_geto);
    cl->AddCmd("ffff_getf4_v",  'GTV4', n_getf4);
    cl->AddCmd("v_seti_i",      'SETI', n_seti);
    cl->AddCmd("v_setf_f",      'SETF', n_setf);
    cl->AddCmd("v_setb_b",      'SETB', n_setb);
    cl->AddCmd("v_sets_s",      'SETS', n_sets);
    cl->AddCmd("v_seto_o",      'SETO', n_seto);
    cl->AddCmd("v_setf4_ffff",  'STV4', n_setf4);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    gettype
    @input
    v
    @output
    s (Type = [void int float bool string object float4])
    @info
    Returns the datatype the variable is set to. If void
    is returned the variable is empty.
*/
static void n_gettype(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    char *st;
    nArg::Type t = self->GetType();
    switch (t) {
        case nArg::Int:            st = "int"; break;
        case nArg::Float:          st = "float"; break;
        case nArg::String:         st = "string"; break;
        case nArg::Bool:           st = "bool"; break;
        case nArg::Object:         st = "object"; break;
        case nArg::Float4:         st = "float4"; break;
        default:                   st="void"; break;
    }
    cmd->Out()->SetS(st);
}

//-------------------------------------------------------------------
/**
    @cmd
    geti

    @input
    v

    @output
    i (Value)

    @info
    Returns the content of the variable if it is an int
    variable. If it's not an int variable, 0 is returned and
    an error message is printed.
*/
static void n_geti(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    cmd->Out()->SetI(self->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    getf

    @input
    v

    @output
    f (Value)

    @info
    Returns the content of the variable if it is a float
    variable. If it's not a float variable, 0.0 is returned
    and an error message is printed.
*/
static void n_getf(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    cmd->Out()->SetF(self->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getb

    @input
    v

    @output
    b (Value)

    @info
    Returns the content of the variable if it is a bool
    variable. If it's not a bool, false is returned and an
    error message is printed.
*/
static void n_getb(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    cmd->Out()->SetB(self->GetB());
}

//-------------------------------------------------------------------
/**
    @cmd
    gets

    @input
    v

    @output
    s (Value)

    @info
    Returns the content of the variable if it is a string
    variable. If it's not a string variable, "" is returned
    and an error message is printed.
*/
static void n_gets(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    cmd->Out()->SetS(self->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    geto

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Returns the content of the variable as an object handle. If it's
    not an object handle, NULL is returned and an error message is printed.
*/
static void n_geto(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    cmd->Out()->SetO(self->GetO());
}

//-------------------------------------------------------------------
/**
    @cmd
    getf4

    @input
    v

    @output
    f(X), f(Y), f(Z), f(W)

    @info
    Return content as 4D vector.
*/
static void n_getf4(void* slf, nCmd* cmd)
{
    nEnv* self = (nEnv*) slf;
    const nFloat4& f = self->GetF4();
    cmd->Out()->SetF(f.x);
    cmd->Out()->SetF(f.y);
    cmd->Out()->SetF(f.z);
    cmd->Out()->SetF(f.w);
}

//-------------------------------------------------------------------
/**
    @cmd
    seti

    @input
    i (Value)

    @output
    v

    @info
    Sets the content of the variable to the passed integer value.
*/
static void n_seti(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetI(cmd->In()->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    setf

    @input
    f (Value)

    @output
    v

    @info
    Sets the content of the variable to the passed float value.
*/
static void n_setf(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetF(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    setb

    @input
    b (Value)

    @output
    v

    @info
    Sets the content of the variable to the passed boolean value.
*/
static void n_setb(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetB(cmd->In()->GetB());
}

//-------------------------------------------------------------------
/**
    @cmd
    sets

    @input
    s (Value)

    @output
    v

    @info
    Sets the content of the variable to the passed string.
*/
static void n_sets(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetS(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    seto

    @input
    s (ObjectHandle)

    @output
    v

    @info
    Sets the content of the variable to the passed object handle.
*/
static void n_seto(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetO((nRoot *)cmd->In()->GetO());
}

//-------------------------------------------------------------------
/**
    @cmd
    setf4

    @input
    f(X), f(Y), f(Z), f(W)

    @output
    v

    @info
    Sets the content of the variable to a 4D vector.
*/
static void n_setf4(void* slf, nCmd* cmd)
{
    nEnv* self = (nEnv*) slf;
    nFloat4 f;
    f.x = cmd->In()->GetF();
    f.y = cmd->In()->GetF();
    f.z = cmd->In()->GetF();
    f.w = cmd->In()->GetF();
    self->SetF4(f);
}

//-------------------------------------------------------------------
/**
     - 02-Jan-99   floh    machine generated
*/
bool nEnv::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nRoot::SaveCmds(fs))
    {
        nCmd *cmd;
        switch (this->GetType())
        {
            case nArg::Int:
                cmd = fs->GetCmd(this, 'SETI');
                cmd->In()->SetI(this->GetI());
                fs->PutCmd(cmd);
                break;

            case nArg::Float:
                cmd = fs->GetCmd(this, 'SETF');
                cmd->In()->SetF(this->GetF());
                fs->PutCmd(cmd);
                break;

            case nArg::String:
                cmd = fs->GetCmd(this, 'SETS');
                cmd->In()->SetS(this->GetS());
                fs->PutCmd(cmd);
                break;

            case nArg::Bool:
                cmd = fs->GetCmd(this, 'SETB');
                cmd->In()->SetB(this->GetB());
                fs->PutCmd(cmd);
                break;

            case nArg::Object:
                cmd = fs->GetCmd(this, 'SETO');
                cmd->In()->SetO(this->GetO());
                fs->PutCmd(cmd);
                break;

            case nArg::Float4:
                {
                    cmd = fs->GetCmd(this, 'STV4');
                    const nFloat4& f = this->GetF4();
                    cmd->In()->SetF(f.x);
                    cmd->In()->SetF(f.y);
                    cmd->In()->SetF(f.z);
                    cmd->In()->SetF(f.w);
                    fs->PutCmd(cmd);
                }
                break;

            default: break;
        }
        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
