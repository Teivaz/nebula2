#ifndef N_SIGNALEMITTER_H
#define N_SIGNALEMITTER_H
//------------------------------------------------------------------------------
/**
    @class nSignalEmitter
    @ingroup NebulaSignals

    @brief This is the main class for the public consumption and how the
    programmer will interact with signals.
    
    It is a mix-in class, and has been added as a parent class to nObject.

    In terms of data, it simply tracks the list of signal bindings that have
    been added to that object (using nSignalBindingSet).

    The main work is in the methods that it provides for managing and
    emitting signals.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/ncmd.h"
#include "util/nkeyarray.h"
#include "signals/nsignal.h"
#include "signals/nsignalregistry.h"

//------------------------------------------------------------------------------
class nObject;
class nCmdProto;
class nSignalBinding;
class nSignalBindingSet;
class nClass;

//------------------------------------------------------------------------------
void n_initcmds_nsignalemitter(nClass * cl);

//------------------------------------------------------------------------------
class nSignalEmitter
{
public:
    /// constructor
    nSignalEmitter();
    /// destructor
    virtual ~nSignalEmitter();

    /// get signal registry data
    nSignalRegistry * GetSignalRegistry() const;
    /// store the emitter's signal list in <tt>signal_list</tt>
    void GetSignals(nHashList *signal_list) const;

    /** @name Binding
        Methods for binding a signal. */
    //@{
    /// bind an already created signal
    bool BindSignal(nFourCC signal4cc, nSignalBinding * binding);
    /// bind signal to a nCmdProto by its pointer
    bool BindSignal(nFourCC signal4cc, nObject * object, nCmdProto * cmdProto, int priority);
    /// bind signal to a nCmdProto by fourcc (able to rebind on invocation)
    bool BindSignal(nFourCC signal4cc, nObject * object, nFourCC cmdFourCC, int priority, bool rebind = false);
    /// bind signal to a nCmdProto by command name (able to rebind on invocation)
    bool BindSignal(nFourCC signal4cc, nObject * object, const char * cmdName, int priority, bool rebind = false);
    /// bind a signal identified by name with a command identified by name
    bool BindSignal(const char * signalName, nObject * object, const char * cmdName, int priority);
    /// Binding for native signal and receiver & member function
#ifdef N_ADV_SIGNALS_ENABLED
    template <class TClass, class TSignalNative>
    bool BindSignal(
        TSignalNative & signal,
        TClass * receiver,
        typename TSignalNative::Traits::template TCmdDispatcher<TClass>::TMemberFunction memf,
        int priority)
    {
        typedef typename TSignalNative::template TBindingNative<TClass>::Type TBinding;
        TBinding * binding = n_new(TBinding)(receiver, memf, priority);
        return this->BindSignal(signal.GetId(), binding);
    }
#endif
    //@}

    /** @name Unbinding
        Methods for unbinding a signal or object. */
    //@{
    /// remove the provided signal binding
    bool UnbindSignal(nFourCC signal4cc, nSignalBinding * binding);
    /// remove all bindings matching the given signal, object, cmdproto
    bool UnbindSignal(nFourCC signal4cc, const nObject * object, const nCmdProto * cmdProto);
    /// remove all bindings matching the given signal, object, command fourcc
    bool UnbindSignal(nFourCC signal4cc, const nObject * object, nFourCC cmdFourCC);
    /// remove all bindings matching the given signal, object, command name
    bool UnbindSignal(nFourCC signal4cc, const nObject * object, const char * cmdName);
    /// remove all bindings matching the given signal, object, command name
    bool UnbindSignal(const char * signalName, const nObject * object, const char * cmdName);
    /// remove all bindings matching the given signal and object provided
    bool UnbindTargetObject(nFourCC signal4cc, const nObject * object);
    /// remove all bindings matching the given signal and object provided
    bool UnbindTargetObject(const char * signalName, const nObject * object);
    /// remove all bindings where the object provided is involved
    bool UnbindTargetObject(const nObject * object);
    /// remove all signal bindings
    void UnbindAllSignals();
    //@}

    /** @name Emission
        Methods for emitting a signal. */
    //@{
    /// emit signal with provided object pointer and va_list
    bool EmitSignal(nSignal * signal, va_list args);
    /// emit signal with provided signal object pointer and var arg list
    bool EmitSignal(nSignal * signal, ...);
    /// emit signal with provided signal fourcc and var arg list
    bool EmitSignal(nFourCC signalId, ...);
    /// emit signal with provided signal name and var arg list
    bool EmitSignal(const char * signalName, ...);
    //@}

    /** @name Posting
        Methods for posting a signal. */
    //@{
    /// post signal for execution at relative time t with provided object pointer and va_list
    bool PostSignal(nTime relT, nSignal * signal, va_list args);
    /// post signal for execution at relative time t with provided signal object pointer and var arg list
    bool PostSignal(nTime relT, nSignal * signal, ...);
    /// post signal for execution at relative time t with provided signal fourcc and var arg list
    bool PostSignal(nTime relT, nFourCC signalId, ...);
    /// post signal for execution at relative time t with provided signal name and var arg list
    bool PostSignal(nTime relT, const char * signalName, ...);
    //@}

