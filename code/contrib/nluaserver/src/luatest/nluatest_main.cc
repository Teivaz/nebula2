#define N_IMPLEMENTS nLuaTest
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//------------------------------------------------------------------------------
#include "kernel/ncmdprotonative.h"
#include "luatest/nluatest.h"

nNebulaScriptClass(nLuaTest, "nroot")

//------------------------------------------------------------------------------
/**
*/
nLuaTest::nLuaTest()
{
}

//------------------------------------------------------------------------------
/**
*/
nLuaTest::~nLuaTest()
{
}

//------------------------------------------------------------------------------
/**
  Tests script side cmds.
*/
void nLuaTest::TestScriptCmds()
{
    nRoot* node = this->kernelServer->Lookup("/tree");
    n_assert(node);
    nCmdProto* sampleCmdProto = node->GetClass()->FindCmdByName("SampleCmd");
    nCmdProto* sampleCmd2Proto = node->GetClass()->FindCmdByName("SampleCmd2");

    // test 1
    nCmd* cmd = sampleCmdProto->NewCmd();
    if (cmd)
    {
        cmd->In()->SetI(1);
        if (node->Dispatch(cmd))
        {
            if (cmd->Out()->GetI() == 1)
                n_printf("Test 1: pass\n");
            else
                n_printf("Test 1: fail\n");
        }
        else
            n_printf("nebula.tree.SampleCmd not implemented!\n");

        sampleCmdProto->RelCmd(cmd);
    }

    // test 2
    cmd = sampleCmd2Proto->NewCmd();
    if (cmd)
    {
        cmd->In()->SetS("true");
        cmd->In()->SetI(1);
        if (node->Dispatch(cmd))
        {
            if ((cmd->Out()->GetB() == true) && (cmd->Out()->GetI() == 1))
                n_printf("Test 2: pass\n");
            else
                n_printf("Test 2: fail\n");
        }
        else
            n_printf("nebula.tree.SampleCmd2 not implemented!\n");

        sampleCmd2Proto->RelCmd(cmd);
    }

    // test 3
    node = this->kernelServer->Lookup("/tree/node0");
    n_assert(node);
    cmd = sampleCmdProto->NewCmd();
    if (cmd)
    {
        cmd->In()->SetI(1);
        if (node->Dispatch(cmd))
        {
            if (cmd->Out()->GetI() == 2)
                n_printf("Test 3: pass\n");
            else
                n_printf("Test 3: fail\n");
        }
        else
            n_printf("nebula.tree.node0.SampleCmd not implemented!\n");

        sampleCmdProto->RelCmd(cmd);
    }

    // test 4
    cmd = sampleCmd2Proto->NewCmd();
    if (cmd)
    {
        cmd->In()->SetS("false");
        cmd->In()->SetI(1);
        if (node->Dispatch(cmd))
        {
            if ((cmd->Out()->GetB() == false) && (cmd->Out()->GetI() == 2))
                n_printf("Test 4: pass\n");
            else
                n_printf("Test 4: fail\n");
        }
        else
            n_printf("nebula.tree.node0.SampleCmd2 not implemented!\n");

        sampleCmd2Proto->RelCmd(cmd);
    }
}

//------------------------------------------------------------------------------
/**
  Removes some objects from the NOH (hopefuly leaving some zombie thunks
  on the lua side as a result... muaahahaha).
*/
void nLuaTest::MakeZombies()
{
    nRoot* node = this->kernelServer->Lookup("/tree/node1/node11");
    if (node)
        node->Release();
    node = this->kernelServer->Lookup("/tree/node2");
    if (node)
        node->Release();
}

//------------------------------------------------------------------------------
void nLuaTest::EatAnObject(nRoot* obj)
{
    n_assert(obj->IsA(this->kernelServer->FindClass("nroot")));
}

//------------------------------------------------------------------------------
/**
    @cmd
    nativeruntimecmd1

    @input
    fff

    @output
    i

    @info
    This cmd is added to nLuaTest at run-time from C++.
    It takes 3 floats as input and outputs an integer that represents the sum.
*/
static
void
n_nativeruntimecmd1(void* slf, nCmd* cmd)
{
    nLuaTest* self = (nLuaTest*)slf;
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    int sum = (int)(f1 + f2 + f3);
    cmd->Out()->SetI(sum);
}

