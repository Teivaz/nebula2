#ifndef N_ROOT_H
#define N_ROOT_H
//------------------------------------------------------------------------------
/**
    @class nRoot

    nRoot defines the basic functionality and interface for
    the Nebula class hierarchy:

    - reference counting
    - RTTI a class is identified by a string name
    - object serialization
    - script interface   
    - linkage into the hierarchical name space

    Rules for subclasses:
    - only the default constructor is allowed
    - never create or destroy nRoot objects through delete
    - use nKernelServer::New() to create an object and
      the objects Release() method to destroy it

    NOTE: the hash list based approach has been replaced by
    a normal linear string list. This saves 
    a great amount of per-object-memory-overhead, but may be slower when
    searching by name. Need to collect feedback how bad the 
    slowdown really is, since name lookups don't happen too
    often anyway. I feel the memory savings are more important.

    -Floh.

    See also @ref N2ScriptInterface_nroot

    (C) 1999 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nlist.h"
#include "util/nnode.h"
#include "kernel/ncmd.h"
#include "kernel/nref.h"
#include "kernel/nmutex.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nCmd;
class nClass;
class nKernelServer;
class nPersistServer;
template<class TYPE> class nRef;

class nRoot : public nNode 
{
public:
    /// constructor (DONT CALL DIRECTLY, USE nKernelServer::New() INSTEAD)
    nRoot();

    /// initialize after linkage into name hierarchy
    virtual void Initialize();
    /// release object (USE INSTEAD OF DESTRUCTOR!)
    virtual bool Release();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// get instance size
    virtual int GetInstanceSize() const;
    /// get summed instance including children
    int GetTreeSize() const;

    /// lock the object's main mutex
    void LockMutex();
    /// unlock the object's main mutex
    void UnlockMutex();

    /// increment ref count of object 
    int AddRef();
    /// get refcount of object
    int GetRefCount() const;

    /// add external object reference
    void AddObjectRef(nRef<nRoot> *);
    /// remove external object reference
    void RemObjectRef(nRef<nRoot> *);
    /// get list of refs pointing to me
    nList *GetRefs();
    
    /// get pointer to my class object
    nClass *GetClass() const;
    /// return true if part of class hierarchy
    bool IsA(nClass *) const;
    /// return true instance of class
    bool IsInstanceOf(nClass *) const; 

    /// save object to persistency stream
    bool Save();
    /// save object under different name
    bool SaveAs(const char* name);
    /// create new object as clone of this object
    nRoot *Clone(const char *name);

    /// invoke nCmd on object
    bool Dispatch(nCmd *);
    /// get cmd proto list from object
    void GetCmdProtos(nHashList *);

    /// set name of object
    void SetName(const char *str);
    /// get my name
    const char *GetName() const;
    /// get full path name of object
    nString GetFullName();
    /// get relative path name to other object
    nString GetRelPath(nRoot *other);
    /// find child object by name using const char*
    nRoot *Find(const char *str);
    /// add child object at start of child list
    void AddHead(nRoot *n);
    /// add child object at end of child list
    void AddTail(nRoot *n);
    /// remove child object at start of child list
    nRoot *RemHead();
    /// remove child object at end of child list
    nRoot *RemTail();
    /// remove myself from parent
    void Remove();
    /// sort children alphabetically
    void Sort();

    /// get parent object
    nRoot *GetParent() const;
    /// get first child
    nRoot *GetHead() const;
    /// get last child
    nRoot *GetTail() const;
    /// get next sibling
    nRoot *GetSucc() const;
    /// get previous sibling
    nRoot *GetPred() const;

    /// set savemode flags
    void SetSaveModeFlags(int);
    /// unset savemode flags
    void UnsetSaveModeFlags(int);

    /// pointer to kernel server
    static nKernelServer* kernelServer;

    // object flags (use with SetSaveModeFlag, UnsetFlag, GetFlag)
    enum {
        N_FLAG_SAVEUPSIDEDOWN = (1<<0),     // save children first, then own state
        N_FLAG_SAVESHALLOW    = (1<<1),     // do not save child objects!
    };

protected:
    friend class nClass;
        
    /// destructor (DONT CALL DIRECTLY, USE Release() INSTEAD)
    virtual ~nRoot();
    /// invalidate all references
    void InvalidateAllRefs();
    /// set pointer to my class object
    void SetClass(nClass *);

    nString name;
    nList refList;
    nClass* instanceClass;
    nRoot* parent;
    nList childList;
    ushort refCount;
    ushort saveModeFlags;
    nMutex mutex;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRoot::SetName(const char *str)
{
    this->name = str;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRoot::GetName() const
{
    return this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nRoot::GetParent() const
{
    return this->parent;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nRoot::GetHead() const
{
    return (nRoot*) this->childList.GetHead();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nRoot::GetTail() const
{
    return (nRoot*) this->childList.GetTail();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nRoot::GetSucc() const
{
    return (nRoot *) nNode::GetSucc();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nRoot::GetPred() const
{ 
    return (nRoot*) nNode::GetPred();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRoot::AddHead(nRoot *n)
{
    n->parent = this;
    this->childList.AddHead(n);    
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRoot::AddTail(nRoot *n)
{
    n->parent = this;
    this->childList.AddTail(n);
}   

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nRoot::RemHead()
{
    nRoot* n = (nRoot*) this->childList.RemHead();
    if (n)
    {
        n->parent = 0;
    }
    return n;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nRoot::RemTail()
{
    nRoot* n = (nRoot*) this->childList.RemTail();
    if (n)
    {
        n->parent = 0;
    }
    return n;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRoot::Remove()
{
    nNode::Remove();
    this->parent = 0;
}


//------------------------------------------------------------------------------
/**
     - 04-Nov-98   floh     Accept special objects ".." und "."
     - 17-May-99   floh     Optimized (strcmp() is no longer called).
*/
inline
nRoot*
nRoot::Find(const char *str)
{
    n_assert(str);

    // handle special cases '.' and '..'
    if (str[0] == '.') 
    {
        if (str[1] == 0) 
        {
            return this;
        }
        else if ((str[1]=='.') && (str[2]==0)) 
        {
            return this->parent;
        }
    }

    // find child with string compare
    nRoot* child;
    for (child = this->GetHead(); child; child = child->GetSucc())
    {
        if (strcmp(child->name.Get(), str) == 0)
        {
            return child;
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRoot::SetSaveModeFlags(int f)
{
    this->saveModeFlags |= f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRoot::UnsetSaveModeFlags(int f)
{
    this->saveModeFlags &= ~f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRoot::SetClass(nClass* cl)
{
    this->instanceClass = cl;
}

//------------------------------------------------------------------------------
/**
*/
inline
nClass*
nRoot::GetClass() const
{
    return this->instanceClass;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nRoot::IsA(nClass *cl) const
{
    nClass *actClass = this->instanceClass;
    do 
    {
        if (actClass == cl) 
        {
            return true;
        }
    } while ((actClass = actClass->GetSuperClass()));
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nRoot::IsInstanceOf(nClass *cl) const
{
    return (cl == this->instanceClass);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRoot::GetRefCount() const
{
    return this->refCount;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRoot::AddRef()
{
    return ++this->refCount;
}

//------------------------------------------------------------------------------
/**
    Hang the reference to an object on @c refList.

     - 15-Jul-99   floh    created
     - 30-Aug-00   floh    + removed call to nRoot::ValidRef()
*/
inline
void 
nRoot::AddObjectRef(nRef<nRoot> *r)
{
    this->refList.AddTail((nNode *)r);
}

//------------------------------------------------------------------------------
/**
    Remove the reference to @c r from the @c refList.

     - 15-Jul-99   floh    created
     - 30-Aug-00   floh    + removed call to nRoot::InvalidRef()
*/
inline
void 
nRoot::RemObjectRef(nRef<nRoot> *r)
{
    ((nNode*)r)->Remove();
}

//------------------------------------------------------------------------------
/**
*/
inline
nList*
nRoot::GetRefs()
{
    return &(this->refList);
}

//------------------------------------------------------------------------------
#endif
