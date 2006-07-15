#include "kernel/nobject.h"
#include "signals/nsignal.h"
#include "signals/nsignalbinding.h"
#include "signals_tutorial/nsignaltestemitter.h"

NSIGNAL_DEFINE(nSignalTestEmitter,Testbii);
NSIGNAL_DEFINE(nSignalTestEmitter,Testbi2);
NSIGNAL_DEFINE(nSignalTestEmitter,Testffff);
NSIGNAL_DEFINE(nSignalTestEmitter,Testbbifs);
NSIGNAL_DEFINE(nSignalTestEmitter,Testvv);
NSIGNAL_DEFINE(nSignalTestEmitter,Testiv);
NSIGNAL_DEFINE(nSignalTestEmitter,Testfv);

void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->EndCmds();

    cl->BeginSignals(8);
    cl->AddSignal(NSIGNAL_OBJECT(nSignalTestEmitter,Testbii));
    cl->AddSignal(NSIGNAL_OBJECT(nSignalTestEmitter,Testbi2));
    cl->AddSignal(NSIGNAL_OBJECT(nSignalTestEmitter,Testffff));
    cl->AddSignal(NSIGNAL_OBJECT(nSignalTestEmitter,Testbbifs));
    cl->AddSignal(NSIGNAL_OBJECT(nSignalTestEmitter,Testvv));
    cl->AddSignal(NSIGNAL_OBJECT(nSignalTestEmitter,Testiv));
    cl->AddSignal(NSIGNAL_OBJECT(nSignalTestEmitter,Testfv));
    cl->EndSignals();
}
