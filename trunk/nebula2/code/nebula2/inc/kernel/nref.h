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

    Operations:

    Assigning ptr to ref:
    ref=ptr OR ref.set(ptr)

    Invalidating:
    ref=0 OR ref.invalidate() OR ref.set(0)

    Checking if pointer is valid (non-null):
    ref.isvalid()



    - 06-May-04     floh    added more operator, so that nRef's can be
                            used more like normal C++ pointers

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
    nRef(const nRef&);
    /// destructor
    ~nRef();
    /// assign TYPE pointer
    nRef& operator=(TYPE *obj);
    /// assign nRef object
    nRef& operator=(const nRef& rhs);
    /// equality operator
    bool operator==( const nRef& rhs);
    /// inequality operator
    bool operator!=(const nRef<TYPE>& rhs);
    /// shortcut equality operator
    bool operator==(TYPE* obj);
    /// shortcut inequality operator
    bool operator!=(TYPE* obj);
    /// override -> operator
    TYPE* operator->() const;
    /// dereference operator
    TYPE& operator*() const;
    /// cast operator
    operator TYPE*() const;
    /// check if target object exists
    bool isvalid() const;
    /// invalidate the ref
    void invalidate();
    /// set target object
    void set(TYPE *obj);
    /// get target object
    TYPE* get() const;

protected:
    TYPE* targetObject;
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
    ((nRoot*)this->targetObject)->AddObjectRef((nRef<nRoot>*)this);
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
        ((nRoot*)this->targetObject)->AddObjectRef((nRef<nRoot> *)this);
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
        ((nRoot*)this->targetObject)->RemObjectRef((nRef<nRoot> *)this);
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
        ((nRoot*)this->targetObject)->RemObjectRef((nRef<nRoot> *)this);
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
    this->targetObject = obj;
    if (obj) 
    {
        ((nRoot*)this->targetObject)->AddObjectRef((nRef<nRoot> *)this);
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
    n_assert2(this->targetObject, "Null pointer access through nRef!");
    return (TYPE*) this->targetObject;
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
    n_assert2(this->targetObject, "Null pointer access through nRef!");
    return this->targetObject;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
TYPE&
nRef<TYPE>::operator*() const
{
    n_assert2(this->targetObject, "Null pointer access through nRef!");
    return *this->targetObject;
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
    this->set(rhs.targetObject);
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nRef<TYPE>&
nRef<TYPE>::operator=(TYPE *obj)
    {
    this->set(obj);
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nRef<TYPE>::operator TYPE*() const
{
    n_assert2(this->targetObject, "Null pointer access through nRef!");
    return this->targetObject;
}

//------------------------------------------------------------------------------
/**
    Equality operator.
*/
template<class TYPE>
inline
bool
nRef<TYPE>::operator==(const nRef<TYPE>& rhs)
{
    return (this->targetObject == rhs.targetObject);
}

//------------------------------------------------------------------------------
/**
    Inequality operator.
*/
template<class TYPE>
inline
bool
nRef<TYPE>::operator!=(const nRef<TYPE>& rhs)
{
    return (this->targetObject != rhs.targetObject);
}

//------------------------------------------------------------------------------
/**
    Equality operator.
*/
template<class TYPE>
inline
bool
nRef<TYPE>::operator==(TYPE* obj)
{
    return (obj == this->targetObject);
}

//------------------------------------------------------------------------------
/**
    Inequality operator.
*/
template<class TYPE>
inline
bool
nRef<TYPE>::operator!=(TYPE* obj)
{
    return (obj != this->targetObject);
}

//------------------------------------------------------------------------------
#endif
