/**
   @file receiverscript_cmds.cc
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>

   (C) 2005 Tragnarion Studios
*/
#include "signals_tutorial/receiverscript.h"

static void n_trigger( void *, nCmd * );
static void n_onint( void *, nCmd * );

//------------------------------------------------------------------------------
/**
    @scriptclass
    receiverscript

    @cppclass
    ReceiverScript

    @superclass
    nroot

    @classinfo
    Test Receiver of signals.
*/
void
n_initcmds( nClass * cl )
{
    cl->BeginCmds();
    cl->AddCmd( "v_trigger_v", 'TRIG', n_trigger );
    cl->AddCmd( "v_onint_i", 'OINT', n_onint );
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    trigger
    @input
    v
    @output
    v
    @info
    Test command.
*/
static void n_trigger( void *o, nCmd * /*cmd*/ )
{
    ReceiverScript *self = static_cast<ReceiverScript *>( o );
    self->Trigger();
}

//------------------------------------------------------------------------------
/**
    @cmd
    onint
    @input
    i
    @output
    v
    @info
    Test command with parameter.
*/
static void n_onint( void *o, nCmd * cmd )
{
    ReceiverScript *self = static_cast<ReceiverScript *>( o );
    self->OnInt( cmd->In()->GetI() );
}
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
