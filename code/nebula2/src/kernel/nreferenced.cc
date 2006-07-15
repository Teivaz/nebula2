//------------------------------------------------------------------------------
//  (c) 2004 Vadim Macagon
//  Refactored out of nRoot.
//------------------------------------------------------------------------------
#include "kernel/nreferenced.h"
#include "kernel/nref.h"

//------------------------------------------------------------------------------
/**
*/
nReferenced::nReferenced() :
    refCount(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nReferenced::~nReferenced()
{
    n_assert(this->refCount == 0);
    
    // invalidate all refs to me
    this->InvalidateAllRefs();
}

//------------------------------------------------------------------------------
/**
*/
bool 
nReferenced::Release()
{
    n_assert(this->refCount > 0);
    bool retval = false;
    this->refCount--;
    if (this->refCount == 0) 
    {
        n_delete(this);
        retval = true;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
void 
nReferenced::InvalidateAllRefs()
{
    nRef<nReferenced> *r;
    while ((r = (nRef<nReferenced> *) this->refList.GetHead())) 
    {
        r->invalidate();
    }
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
