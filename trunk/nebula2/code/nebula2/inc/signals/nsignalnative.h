#ifndef N_SIGNALNATIVE_H
#define N_SIGNALNATIVE_H
//------------------------------------------------------------------------------
/**
    @class nSignalNative
    @ingroup NebulaSignals

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/ncmdprototraits.h"
#include "signals/nsignal.h"
#include "signals/nsignalbindingnative.h"
#include "signals/nsignalbindingset.h"

#include "loki/static_check.h"

//------------------------------------------------------------------------------
#define NSIGNAL_TYPE(CLASS,SIGNALNAME)      CLASS::TSignal ## SIGNALNAME
#define NSIGNAL_OBJECT(CLASS,SIGNALNAME)    CLASS::Signal ## SIGNALNAME

#define NSIGNAL_DECLARE(SIGNAL4CC, RETTYPE, NAME, NUMIN, INARGS, NUMOUT, OUTARGS) \
    typedef nSignalNative<                                  \
        RETTYPE,                                            \
        TYPELIST_ ## NUMIN ## INARGS,                       \
        TYPELIST_ ## NUMOUT ## OUTARGS,                     \
        SIGNAL4CC                                           \
    > TSignal ## NAME;                                      \
    static TSignal ## NAME Signal ## NAME;

#define NSIGNAL_DEFINE(CLASS,NAME)                          \
    NSIGNAL_TYPE(CLASS,NAME) NSIGNAL_OBJECT(CLASS,NAME)(#NAME);

#define NSIGNAL_DEFINE_NAME(CLASS,NAME,SIGNALNAME)          \
    NSIGNAL_TYPE(CLASS,NAME) NSIGNAL_OBJECT(CLASS,NAME)(#SIGNALNAME);

//------------------------------------------------------------------------------
extern char _nSignalPrototypeBuffer[N_MAXPATH];

//------------------------------------------------------------------------------
/**
*/
template <class TR, class TListIn, class TListOut, int signal4cc>
class nSignalNative : public nSignal
{
public:

    // signature information type
    typedef nCmdProtoTraits<TR,TListIn,TListOut> Traits;

    // shortcut for native binding for this signal
    template <class TClass>
    struct TBindingNative
    {
        typedef nSignalBindingNative<TClass, TR, TListIn, TListOut> Type;
    };

    /// constructor
    nSignalNative(const char * signalName);
    /// destructor
    virtual ~nSignalNative();

    /// delete signal in the case it is possible
    virtual void Release();

    /// Emit a signal from scripting
    virtual bool Dispatch(void * obj, nCmd * cmd);
    /// Emit a signal with arguments in va_list type
    virtual bool Dispatch(nSignalEmitter * emitter, va_list args);

    // Following functors are provided for type-safe calls, but they
    // are really forwarding to the Emit function with va_list argument
    bool operator()(nSignalEmitter * emitter);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4, typename Traits::TPA5 p5);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4, typename Traits::TPA5 p5, typename Traits::TPA6 p6);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4, typename Traits::TPA5 p5, typename Traits::TPA6 p6, typename Traits::TPA7 p7);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4, typename Traits::TPA5 p5, typename Traits::TPA6 p6, typename Traits::TPA7 p7, typename Traits::TPA8 p8);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4, typename Traits::TPA5 p5, typename Traits::TPA6 p6, typename Traits::TPA7 p7, typename Traits::TPA8 p8, typename Traits::TPA9 p9);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4, typename Traits::TPA5 p5, typename Traits::TPA6 p6, typename Traits::TPA7 p7, typename Traits::TPA8 p8, typename Traits::TPA9 p9, typename Traits::TPA10 p10);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4, typename Traits::TPA5 p5, typename Traits::TPA6 p6, typename Traits::TPA7 p7, typename Traits::TPA8 p8, typename Traits::TPA9 p9, typename Traits::TPA10 p10, typename Traits::TPA11 p11);
    bool operator()(nSignalEmitter * emitter, typename Traits::TPA1 p1, typename Traits::TPA2 p2, typename Traits::TPA3 p3, typename Traits::TPA4 p4, typename Traits::TPA5 p5, typename Traits::TPA6 p6, typename Traits::TPA7 p7, typename Traits::TPA8 p8, typename Traits::TPA9 p9, typename Traits::TPA10 p10, typename Traits::TPA11 p11, typename Traits::TPA12 p12);

};
//------------------------------------------------------------------------------
/**
*/
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
nSignalNative<TR,TListIn,TListOut,signal4cc>::nSignalNative(const char * signalName)
    : nSignal(
        signalName,
        Traits::CalcPrototype(_nSignalPrototypeBuffer, signalName),
        Traits::CalcFourCC(signalName, signal4cc)
    )
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
nSignalNative<TR,TListIn,TListOut,signal4cc>::~nSignalNative()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    These type of signals (nSignalN) cannot be created. They are meant to be
    used in C++ code, and they are statically created and allocated. So
    nothing must be done to destroy them.