protected:
    friend class nSignal;

    nSignalBindingSet * FindSignalBindingSet(nFourCC signal4cc);

    /// return the signal id (fourcc) from the signal name
    nFourCC GetSignalId(const char * signalName) const;

    /// binding set container indexed by signal fourcc
    nKeyArray<nSignalBindingSet *> * bindingSets;
};

//------------------------------------------------------------------------------
inline
nSignalEmitter::nSignalEmitter() :
    bindingSets(0)
{
}

//------------------------------------------------------------------------------
inline
nSignalEmitter::~nSignalEmitter()
{
    if (this->bindingSets)
    {
        this->UnbindAllSignals();
    }
}

//------------------------------------------------------------------------------
/**
    Emit signal with provided object pointer and va_list
*/
inline
bool
nSignalEmitter::EmitSignal(nSignal * signal, va_list args)
{
    n_assert(signal);
    return signal->Dispatch(this, args);
}

//------------------------------------------------------------------------------
/**
    Emit signal with provided signal object pointer and var arg list
*/
inline
bool
nSignalEmitter::EmitSignal(nSignal * signal, ...)
{
    n_assert(signal);
    va_list marker;
    va_start(marker, signal);
    bool ret = this->EmitSignal(signal, marker);
    va_end(marker);
    return ret;
}

//------------------------------------------------------------------------------
/**
    Emit signal with provided signal fourcc and var arg list
*/
inline
bool
nSignalEmitter::EmitSignal(nFourCC signalId, ...)
{
    nSignal * signal = this->GetSignalRegistry()->FindSignalById(signalId);
    if (!signal)
    {
        return false;
    }

    va_list marker;
    va_start(marker, signalId);
    bool ret = this->EmitSignal(signal, marker);
    va_end(marker);

    return ret;
}

//------------------------------------------------------------------------------
/**
    Emit signal with provided signal name and var arg list
*/
inline
bool
nSignalEmitter::EmitSignal(const char * signalName, ...)
{
    n_assert(signalName);
    nSignal * signal = this->GetSignalRegistry()->FindSignalByName(signalName);
    if (!signal)
    {
        return false;
    }

    va_list marker;
    va_start(marker, signalName);
    bool ret = this->EmitSignal(signal, marker);
    va_end(marker);
    return ret;
}

//------------------------------------------------------------------------------
inline
bool
nSignalEmitter::PostSignal(nTime relT, nSignal * signal, ...)
{
    n_assert(signal);

    va_list marker;
    va_start(marker, signal);
    bool ret = this->PostSignal(relT, signal, marker);
    va_end(marker);
    return ret;
}

//------------------------------------------------------------------------------
inline
bool
nSignalEmitter::PostSignal(nTime relT, nFourCC signalId, ...)
{
    nSignal * signal = this->GetSignalRegistry()->FindSignalById(signalId);
    if (!signal)
    {
        return false;
    }

    va_list marker;
    va_start(marker, signalId);
    bool ret = this->PostSignal(relT, signal, marker);
    va_end(marker);
    return ret;
}

//------------------------------------------------------------------------------
inline
bool
nSignalEmitter::PostSignal(nTime relT, const char * signalName, ...)
{
    n_assert(signalName);
    nSignal * signal = this->GetSignalRegistry()->FindSignalByName(signalName);
    if (!signal)
    {
        return false;
    }

    va_list marker;
    va_start(marker, signalName);
    bool ret = this->PostSignal(relT, signal, marker);
    va_end(marker);
    return ret;
}

//------------------------------------------------------------------------------
inline
nSignalBindingSet *
nSignalEmitter::FindSignalBindingSet(nFourCC signal4cc)
{
    if (!this->bindingSets)
    {
        // the bindingSet need a growable nKeyArray to bind signals from scripting
        this->bindingSets = n_new(nKeyArray<nSignalBindingSet *>)(this->GetSignalRegistry()->GetNumSignals(),5);
        n_assert(this->bindingSets);
    }

    nSignalBindingSet * bs = NULL;

    if (this->bindingSets->Find(signal4cc, bs))
    {
        return bs;
    }

    return 0;
}

//------------------------------------------------------------------------------
inline
nFourCC
nSignalEmitter::GetSignalId(const char * signalName) const
{
    const nSignal * signal = this->GetSignalRegistry()->FindSignalByName(signalName);

    if (signal)
    {
        return signal->GetId();
    }

    return 0;
}

//------------------------------------------------------------------------------

#endif
