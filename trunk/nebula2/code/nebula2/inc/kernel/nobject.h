#ifndef N_OBJECT_H
#define N_OBJECT_H
//------------------------------------------------------------------------------
/**
    @class nObject
    @ingroup Kernel

    Provides:
    - reference counting and tracking (through nReferenced)
    - Nebula RTTI, a class is identified by a string name

    Rules for subclasses:
    - only the default constructor is allowed
    - never use new/delete (or variants like n_new/n_delete) with nObject
      objects
    - use nKernelServer::New() to create an object and
      the object's Release() method to destroy it

    See also @ref N2ScriptInterface_nobject

    (c) 2004 Vadim Macagon
    Refactored out of nRoot.
*/

#include "kernel/ntypes.h"
#include "kernel/nclass.h"
#include "kernel/ncmd.h"
#include "kernel/nmutex.h"
#include "kernel/nreferenced.h"
#include "signals/nsignalemitter.h"

//------------------------------------------------------------------------------
class nCmd;
class nKernelServer;
class nPersistServer;

//------------------------------------------------------------------------------
class nObject : public nReferenced, public nSignalEmitter
{
public:
    /// constructor (DONT CALL DIRECTLY, USE nKernelServer::New() INSTEAD)
    nObject();

    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// get instance size
    virtual int GetInstanceSize() const;

    /// save object under different name
    virtual bool SaveAs(const char* name);
    /// create new object as clone of this object
    virtual nObject* Clone(const char* unused = 0);

    /// get pointer to my class object
    nClass* GetClass() const;
    /// return true if part of class hierarchy
    bool IsA(const nClass*) const;
    /// return true if part of class hierarchy
    bool IsA(const char*) const;
    /// return true instance of class
    bool IsInstanceOf(const nClass*) const;
    /// return true instance of class
    bool IsInstanceOf(const char*) const;
    /// invoke nCmd on object
    bool Dispatch(nCmd*);
    /// get cmdproto list from object
    void GetCmdProtos(nHashList*);

    /// pointer to kernel server
    static nKernelServer* kernelServer;

protected:
    friend class nClass;

    /// destructor (DONT CALL DIRECTLY, USE Release() INSTEAD)
    virtual ~nObject();
    /// set pointer to my class object
    void SetClass(nClass*);

    nClass* instanceClass;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nObject::SetClass(nClass* cl)
{
    this->instanceClass = cl;
}

//------------------------------------------------------------------------------
/**
*/
inline
nClass*
nObject::GetClass() const
{
    return this->instanceClass;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nObject::IsA(const nClass* cl) const
{
    nClass* actClass = this->instanceClass;
    do
    {
        if (actClass == cl)
        {
            return true;
        }
    } while (actClass = actClass->GetSuperClass());
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nObject::IsInstanceOf(const nClass* cl) const
{
    return (cl == this->instanceClass);
}
//------------------------------------------------------------------------------
#endif // N_OBJECT_H
