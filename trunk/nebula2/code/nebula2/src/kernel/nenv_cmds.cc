#define N_IMPLEMENTS nEnv
#define N_KERNEL
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
static void n_seti(void *, nCmd *);
static void n_setf(void *, nCmd *);
static void n_setb(void *, nCmd *);
static void n_sets(void *, nCmd *);
static void n_seto(void *, nCmd *);

//-------------------------------------------------------------------
//  CLASS
//  nenv
//  SUPERCLASS
//  nroot
//  INCLUDE
//  kernel/nenv.h
//  INFO
//  The nenv class is similar to an environment variable.
//  Just create, set getypten value and query it 
//-------------------------------------------------------------------
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("s_gettype_v",'GTYP',n_gettype);
    cl->AddCmd("i_geti_v",'GETI',n_geti);
    cl->AddCmd("f_getf_v",'GETF',n_getf);
    cl->AddCmd("b_getb_v",'GETB',n_getb);
    cl->AddCmd("s_gets_v",'GETS',n_gets);
    cl->AddCmd("o_geto_v",'GETO',n_geto);
    cl->AddCmd("v_seti_i",'SETI',n_seti);
    cl->AddCmd("v_setf_f",'SETF',n_setf);
    cl->AddCmd("v_setb_b",'SETB',n_setb);
    cl->AddCmd("v_sets_s",'SETS',n_sets);
    cl->AddCmd("v_seto_o",'SETO',n_seto);
    cl->EndCmds();
}

//-------------------------------------------------------------------
//  CMD
//  gettype
//  INPUT
//  v
//  OUTPUT
//  s (Type = [void int float bool string object]
//  INFO
//  Returns the datatype the variable is set to. If void 
//  is returned the variable is empty. 
//-------------------------------------------------------------------
static void n_gettype(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    char *st;
    nArg::ArgType t = self->GetType();
    switch (t) {
        case nArg::ARGTYPE_INT:    st="int"; break;
        case nArg::ARGTYPE_FLOAT:  st="float"; break;
        case nArg::ARGTYPE_STRING: st="string"; break;
        case nArg::ARGTYPE_BOOL:   st="bool"; break;
        case nArg::ARGTYPE_OBJECT: st="object"; break;
        default:                   st="void"; break;
    }
    cmd->Out()->SetS(st);
}

//-------------------------------------------------------------------
//  CMD
//  geti
//  INPUT
//  v
//  OUTPUT
//  i (Value)
//  INFO
//  Returns the content of the variable if it is an int 
//  variable. If it's not an int variable 0 is returned and 
//  an error message. 
//-------------------------------------------------------------------
static void n_geti(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    int i = 0;
    if (self->GetType() == nArg::ARGTYPE_INT) i=self->GetI();
    else n_printf("Not an integer object!\n");
    cmd->Out()->SetI(i);
}

//-------------------------------------------------------------------
//  CMD
//  getf
//  INPUT
//  v
//  OUTPUT
//  f (Value)
//  INFO
//  Returns the content of the variable if it is a float 
//  variable. If it's not a float variable 0.0 is returned. 
//-------------------------------------------------------------------
static void n_getf(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    {
        float f = 0.0;
        if (self->GetType() == nArg::ARGTYPE_FLOAT) f=self->GetF();
        else n_printf("Not a float object!\n");
        cmd->Out()->SetF(f);
    }
}

//-------------------------------------------------------------------
//  CMD
//  getb
//  INPUT
//  v
//  OUTPUT
//  b (Value)
//  INFO
//  Returns the content of the variable if it is a bool 
//  variable. If it's not a bool false is returned. 
//-------------------------------------------------------------------
static void n_getb(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    {
        bool b = false;
        if (self->GetType() == nArg::ARGTYPE_BOOL) b=self->GetB();
        else n_printf("Not a bool object!\n");
        cmd->Out()->SetB(b);
    }
}

//-------------------------------------------------------------------
//  CMD
//  gets
//  INPUT
//  v
//  OUTPUT
//  s (Value)
//  INFO
//  Returns the content of the variable if it is a string 
//  variable. If it's not a string variable "" is returned. 
//-------------------------------------------------------------------
static void n_gets(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    {
        const char *s = "<error>";
        if (self->GetType() == nArg::ARGTYPE_STRING) s=self->GetS();
        else n_printf("Not a string object!\n");
        cmd->Out()->SetS(s);
    }
}

//-------------------------------------------------------------------
//  CMD
//  geto
//  INPUT
//  v
//  OUTPUT
//  o (ObjectHandle)
//  INFO
//  Returns the content of the variable as an object handle. 
//-------------------------------------------------------------------
static void n_geto(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    {
        void *o = NULL;
        if (self->GetType() == nArg::ARGTYPE_OBJECT) o=self->GetO();
        else n_printf("Not an object handle!\n");
        cmd->Out()->SetO(o);
    }
}

//-------------------------------------------------------------------
//  CMD
//  seti
//  INPUT
//  i (Value)
//  OUTPUT
//  v
//  INFO
//  Sets the content of the variable to the passed integer value. 
//-------------------------------------------------------------------
static void n_seti(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetI(cmd->In()->GetI());
}

//-------------------------------------------------------------------
//  CMD
//  setf
//  INPUT
//  f (Value)
//  OUTPUT
//  v
//  INFO
//  Sets the content of the variable to the passed float value. 
//-------------------------------------------------------------------
static void n_setf(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetF(cmd->In()->GetF());
}

//-------------------------------------------------------------------
//  CMD
//  setb
//  INPUT
//  b (Value)
//  OUTPUT
//  v
//  INFO
//  Sets the content of the variable to the passed boolean value. 
//-------------------------------------------------------------------
static void n_setb(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetB(cmd->In()->GetB());
}

//-------------------------------------------------------------------
//  CMD
//  sets
//  INPUT
//  s (Value)
//  OUTPUT
//  v
//  INFO
//  Sets the content of the variable to the passed string. 
//-------------------------------------------------------------------
static void n_sets(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetS(cmd->In()->GetS());
}

//-------------------------------------------------------------------
//  CMD
//  seto
//  INPUT
//  s (ObjectHandle)
//  OUTPUT
//  v
//  INFO
//  Sets the content of the variable to the passed object handle. 
//-------------------------------------------------------------------
static void n_seto(void *o, nCmd *cmd)
{
    nEnv *self = (nEnv *) o;
    self->SetO((nRoot *)cmd->In()->GetO());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nEnv::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nRoot::SaveCmds(fs)) 
    {
        nCmd *cmd;
        switch (this->GetType()) 
        {
            case nArg::ARGTYPE_INT:
                cmd = fs->GetCmd(this, 'SETI');
                cmd->In()->SetI(this->GetI());
                fs->PutCmd(cmd);
                break;

            case nArg::ARGTYPE_FLOAT:
                cmd = fs->GetCmd(this, 'SETF');
                cmd->In()->SetF(this->GetF());
                fs->PutCmd(cmd);
                break;

            case nArg::ARGTYPE_STRING:
                cmd = fs->GetCmd(this, 'SETS');
                cmd->In()->SetS(this->GetS());
                fs->PutCmd(cmd);
                break;

            case nArg::ARGTYPE_BOOL:
                cmd = fs->GetCmd(this, 'SETB');
                cmd->In()->SetB(this->GetB());
                fs->PutCmd(cmd);
                break;

            case nArg::ARGTYPE_OBJECT:
                cmd = fs->GetCmd(this, 'SETO');
                cmd->In()->SetO(this->GetO());
                fs->PutCmd(cmd);
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
