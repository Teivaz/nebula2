#ifndef FOUNDATION_REFCOUNTED_H
#define FOUNDATION_REFCOUNTED_H
//------------------------------------------------------------------------------
/**
    @class Foundation::RefCounted

    Class with simple refcounting mechanism, and security check at
    application shutdown. Subsystems are advised to derive their
    classes from the RefCounted class and enforce a correct
    AddRef()/Release() behavior.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nnode.h"
#include "foundation/factory.h"
#include "foundation/rtti.h"
#include "foundation/server.h"

//------------------------------------------------------------------------------
namespace Foundation
{
class RefCounted : private nNode
{
    DeclareRtti;

public:
    /// constructor
    RefCounted();
    /// increment refcount by one
    void AddRef();
    /// decrement refcount and destroy object if refcount if zero
    void Release();
    /// get the current refcount
    int GetRefCount() const;
    /// return true if this object is instance of given class
    bool IsInstanceOf(const Rtti& other) const;
    /// return true if this object is instance of given class, or a derived class
    bool IsA(const Rtti& other) const;
    /// return true if this object is instance of given class, or a derived class, by string SLOW!
    bool IsInstanceOf(const nString& other) const;
    /// return true if this object is instance of class given by string, SLOW!
    bool IsA(const nString& other) const;
    /// get the class name
    const nString& GetClassName() const;

protected:
    /// destructor (called when refcount reaches zero)
    virtual ~RefCounted() = 0;

private:
    friend class Server;

    int refCount;
};

// Declare factory macro.
#define DeclareFactory(classname) \
public: \
    static classname* Create(); \
    static Foundation::RefCounted* InternalCreate(); \
	static bool RegisterFactoryFunction(); \
private:

// Register factory macro.
#define RegisterFactory(classname) \
static bool factoryRegistered_##classname = classname::RegisterFactoryFunction();

// Implement factory macro.
#define ImplementFactory(classname) \
classname* classname::Create() \
{ \
    return (classname*)classname::InternalCreate(); \
} \
Foundation::RefCounted* classname::InternalCreate() \
{ \
	classname* result = n_new(classname); \
	n_assert(result != 0); \
	return result; \
} \
bool classname::RegisterFactoryFunction() \
{ \
	if (!Foundation::Factory::Instance()->Has(#classname)) \
	{ \
		Foundation::Factory::Instance()->Add(classname::InternalCreate, #classname); \
	} \
	return true; \
}


//------------------------------------------------------------------------------
/**
    Increment the refcount of the object.
*/
inline
void
RefCounted::AddRef()
{
    ++this->refCount;
}

//------------------------------------------------------------------------------
/**
    Decrement the refcount and destroy object if refcount is zero.
*/
inline
void
RefCounted::Release()
{
    n_assert(this->refCount > 0);
    if (--this->refCount == 0)
    {
        n_delete(this);
    }
}

//------------------------------------------------------------------------------
/**
    Return the current refcount of the object.
*/
inline
int
RefCounted::GetRefCount() const
{
    return this->refCount;
}

//------------------------------------------------------------------------------
/**
    Return true if this object is an instance of the given class.
*/
inline
bool
RefCounted::IsInstanceOf(const Rtti& other) const
{
    return this->GetRtti() == &other;
}

//------------------------------------------------------------------------------
/**
    Return true if this object is an instance of the given class.
*/
inline
bool
RefCounted::IsInstanceOf(const nString& other) const
{
    return this->GetRtti()->GetName() == other;
}

//------------------------------------------------------------------------------
/**
    Return true if this object is instance of the given class, or of a derived
    class.
*/
inline
bool
RefCounted::IsA(const Rtti& other) const
{
    const Rtti* i;
    for (i = this->GetRtti(); i != 0; i = i->GetParent())
    {
        if (i == &other)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Return true if this object is instance of the given class, or of a derived
    class.
*/
inline
bool
RefCounted::IsA(const nString& other) const
{
    const Rtti* i;
    for (i = this->GetRtti(); i != 0; i = i->GetParent())
    {
        if (i->GetName() == other)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Get the class name of the object.
*/
inline
const nString&
RefCounted::GetClassName() const
{
    return this->GetRtti()->GetName();
}

} // namespace Foundation
//------------------------------------------------------------------------------
#endif
