#ifndef N_CREF_H
#define N_CREF_H
//------------------------------------------------------------------------------
/**
    @class CCRef

    @brief Just a pointer that isn't allowed to be NULL

    If it is NULL, and gets dereferenced, it sends a readable error instead of randomly crashing.
        Inspired by nRef, but this is lighter weight, doesn't require its object to be an nRoot,
        doesn't cast, can be implicitly converted to a normal pointer, and is just all-around 
        better, so there.

    (C) 2004 Rafael Van Daele-Hunt
*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<class TYPE> class CCRef
{
    typedef TYPE* TypePtr;
    TYPE* m_pObj;
public:
    explicit CCRef( TYPE* obj = 0 ) : m_pObj( obj ) {}; // also handles copy construction
    template<typename CONVERTIBLE_TYPE> CCRef( const CCRef<CONVERTIBLE_TYPE>& other ) :	m_pObj( (CONVERTIBLE_TYPE*)other ) {}

    operator TypePtr() const { return m_pObj; }

    TYPE& operator*() const; // a const CCRef<T> is like a T* const: you can change the value pointed at, just not the pointer itself
    TYPE* operator->() const; // to get the effect of const T*, use CCRef<const T>

    void operator=(TYPE* obj) { m_pObj = obj; }  
    template<typename CONVERTIBLE_TYPE> CCRef<TYPE>& operator=(const CCRef<CONVERTIBLE_TYPE>& rhs )
    {
        m_pObj = (CONVERTIBLE_TYPE*)rhs;
        return *this;
    }
};

template<class T> inline T * get_pointer(CCRef<T> const & p)
{
    return (T*)p;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<class TYPE> inline
TYPE& CCRef<TYPE>::operator*() const
{    
    n_assert2( m_pObj, "CCRef: attempt to dereference NULL!" );
    return *m_pObj;
}
//------------------------------------------------------------------------------
/**
*/
template<class TYPE> inline 
TYPE* CCRef<TYPE>::operator->() const
{
    n_assert2( m_pObj, "CCRef: attempt to dereference NULL!" );
    return m_pObj;
}

//------------------------------------------------------------------------------

#endif
