#include "signals_tutorial/emitter.h"

NSIGNAL_DEFINE( Emitter, Trigger );
NSIGNAL_DEFINE( Emitter, Trigger2 );
NSIGNAL_DEFINE( Emitter, OnInt );
NSIGNAL_DEFINE( Emitter, GetKey );

void 
n_initcmds( nClass * cl )
{

    cl->BeginCmds();
    cl->EndCmds();

    cl->BeginSignals( 1 );
    cl->AddSignal( NSIGNAL_OBJECT( Emitter, Trigger ) );
    cl->AddSignal( NSIGNAL_OBJECT( Emitter, Trigger2 ) );
    cl->AddSignal( NSIGNAL_OBJECT( Emitter, OnInt ) );
    cl->AddSignal( NSIGNAL_OBJECT( Emitter, GetKey ) );
    cl->EndSignals();
}