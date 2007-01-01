//------------------------------------------------------------------------------
/**
    @file nsignalemitter.cc

    (c) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/ncmdproto.h"
#include "signals/nsignalemitter.h"
#include "signals/nsignal.h"
#include "signals/nsignalbindingset.h"
#include "signals/nsignalbindingcmdproto.h"
#include "signals/nsignalserver.h"

//------------------------------------------------------------------------------
nSignalRegistry*
nSignalEmitter::GetSignalRegistry() const
{
    /*  Ugly hack, this is more efficient than converting GetSignalRegistry to
        a virtual function. In the case more than one class can be derived from
        nSignalEmitter, or we want to override GetSignalRegistry then convert to
        virtual */
    return ((static_cast<const nObject*>(this))->GetClass());
}

//------------------------------------------------------------------------------
bool
nSignalEmitter::BindSignal(nFourCC signal4cc, nSignalBinding* binding)
{
    // check signal found in the signal class information
    nSignal * signal = this->GetSignalRegistry()->FindSignalById(signal4cc);
    if (0 == signal)
    {
        return false;
    }

    // check compatibility of signal and binding
    if (!signal->CheckCompatibleProto(binding->GetProtoDef()))
    {
        return false;
    }

    // check if signal has already a binding set created
    nSignalBindingSet* bs = this->FindSignalBindingSet(signal4cc);
    if (0 == bs)
    {
        bs = n_new(nSignalBindingSet());
        n_assert(bs);
        this->bindingSets->Add(signal4cc, bs);
    }
    bs->AddBinding(binding);

    return true;
}

