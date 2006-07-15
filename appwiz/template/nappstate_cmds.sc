//----------------------------------------------------------------------------
//  %(appStateNameL)s_cmds.cc 
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#include "%(subDirL)s/%(appStateNameL)s.h"

//----------------------------------------------------------------------------
/**
    @scriptclass
    %(appStateNameL)s

    @cppclass
    %(appStateName)s
    
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
