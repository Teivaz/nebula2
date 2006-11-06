#ifndef N_ARRAY2_H
#define N_ARRAY2_H
//------------------------------------------------------------------------------
/**
    @class nArray2
    @ingroup NebulaDataTypes
    @brief Fixed size two-dimensional array.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
template<class TYPE>
class nArray2
{
public:
    /// default constructor
    nArray2();
    /// constructor
    nArray2(uint width, uint height);
    /// destructor
    ~nArray2();
    /// assignment operator
    nArray2<TYPE>& operator=(const nArray2<TYPE>& rhs);
    /// reset array's size
    void SetSize(uint width, uint height);
    /// get number of columns
    uint GetWidth() const;
    /// get number of rows
    uint GetHeight() const;
    /// item at index (x, y).
    TYPE& At(uint x, uint y) const;
    /// set value at (x, y)
    void Set(uint x, uint y, const TYPE& elm);
    /// is index <i, j> inside bounds?
    bool ValidIndex(uint x, uint y) const;
    /// clear with given value
    void Clear(const TYPE& elm);

private:
    /// allocate empty array
    void Alloc(uint w, uint h);
    /// copy content
    void Copy(const nArray2<TYPE>& src);
    /// delete content
    void Delete();

    uint width;
    uint height;
    TYPE* elements;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nArray2<TYPE>::nArray2() :
    width(0),
    height(0),
    elements(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray2<TYPE>::Alloc(uint w, uint h)
{
    n_assert(0 == this->elements);
    n_assert((w > 0) && (h > 0));
    this->width = w;
    this->height = h;
    this->elements = n_new_array(TYPE, w * h);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nArray2<TYPE>::nArray2(uint w, uint h) :
    elements(0)
{
    this->Alloc(w, h);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray2<TYPE>::Copy(const nArray2<TYPE>& src)
{
    this->Alloc(src.width, src.height);
    uint num = this->width * this->height;
    uint i;
    for (i = 0; i < num; i++)
    {
        this->elements[i] = src.elements[i];
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray2<TYPE>::Delete()
{
    this->width = 0;
    this->height = 0;
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
nArray2<TYPE>::~nArray2()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nArray2<TYPE>&
nArray2<TYPE>::operator=(const nArray2<TYPE>& rhs)
{
    this->Delete();
    this->Copy(rhs);
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray2<TYPE>::SetSize(uint w, uint h)
{
    this->Delete();
    if ((w > 0) && (h > 0))
    {
        this->Alloc(w, h);
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
uint
nArray2<TYPE>::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
uint
nArray2<TYPE>::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool
nArray2<TYPE>::ValidIndex(uint x, uint y) const
{
    return ((x < this->width) && (y < this->height));
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE&
nArray2<TYPE>::At(uint x, uint y) const
{
    n_assert(this->ValidIndex(x, y));
    n_assert(this->elements);
    return this->elements[y * this->width + x];
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray2<TYPE>::Set(uint x, uint y, const TYPE& elm)
{
    n_assert(this->ValidIndex(x, y));
    n_assert(this->elements);
    this->elements[y * this->width + x] = elm;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray2<TYPE>::Clear(const TYPE& elm)
{
    uint y;
    for (y = 0; y < this->height; y++)
    {
        uint x;
        for (x = 0; x < this->width; x++)
        {
            this->Set(x, y, elm);
        }
    }
}

//------------------------------------------------------------------------------
#endif
