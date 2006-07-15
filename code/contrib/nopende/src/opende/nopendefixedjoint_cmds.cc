//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendefixedjoint.h"
#include "kernel/npersistserver.h"

static void n_Fix( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendefixedjoint
    
    @superclass
    nopendejoint

    @classinfo
    Encapsulates some fixed joint related dJoint functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_Fix_v", 'FIX_', n_Fix );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    Fix

    @input
    v

    @output
    v

    @info
    Call this on the fixed joint after it has been attached to remember 
    the current desired relative offset between the bodies it is attached to.
*/
static
void n_Fix( void* slf, nCmd* cmd )
{
  nOpendeFixedJoint* self = (nOpendeFixedJoint*)slf;
  self->Fix();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeFixedJoint::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeJoint::SaveCmds( ps ) )
    {
        return true;
    }
    return false;
}
