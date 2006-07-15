//----------------------------------------------------------------------------
//   %(classNameL)s_cmds.cc
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#include "%(subDirL)s/%(classNameL)s.h"

//----------------------------------------------------------------------------
/**
    @scriptclass
    %(classNameL)s

    @cppclass
    %(className)s
    
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