*/
template <class TR, class TListIn, class TListOut, int signal4cc>
void
nSignalNative<TR,TListIn,TListOut,signal4cc>::Release()
{
    /// empty
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::Dispatch(void * obj, nCmd * cmd)
{
    // get binding list from the emitter for this signal
    nSignalEmitter * emitter = reinterpret_cast<nObject *> (obj);
    nSignalBindingSet * bs = this->GetBindingSet(emitter, this->GetId());

    // if binding set exists then call every binding in the list
    if (bs)
    {
        nSignalBinding * binding = (bs->GetHead());
        while (binding)
        {
            if (binding->IsValid())
            {
                binding->Invoke(cmd);
                binding = binding->GetSucc();
            }
            else
            {
                nSignalBinding * tmp = binding->GetSucc();
                n_delete(binding);
                binding = tmp;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::Dispatch(nSignalEmitter * emitter, va_list args)
{
    // get binding list from the emitter for this signal
    nSignalBindingSet * bs = this->GetBindingSet(emitter, this->GetId());

    // if binding set exists then call every binding in the list
    if (bs)
    {
        nSignalBinding * binding = (bs->GetHead());
        while (binding)
        {
            if (binding->IsValid())
            {
                binding->Invoke(args);
                binding = binding->GetSucc();
            }
            else
            {
                nSignalBinding * tmp = binding->GetSucc();
                n_delete(binding);
            }
        }
    }

    return true;
}
//------------------------------------------------------------------------------
/*
    Defines all the operator () member functions for type-safe invocation
*/
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter)
{
    STATIC_CHECK(0 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1
)
{
    STATIC_CHECK(1 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2
)
{
    STATIC_CHECK(2 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3
)
{
    STATIC_CHECK(3 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4
)
{
    STATIC_CHECK(4 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4,
    typename Traits::TPA5 arg5
)
{
    STATIC_CHECK(5 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4, arg5);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4,
    typename Traits::TPA5 arg5,
    typename Traits::TPA6 arg6
)
{
    STATIC_CHECK(6 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4, arg5, arg6);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4,
    typename Traits::TPA5 arg5,
    typename Traits::TPA6 arg6,
    typename Traits::TPA7 arg7
)
{
    STATIC_CHECK(7 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4,
    typename Traits::TPA5 arg5,
    typename Traits::TPA6 arg6,
    typename Traits::TPA7 arg7,
    typename Traits::TPA8 arg8
)
{
    STATIC_CHECK(8 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4,
    typename Traits::TPA5 arg5,
    typename Traits::TPA6 arg6,
    typename Traits::TPA7 arg7,
    typename Traits::TPA8 arg8,
    typename Traits::TPA9 arg9
)
{
    STATIC_CHECK(9 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4,
    typename Traits::TPA5 arg5,
    typename Traits::TPA6 arg6,
    typename Traits::TPA7 arg7,
    typename Traits::TPA8 arg8,
    typename Traits::TPA9 arg9,
    typename Traits::TPA10 arg10
)
{
    STATIC_CHECK(10 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4,
    typename Traits::TPA5 arg5,
    typename Traits::TPA6 arg6,
    typename Traits::TPA7 arg7,
    typename Traits::TPA8 arg8,
    typename Traits::TPA9 arg9,
    typename Traits::TPA10 arg10,
    typename Traits::TPA11 arg11
)
{
    STATIC_CHECK(11 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
}

//------------------------------------------------------------------------------
template <class TR, class TListIn, class TListOut, int signal4cc>
inline
bool
nSignalNative<TR,TListIn,TListOut,signal4cc>::operator()(nSignalEmitter * emitter,
    typename Traits::TPA1 arg1,
    typename Traits::TPA2 arg2,
    typename Traits::TPA3 arg3,
    typename Traits::TPA4 arg4,
    typename Traits::TPA5 arg5,
    typename Traits::TPA6 arg6,
    typename Traits::TPA7 arg7,
    typename Traits::TPA8 arg8,
    typename Traits::TPA9 arg9,
    typename Traits::TPA10 arg10,
    typename Traits::TPA11 arg11,
    typename Traits::TPA12 arg12
)
{
    STATIC_CHECK(12 == Traits::NumAllArgs, Signal_Emission_With_Wrong_Number_Of_Arguments);
    return this->Emit(emitter, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
}

//------------------------------------------------------------------------------
#endif // N_SIGNALNATIVE_H
