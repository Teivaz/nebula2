#ifndef N_HARDREF_H
#define N_HARDREF_H
//------------------------------------------------------------------------------
/**
    @class nHardRef
    @ingroup NebulaSmartPointers

    @brief Implements reference to moveable target object.

    Use the nHardRef object to reference other objects if you
    need deferred pointer resolve, but require the reference
    to keep uptodate if the target objects path in the
    hierarchy changes. The nHardRef objects can either
    be initialized by name (requires a manual call to
    nHardRef::resolve() when it's guaranteed that the
    target object exists), or by pointer (which implies that
    there's a valid target object).

    IMPLEMENTATION NOTES
    nHardRef has 3 states:
        1) name valid, pointer invalid
        2) pointer valid, name invalid
        3) name invalid, pointer invalid

    State 3 is only entered right after creation, or if the
    target object is killed.
*/
#include "kernel/nref.h"
#include "util/nstrnode.h"
#include "kernel/nhardrefserver.h"

//------------------------------------------------------------------------------
class nKernelServer;

template<class TYPE>
class nHardRef : public nRef<TYPE>
{
    friend class nHardRefServer;

public:
    /// default constructor if used in arrays or containers
    nHardRef();
    /// preferred constructor
    nHardRef(nKernelServer* kServer);
    /// the destructor
    ~nHardRef();

    /// deferred initialization, call if default constructor used
    void initialize(nKernelServer* kServer);
    /// validate name, invalidate object pointer
    void set(const char *objName);
    /// validate object pointer, invalidate name
    void set(TYPE *objPointer);
    /// get (current) target name
    nString getname() const;
    /// assign object pointer, invalidate name
    void operator=(TYPE* obj);
    /// assign name, invalidate object pointer
    void operator=(const char *objName);

private:
    /// if name valid, invalidate it
    void invalidate_name();
    /// manually validate object pointer from name
    void resolve();

    nKernelServer *ks;
    nStrNode strNode;
};

//------------------------------------------------------------------------------
/**
    The default constructor, use only if necessary, before the
    object can be used, nHardRef::initialize() must be called!
*/
template<class TYPE>
nHardRef<TYPE>::nHardRef() :
    ks(0),
    strNode((void*) this)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Prefer this constructor over the default constructor.
    nHardRef::initialize() shouldn't be called.

    @param kernelServer      pointer to the Nebula kernel server

    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
nHardRef<TYPE>::nHardRef(nKernelServer* kernelServer):
    ks(kernelServer),
    strNode((void*) this)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Unlinks the embedded strNode, if it is currently linked into
    a list (which would be the nHardRefServer's list of unresolved
    nHardRefs).
*/
template<class TYPE>
nHardRef<TYPE>::~nHardRef()
{
    if (this->strNode.IsLinked())
    {
        this->strNode.Remove();
    }
}

//------------------------------------------------------------------------------
/**
    When using the default constructor, the object must be
    initialized via this method.

    @param kernelServer      pointer to the Nebula kernel server

    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
void
nHardRef<TYPE>::initialize(nKernelServer* kernelServer)
{
    this->ks = kernelServer;
}

//------------------------------------------------------------------------------
/**
    Invalidate the internal target object name.

    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
void
nHardRef<TYPE>::invalidate_name()
{
    // if currently linked into nHardRefServer, remove string node
    if (this->strNode.IsLinked())
    {
        this->strNode.Remove();
    }

    // clear the string node's name
    this->strNode.SetName(0);
}

//------------------------------------------------------------------------------
/**
    Set name of target object, this will invalidate the current
    internal object pointer. The object pointer will NOT be
    resolved, because the target object may not even exist yet.
    Before any object functionality can be used, the reference
    MUST be resolved by calling nHardRef::resolve(), the target
    object must exist when this happens. Any operation that requires
    a valid target pointer will fail until resolve() is called
    (this behavior is different then nAutoRef and nDynAutoRef).

    @param objName      full path name of target object

    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
void
nHardRef<TYPE>::set(const char* objName)
{
    n_assert(this->ks);

    // invalidate object pointer
    nRef<TYPE>::invalidate();

    if (objName)
    {
        // set a new name and register with nHardRefServer
        this->strNode.SetName(objName);
        this->ks->GetHardRefServer()->RegisterHardRef(this->ks, (nHardRef<nRoot> *)this);
    }
    else
    {
        this->invalidate_name();
    }
}

//------------------------------------------------------------------------------
/**
    Set the pointer to target object, the target object must
    exist (naturally), invalidates any target object name inside
    the nHardRef.

    @param objPointer   pointer to a valid nRoot derived object

    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
void
nHardRef<TYPE>::set(TYPE* objPointer)
{
    // invalidate name
    this->invalidate_name();

    // invalidate current object pointer and set new one
    nRef<TYPE>::invalidate();
    nRef<TYPE>::set(objPointer);
}

//------------------------------------------------------------------------------
/**
    If the ref is already resolved, do nothing, otherwise lookup
    target object from name, and invalidate name. Trying to resolve
    uninitialized hardrefs is allowed.

    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
void
nHardRef<TYPE>::resolve()
{
    const char *name = this->strNode.GetName();
    if ((!this->isvalid()) && name)
    {
        TYPE* objPointer = (TYPE *) ks->Lookup(name);
        n_assert(objPointer);

        // invalidate name
        this->invalidate_name();

        // validate object pointer
        nRef<TYPE>::set(objPointer);
    }
}

//------------------------------------------------------------------------------
/**
    Get the name of the target object. Return either embedded
    name (if valid), or if a valid pointer has been resolved,
    dynamically build the name from the target object.

    @return         the name


    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
nString
nHardRef<TYPE>::getname() const
{
    nString str;
    if (this->isvalid())
    {
        str = this->targetObject->GetFullName();
    }
    else
    {
        str = this->strNode.GetName();
    }
    return str;
}

//------------------------------------------------------------------------------
/**
    Overloaded = operator to set target object name.

    @param objName  full path to target object
    @see nHardRef::set(const char *objName)

    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
void
nHardRef<TYPE>::operator=(const char* objName)
{
    this->set(objName);
}

//------------------------------------------------------------------------------
/**
    Overloaded = operator to set target object pointer.

    @param objPointer   pointer to target object
    @see nHardRef::set(TYPE *objPointer)

    history:
    - 23-Jan-2001   floh    created
*/
template<class TYPE>
void
nHardRef<TYPE>::operator=(TYPE* objPointer)
{
    this->set(objPointer);
}

//------------------------------------------------------------------------------
#endif
