#define N_IMPLEMENTS nLuaTest
//------------------------------------------------------------------------------
//  (c) 2003 Vadim Macagon
//------------------------------------------------------------------------------
#include "luaserver/nluatest.h"

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
//  EOF
//------------------------------------------------------------------------------
