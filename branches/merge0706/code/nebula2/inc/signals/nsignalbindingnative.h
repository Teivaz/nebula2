#ifndef N_SIGNALBINDINGNATIVE_H
#define N_SIGNALBINDINGNATIVE_H
//------------------------------------------------------------------------------
/**
    @class nSignalBindingNative
    @ingroup NebulaSignals
    @brief Fast binding in native world.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/nref.h"
#include "kernel/ncmdprototraits.h"
#include "signals/nsignalbinding.h"

template <class TClass, class TR, class TListIn, class TListOut>
class nSignalBindingNative : public nSignalBinding
{
public:

    // Nebula cmd proto traits
    typedef typename nCmdProtoTraits<TR,TListIn,TListOut> Traits;
    typedef typename Traits::TCmdDispatcher<TClass> TCmdDispatcher;
    typedef typename Traits::TValistDispatcher<TClass> TValistDispatcher;
    typedef typename TCmdDispatcher::TMemberFunction TMemberFunction;

    /// constructor
    nSignalBindingNative(TClass * obj, TMemberFunction memfun, int priority);
    /// destructor
    virtual ~nSignalBindingNative();

    /** @name Invocation
        Methods for invoking a signal binding. */
    //@{
    /// Invocation used from scripting side
    virtual bool Invoke(nCmd * cmd);
    /// Invocation used from native side
    virtual bool Invoke(va_list args);
    //@}

    /// Return prototype string
    virtual const char * GetProtoDef() const;

    /// Return true if binding points to the object provided
    virtual bool IsBoundWithObject(const nObject * objectPtr) const;
    /// Return true if the binding is valid
    virtual bool IsValid() const;

protected:

    nRef<TClass> refObject;
    TMemberFunction memf;
    nString prototype;
};

//------------------------------------------------------------------------------
template <class TClass, class TR, class TListIn, class TListOut>
inline
nSignalBindingNative<TClass,TR,TListIn,TListOut>::nSignalBindingNative(TClass * obj, TMemberFunction memfun, int priority) :
    nSignalBinding(priority),
    refObject(obj),
    memf(memfun)
{
    char signature[N_MAXPATH];

    Traits::CalcPrototype(signature, "binding");
    prototype.Set(signature);
}

//------------------------------------------------------------------------------
template <class TClass, class TR, class TListIn, class TListOut>
inline
nSignalBindingNative<TClass,TR,TListIn,TListOut>::~nSignalBindingNative()
{
    /// empty
}

//------------------------------------------------------------------------------
template <class TClass, class TR, class TListIn, class TListOut>
inline
bool
nSignalBindingNative<TClass,TR,TListIn,TListOut>::Invoke(nCmd * cmd)
{
    cmd->Rewind();
    return TCmdDispatcher::Dispatch(this->refObject.get(), this->memf, cmd);
}

//------------------------------------------------------------------------------
template <class TClass, class TR, class TListIn, class TListOut>
inline
bool
nSignalBindingNative<TClass,TR,TListIn,TListOut>::Invoke(va_list args)
{
    /// XXX: how to pass the return value from TValistDispatcher::Dispatch ?
    TValistDispatcher::Dispatch<Traits::ReturnsVoid>(this->refObject.get(), this->memf, args);
    return true;
}

//------------------------------------------------------------------------------
template <class TClass, class TR, class TListIn, class TListOut>
inline
const char *
nSignalBindingNative<TClass,TR,TListIn,TListOut>::GetProtoDef() const
{
    return this->prototype.Get();
}

//------------------------------------------------------------------------------
template <class TClass, class TR, class TListIn, class TListOut>
inline
bool
nSignalBindingNative<TClass,TR,TListIn,TListOut>::IsBoundWithObject(const nObject * objectPtr) const
{
    return (this->refObject.get() == objectPtr);
}

//------------------------------------------------------------------------------
template <class TClass, class TR, class TListIn, class TListOut>
inline
bool
nSignalBindingNative<TClass,TR,TListIn,TListOut>::IsValid() const
{
    return this->refObject.isvalid();
}

//------------------------------------------------------------------------------
#endif // N_SIGNALBINDGFUNCTOR_H