//------------------------------------------------------------------------------
/**
    @cmd
    nativeruntimecmd2

    @input
    s

    @output
    s

    @info
    This cmd is added to nLuaTest at run-time from C++.
    It takes one string as input and outputs an identical string.
*/
static
void
n_nativeruntimecmd2(void* slf, nCmd* cmd)
{
    nLuaTest* self = (nLuaTest*)slf;
    const char* in = cmd->In()->GetS();
    const char* out = n_strdup(in);
    cmd->Out()->SetS(out);
    n_free((void*)out);
}

//------------------------------------------------------------------------------
/**
    Adds and tests natively implemented cmds.
*/
void nLuaTest::TestNativeCmds()
{
    // add the cmd protos to the nLuaTest class
    nClass* myClass = this->GetClass();
    myClass->BeginScriptCmds(2);
    nCmdProtoNative* cmdProtoNative = new nCmdProtoNative("i_NativeRuntimeCmd1_fff",
                                                          0, // id isn't used
                                                          n_nativeruntimecmd1);
    myClass->AddScriptCmd((nCmdProto*)cmdProtoNative);
    cmdProtoNative = new nCmdProtoNative("s_NativeRuntimeCmd2_s",
                                         0, // id isn't used
                                         n_nativeruntimecmd2);
    myClass->AddScriptCmd((nCmdProto*)cmdProtoNative);
    myClass->EndScriptCmds();

    // test NativeRuntimeCmd1
    nCmdProto* cmdProto = myClass->FindCmdByName("NativeRuntimeCmd1");
    nCmd* cmd = cmdProto->NewCmd();
    cmd->In()->SetF(1.0f);
    cmd->In()->SetF(2.0f);
    cmd->In()->SetF(3.0f);
    if (this->Dispatch(cmd))
    {
        if (cmd->Out()->GetI() == 6)
            n_printf("NativeRuntimeCmd1 Test Passed\n");
        else
            n_printf("NativeRuntimeCmd1 Test Failed\n");
    }
    cmdProto->RelCmd(cmd);

    // test NativeRuntimeCmd2
    cmdProto = myClass->FindCmdByName("NativeRuntimeCmd2");
    cmd = cmdProto->NewCmd();
    cmd->In()->SetS("Bootylicious");
    if (this->Dispatch(cmd))
    {
        if (strcmp(cmd->Out()->GetS(), "Bootylicious") == 0)
            n_printf("NativeRuntimeCmd2 Test Passed\n");
        else
            n_printf("NativeRuntimeCmd2 Test Failed\n");
    }
    cmdProto->RelCmd(cmd);
}

//------------------------------------------------------------------------------
/**
    @cmd
    nativerootcmd

    @input
    fff

    @output
    i

    @info
    This cmd is added to nRoot at run-time from C++.
    It takes 3 floats as input and outputs an integer that represents the sum.
*/
static
void
n_nativerootcmd(void* slf, nCmd* cmd)
{
    nLuaTest* self = (nLuaTest*)slf;
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    int sum = (int)(f1 + f2 + f3);
    cmd->Out()->SetI(sum);
}

//------------------------------------------------------------------------------
/**
    Tests a natively implemented cmd that was added at run-time to nRoot.

    By the time this method gets called cmds.lua has already been executed,
    as such nRoot already has 2 script-side implemented cmds. Here we add
    a third cmd that's implemented in C++.

    Note that script-side implemented cmds can have a unique implementation
    for every instance of an nRoot. However, cmds implemented in C++ will have
    a single implementation for all instances of nRoot. This limitation of C++
    implemented cmds comes about due to the fact that we have to store a
    function pointer in each cmd prototype, whereas script implemented cmds can
    just be looked up by name (stored in the cmd prototype).
*/
void nLuaTest::AddNativeCmdToRoot()
{
    nRoot* node = this->kernelServer->Lookup("/tree");
    n_assert(node);

    nCmdProtoNative* cmdProto = new nCmdProtoNative("i_NativeRootCmd_fff",
                                                    0, // id not used
                                                    n_nativerootcmd);
    /*
        This is very naughty since EndScriptCmds() has already been called
        by the time we get here, fortunately for us EndScriptCmds() doesn't
        actually do anything at the present time anyway :)
    */
    node->GetClass()->AddScriptCmd(cmdProto);

    /*
        Now every nRoot (and nRoot-derived) instance has the same
        NativeRootCmd cmd, that can be called from C++. It can also
        be called from script, but only using call(). Thunks aren't
        able to access the new cmd, in order to allow them to do so
        we'd have to update the Lua class cache. Alternatively it
        may be possible to modify the thunk metatable to look
        for the cmd elsewhere when it fails to find it in the class
        cache.
    */
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
