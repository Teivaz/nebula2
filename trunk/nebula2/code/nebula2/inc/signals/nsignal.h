#ifndef N_SIGNAL_H
#define N_SIGNAL_H
//------------------------------------------------------------------------------
/**
    @file nsignal.h
    @class nSignal
    @ingroup NebulaSignals

    nSignal, while largely internal, is a key concept involved here in the
    signal system. It is really just a name and a type signature:

   * Name of signal
   * FourCC for signal
   * Type signature of the signal

   nSignal is a per-nClass structure.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "util/nstring.h"
#include "kernel/ntypes.h"
#include "kernel/ncmdproto.h"

//------------------------------------------------------------------------------
class nSignalEmitter;
class nSignalBindingSet;

//------------------------------------------------------------------------------
/**
    nSignal is used to store signal objects. It may seem a bit strange to
    make it a subclass of nCmdProto, but it is quite convenient and it saves
    some coding.  In the same way, nCmd will prove useful in order to
    represent the invocation of signals.

    Signals created from scripting are objects from the class nSignal, while
    signals created from C++ side are from nSignalNative subclasses
    (@see nSignalNative).
*/
class nSignal : public nCmdProto
{
public:
    /// constructor
    nSignal(const char * signalName, const char *proto_def, nFourCC id);
    /// destructor
    virtual ~nSignal();
    /// delete signal in the case it is possible
    virtual void Release(void);

    /// Emit a signal from scripting
    virtual bool Dispatch(void *, nCmd *);
    /// Emit a signal with arguments in va_list type
    virtual bool Dispatch(nSignalEmitter *, va_list args);
    /// Emit a signal with variable argument list
    bool Emit(nSignalEmitter *, ...);

    /// @return true if the prototype and the signal are compatible
    bool CheckCompatibleProto(const char * proto) const;

protected:

    nSignalBindingSet * GetBindingSet(nSignalEmitter * emitter, nFourCC signalId) const;
};

//------------------------------------------------------------------------------
/**
*/
inline
nSignal::nSignal(const char * signalName, const char *proto_def, nFourCC id) :
    nCmdProto(proto_def, id)
{
    // set name of the signal
    this->SetName(signalName);
}

//------------------------------------------------------------------------------
/**
*/
inline
nSignal::~nSignal()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Delete the signal object, in the case of signals defined in scripting code,
    which are allocated dynamically and they are created directly from nSignal
    class.
*/
inline
void
nSignal::Release(void)
{
    n_delete(this);
}

//------------------------------------------------------------------------------
/**
    Emit a signal with variable argument list
    @param emitter  signal emitter
*/
inline
bool
nSignal::Emit(nSignalEmitter * emitter, ...)
{
    va_list marker;
    va_start(marker, emitter);
    bool ret = this->Dispatch(emitter, marker);
    va_end(marker);

    return ret;
}

//------------------------------------------------------------------------------
inline
bool
nSignal::CheckCompatibleProto(const char * proto) const
{
    // get info about signal prototype
    ProtoDefInfo infoSignal(this->GetProtoDef());
    // get info about binding prototype
    ProtoDefInfo infoBinding(proto);

    // check both proto_def are valid
    if (!infoSignal.valid || !infoBinding.valid)
    {
        return false;
    }

    // check number of input arguments
    if (infoSignal.numInArgs != infoBinding.numInArgs)
    {
        return false;
    }

    // check number of output arguments
    if (infoSignal.numOutArgs != infoBinding.numOutArgs)
    {
        return false;
    }

    // check input arguments types are the same
    if (strcmp(infoSignal.inArgs, infoBinding.inArgs))
    {
        return false;
    }

    // check output arguments types are the same
    if (strcmp(infoSignal.outArgs, infoBinding.outArgs))
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
#endif
