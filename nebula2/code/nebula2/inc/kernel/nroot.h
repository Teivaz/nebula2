#ifndef N_ROOT_H
#define N_ROOT_H
//------------------------------------------------------------------------------
/**
    @class nRoot
    @ingroup Kernel

    nRoot defines the basic functionality and interface for
    NOH dependent classes in the Nebula class hierarchy.

    It provides:
    - reference counting and tracking (through nObject)
    - Nebula RTTI (through nObject)
    - object serialization
    - script interface
    - linkage into the hierarchical name space

    Rules for subclasses:
    - only the default constructor is allowed
    - never use new/delete (or variants like n_new/n_delete) with
      nRoot objects
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

#include "util/nstring.h"
#include "util/nnode.h"
#include "kernel/nobject.h"

//------------------------------------------------------------------------------
class nRoot : public nObject, public nNode
{
public:
    /// constructor (DONT CALL DIRECTLY, USE nKernelServer::New() INSTEAD)
    nRoot();

    /// initialize after linkage into name hierarchy
    virtual void Initialize();
    /// release object (USE INSTEAD OF DESTRUCTOR!)
    virtual bool Release();
    /// get summed instance including children
    int GetTreeSize() const;

    /// lock the object's main mutex
    void LockMutex();
    /// unlock the object's main mutex
    void UnlockMutex();

    /// save object to persistency stream
    bool Save();
    /// save object under different name
    virtual bool SaveAs(const char* name);
    /// create new object as clone of this object
    virtual nObject *Clone(const char *name);

    /// set name of object
    void SetName(const char *str);
    /// get my name
    const char *GetName() const;
    /// get full path name of object
    nString GetFullName() const;
    /// get relative path name to other object
    nString GetRelPath(const nRoot *other) const;
    /// find child object by name using const char*
    nRoot *Find(const char *str);
    /// find child object by name using nAtom
    // nRoot* Find(const nAtom& atom);
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

    // object flags (use with SetSaveModeFlag, UnsetFlag, GetFlag)
    enum {
        N_FLAG_SAVEUPSIDEDOWN = (1<<0),     // save children first, then own state
        N_FLAG_SAVESHALLOW    = (1<<1),     // do not save child objects!
    };

protected:

    /// destructor (DONT CALL DIRECTLY, USE Release() INSTEAD)
    virtual ~nRoot();

    // nAtom nameAtom;
    nString name;
    nRoot* parent;
    nList childList;
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
        if (child->name == str)
        {
            return child;
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    Find with nAtom. This can be faster then Find(const char*) if the
    string already exists as an atom.
*/
/*
inline
nRoot*
nRoot::Find(const nAtom& atom)
{
    // handle special cases '.' and '..'
    const char* str = atom.AsChar();
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
    // find child with atom compare
    nRoot* child;
    for (child = this->GetHead(); child; child = child->GetSucc())
    {
        if (atom == child->nameAtom)
        {
            return child;
        }
    }
    // fallthrough: not found
    return 0;
}
*/

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
#endif
