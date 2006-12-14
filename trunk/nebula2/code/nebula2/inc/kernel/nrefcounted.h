#ifndef N_REFCOUNTED_H
#define N_REFCOUNTED_H
//------------------------------------------------------------------------------
/**
    @class nRefCounted
    @ingroup Kernel

    A simplest possible refcounted super class, if you need refcounting but
    don't want the nRoot overhead.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
class nRefCounted
{
public:
    /// constructor
    nRefCounted();
    /// increment refcount
    void AddRef();
    /// release object (call instead of destructor)
    int Release();

protected:
    /// destructor
    virtual ~nRefCounted();

private:
    int refCount;
};

//------------------------------------------------------------------------------
/**
*/
inline
nRefCounted::nRefCounted() :
    refCount(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRefCounted::AddRef()
{
    ++this->refCount;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRefCounted::Release()
{
    n_assert(this->refCount >= 0);
    if (0 == this->refCount)
    {
        n_delete(this);
        return 0;
    }
    return --this->refCount;
}

//------------------------------------------------------------------------------
#endif
