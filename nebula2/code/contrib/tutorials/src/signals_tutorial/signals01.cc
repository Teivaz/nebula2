//------------------------------------------------------------------------------
/**
    @file signals01.cc
    @ingroup Nebula2TutorialsSignals

    @brief Signal Example 01 - Defining, Binding, Emitting and Posting signals.

    A simple example of signals, both synchronous and asynchronous.
*/

#include "kernel/nkernelserver.h"
#include "signals/nsignal.h"
#include "signals/nsignalbinding.h"
#include "signals/nsignalserver.h"
#include "signals/nsignalnative.h"
#include "signals/nsignalbindingnative.h"

#include "signals_tutorial/nsignaltestemitter.h"
#include "signals_tutorial/nsignaltestreceiver.h"

// Need this to get our test classes to work.
nNebulaUsePackage(signals01);

int
main(int argc, const char** argv)
{
    nKernelServer* kernelServer = new nKernelServer();

    kernelServer->AddPackage(signals01);

    // initialize signal server
    nSignalServer ss;

    nSignalTestEmitter * emitter = static_cast<nSignalTestEmitter *>
        (nKernelServer::Instance()->New("nsignaltestemitter", "/signals/emitter"));
    nSignalTestReceiver * receiver = static_cast<nSignalTestReceiver *>
        (nKernelServer::Instance()->New("nsignaltestreceiver", "/signals/receiver1"));
    nSignalTestReceiver * receiver2 = static_cast<nSignalTestReceiver *>
        (nKernelServer::Instance()->New("nsignaltestreceiver", "/signals/receiver2"));

    emitter->BindSignal(nSignalTestEmitter::SignalTestbii,
                        receiver, &nSignalTestReceiver::Signaledbi, 0);
    emitter->BindSignal(nSignalTestEmitter::SignalTestbii,
                        receiver2, &nSignalTestReceiver::Signaledbi, 0);
    emitter->BindSignal(nSignalTestEmitter::SignalTestffff,
                        receiver2, &nSignalTestReceiver::Signaledffff, 0);
    emitter->BindSignal(nSignalTestEmitter::SignalTestffff,
                        receiver2, &nSignalTestReceiver::Signaledffff, 0);
    emitter->BindSignal(nSignalTestEmitter::SignalTestffff,
                        receiver2, &nSignalTestReceiver::Signaledffff, 0);

    emitter->SignalTestbii(emitter, 1);
    emitter->SignalTestbii(emitter, 2);
    emitter->SignalTestbii(emitter, 3);
    emitter->SignalTestbi2(emitter, 4);
    emitter->SignalTestffff(emitter, 1.0f, 2.0f, 3.0f);

    // The following should fail to compile (type safety rocks!)
    //emitter->SignalTestbii(emitter, 1.0f);
    //emitter->SignalTestbii(emitter, 1.0f, 2.0f);
    //emitter->SignalTestbii(emitter, "hello");
    //emitter->SignalTestbii(emitter, emitter);
    //emitter->SignalTestbii(emitter, receiver);

    // Another way for signal emission (not possible to check params)
    emitter->EmitSignal("Testbii", 1);
    emitter->EmitSignal("Testbii", 2);
    emitter->EmitSignal("Testbii", 3);
    emitter->EmitSignal("Testffff", 1.0f, 2.0f, 3.0f);
    emitter->EmitSignal("Testffff", 5.0f, 6.0f, 7.0f);

    // Post some signals to execute in the future.
    emitter->PostSignal(5.f, "Testbii", 1);
    emitter->PostSignal(6.f, "Testbii", 2);
    emitter->PostSignal(4.f, "Testbii", 3);
    emitter->PostSignal(4.f, "Testffff", 10.0f, 20.0f, 30.0f);

    // Manually trigger the signal server to make it think sufficient
    // time has elapsed to execute the posted signals
    ss.Trigger(10.f);

    delete kernelServer;
    return 0;
}

