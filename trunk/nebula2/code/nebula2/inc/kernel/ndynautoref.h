#ifndef N_DYNAUTOREF_H
#define N_DYNAUTOREF_H
//------------------------------------------------------------------------------
/**
    @class nDynAutoRef
    @ingroup NebulaSmartPointers

    nAutoRef with dynamically allocated name string.
*/
#include "kernel/nautoref.h"

//------------------------------------------------------------------------------
template<class TYPE> class nDynAutoRef : public nAutoRef<TYPE> {
public:
    /// default constructor
    nDynAutoRef();
    /// constructor with initial target name
    nDynAutoRef(const char* tarName);
    /// destructor
    ~nDynAutoRef();
    /// copy operator
    nDynAutoRef& operator=(const nDynAutoRef& rhs);

    /// set path name
    void set(const char* name);
    /// path name assignemnt operator
    void operator=(const char* name);
    /// object pointer assignment operator (leaves path name invalid)
    void operator=(TYPE* obj);
};

//------------------------------------------------------------------------------
/**
    Set path name to referenced object. The referenced object must not exist
    at the time set() is called. The path will be resolved when the nDynAutoRef
    is first accessed (with get() or the -> operator).
*/
template<class TYPE>
inline
void
nDynAutoRef<TYPE>::set(const char* name)
{
    this->invalidate();
    if (this->targetName)
    {
        n_free((void*)this->targetName);
    }
    if (name)
    {
        this->targetName = n_strdup(name);
    }
    else
    {
        this->targetName = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nDynAutoRef<TYPE>::nDynAutoRef()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nDynAutoRef<TYPE>::nDynAutoRef(const char* tarName)
{
    this->set(tarName);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nDynAutoRef<TYPE>::~nDynAutoRef()
{
    if (this->targetName)
    {
        n_free((void*)this->targetName);
        this->targetName = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Copy operator.
*/
template<class TYPE>
inline
nDynAutoRef<TYPE>&
nDynAutoRef<TYPE>::operator=(const nDynAutoRef<TYPE>& rhs)
{
    this->set(rhs.getname());
    return (*this);
}

//------------------------------------------------------------------------------
/**
    Path name assignment operator (same functionality as set).
*/
template<class TYPE>
inline
void
nDynAutoRef<TYPE>::operator=(const char* name)
{
    this->set(name);
}

//------------------------------------------------------------------------------
/**
    Sometimes its useful to use a nDynAutoRef object just like a nRef
    (assign an object pointer but no name). This assignment operator does
    just that. Note that there is no valid path name after this method
    returns.
*/
template<class TYPE>
inline
void
nDynAutoRef<TYPE>::operator=(TYPE* obj)
{
    this->invalidate();
    nRef<TYPE>::set(obj);
}

//-------------------------------------------------------------------
#endif
