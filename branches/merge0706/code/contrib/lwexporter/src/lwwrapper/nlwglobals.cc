//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwrapper/nlwglobals.h"

namespace // anonymous namespace, local to this translation unit
{
    GlobalFunc* globalFunc = 0;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWGlobals::IsGlobalFuncSet()
{
    return (globalFunc != 0);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWGlobals::SetGlobalFunc(GlobalFunc* func)
{
    globalFunc = func;
}

//----------------------------------------------------------------------------
/**
*/
GlobalFunc* 
nLWGlobals::GetGlobalFunc()
{
    n_assert(globalFunc);
    return globalFunc;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
