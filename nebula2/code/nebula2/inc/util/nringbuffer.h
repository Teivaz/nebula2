#ifndef N_RINGBUFFER_H
#define N_RINGBUFFER_H
//------------------------------------------------------------------------------
/**
    A ring buffer class.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
template<class TYPE> class nRingBuffer 
{
public:
    /// constructor 1
    nRingBuffer(int _num);
    /// default constructor
    nRingBuffer();
    /// destructor
    ~nRingBuffer();
    /// initialize, only use when default constructor has been used
    void Initialize(int _num);
    /// returns true if ringbuffer is valid
    bool IsValid() const;
    /// return true if ringbuffer is empty
    bool IsEmpty() const;
    /// add unitialized element to buffer
    TYPE* Add();
    /// return pointer to head element
    TYPE* GetHead() const;
    /// return pointer to tail element
    TYPE* GetTail() const;
    /// return pointer to next element
    TYPE* GetSucc(TYPE* e) const; 
    /// return pointer to previous element
    TYPE* GetPred(TYPE* e) const;
    /// return pointer to start of ringbuffer array
    TYPE* GetStart() const;
    /// return pointer to end of ringbuffer array
    TYPE *GetEnd() const;

private:
    TYPE *start;                        // start of ring buffer array
    TYPE *end;                          // end+1 of ring buffer array
    TYPE *tail;                         // oldest valid element
    TYPE *head;                         // youngest element+1
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nRingBuffer<TYPE>::nRingBuffer<TYPE>(int _num)
{
    _num++; // there is always 1 empty element in buffer
    start = n_new TYPE[_num];
    end   = start + _num;
    tail  = start;
    head  = start;
}

//------------------------------------------------------------------------------
/**
    NOTE: you must call Initialize() when using the default constructor!
*/
template<class TYPE>
nRingBuffer<TYPE>::nRingBuffer<TYPE>() :
    start(0),
    end(0),
    tail(0),
    head(0)
{
    // empty
}

//---------------------------------------------------------------
/**
*/
template<class TYPE>
nRingBuffer<TYPE>::~nRingBuffer<TYPE>() 
{
    if (start)
    {
        n_delete[] start;
    }
};

//------------------------------------------------------------------------------
/**
    Initialize with n elements, may only be called when
    default constructor has been used.
*/
template<class TYPE>
void
nRingBuffer<TYPE>::Initialize(int _num)
{
    n_assert(!start);
    _num++; // there is always 1 empty element in buffer
    start = n_new TYPE[_num];
    end   = start + _num;
    tail  = start;
    head  = start;

n_printf("nRingBuffer initialized with %d elelements of size %d\n", _num, sizeof(TYPE));
}

//------------------------------------------------------------------------------
/**
    Return true if ring buffer is valid.
*/
template<class TYPE>
bool 
nRingBuffer<TYPE>::IsValid() const
{
    return start ? true : false;
}

//------------------------------------------------------------------------------
/**
    Checks if ring buffer is empty

    Returns true if head and tail are in the same position otherwise
    false.
*/
template<class TYPE>
bool 
nRingBuffer<TYPE>::IsEmpty() const
{
    return (head == tail) ? true : false;
}

//------------------------------------------------------------------------------
/**
    Add new unitialized head element to ringbuffer.
*/
template<class TYPE>
TYPE*
nRingBuffer<TYPE>::Add() 
{
    n_assert(start);
    TYPE *e = head;
    head++;
    if (head == end) 
    {
        head = start;
    }
    if (head == tail) 
    {
        tail++;
        if (tail == end) 
        {
            tail = start;
        }
    }
    return e;
}
  
//------------------------------------------------------------------------------
/**
    Return head element (youngest element).
*/
template<class TYPE>
TYPE*
nRingBuffer<TYPE>::GetHead() const
{
    if (head == tail) 
    {
        // empty ringbuffer
        return 0;    
    }
    TYPE *e = head - 1;
    if (e < start) 
    {
        e = end - 1;
    }
    return e;
}

//------------------------------------------------------------------------------
/**
    Return tail element (oldest element).
*/
template<class TYPE>
TYPE*
nRingBuffer<TYPE>::GetTail() const
{
    if (head == tail) 
    {   
        // empty ringbuffer
        return 0;
    }
    return tail;
};

//------------------------------------------------------------------------------
/**
    Get next element (from head to tail).
*/
template<class TYPE>
TYPE*
nRingBuffer<TYPE>::GetSucc(TYPE* e) const
{
    n_assert(e);
    if (e == tail) 
    {
        return 0;
    }
    e--;
    if (e < start) 
    {
        e = end - 1;
    }
    return e;
};

//------------------------------------------------------------------------------
/**
    Get previous element (from tail to head).
*/
template<class TYPE>
TYPE*
nRingBuffer<TYPE>::GetPred(TYPE* e) const
{
    n_assert(e);
    e++;
    if (e == end)  
    {
        e = start;
    }

    if (e == head) 
    {
        return 0;
    }
    else         
    {
        return e;
    }
};

//------------------------------------------------------------------------------
/**
    Return physical start of ringbuffer array.
    Only useful for accessing the ringbuffer array elements directly.
*/
template<class TYPE>
TYPE*
nRingBuffer<TYPE>::GetStart() const
{
    return this->start;
}

//------------------------------------------------------------------------------
/**
    Return physical end of ringbuffer array.   
    Only useful for accessing the ringbuffer array elements directly.
*/
template<class TYPE>
TYPE*
nRingBuffer<TYPE>::GetEnd() const
{
    return this->end;
}

//------------------------------------------------------------------------------
#endif
