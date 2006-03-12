#ifndef N_FIXEDARRAY_H
#define N_FIXEDARRAY_H
//------------------------------------------------------------------------------
/**
    @class nFixedArray
    @ingroup NebulaDataTypes

    @brief A fixed size, bounds checked array.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
template<class TYPE> class nFixedArray
{
public:
    /// default constructor
    nFixedArray();
    /// constructor with given size
    nFixedArray(int s);
    /// copy constructor
    nFixedArray(const nFixedArray<TYPE>& rhs);
    /// destructor
    ~nFixedArray();
    /// set size of array (deletes previous contents!)
    void SetSize(int s);
    /// get array size
    int Size() const;
    /// fill array with a given value
    void Clear(TYPE elm);
    /// assignment operator
    void operator=(const nFixedArray<TYPE>& rhs);
    /// [] operator
    TYPE& operator[](int index) const;
    /// find index (linear search, slow)
    int Find(const TYPE& e) const;

private:
    /// delete content
    void Delete();
    /// allocate array for given size
    void Allocate(int s);
    /// copy content
    void Copy(const nFixedArray<TYPE>& src);

    int size;
    TYPE* elements;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nFixedArray<TYPE>::nFixedArray() :
    size(0),
    elements(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nFixedArray<TYPE>::Delete()
{
    if (this->elements)
    {
        n_delete_array(this->elements);
        this->elements = 0;
    }
    this->size = 0;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nFixedArray<TYPE>::Allocate(int s)
{
    this->Delete();
    if (s > 0)
    {
        this->elements = n_new_array(TYPE,s);
        this->size = s;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nFixedArray<TYPE>::Copy(const nFixedArray<TYPE>& rhs)
{
    if (this != &rhs)
    {
        this->Allocate(rhs.size);
        int i;
        for (i = 0; i < this->size; i++)
        {
            this->elements[i] = rhs.elements[i];
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nFixedArray<TYPE>::nFixedArray(int s) :
    size(0),
    elements(0)
{
    this->Allocate(s);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nFixedArray<TYPE>::nFixedArray(const nFixedArray<TYPE>& rhs) :
    size(0),
    elements(0)
{
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nFixedArray<TYPE>::~nFixedArray()
{
    if (this->elements)
    {
        n_delete_array(this->elements);
        this->elements = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nFixedArray<TYPE>::operator=(const nFixedArray<TYPE>& rhs)
{
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE&
nFixedArray<TYPE>::operator[](int index) const
{
    n_assert(this->elements);
    if ((index < 0) || (index >= this->size))
    {
        n_error("nFixedArray::operator[]: index out of bounds: %d, array size is %d", index, this->size);
    }
    return this->elements[index];
}

//------------------------------------------------------------------------------
/**
    Set the size of the array.
*/
template<class TYPE>
void
nFixedArray<TYPE>::SetSize(int s)
{
    if (this->size != s)
    {
        this->Allocate(s);
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
int
nFixedArray<TYPE>::Size() const
{
    return this->size;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nFixedArray<TYPE>::Clear(TYPE elm)
{
    if (this->elements)
    {
        int i;
        for (i = 0; i < this->size; i++)
        {
            this->elements[i] = elm;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Find index of identical element. Returns -1 if no identical element
    has been found.
*/
template<class TYPE>
int
nFixedArray<TYPE>::Find(const TYPE& e) const
{
    int i;
    for (i = 0; i < this->size; i++)
    {
        if (e == this->elements[i])
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
#endif

