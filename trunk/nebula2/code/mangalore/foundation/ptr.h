#ifndef FOUNDATION_PTR_H
#define FOUNDATION_PTR_H
//------------------------------------------------------------------------------
/**
    @class Ptr

    Implements a smart pointer for RefCounted objects. Can be used like
    a normal C++ pointer in most cases.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
#include "kernel/ntypes.h"

template<class TYPE>
class Ptr
{
public:
    /// constructor
    Ptr();
    /// construct from C++ pointer
    Ptr(TYPE* p);
    /// construct from smart pointer
    Ptr(const Ptr<TYPE>& p);
    /// destructor
    ~Ptr();
    /// assignment operator
    void operator=(const Ptr<TYPE>& rhs);
    /// assignment operator
    void operator=(TYPE* rhs);
    /// equality operator
    bool operator==(const Ptr<TYPE>& rhs) const;
    /// inequality operator
    bool operator!=(const Ptr<TYPE>& rhs) const;
    /// shortcut equality operator
    bool operator==(const TYPE* rhs) const;
    /// shortcut inequality operator
    bool operator!=(const TYPE* rhs) const;
    /// safe -> operator
    TYPE* operator->() const;
    /// safe dereference operator
    TYPE& operator*() const;
    /// safe pointer cast operator
    operator TYPE*() const;
    /// create instance
    void create();
    /// check if pointer is valid
    bool isvalid() const;
    /// return direct pointer (asserts if null pointer)
    TYPE* get() const;
    /// return direct pointer (returns null pointer)
    TYPE* get_unsafe() const;

private:
    TYPE* ptr;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
Ptr<TYPE>::Ptr() :
    ptr(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
Ptr<TYPE>::Ptr(TYPE* p) :
    ptr(p)
{
    if (0 != this->ptr)
    {
        this->ptr->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
Ptr<TYPE>::Ptr(const Ptr<TYPE>& p) :
    ptr(p.ptr)
{
    if (0 != this->ptr)
    {
        this->ptr->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
Ptr<TYPE>::~Ptr()
{
    if (0 != this->ptr)
    {
        this->ptr->Release();
        this->ptr = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
Ptr<TYPE>::operator=(const Ptr<TYPE>& rhs)
{
    if (this->ptr)
    {
        this->ptr->Release();
    }
    this->ptr = rhs.ptr;
    if (this->ptr)
    {
        this->ptr->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
Ptr<TYPE>::operator=(TYPE* rhs)
{
    if (this->ptr)
    {
        this->ptr->Release();
    }
    this->ptr = rhs;
    if (this->ptr)
    {
        this->ptr->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool
Ptr<TYPE>::operator==(const Ptr<TYPE>& rhs) const
{
    return (this->ptr == rhs.ptr);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool
Ptr<TYPE>::operator!=(const Ptr<TYPE>& rhs) const
{
    return (this->ptr != rhs.ptr);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool
Ptr<TYPE>::operator==(const TYPE* rhs) const
{
    return (this->ptr == rhs);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool
Ptr<TYPE>::operator!=(const TYPE* rhs) const
{
    return (this->ptr != rhs);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE*
Ptr<TYPE>::operator->() const
{
    n_assert2(this->ptr, "NULL pointer access in Ptr::operator->()!");
    return this->ptr;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE&
Ptr<TYPE>::operator*() const
{
    n_assert2(this->ptr, "NULL pointer access in Ptr::operator*()!");
    return *this->ptr;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
Ptr<TYPE>::operator TYPE*() const
{
    n_assert2(this->ptr, "NULL pointer access in Ptr::operator TYPE*()!");
    return this->ptr;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
Ptr<TYPE>::create()
{
    n_assert(0 == this->ptr);
    this->ptr = n_new(TYPE);
    this->ptr->AddRef();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool
Ptr<TYPE>::isvalid() const
{
    return (0 != this->ptr);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE*
Ptr<TYPE>::get() const
{
    n_assert2(this->ptr, "NULL pointer access in Ptr::get()!");
    return this->ptr;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE*
Ptr<TYPE>::get_unsafe() const
{
    return this->ptr;
}

//------------------------------------------------------------------------------
#endif





