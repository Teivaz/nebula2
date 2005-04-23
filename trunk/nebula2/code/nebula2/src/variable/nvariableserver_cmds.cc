//------------------------------------------------------------------------------
//  nvariableserver_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "variable/nvariableserver.h"

static void n_declarevariable(void* slf, nCmd* cmd);
static void n_getnumvariables(void* slf, nCmd* cmd);
static void n_getvariableat(void* slf, nCmd* cmd);
static void n_getfloatvariable(void* slf, nCmd* cmd);
static void n_getintvariable(void* slf, nCmd* cmd);
static void n_getvectorvariable(void* slf, nCmd* cmd);
static void n_getstringvariable(void* slf, nCmd* cmd);
static void n_setfloatvariable(void* slf, nCmd* cmd);
static void n_setintvariable(void* slf, nCmd* cmd);
static void n_setvectorvariable(void* slf, nCmd* cmd);
static void n_setstringvariable(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nvariableserver    

    @cppclass
    nVariableServer

    @superclass
    nroot

    @classinfo
    Manages variable declarations. A variable declaration consists
    of a variable name and an associated variable fourcc code.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("i_declarevariable_ss",      'DCLV', n_declarevariable);
    cl->AddCmd("i_getnumvariables_v",       'GNMV', n_getnumvariables);
    cl->AddCmd("ss_getvariableat_i",        'GVAT', n_getvariableat);
    cl->AddCmd("f_getfloatvariable_s",      'GFLV', n_getfloatvariable);
    cl->AddCmd("i_getintvariable_s",        'GINV', n_getintvariable);
    cl->AddCmd("ffff_getvectorvariable_s",  'GVCV', n_getvectorvariable);
    cl->AddCmd("s_getstringvariable_s",     'GSTV', n_getstringvariable);
    cl->AddCmd("v_setfloatvariable_sf",     'SFLV', n_setfloatvariable);
    cl->AddCmd("v_setintvariable_si",       'SINV', n_setintvariable);
    cl->AddCmd("v_setvectorvariable_sffff", 'SVCV', n_setvectorvariable);
    cl->AddCmd("v_setstringvariable_ss",    'SSTV', n_setstringvariable);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    declarevariable
    @input
    s(VariableName), s(VariableFourCC)
    @output
    i(VariableHandle)
    @info
    Add a new variable declaration. Declaring variables is not required but
    the only way to get variables with both a valid name and a valid fourcc
    code.

    Returns the variable's handle.
*/
static void
n_declarevariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    nVariable::Handle handle;
    handle = self->DeclareVariable(s0, nVariableServer::StringToFourCC(s1));
    cmd->Out()->SetI((int)handle);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumvariables
    @input
    v
    @output
    i(NumVariables)
    @info
    Get number of variable declarations.
*/
static void
n_getnumvariables(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    cmd->Out()->SetI(self->GetNumVariables());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvariableat
    @input
    i(VariableIndex)
    @output
    s(VariableName), s(VariableFourCC)
    @info
    Return the variable name and fourcc code at given index.
*/
static void
n_getvariableat(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    const char* varName;
    nFourCC varFourCC;
    char buf[5];
    self->GetVariableAt(cmd->In()->GetI(), varName, varFourCC);
    cmd->Out()->SetS(varName);
    cmd->Out()->SetS(nVariableServer::FourCCToString(varFourCC, buf, sizeof(buf)));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfloatvariable
    @input
    s(VarName)
    @output
    f(Value)
    @info
    Get float value from a global variable.
*/
static void
n_getfloatvariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    nVariable::Handle var = self->GetVariableHandleByName(cmd->In()->GetS());
    cmd->Out()->SetF(self->GetFloatVariable(var));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getintvariable
    @input
    s(VarName)
    @output
    i(Value)
    @info
    Get integer value from a global variable.
*/
static void
n_getintvariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    nVariable::Handle var = self->GetVariableHandleByName(cmd->In()->GetS());
    cmd->Out()->SetI(self->GetIntVariable(var));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvectorvariable
    @input
    s(VarName)
    @output
    ffff(Value)
    @info
    Get vector4 value from a global variable.
*/
static void
n_getvectorvariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    nVariable::Handle var = self->GetVariableHandleByName(cmd->In()->GetS());
    cmd->Out()->SetV4(self->GetVectorVariable(var));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstringvariable
    @input
    s(VarName)
    @output
    s(Value)
    @info
    Get string value from a global variable.
*/
static void
n_getstringvariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    nVariable::Handle var = self->GetVariableHandleByName(cmd->In()->GetS());
    cmd->Out()->SetS(self->GetStringVariable(var));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfloatvariable
    @input
    s(VarName), f(Value)
    @output
    v
    @info
    Set float value of a global variable.
*/
static void
n_setfloatvariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    nVariable::Handle var = self->GetVariableHandleByName(cmd->In()->GetS());
    self->SetFloatVariable(var, cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setintvariable
    @input
    s(VarName), i(Value)
    @output
    v
    @info
    Set integer value of a global variable.
*/
static void
n_setintvariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    nVariable::Handle var = self->GetVariableHandleByName(cmd->In()->GetS());
    self->SetIntVariable(var, cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvectorvariable
    @input
    s(VarName), f(ValX), f(ValY), f(ValZ), f(ValW)
    @output
    v
    @info
    Set vector value of a global variable.
*/
static void
n_setvectorvariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    nVariable::Handle var = self->GetVariableHandleByName(cmd->In()->GetS());
    vector4 vec;
    vec.x = cmd->In()->GetF();
    vec.y = cmd->In()->GetF();
    vec.z = cmd->In()->GetF();
    vec.w = cmd->In()->GetF();
    self->SetVectorVariable(var, vec);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstringvariable
    @input
    s(VarName), s(Value)
    @output
    v
    @info
    Set string value of a global variable.
*/
static void
n_setstringvariable(void* slf, nCmd* cmd)
{
    nVariableServer* self = (nVariableServer*) slf;
    nVariable::Handle var = self->GetVariableHandleByName(cmd->In()->GetS());
    self->SetStringVariable(var, cmd->In()->GetS());
}
