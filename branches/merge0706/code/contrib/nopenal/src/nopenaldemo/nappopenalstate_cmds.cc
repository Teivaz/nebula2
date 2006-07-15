//----------------------------------------------------------------------------
//  nappopenalstate_cmds.cc 
//
//  (C)2005 Kim, Hyoun Woo
//----------------------------------------------------------------------------
#include "nopenaldemo/nappopenalstate.h"

//----------------------------------------------------------------------------
/**
    @scriptclass
    nappopenalstate

    @cppclass
    nAppOpenALState
    
    @superclass
    nappstate

    @classinfo

*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    // clazz->AddCmd("v_Xxx_v", 'XXXX', n_Xxx);
    // etc.
    clazz->EndCmds();
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
