#ifndef N_REFERENCED_H
#define N_REFERENCED_H
//------------------------------------------------------------------------------
/**
    @class nReferenced
    @ingroup Kernel

    Provides simple reference counting as well as tracking references to self.
    Never destroy nReferenced objects through delete.
*/

#include "util/nlist.h"

//------------------------------------------------------------------------------
template<class TYPE> class nRef;

class nReferenced
{
public:
    nReferenced();

    /// release object (USE INSTEAD OF DESTRUCTOR!)
    virtual bool Release();

    /// increment ref count of object 
    int AddRef();
    /// get refcount of object
    int GetRefCount() const;

    /// add external object reference
    void AddObjectRef(nRef<nReferenced> *);
    /// remove external object reference
    void RemObjectRef(nRef<nReferenced> *);
    /// get list of refs pointing to me
    nList *GetRefs();

    
protected:
    /// destructor (DONT CALL DIRECTLY, USE Release() INSTEAD)
    virtual ~nReferenced();
    /// invalidate all references
    void InvalidateAllRefs();
    
    nList refList;
    int refCount;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nReferenced::GetRefCount() const
{
    return this->refCount;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nReferenced::AddRef()
{
    return ++this->refCount;
}

//------------------------------------------------------------------------------
/**
    Hang the reference to an object on @c refList.
*/
inline
void 
nReferenced::AddObjectRef(nRef<nReferenced> *r)
{
    this->refList.AddTail((nNode *)r);
}

//------------------------------------------------------------------------------
/**
    Remove the reference to @c r from the @c refList.
*/
inline
void 
nReferenced::RemObjectRef(nRef<nReferenced> *r)
{
    ((nNode*)r)->Remove();
}

//------------------------------------------------------------------------------
/**
*/
inline
nList*
nReferenced::GetRefs()
{
    return &(this->refList);
}

//------------------------------------------------------------------------------
#endif // N_REFERENCED_H
