#ifndef N_LUATEST_H
#define N_LUATEST_H
//------------------------------------------------------------------------------
/**
  @class nLuaTest
  @ingroup NLuaServerContribModule

  @brief A little test suite for testing the Lua server.

  (c) 2003 Vadim Macagon
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

//------------------------------------------------------------------------------
class nLuaTest : public nRoot
{
  public:
    /// constructor
    nLuaTest();
    /// destructor
    virtual ~nLuaTest();
    
    void TestScriptCmds();
    void MakeZombies();
    void EatAnObject( nRoot* obj );
    
    void TestNativeCmds();
    void AddNativeCmdToRoot();
};

//------------------------------------------------------------------------------
#endif