//------------------------------------------------------------------------------
/**
   Bind signal to a nCmdProto by its pointer
*/
bool
nSignalEmitter::BindSignal(nFourCC signal4cc, nObject* object, nCmdProto* cmdProto, int priority)
{
    n_assert(object);
    n_assert(cmdProto);

    nSignalBindingCmdProto* binding = n_new(nSignalBindingCmdProto(object, cmdProto, priority));
    bool result = false;
    if (binding)
    {
        result = this->BindSignal(signal4cc, binding);
        if (!result)
        {
            n_delete(binding);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
   Bind signal to a nCmdProto by nFourCC (able to rebind on invocation)
*/
bool
nSignalEmitter::BindSignal(nFourCC signal4cc, nObject* object, nFourCC cmdFourCC, int priority, bool rebind)
{
    n_assert(object);

    nSignalBindingCmdProto* binding = n_new(nSignalBindingCmdProto(object, cmdFourCC, priority, rebind));
    bool result = false;
    if (binding)
    {
        result = this->BindSignal(signal4cc, binding);
        if (!result)
        {
            n_delete(binding);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
   Bind signal to a nCmdProto by command name (able to rebind on invocation)
*/
bool
nSignalEmitter::BindSignal(nFourCC signal4cc, nObject* object, const char* cmdName, int priority, bool rebind)
{
    n_assert(object);
    n_assert(cmdName);

    nSignalBindingCmdProto* binding = n_new(nSignalBindingCmdProto(object, cmdName, priority, rebind));
    bool result = false;
    if (binding)
    {
        result = this->BindSignal(signal4cc, binding);
        if (!result)
        {
            n_delete(binding);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
   Bind signal identified by name to a nCmdProto by command name (No rebinding done).
   Aimed to be used from scripting.
*/
bool
nSignalEmitter::BindSignal(const char* signalName, nObject* object, const char* cmdName, int priority)
{
    n_assert(signalName);
    n_assert(object);
    n_assert(cmdName);

    nSignal * signal = this->GetSignalRegistry()->FindSignalByName(signalName);
    if (0 == signal)
    {
        return false;
    }

    return this->BindSignal(signal->GetId(), object, cmdName, priority, false);
}

//------------------------------------------------------------------------------
/**
    Remove the provided signal binding
*/
bool
nSignalEmitter::UnbindSignal(nFourCC signal4cc, nSignalBinding* binding)
{
    nSignalBindingSet* bs = this->FindSignalBindingSet(signal4cc);
    if (0 == bs)
    {
        return false;
    }

    return bs->RemoveBinding(binding);
}

//------------------------------------------------------------------------------
/**
    Remove all bindings matching the given signal, object, cmdproto
*/
bool
nSignalEmitter::UnbindSignal(nFourCC signal4cc, const nObject * object, const nCmdProto * cmdProto)
{
    nSignalBindingSet* bs = this->FindSignalBindingSet(signal4cc);
    if (0 == bs)
    {
        return false;
    }

    return bs->RemoveBinding(object, cmdProto);
}

//------------------------------------------------------------------------------
/**
    Remove all bindings matching the given signal, object, cmdproto
*/
bool
nSignalEmitter::UnbindSignal(nFourCC signal4cc, const nObject* object, nFourCC cmdFourCC)
{
    nSignalBindingSet* bs = this->FindSignalBindingSet(signal4cc);
    if (0 == bs)
    {
        return false;
    }

    return bs->RemoveBinding(object, cmdFourCC);
}

//------------------------------------------------------------------------------
/**
    Remove all bindings matching the given signal, object, command name
*/
bool
nSignalEmitter::UnbindSignal(nFourCC signal4cc, const nObject* object, const char* cmdName)
{
    nSignalBindingSet* bs = this->FindSignalBindingSet(signal4cc);
    if (0 == bs)
    {
        return false;
    }

    return bs->RemoveBinding(object, cmdName);
}

//------------------------------------------------------------------------------
/**
    Remove all bindings matching the given signal, object, command name
*/
bool
nSignalEmitter::UnbindSignal(const char * signalName, const nObject* object, const char* cmdName)
{
    const nSignal* signal = this->GetSignalRegistry()->FindSignalByName(signalName);
    if (0 == signal)
    {
        return false;
    }

    nSignalBindingSet* bs = this->FindSignalBindingSet(signal->GetId());
    if (0 == bs)
    {
        return false;
    }

    // get cmdProto from the class
    return bs->RemoveBinding(object, cmdName);
}

//------------------------------------------------------------------------------
/**
    Remove all bindings matching the given signal and object provided
*/
bool
nSignalEmitter::UnbindTargetObject(nFourCC signal4cc, const nObject* object)
{
    n_assert(object);

    nSignalBindingSet* bs = this->FindSignalBindingSet(signal4cc);
    if (0 == bs)
    {
        return false;
    }

    return bs->RemoveBinding(object);
}

//------------------------------------------------------------------------------
/**
    Remove all bindings matching the given signal and object provided
*/
bool
nSignalEmitter::UnbindTargetObject(const char* signalName, const nObject* object)
{
    n_assert(signalName);
    n_assert(object);

    nSignal* signal = this->GetSignalRegistry()->FindSignalByName(signalName);
    if (0 == signal)
    {
        return false;
    }

    return this->UnbindTargetObject(signal->GetId(), object);
}

//------------------------------------------------------------------------------
/**
    Remove all binding where the object provided is involved
*/
bool
nSignalEmitter::UnbindTargetObject(const nObject* object)
{
    n_assert(object);
    bool found = false;

    for (int i = 0; i < this->bindingSets->Size(); i++)
    {
        nSignalBindingSet* bs = this->bindingSets->GetElementAt(i);
        n_assert(bs);
        found |= bs->RemoveBinding(object);
    }

    return found;
}

//------------------------------------------------------------------------------
/**
    Remove all signal bindings
*/
void
nSignalEmitter::UnbindAllSignals()
{
    if (this->bindingSets)
    {
        /// remove all binding sets
        while (this->bindingSets->Size() > 0)
        {
            nSignalBindingSet* bs = this->bindingSets->GetElementAt(0);
            n_delete(bs);
            this->bindingSets->RemByIndex(0);
        }
        n_delete(this->bindingSets);
        this->bindingSets = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Post signal for execution at relative time <tt>relT</tt> with the provided
    object pointer and va_list
*/
bool
nSignalEmitter::PostSignal(nTime relT, nSignal* signal, va_list args)
{
    n_assert(signal);

    nCmd* cmd = signal->NewCmd();
    n_assert(cmd);
    cmd->CopyInArgsFrom(args);
    bool ret = nSignalServer::Instance()->PostCmd(relT, static_cast<nObject*>(this), cmd);
    return ret;
}

//------------------------------------------------------------------------------
/**
*/
void
nSignalEmitter::GetSignals(nHashList* signal_list) const
{
    // for each superclass attach it's signal names to the list
    nClass* cl = static_cast<nClass*>(this->GetSignalRegistry());

    // for each superclass...
    do
    {
        nHashList* cl_signals = cl->GetSignalList();
        if (cl_signals)
        {
            nSignal* signal;
            for (signal=(nSignal*)cl_signals->GetHead();
                 signal;
                 signal=(nSignal*)signal->GetSucc())
            {
                nHashNode* node = n_new(nHashNode(signal->GetName()));
                node->SetPtr((void*)signal);
                signal_list->AddTail(node);
            }
        }
    } while (0 != (cl = cl->GetSuperClass()));
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
