#ifndef N_REF_H
#define N_REF_H
//------------------------------------------------------------------------------
/**
    @class nRef
    @ingroup NebulaSmartPointers

    nRef and nAutoRef implement safe pointers to objects.
    If one object keeps a pointer to another object,
    and the pointed-to object goes away, the first object is
    left with an invalid object pointer. An nRef creates
    a wire between the 2 objects, if the referenced object goes
    away, the nRef will be invalidated and the object
    holding the reference will be notified.

    Technically, it's a class template implementing a smart pointer.

    (C) 1999 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
template<class TYPE> class nRef : public nNode 
{
public:
    /// default constructor
    nRef();
    /// constructor with target object
    nRef(TYPE* o);
    /// copy constructor
    nRef( const nRef& );
    /// destructor
    ~nRef();
    /// invalidate the ref
    void invalidate();
    /// set target object
    void set(TYPE *obj);
    /// get target object
    TYPE* get() const;
    /// check if target object exists
    bool isvalid() const;
    /// override -> operator
    TYPE* operator->() const;
    /// assign TYPE pointer
    void operator=(TYPE *obj);
    /// assign nRef object
    nRef& operator=(const nRef& rhs);
    /// equality operator
    friend bool operator==(const nRef&a, const nRef& b);
    /// inequality operator
    friend bool operator!=(const nRef&a, const nRef& b);

protected:
    nRoot *targetObject;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> 
inline
nRef<TYPE>::nRef() :
    targetObject(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nRef<TYPE>::nRef(TYPE* o) :
    targetObject(o)
{
    n_assert(o);
    this->targetObject->AddObjectRef((nRef<nRoot> *)this);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nRef<TYPE>::nRef( const nRef<TYPE>& rhs ) :
    targetObject( rhs.get() )
{
    if (targetObject) 
    {
        this->targetObject->AddObjectRef((nRef<nRoot> *)this);
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nRef<TYPE>::~nRef()
{
    if (this->targetObject) 
    {
        this->targetObject->RemObjectRef((nRef<nRoot> *)this);
        this->targetObject = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
void 
nRef<TYPE>::invalidate()
{
    if (this->targetObject) 
    {
        this->targetObject->RemObjectRef((nRef<nRoot> *)this);
    }
    this->targetObject = 0;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
void
nRef<TYPE>::set(TYPE* obj)
{
    this->invalidate();
    this->targetObject = (nRoot *) obj;
    if (obj) 
    {
        this->targetObject->AddObjectRef((nRef<nRoot> *)this);
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
TYPE*
nRef<TYPE>::get() const
{    
    if (!this->targetObject) 
    {
        n_error("nRef: No target object!\n");
    }
    return (TYPE *) this->targetObject;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
bool
nRef<TYPE>::isvalid(void) const
{
    return this->targetObject ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
TYPE*
nRef<TYPE>::operator->() const
{
    return this->get();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
void 
nRef<TYPE>::operator=(TYPE *obj)
{
    this->set(obj);
}

//------------------------------------------------------------------------------
/**
    Copy operator.
*/
template<class TYPE>
inline
nRef<TYPE>&
nRef<TYPE>::operator=(const nRef<TYPE>& rhs)
{
    if (rhs.isvalid())
    {
        this->set(rhs.get());
    }
    else
    {
        this->invalidate();
    }
    return *this;
}

//------------------------------------------------------------------------------
/**
    Equality operator.
*/
template<class TYPE>
inline
bool
operator==(const nRef<TYPE>& a, const nRef<TYPE>& b)
{
    return (a.targetObject == b.targetObject);
}

//------------------------------------------------------------------------------
/**
    Inequality operator.
*/
template<class TYPE>
inline
bool
operator!=(const nRef<TYPE>& a, const nRef<TYPE>& b)
{
    return (a.targetObject != b.targetObject);
}

//------------------------------------------------------------------------------
#endif
