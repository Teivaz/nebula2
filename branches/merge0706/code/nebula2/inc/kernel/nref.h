#ifndef N_REF_H
#define N_REF_H
//------------------------------------------------------------------------------
/**
    @class nRef
    @ingroup NebulaSmartPointers

    nRef implements safe pointers to nReferenced derived objects which will 
    invalidate themselves when the target object goes away. nAutoRef works
    similarly to nRef but is also NOH aware and as such takes pointers to nRoot 
    derived objects. Usage of nRef/nAutoRef helps you avoid dangling pointers and 
    also protects against dereferencing a null pointer.

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

#include "kernel/nreferenced.h"

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
    /// get target object (safe)
    TYPE* get() const;
    /// get target object (unsafe, may return 0)
    TYPE* get_unsafe() const;

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
    ((nReferenced*)this->targetObject)->AddObjectRef((nRef<nReferenced>*)this);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nRef<TYPE>::nRef(const nRef<TYPE>& rhs) :
    targetObject(rhs.get_unsafe())
{
    if (targetObject) 
    {
        ((nReferenced*)this->targetObject)->AddObjectRef((nRef<nReferenced> *)this);
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
        ((nReferenced*)this->targetObject)->RemObjectRef((nRef<nReferenced> *)this);
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
        ((nReferenced*)this->targetObject)->RemObjectRef((nRef<nReferenced> *)this);
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
        ((nReferenced*)this->targetObject)->AddObjectRef((nRef<nReferenced> *)this);
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
TYPE*
nRef<TYPE>::get_unsafe() const
{
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
