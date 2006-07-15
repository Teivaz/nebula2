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
nNebulaUsePackage(nkernel);
nNebulaUsePackage(signals01);

int
main(int argc, const char** argv)
{
    nKernelServer* kernelServer = new nKernelServer();

    kernelServer->AddPackage(nkernel);
    kernelServer->AddPackage(signals01);

    // initialize signal server
    nSignalServer * ss = static_cast<nSignalServer *>( kernelServer->New( "nsignalserver", "/sys/servers/signals" ) );

    nSignalTestEmitter * emitter = static_cast<nSignalTestEmitter *>
        (nKernelServer::Instance()->New("nsignaltestemitter", "/signals/emitter"));
    nSignalTestReceiver * receiver = static_cast<nSignalTestReceiver *>
        (nKernelServer::Instance()->New("nsignaltestreceiver", "/signals/receiver1"));
    nSignalTestReceiver * receiver2 = static_cast<nSignalTestReceiver *>
        (nKernelServer::Instance()->New("nsignaltestreceiver", "/signals/receiver2"));

    emitter->BindSignal(nSignalTestEmitter::SignalTestbii,
                        receiver, &nSignalTestReceiver::Signaledbi, 0);
    emitter->BindSignal(nSignalTestEmitter::SignalTestbii,
                        receiver2, &nSignalTestReceiver::Signaledbi, 2);
    emitter->BindSignal(nSignalTestEmitter::SignalTestbii,
                        receiver, &nSignalTestReceiver::Signaledbi, 1);

    emitter->BindSignal(nSignalTestEmitter::SignalTestffff,
                        receiver2, &nSignalTestReceiver::Signaledffff, 2);
    emitter->BindSignal(nSignalTestEmitter::SignalTestffff,
                        receiver2, &nSignalTestReceiver::Signaledffff, 2);
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
    emitter->EmitSignal("testbii", 1);
    emitter->EmitSignal("testbii", 2);
    emitter->EmitSignal("testbii", 3);
    emitter->EmitSignal("testffff", 1.0f, 2.0f, 3.0f);
    emitter->EmitSignal("testffff", 5.0f, 6.0f, 7.0f);

    // Post some signals to execute in the future.
    emitter->PostSignal(5.f, "testbii", 1);
    emitter->PostSignal(6.f, "testbii", 2);
    emitter->PostSignal(4.f, "testbii", 3);
    emitter->PostSignal(4.f, "testffff", 10.0f, 20.0f, 30.0f);

    // Manually trigger the signal server to make it think sufficient
    // time has elapsed to execute the posted signals
    ss->Trigger( 10.0f );

    delete kernelServer;
    return 0;
}

