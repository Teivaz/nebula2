#ifndef N_ARRAY_H
#define N_ARRAY_H
//------------------------------------------------------------------------------
/**
    @class nArray
    @ingroup NebulaDataTypes

    @brief A dynamic array template class, similar to the stl vector class.

    Can also be set to a fixed size (SetFixedSize()) if the size of
    the array is known beforehand. This eliminates the memory overhead
    for pre-allocated elements if the array works in dynamic mode. To
    prevent the array from pre-allocating any memory on construction
    call the nArray(0, 0) constructor.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
template<class TYPE> class nArray
{
public:
    typedef TYPE* iterator;

    /// behavior flags
    enum
    {
        DoubleGrowSize = (1<<0),    // when set, grow size doubles each turn
    };

    /// constructor with default parameters
    nArray();
    /// constructor with initial size and grow size
    nArray(int initialSize, int initialGrow);
    /// constructor with initial size, grow size and initial values
    nArray(int initialSize, int initialGrow, const TYPE& initialValue);
    /// copy constructor
    nArray(const nArray<TYPE>& rhs);
    /// destructor
    ~nArray();
    /// assignment operator
    nArray<TYPE>& operator=(const nArray<TYPE>& rhs);
    /// [] operator
    TYPE& operator[](int index) const;
    /// equality operator
    bool operator==(const nArray<TYPE>& rhs) const;
    /// inequality operator
    bool operator!=(const nArray<TYPE>& rhs) const;

    /// set behavior flags
    void SetFlags(int f);
    /// get behavior flags
    int GetFlags() const;
    /// clear contents and set a fixed size
    void SetFixedSize(int size);
    /// push element to back of array
    TYPE& PushBack(const TYPE& elm);
    /// append element to array (synonym for PushBack())
    void Append(const TYPE& elm);
    /// append the contents of an array to this array
    void AppendArray(const nArray<TYPE>& rhs);
    /// reserve 'num' elements at end of array and return pointer to first element
    iterator Reserve(int num);
    /// get number of elements in array
    int Size() const;
    /// get overall allocated size of array in number of elements
    int AllocSize() const;
    /// set element at index, grow array if necessary
    TYPE& Set(int index, const TYPE& elm);
    /// return reference to nth element in array
    TYPE& At(int index);
    /// return reference to first element
    TYPE& Front() const;
    /// return reference to last element
    TYPE& Back() const;
    /// return true if array empty
    bool Empty() const;
    /// erase element at index
    void Erase(int index);
    /// quick erase, does not call operator= or destructor
    void EraseQuick(int index);
    /// erase element pointed to by iterator
    iterator Erase(iterator iter);
    /// quick erase, does not call operator= or destructor
    iterator EraseQuick(iterator iter);
    /// insert element at index
    void Insert(int index, const TYPE& elm);
    /// insert element into sorted array
    void InsertSorted(const TYPE& elm);
    /// clear array (calls destructors)
    void Clear();
    /// reset array (does NOT call destructors)
    void Reset();
    /// return iterator to beginning of array
    iterator Begin() const;
    /// return iterator to end of array
    iterator End() const;
    /// find identical element in array, return iterator
    iterator Find(const TYPE& elm) const;
    /// find identical element in array, return index
    int FindIndex(const TYPE& elm) const;
    /// find array range with element
    void Fill(int first, int num, const TYPE& elm);
    /// clear contents and preallocate with new attributes
    void Reallocate(int initialSize, int grow);
    /// returns new array with elements which are not in rhs (slow!)
    nArray<TYPE> Difference(const nArray<TYPE>& rhs) const;
    /// sort the array
    void Sort();
    /// do a binary search, requires a sorted array
    int BinarySearchIndex(const TYPE& elm) const;

private:
    /// check if index is in valid range, and grow array if necessary
    void CheckIndex(int);
    /// construct an element (call placement new)
    void Construct(TYPE* elm);
    /// construct an element (call placement new)
    void Construct(TYPE* elm, const TYPE& val);
    /// destroy an element (call destructor without freeing memory)
    void Destroy(TYPE* elm);
    /// copy content
    void Copy(const nArray<TYPE>& src);
    /// delete content
    void Delete();
    /// grow array
    void Grow();
    /// grow array to target size
    void GrowTo(int newAllocSize);
    /// move elements, grows array if needed
    void Move(int fromIndex, int toIndex);
    /// unsafe quick move, does not call operator= or destructor
    void MoveQuick(int fromIndex, int toIndex);

    int growSize;           // grow by this number of elements if array exhausted
    int allocSize;          // number of elements allocated
    int numElements;        // number of elements in array
    int flags;
    TYPE* elements;         // pointer to element array
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nArray<TYPE>::nArray() :
    growSize(16),
    allocSize(0),
    numElements(0),
    flags(0)
{
    this->elements = 0;
}

//------------------------------------------------------------------------------
/**
    Note: 'grow' can be zero to create a static preallocated array.
*/
template<class TYPE>
nArray<TYPE>::nArray(int initialSize, int grow) :
    growSize(grow),
    allocSize(initialSize),
    numElements(0),
    flags(0)
{
    n_assert(initialSize >= 0);
    if (initialSize > 0)
    {
        this->elements = (TYPE*)n_malloc(sizeof(TYPE) * this->allocSize);
    }
    else
    {
        this->elements = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Note: 'grow' can be zero to create a static preallocated array.
*/
template<class TYPE>
nArray<TYPE>::nArray(int initialSize, int grow, const TYPE& initialValue) :
    growSize(grow),
    allocSize(initialSize),
    numElements(initialSize),
    flags(0)
{
    n_assert(initialSize >= 0);
    if (initialSize > 0)
    {
        this->elements = (TYPE*)n_malloc(sizeof(TYPE) * this->allocSize);
        int i;
        for (i = 0; i < initialSize; i++)
        {
            this->Construct(this->elements + i, initialValue);
        }
    }
    else
    {
        this->elements = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::Copy(const nArray<TYPE>& src)
{
    n_assert(0 == this->elements);

    this->growSize    = src.growSize;
    this->allocSize   = src.allocSize;
    this->numElements = src.numElements;
    this->flags       = src.flags;
    if (this->allocSize > 0)
    {
        this->elements = (TYPE*)n_malloc(sizeof(TYPE) * this->allocSize);
        int i;
        for (i = 0; i < this->numElements; i++)
        {
            this->Construct(this->elements + i, src.elements[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::Delete()
{
    if (this->elements)
    {
        for (int i = 0; i < this->numElements; i++)
        {
            this->Destroy(this->elements + i);
        }
        n_free(this->elements);
        this->elements = 0;
    }
    this->growSize = 0;
    this->allocSize = 0;
    this->numElements = 0;
    this->flags = 0;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline void
nArray<TYPE>::Destroy(TYPE* elm)
{
    elm->~TYPE();
}

//------------------------------------------------------------------------------
/**
construct an element (call placement new)
*/
template<class TYPE>
inline void
nArray<TYPE>::Construct(TYPE* elm)
{
    n_placement_new(elm, TYPE);
}

//------------------------------------------------------------------------------
/**
    construct an element (call placement new)
*/
template<class TYPE>
inline void
nArray<TYPE>::Construct(TYPE* elm, const TYPE &val)
{
    // FIXME: since the nebula2 code have been assuming nArray to have demand on TYPE::operator =
    // it will be better to keep the assumption to avoid errors
    // copy constructor will better in efficient
    //   n_placement_new(elm, TYPE(val));
    n_placement_new(elm, TYPE);
    *elm = val;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nArray<TYPE>::nArray(const nArray<TYPE>& rhs) :
    growSize(0),
    allocSize(0),
    numElements(0),
    elements(0),
    flags(0)
{
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nArray<TYPE>::~nArray()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::SetFlags(int f)
{
    this->flags = f;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
int
nArray<TYPE>::GetFlags() const
{
    return this->flags;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::Reallocate(int initialSize, int grow)
{
    this->Delete();
    this->growSize    = grow;
    this->allocSize   = initialSize;
    this->numElements = 0;
    if (initialSize > 0)
    {
        this->elements = (TYPE*)n_malloc(sizeof(TYPE) * initialSize);
    }
    else
    {
        this->elements = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Set a new fixed size. This will throw away the current content, and
    create preallocate a new array which cannot grow. All elements in
    the array will be valid.
*/
template<class TYPE>
void
nArray<TYPE>::SetFixedSize(int size)
{
    this->Reallocate(size, 0);
    this->numElements = size;

    // must construct the elements
    for (int i = 0; i < size; i++)
    {
        this->Construct(this->elements + i);
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nArray<TYPE>&
nArray<TYPE>::operator=(const nArray<TYPE>& rhs)
{
    if (this != &rhs)
    {
        this->Delete();
        this->Copy(rhs);
    }
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::GrowTo(int newAllocSize)
{
    TYPE* newArray = (TYPE*)n_malloc(sizeof(TYPE) * newAllocSize);

    if (this->elements)
    {
        // copy over contents
        for (int i = 0; i < this->numElements; i++)
        {
            this->Construct(newArray + i, this->elements[i]);
            this->Destroy(this->elements + i);
        }

        // discard old array and update contents
        n_free(this->elements);
    }
    this->elements  = newArray;
    this->allocSize = newAllocSize;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::Grow()
{
    n_assert(this->growSize > 0);
    int growToSize;
    if ((DoubleGrowSize & this->flags) != 0)
    {
        // double growth behavior
        if (0 == this->allocSize)
        {
            growToSize = growSize;
        }
        else
        {
            growToSize = 2 * this->allocSize;
        }
    }
    else
    {
        // classic linear growth behavior
        growToSize = this->allocSize + this->growSize;
    }
    this->GrowTo(growToSize);
}

//------------------------------------------------------------------------------
/**
     - 30-Jan-03   floh    serious bugfixes!
     - 07-Dec-04   jo      bugfix: neededSize >= this->allocSize becomes
                                   neededSize > allocSize
*/
template<class TYPE>
void
nArray<TYPE>::Move(int fromIndex, int toIndex)
{
    n_assert(this->elements);
    n_assert(fromIndex < this->numElements);

    // nothing to move?
    if (fromIndex == toIndex)
    {
        return;
    }

    // compute number of elements to move
    int num = this->numElements - fromIndex;

    // check if array needs to grow
    int neededSize = toIndex + num;
    while (neededSize > this->allocSize)
    {
        this->Grow();
    }

    if (fromIndex > toIndex)
    {
        // this is a backward move
        // create front elements first
        int createCount = fromIndex - toIndex;
        TYPE* from = this->elements + fromIndex;
        TYPE* to = this->elements + toIndex;
        for (int i = 0; i < createCount; i++)
        {
            this->Construct(to + i, from[i]);
        }

        // copy remaining elements
        int copyCount = num - createCount;
        from = this->elements + fromIndex + createCount;
        to = this->elements + toIndex + createCount;
        for (int i = 0; i < copyCount; i++) 
        {
            to[i] = from[i];
        }

        // destroy remaining elements
        for (int i = toIndex + num; i < this->numElements; i++)
        {
            this->Destroy(this->elements + i);
        }
    }
    else
    {
        // this is a forward move
        // create front elements first
        int createCount = toIndex - fromIndex;
        TYPE* from = this->elements + fromIndex + num - createCount;
        TYPE* to = this->elements + fromIndex + num;
        for (int i = 0; i < createCount; i++)
        {
            this->Construct(to + i, from[i]);
        }

        // copy remaining elements, this time backward copy
        int copyCount = num - createCount;
        from = (TYPE*)this->elements + fromIndex;
        to = (TYPE*)this->elements + toIndex;
        for (int i = copyCount - 1; i >= 0; i--) 
        {
            to[i] = from[i];
        }

        // destroy freed elements
        for (int i = fromIndex; i < toIndex; i++)
        {
            this->Destroy(this->elements + i);
        }

        // be aware of these uninitialized element slots
        // as far as I known, this part is only used in nArray::Insert
        // and it will fill in the blank element slot
    }

    // adjust array size
    this->numElements = toIndex + num;
}

//------------------------------------------------------------------------------
/**
    Very fast move which does not call assignment operators or destructors,
    so you better know what you do!
*/
template<class TYPE>
void
nArray<TYPE>::MoveQuick(int fromIndex, int toIndex)
{
    n_assert(this->elements);
    n_assert(fromIndex < this->numElements);

    // compute number of elements to move
    int num = this->numElements - fromIndex;

    // nothing to move?
    if (fromIndex == toIndex)
    {
        return;
    }

    // do a direct memory move
    memmove(this->elements + toIndex, this->elements + fromIndex, sizeof(TYPE) * num);

    // adjust array size
    this->numElements = toIndex + num;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE&
nArray<TYPE>::PushBack(const TYPE& elm)
{
    // grow allocated space if exhausted
    if (this->numElements == this->allocSize)
    {
        this->Grow();
    }
    n_assert(this->elements);
    this->Construct(this->elements + this->numElements, elm);
    return this->elements[this->numElements++];
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::Append(const TYPE& elm)
{
    // grow allocated space if exhausted
    if (this->numElements == this->allocSize)
    {
        this->Grow();
    }
    n_assert(this->elements);
    this->Construct(this->elements + this->numElements++, elm);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::AppendArray(const nArray<TYPE>& rhs)
{
    int i;
    int num = rhs.Size();
    if (this->numElements + num > this->allocSize)
    {
        this->GrowTo(this->numElements + num);
    }
    for (i = 0; i < num; i++)
    {
        this->Append(rhs[i]);
    }
}

//------------------------------------------------------------------------------
/**
    Make room for N new elements at the end of the array, and return a pointer
    to the start of the reserved area. This can be (carefully!) used as a fast
    shortcut to fill the array directly with data.
*/
template<class TYPE>
typename nArray<TYPE>::iterator
nArray<TYPE>::Reserve(int num)
{
    n_assert(num > 0);
    int maxElement = this->numElements + num;
    if (maxElement > this->allocSize)
    {
        this->GrowTo(maxElement);
    }
    n_assert(this->elements);
    iterator iter = this->elements + this->numElements;
    for (int i = this->numElements; i < maxElement; i++)
    {
        this->Construct(this->elements + i);
    }
    this->numElements = maxElement;
    return iter;
}

//------------------------------------------------------------------------------
/**
    This will check if the provided index is in the valid range. If it is
    not the array will be grown to that index.
*/
template<class TYPE>
void
nArray<TYPE>::CheckIndex(int index)
{
    if (index >= this->numElements)
    {
        // grow array if necessary
        if (index >= this->allocSize)
        {
            n_assert(this->growSize > 0);
            this->GrowTo(index + this->growSize);
        }
        n_assert(index < this->allocSize);
        for (int i = this->numElements; i <= index; i++)
        {
            this->Construct(this->elements + i);
        }
        // update number of contained elements
        this->numElements = index + 1;
    }
}

//------------------------------------------------------------------------------
/**
    - 12-05-2004    brucem  Close bug #203 by removing an assert that
                            prevented this from auto-growing. Assumed
                            intent was for auto-growth given the use
                            of CheckIndex.
*/
template<class TYPE>
TYPE&
nArray<TYPE>::Set(int index, const TYPE& elm)
{
    this->CheckIndex(index);
    this->elements[index] = elm;
    return this->elements[index];
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
int
nArray<TYPE>::Size() const
{
    return this->numElements;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
int
nArray<TYPE>::AllocSize() const
{
    return this->allocSize;
}

//------------------------------------------------------------------------------
/**
    Access an element. This method may grow the array if the index is
    outside the array range.
*/
template<class TYPE>
TYPE&
nArray<TYPE>::At(int index)
{
    this->CheckIndex(index);
    return this->elements[index];
}

//------------------------------------------------------------------------------
/**
    Access an element. This method will NOT grow the array, and instead do
    a range check, which may throw an assertion.
*/
template<class TYPE>
TYPE&
nArray<TYPE>::operator[](int index) const
{
    n_assert(this->elements && index >= 0 && index < this->numElements);
    return this->elements[index];
}

//------------------------------------------------------------------------------
/**
    The equality operator returns true if all elements are identical. The
    TYPE class must support the equality operator.
*/
template<class TYPE>
bool
nArray<TYPE>::operator==(const nArray<TYPE>& rhs) const
{
    if (rhs.Size() == this->Size())
    {
        int i;
        int num = this->Size();
        for (i = 0; i < num; i++)
        {
            if (!(this->elements[i] == rhs.elements[i]))
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    The inequality operator returns true if at least one element in the
    array is different, or the array sizes are different.
*/
template<class TYPE>
bool
nArray<TYPE>::operator!=(const nArray<TYPE>& rhs) const
{
    return !(*this == rhs);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE&
nArray<TYPE>::Front() const
{
    n_assert(this->elements && this->numElements > 0);
    return this->elements[0];
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE&
nArray<TYPE>::Back() const
{
    n_assert(this->elements && this->numElements > 0);
    return this->elements[this->numElements - 1];
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool
nArray<TYPE>::Empty() const
{
    return this->numElements == 0;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::Erase(int index)
{
    n_assert(this->elements && index >= 0 && index < this->numElements);
    if (index == this->numElements - 1)
    {
        // special case: last element
        this->Destroy(this->elements + index);
        this->numElements--;
    }
    else
    {
        this->Destroy(this->elements + index);
        this->Move(index + 1, index);
    }
}

//------------------------------------------------------------------------------
/**
    Quick erase, uses memmove() and does not call assignment operators
    or destructor, so be careful about that!
*/
template<class TYPE>
void
nArray<TYPE>::EraseQuick(int index)
{
    n_assert(this->elements && index >= 0 && index < this->numElements);
    if (index == this->numElements - 1)
    {
        // special case: last element
        this->numElements--;
    }
    else
    {
        this->MoveQuick(index + 1, index);
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
typename nArray<TYPE>::iterator
nArray<TYPE>::Erase(typename nArray<TYPE>::iterator iter)
{
    n_assert(this->elements && iter >= this->elements && iter < this->elements + this->numElements);
    this->Erase(int(iter - this->elements));
    return iter;
}

//------------------------------------------------------------------------------
/**
    Quick erase, uses memmove() and does not call assignment operators
    or destructor, so be careful about that!
*/
template<class TYPE>
typename nArray<TYPE>::iterator
nArray<TYPE>::EraseQuick(typename nArray<TYPE>::iterator iter)
{
    n_assert(this->elements && iter >= this->elements && iter < this->elements + this->numElements);
    this->EraseQuick(int(iter - this->elements));
    return iter;
}


//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nArray<TYPE>::Insert(int index, const TYPE& elm)
{
    n_assert(index >= 0 && index <= this->numElements);
    if (index == this->numElements)
    {
        // special case: append element to back
        this->PushBack(elm);
    }
    else
    {
        this->Move(index, index + 1);
        this->Construct(this->elements + index, elm);
    }
}

//------------------------------------------------------------------------------
/**
    The current implementation of this method does not shrink the
    preallocated space. It simply sets the array size to 0.
*/
template<class TYPE>
void
nArray<TYPE>::Clear()
{
    int i;
    for (i = 0; i < this->numElements; i++)
    {
        this->Destroy(this->elements + i);
    }
    this->numElements = 0;
}

//------------------------------------------------------------------------------
/**
    This is identical with Clear(), but does NOT call destructors (it just
    resets the numElements member. USE WITH CARE!
*/
template<class TYPE>
void
nArray<TYPE>::Reset()
{
    this->numElements = 0;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
typename nArray<TYPE>::iterator
nArray<TYPE>::Begin() const
{
    return this->elements;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
typename nArray<TYPE>::iterator
nArray<TYPE>::End() const
{
    return this->elements + this->numElements;
}

//------------------------------------------------------------------------------
/**
    Find element in array, return iterator, or 0 if element not
    found.

    @param  elm     element to find
    @return         element iterator, or 0 if not found
*/
template<class TYPE>
typename nArray<TYPE>::iterator
nArray<TYPE>::Find(const TYPE& elm) const
{
    int index;
    for (index = 0; index < this->numElements; index++)
    {
        if (this->elements[index] == elm)
        {
            return this->elements + index;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Find element in array, return element index, or -1 if element not
    found.

    @param  elm     element to find
    @return         index to element, or -1 if not found
*/
template<class TYPE>
int
nArray<TYPE>::FindIndex(const TYPE& elm) const
{
    int index;
    for (index = 0; index < this->numElements; index++)
    {
        if (this->elements[index] == elm)
        {
            return index;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Fills an array range with the given element value. Will grow the
    array if necessary

    @param  first   index of first element to start fill
    @param  num     num elements to fill
    @param  elm     fill value
*/
template<class TYPE>
void
nArray<TYPE>::Fill(int first, int num, const TYPE& elm)
{
    n_assert(first >= 0 && first <= this->numElements && num >= 0);
    int end = first + num;
    if (end > this->numElements)
    {
        // only allocate memory when needed
        if (end > this->allocSize)
        {
            this->GrowTo(end);
        }
        // fill the tailing elements
        for (int i = this->numElements; i < end; i++)
        {
            this->Construct(this->elements + i, elm);
        }
        // the rest
        end = this->numElements;
        this->numElements = first + num;
    }
    for (int i = first; i < end; i++)
    {
        this->Destroy(this->elements + i);
        this->Construct(this->elements + i, elm);
    }
}

//------------------------------------------------------------------------------
/**
    Returns a new array with all element which are in rhs, but not in this.
    Carefull, this method may be very slow with large arrays!
*/
template<class TYPE>
nArray<TYPE>
nArray<TYPE>::Difference(const nArray<TYPE>& rhs) const
{
    nArray<TYPE> diff;
    int i;
    int num = rhs.Size();
    for (i = 0; i < num; i++)
    {
        if (0 == this->Find(rhs[i]))
        {
            diff.Append(rhs[i]);
        }
    }
    return diff;
}

//------------------------------------------------------------------------------
/**
    Sorts the array. This just calls the STL sort algorithm.
*/
template<class TYPE>
void
nArray<TYPE>::Sort()
{
    std::sort(this->Begin(), this->End());
}

//------------------------------------------------------------------------------
/**
    Does a binary search on the array, returns the index of the identical
    element, or -1 if not found
*/
template<class TYPE>
int
nArray<TYPE>::BinarySearchIndex(const TYPE& elm) const
{
    int num = this->Size();
    if (num > 0)
    {
        int half;
        int lo = 0;
	    int hi = num - 1;
	    int mid;
        while (lo <= hi)
        {
            if ((half = num/2))
            {
                mid = lo + ((num & 1) ? half : (half - 1));
                if (elm < this->elements[mid])
                {
                    hi = mid - 1;
                    num = num & 1 ? half : half - 1;
                }
                else if (elm > this->elements[mid])
                {
                    lo = mid + 1;
                    num = half;
                }
                else
                {
                    return mid;
                }
            }
            else if (num)
            {
                if (elm != this->elements[lo])
                {
                    return -1;
                }
                else
                {
                    return lo;
                }
            }
            else
            {
                break;
            }
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    This inserts the element into a sorted array. In the current
    implementation this is a slow operation O(n). This should be
    optimized to O(log n).
*/
template<class TYPE>
void
nArray<TYPE>::InsertSorted(const TYPE& elm)
{
    if (0 == this->Size())
    {
        // empty shortcut
        this->Append(elm);
    }
    else if (elm < this->Front())
    {
        // front shortcut
        this->Insert(0, elm);
    }
    else if (elm > this->Back())
    {
        // back shortcut
        this->Append(elm);
    }
    else
    {
        // default case
        int i;
        for (i = 0; i < this->Size(); i++)
        {
            if (elm < (*this)[i])
            {
                this->Insert(i, elm);
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
#endif
