//----------------------------------------------------------------------------
//   nappopenaldemo_cmds.cc
//
//  (C)2005 Kim, Hyoun Woo
//----------------------------------------------------------------------------
#include "nopenaldemo/nappopenaldemo.h"

//----------------------------------------------------------------------------
/**
    @scriptclass
    nappopenaldemo

    @cppclass
    nAppOpenALDemo

    @superclass
    napplication

    @classinfo
    a brief description of the class
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
