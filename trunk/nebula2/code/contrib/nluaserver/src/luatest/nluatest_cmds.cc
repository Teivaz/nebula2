#define N_IMPLEMENTS nLuaTest
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//------------------------------------------------------------------------------
#include "luatest/nluatest.h"

static void n_testscriptcmds(void* slf, nCmd* cmd);
static void n_makezombies(void* slf, nCmd* cmd);
static void n_eatanobject(void* slf, nCmd* cmd);
static void n_testnativecmds(void* slf, nCmd* cmd);
static void n_addnativecmdtoroot(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nluatest

    @cppclass
    nLuaTest

    @superclass
    nroot

    @classinfo
    Just a simple test thingie for lua server stuff.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_testscriptcmds_v", 'TSC_', n_testscriptcmds);
    clazz->AddCmd("v_makezombies_v", 'MZOM', n_makezombies);
    clazz->AddCmd("v_eatanobject_o", 'EATO', n_eatanobject);
    clazz->AddCmd("v_testnativecmds_v", 'TNC_', n_testnativecmds);
    clazz->AddCmd("v_addnativecmdtoroot_v", 'ANCR', n_addnativecmdtoroot);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    testscriptcmds

    @input
    v

    @output
    v

    @info
    Test calling of script-side commands from C++.
*/
static
void
n_testscriptcmds(void* slf, nCmd* cmd)
{
    nLuaTest* self = (nLuaTest*)slf;
    self->TestScriptCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    makezombies

    @input
    v

    @output
    v

    @info
    Create some zombie thunks in the internal thunks store.
*/
static
void
n_makezombies(void* slf, nCmd* cmd)
{
    nLuaTest* self = (nLuaTest*)slf;
    self->MakeZombies();
}

//------------------------------------------------------------------------------
/**
    @cmd
    eatanobject

    @input
    o

    @output
    v

    @info
    Takes an object (thunk) as an input and checks it's a valid nRoot.
*/
static
void
n_eatanobject(void* slf, nCmd* cmd)
{
    nLuaTest* self = (nLuaTest*)slf;
    self->EatAnObject((nRoot*)cmd->In()->GetO());
}

//------------------------------------------------------------------------------
/**
    @cmd
    testnativecmds

    @input
    v

    @output
    v

    @info
    Test calling of commands added at run-time from C++.
*/
static
void
n_testnativecmds(void* slf, nCmd* cmd)
{
    nLuaTest* self = (nLuaTest*)slf;
    self->TestNativeCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addnativecmdtoroot

    @input
    v

    @output
    v

    @info
    Adds a natively implemented cmd to nRoot.
*/
static
void
n_addnativecmdtoroot(void* slf, nCmd* cmd)
{
    nLuaTest* self = (nLuaTest*)slf;
    self->AddNativeCmdToRoot();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
