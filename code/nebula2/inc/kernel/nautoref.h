#ifndef N_AUTOREF_H
#define N_AUTOREF_H
//------------------------------------------------------------------------------
/**
    @class nAutoRef
    @ingroup NebulaSmartPointers

    An nRef with the ability to resolve itself from a static object name.

     - 31-Mar-03   floh    now uses static nKernelServer pointer

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nref.h"
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
template<class TYPE> class nAutoRef : public nRef<TYPE> 
{
public:
    /// constructor
    nAutoRef();
    /// constructor with target object name
    nAutoRef(const char* tarName);

    /// sets the object name
    void set(const char* name);
    /// get object pointer
    TYPE* get();
    /// returns the object name
    const char *getname() const;
    /// returns true if the object is valid
    bool isvalid();
    /// override -> operator
    TYPE* operator->();
    /// dereference operator
    TYPE& operator*();
    /// cast operator
    operator TYPE*();
    /// assign operator
    void operator=(const char *name);

protected:
    /// resolve target pointer from name if necessary
    TYPE* check();

    const char* targetName;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nAutoRef<TYPE>::nAutoRef() :
    targetName(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nAutoRef<TYPE>::nAutoRef(const char* tarName) :
    targetName(tarName)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
void
nAutoRef<TYPE>::set(const char* name)
{
    this->invalidate();
    this->targetName = name;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
TYPE*
nAutoRef<TYPE>::check()
{
    if (!this->targetObject) 
    {
        if (!this->targetName) 
        {
            return 0;
        }
        this->targetObject = (TYPE*) nKernelServer::ks->Lookup(this->targetName);
        if (this->targetObject) 
        {
            ((nReferenced*)this->targetObject)->AddObjectRef((nRef<nReferenced> *)this);
        }
    }
    return (TYPE *) this->targetObject;
}    

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
TYPE*
nAutoRef<TYPE>::get()
{
    if (!this->check()) 
    {
        n_error("nAutoRef: no target object '%s'!\n",  this->targetName ? this->targetName : "NOT INITIALIZED");
    }
    return this->targetObject;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
const char*
nAutoRef<TYPE>::getname() const
{
    return this->targetName;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
bool
nAutoRef<TYPE>::isvalid()
{
    return this->check() ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
TYPE*
nAutoRef<TYPE>::operator->()
{
    return this->get();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
TYPE&
nAutoRef<TYPE>::operator*()
{
    return *this->get();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nAutoRef<TYPE>::operator TYPE*()
{
    return this->get();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
void
nAutoRef<TYPE>::operator=(const char *name)
{
    this->set(name);
}

//-------------------------------------------------------------------
#endif
