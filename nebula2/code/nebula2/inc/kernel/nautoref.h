#ifndef N_AUTOREF_H
#define N_AUTOREF_H
//------------------------------------------------------------------------------
/**
    @class nAutoRef
    @ingroup NebulaSmartPointers

    An nRef with the ability to resolve itself from a static object name.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_REF_H
#include "kernel/nref.h"
#endif

//------------------------------------------------------------------------------
template<class TYPE> class nAutoRef : public nRef<TYPE> 
{
public:
    /// constructor
    nAutoRef();
    /// constructor with nKernelServer pointer
    nAutoRef(nKernelServer *ks);
    /// constructor with nKernelServer pointer and target object name
    nAutoRef(nKernelServer* ks, const char* tarName);

    /// initialize if default constructor is used
    void initialize(nKernelServer *ks);
    /// sets the object name
    void set(const char* name);
    /// get object pointer
    TYPE* get();
    /// returns the object name
    const char *getname() const;
    /// returns true if the object is valid
    bool isvalid();
    /// reference operator
    TYPE* operator->();
    /// assign operator
    void operator=(const char *name);

protected:
    TYPE* check();

    const char* targetName;
    nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nAutoRef<TYPE>::nAutoRef() :
    kernelServer(0),
    targetName(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nAutoRef<TYPE>::nAutoRef(nKernelServer* ks) :
    kernelServer(ks),
    targetName(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nAutoRef<TYPE>::nAutoRef(nKernelServer* ks, const char* tarName) :
    kernelServer(ks),
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
nAutoRef<TYPE>::initialize(nKernelServer *ks)
{
    n_assert(ks);
    this->kernelServer = ks;
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
        n_assert(this->kernelServer);
        if (!this->targetName) 
        {
            return 0;
        }
        this->targetObject = this->kernelServer->Lookup(this->targetName);
        if (this->targetObject) 
        {
            this->targetObject->AddObjectRef((nRef<nRoot> *)this);
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
        n_error("nAutoRef: no target object '%s'!\n",  this->targetName ? this->targetName : "NOT INTIIALIZED");
    }
    return (TYPE*) this->targetObject;
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
void
nAutoRef<TYPE>::operator=(const char *name)
{
    this->set(name);
}

//-------------------------------------------------------------------
#endif
