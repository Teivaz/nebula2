/**
    @file signals02.cc
    @ingroup Nebula2TutorialsSignals
    @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>

    @brief Signal Example 02 - A demonstration of nearly all signal features
    including scripting support.

    (C) 2005 Tragnarion Studios
*/
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "signals/nsignalserver.h"
#include "signals_tutorial/emitter.h"
#include "signals_tutorial/receiver.h"
#include "signals_tutorial/receiverscript.h"
#include "kernel/ncmdargtypes.h"
//------------------------------------------------------------------------------
nNebulaUsePackage( nkernel );
nNebulaUsePackage( signals02 );
nNebulaUsePackage( nnebula );
nNebulaUsePackage( npythonserver );
nNebulaUsePackage( nlua );
//------------------------------------------------------------------------------

const float TIME_INC = 2.0f;
const float ACCEL_FACTOR = 1 / 4.0f;

//------------------------------------------------------------------------------
void TestEmit();
void TestPost();
void TestPriority();
void TestOutParameters();
void TestScriptingTCL();
void TestScriptingLUA();
void TestScriptingPYTHON();
void TestScriptingCmdsLUA();
void TestScriptingCmdsPYTHON();
//------------------------------------------------------------------------------
/**
    empty function needed for use Python scripting
*/
void
nPythonRegisterPackages( nKernelServer * /*ks*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
int
main( int argc, const char** argv )
{
    nKernelServer* ks = n_new( nKernelServer() );
    ks->AddPackage( nkernel );
    ks->AddPackage( signals02 );
    ks->AddPackage( nnebula );
    ks->AddPackage( npythonserver );
    ks->AddPackage( nlua );

    ks->New( "nsignalserver", "/sys/servers/signals" );

    TestEmit();
    TestPost();
    TestPriority();
    TestOutParameters();

    // the scriptint tests should have the same output
    TestScriptingTCL();
    TestScriptingLUA();
    TestScriptingPYTHON();

    TestScriptingCmdsLUA();
    TestScriptingCmdsPYTHON();

    n_delete( ks );
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
TestEmit()
{
    n_printf( "\n-- Testing emit(synchronous) signals --\n\n" );

    nKernelServer* ks = nKernelServer::Instance();
    Emitter * emitter = static_cast<Emitter*>( ks->New( "emitter", "/signals/emitter" ) );
    Receiver * receiver = static_cast<Receiver*>( ks->New( "receiver" ) );

    // bind signals with receiver methods
    emitter->BindSignal( Emitter::SignalTrigger, receiver, &Receiver::Trigger, 0 );
    //
    emitter->BindSignal( Emitter::SignalTrigger2, receiver, &Receiver::Trigger, 0 );
    emitter->BindSignal( Emitter::SignalOnInt, receiver, &Receiver::OnInt, 0 );

    // different ways for emit signal Trigger
    emitter->SignalTrigger( emitter );
    emitter->EmitSignal( "trigger" );
    emitter->EmitSignal( &NSIGNAL_OBJECT( Emitter, Trigger ) );
    emitter->EmitSignal( &Emitter::SignalTrigger );
    emitter->EmitSignal( &NSIGNAL_OBJECT( Emitter, Trigger2 ), 1.3f );

    // different ways for emit signal OnInt
    emitter->SignalOnInt( emitter, 1 );
    emitter->EmitSignal( "onint", 2 );
    emitter->EmitSignal( &Emitter::SignalOnInt, 3 );
    emitter->EmitSignal( &NSIGNAL_OBJECT( Emitter, OnInt ), 4 );

    receiver->Release();
    emitter->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
TestPost()
{
    n_printf( "\n-- Testing post(asynchronuos) signals --\n\n" );

    nKernelServer* ks = nKernelServer::Instance();
    nSignalServer * signalServer = nSignalServer::Instance();

    Emitter * emitter = static_cast<Emitter*>( ks->New( "emitter", "/signals/emitter" ) );
    Receiver * receiver = static_cast<Receiver*>( ks->New( "receiver" ) );

    // bind signals with receiver methods
    emitter->BindSignal( Emitter::SignalTrigger, receiver, &Receiver::Trigger, 0 );
    emitter->BindSignal( Emitter::SignalOnInt, receiver, &Receiver::OnInt, 0 );

    // post signal, indicate when it will be executed
    emitter->PostSignal( 2.0f, "trigger" );
    emitter->PostSignal( 4.0f, &NSIGNAL_OBJECT( Emitter, Trigger ) );
    emitter->PostSignal( 8.0f, &Emitter::SignalTrigger );
    emitter->PostSignal( 10.0f, &Emitter::SignalOnInt, 1 );
    emitter->PostSignal( 6.0f, &Emitter::SignalOnInt, 2 );

    // wait for asynchronous signals
    nTime time = 0.0f;
    while( signalServer->AreSignalsPending() )
    {
        signalServer->Trigger( time );
        time += TIME_INC;
        n_sleep( ACCEL_FACTOR * TIME_INC ); // use magic to accelerate time
    }

    receiver->Release();
    emitter->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
TestPriority()
{
    n_printf( "\n-- Testing priority signals --\n\n" );

    nKernelServer* ks = nKernelServer::Instance();
    Emitter * emitter = static_cast<Emitter*>( ks->New( "emitter", "/signals/emitter" ) );
    Receiver * receiver01 = static_cast<Receiver*>( ks->New( "receiver" ) );
    Receiver * receiver02 = static_cast<Receiver*>( ks->New( "receiver" ) );
    Receiver * receiver03 = static_cast<Receiver*>( ks->New( "receiver" ) );

    // set the number of receiver to identify they.
    receiver01->SetNumber( 1 );
    receiver02->SetNumber( 2 );
    receiver03->SetNumber( 3 );

    // bind signals with receiver methods
    // the binds have different priorities, lower number has high priority.
    emitter->BindSignal( Emitter::SignalTrigger, receiver01, &Receiver::Trigger, 6 );
    emitter->BindSignal( Emitter::SignalTrigger, receiver02, &Receiver::Trigger, 4 );
    emitter->BindSignal( Emitter::SignalTrigger, receiver03, &Receiver::Trigger, 2 );

    // emit signal to all receivers binded.
    emitter->EmitSignal( &NSIGNAL_OBJECT( Emitter, Trigger ) );

    receiver03->Release();
    receiver02->Release();
    receiver01->Release();
    emitter->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
TestOutParameters()
{
    n_printf( "\n-- Testing out parameters of signals --\n\n" );

    n_printf( " The out parameters implementation aren't completed and can't be tested\n" );

}

//------------------------------------------------------------------------------
/**
*/
void
TestScriptingTCL()
{
    n_printf( "\n-- Testing scripting signals in TCL --\n\n" );

    nKernelServer* ks = nKernelServer::Instance();
    nSignalServer * signalServer = nSignalServer::Instance();

    nScriptServer* tclScript = static_cast<nScriptServer*>( ks->New( "ntclserver", "/sys/servers/script" ) );
    Emitter * emitter = static_cast<Emitter*>( ks->New( "emitter", "/signals/emitter" ) );
    Receiver * receiver = static_cast<Receiver*>( ks->New( "receiver" ) );
    ReceiverScript * receiverScript = static_cast<ReceiverScript*>(
            ks->New( "receiverscript", "/signals/receiverscript" )
        );

    // bind signals with receiver methods
    emitter->BindSignal( Emitter::SignalTrigger, receiver, &Receiver::Trigger, 0 );

    // create Tcl script
    // the script does binding, emit with name and with actual object, and post.
    nString result;
    nString programTcl(
        // Signal Registration
        "/signals/emitter.addsignal \"v_newsignal_i\" \"SFS1\" \n"

        // Signal Binding
        "/signals/emitter.bindsignal \"trigger\" /signals/receiverscript \"trigger\" 0 \n"
        "/signals/emitter.bindsignal \"onint\" /signals/receiverscript \"onint\" 0 \n"
        "/signals/emitter.bindsignal \"newsignal\" /signals/receiverscript \"onint\" 0 \n"

        // Signal Emit
        "emit /signals/emitter.trigger \n"
        "sel /signals/emitter \n"
        "emit .trigger \n"
        "emit .onint 42 \n"
        "emit .newsignal 13 \n"

        // Signal Post
        "post 2.f .trigger \n"
        "post 4.f .onint 24 \n"
        );
    // run Tcl script
    tclScript->Run( programTcl.Get(), result );

    // emit signal from code that also affect binding in script
    emitter->EmitSignal( &Emitter::SignalTrigger );

    // emit a signal created from script
    emitter->EmitSignal( "newsignal", 31 );

    // wait for asynchronous signals
    n_printf( "wating for asynchronous signals ... \n" );
    nTime time = 0.0f;
    while( signalServer->AreSignalsPending() )
    {
        signalServer->Trigger( time );
        time += TIME_INC;
        n_sleep( ACCEL_FACTOR * TIME_INC ); // use magic to accelerate time
    }

    receiverScript->Release();
    receiver->Release();
    emitter->Release();
    tclScript->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
TestScriptingLUA()
{
    n_printf( "\n-- Testing scripting signals in Lua --\n\n" );

    nKernelServer* ks = nKernelServer::Instance();
    nSignalServer * signalServer = nSignalServer::Instance();

    nScriptServer* luaScript = static_cast<nScriptServer*>( ks->New( "nluaserver", "/sys/servers/luascript" ) );
    Emitter * emitter = static_cast<Emitter*>( ks->New( "emitter", "/signals/emitter" ) );
    Receiver * receiver = static_cast<Receiver*>( ks->New( "receiver" ) );
    ReceiverScript * receiverScript = static_cast<ReceiverScript*>(
            ks->New( "receiverscript", "/signals/receiverscript" )
        );

    // bind signals with receiver methods
    emitter->BindSignal( Emitter::SignalTrigger, receiver, &Receiver::Trigger, 0 );

    // create Lua script
    // the script does binding, emit with name and with actual object, and post.
    nString result;
    nString programLua(
        "emitter = lookup( '/signals/emitter' ) \n"
        "obj = lookup( '/signals/receiverscript' ) \n"
        "sel( '/signals/emitter' ) \n"

        // Signal Registration
        "call( 'addsignal', 'v_newsignal_i', 'SFS1' ) \n"

        // Signal Binding
        "call( 'bindsignal', 'trigger', obj, 'trigger', 0 ) \n"
        "call( 'bindsignal', 'onint', obj, 'onint', 0 ) \n"
        "call( 'bindsignal', 'newsignal', obj, 'onint', 0 ) \n"

        // Signal Emit
        // dont call emit or post with call(), there isn't cmds
        //"call( 'emit', 'trigger' ) \n"
        "emitter:emit( 'trigger' ) \n"
        "emitter:emit( 'trigger' ) \n"
        "emitter:emit( 'onint', 42 ) \n"
        "emitter:emit( 'newsignal', 13 ) \n"

        // Signal Post
        "emitter:post( 2.0, 'trigger' ) \n"
        "emitter:post( 4.0, 'onint', 24 ) \n"
        );
    // run Lua script
    luaScript->Run( programLua.Get(), result );

    // emit signal from code that also affect binding in script
    emitter->EmitSignal( &Emitter::SignalTrigger );

    // emit a signal created from script
    emitter->EmitSignal( "newsignal", 31 );

    // wait for asynchronous signals
    n_printf( "wating for asynchronous signals ... \n" );
    nTime time = 0.0f;
    while( signalServer->AreSignalsPending() )
    {
        signalServer->Trigger( time );
        time += TIME_INC;
        n_sleep( ACCEL_FACTOR * TIME_INC ); // use magic to accelerate time
    }

    receiverScript->Release();
    receiver->Release();
    emitter->Release();
    luaScript->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
TestScriptingPYTHON()
{
    n_printf( "\n-- Testing scripting signals in Python --\n\n" );

    nKernelServer* ks = nKernelServer::Instance();
    nSignalServer * signalServer = nSignalServer::Instance();

    nScriptServer* pyScript = static_cast<nScriptServer*>( ks->New( "npythonserver", "/sys/servers/pyscript" ) );
    Emitter * emitter = static_cast<Emitter*>( ks->New( "emitter", "/signals/emitter" ) );
    Receiver * receiver = static_cast<Receiver*>( ks->New( "receiver" ) );
    ReceiverScript * receiverScript = static_cast<ReceiverScript*>(
            ks->New( "receiverscript", "/signals/receiverscript" )
        );

    // bind signals with receiver methods
    emitter->BindSignal( Emitter::SignalTrigger, receiver, &Receiver::Trigger, 0 );

    // create Python script
    // the script does binding.
    nString result;
    nString programPython(
        "import pynebula; "
        "emitter = pynebula.lookup( '/signals/emitter' );"
        "receiver = pynebula.lookup( '/signals/receiverscript' );"

        // Signal Registration
        "result = emitter.addsignal( 'v_newsignal_i', 'SFS1' );"

        // Signal Bindind
        "result = emitter.bindsignal( 'trigger', receiver, 'trigger', 0 ); "
        "result = emitter.bindsignal( 'onint', receiver ,'onint', 0 ); "
        "result = emitter.bindsignal( 'newsignal', receiver ,'onint', 0 ); "

        // Signal Emit
        "result = emitter.emit( 'trigger' );"
        "result = emitter.emit( 'trigger' );"
        "result = emitter.emit( 'onint', 42 );"
        "result = emitter.emit( 'newsignal', 13 );"

        // Signal Post
        "emitter.post( 2.0, 'trigger' );"
        "emitter.post( 4.0, 'onint', 24 );"
        );
    // run Python script
    pyScript->Run( programPython.Get(), result );

    // emit signal from code that also affect binding in script
    emitter->EmitSignal( &Emitter::SignalTrigger );

    // emit a signal created from script
    emitter->EmitSignal( "newsignal", 31 );

    // wait for asynchronous signals
    n_printf( "wating for asynchronous signals ... \n" );
    nTime time = 0.0f;
    while( signalServer->AreSignalsPending() )
    {
        signalServer->Trigger( time );
        time += TIME_INC;
        n_sleep( ACCEL_FACTOR * TIME_INC ); // use magic to accelerate time
    }

    receiverScript->Release();
    receiver->Release();
    emitter->Release();
    pyScript->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
TestScriptingCmdsLUA()
{
    n_printf( "\n-- Testing scripting cmds and signals in LUA --\n\n" );

    nKernelServer* ks = nKernelServer::Instance();
    nSignalServer * signalServer = nSignalServer::Instance();

    nScriptServer* luaScript = static_cast<nScriptServer*>( ks->New( "nluaserver", "/sys/servers/luascript" ) );
    Emitter * emitter = static_cast<Emitter*>( ks->New( "emitter", "/signals/emitter" ) );
    ReceiverScript * receiverScript01 = static_cast<ReceiverScript*>(
            ks->New( "receiverscript", "/signals/receiverscript01" )
        );
    ReceiverScript * receiverScript02 = static_cast<ReceiverScript*>(
            ks->New( "receiverscript", "/signals/receiverscript02" )
        );

    // create Lua script
    // the script does binding, emit with name and with actual object, and post.
    nString result;
    nString programLua(
        // get and pin the receiverScript01 object
        "node = lookup( '/signals/receiverscript01' ); pin(node) \n"

        // add ScriptCmds1 and ScriptCmds2 implementation
        "function node:ScriptCmds1() \n"
        "   puts( 'function ScriptCmds1 called\\n' ) \n"
        "end \n"

        "function node:ScriptCmds2(num) \n"
        "   puts( 'function ScriptCmds2 called with '.. num ..'\\n' ) \n"
        "end \n"

        // get and pin the receiverScript02 object
        "node = lookup( '/signals/receiverscript02' ); pin(node) \n"

        // add ScriptCmds2 implementation
        "function node:ScriptCmds2(num) \n"
        "   puts( 'function ScriptCmds2 v2.0 called with '.. num ..'\\n' ) \n"
        "end \n"

        // add scriptcmds to class
        "begincmds( 'receiverscript', 2 ) \n"
        "   addcmd( 'receiverscript', 'v_ScriptCmds1_v' ) \n"
        "   addcmd( 'receiverscript', 'v_ScriptCmds2_i' ) \n"
        "endcmds( 'receiverscript' ) \n"
        );
    // run Lua script
    luaScript->Run( programLua.Get(), result );

    // bind with script cmds of receiverScript01
    // The scripts names from script side are case-sensitive.
    emitter->BindSignal( "trigger", receiverScript01, "ScriptCmds1", 0 );
    emitter->BindSignal( "onint", receiverScript01, "ScriptCmds2", 0 );

    // bind with script cmds of receiverScript02
    // the ScriptCmds1 has implementation for this object but the ScriptCmds2
    // hasn't implementation. Bind signal don't fail because it find
    // ScriptCmds2 in the class definition.
    emitter->BindSignal( "trigger", receiverScript02, "ScriptCmds1", 0 );
    emitter->BindSignal( "onint", receiverScript02, "ScriptCmds2", 0 );

    // Emit Trigger signal only affect receiverScript01 because only it has
    // implementation.
    emitter->EmitSignal( "trigger" );
    // Emit OnInt signal affect the two objects, but with different behaviour
    // because the implementation is different in each.
    emitter->EmitSignal( "onint", 10 );

    receiverScript02->Release();
    receiverScript01->Release();
    emitter->Release();
    luaScript->Release();
}
//------------------------------------------------------------------------------
/**
*/
void
TestScriptingCmdsPYTHON()
{
    n_printf( "\n-- Testing scripting cmds and signals in PYTHON --\n\n" );

    n_printf( " The Cmds from Python script aren't implemented already \n");
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
