//------------------------------------------------------------------------------
//  nroot_main.cc
//  (C) 2000 A.Weissflog
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"
#include "kernel/npersistserver.h"

#include <stdlib.h>

nNebulaScriptClass(nRoot, "nobject");

//------------------------------------------------------------------------------
/**
     - 08-Oct-98   floh    created
     - 11-Dec-98   floh    + Objekt-Flags
     - 26-May-99   floh    + ks->num_objects
     - 02-May-00   floh    + parse file name
*/
nRoot::nRoot() :
    parent(0),
    saveModeFlags(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRoot::~nRoot()
{
    n_assert(this->refCount == 0);

    // prevent an invalid cwd in the kernel
    if (this == kernelServer->GetCwd())
    {
        kernelServer->SetCwd(0);
    }

    // release child objects
    nRoot* child;
    while (child = this->GetHead())
    {
        while (!child->Release());
    }

    // if I am a child, remove from parent
    if (this->parent)
    {
        this->Remove();
    }
}

//------------------------------------------------------------------------------
/**
    Will be called by the kernel after the object has been
    constructed and linked into the name space.

     - 13-Nov-00   floh    created
*/
void
nRoot::Initialize()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nRoot::Release()
{
    n_assert(this->refCount > 0);
    if (--this->refCount == 0)
    {
        // do not delete as long as mutex is set
        this->LockMutex();
        n_delete(this);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
     - 08-Oct-98   floh    created
     - 23-Jan-01   floh    why the f*ck was this method recursive???
     - 24-May-04    floh    returns nString
*/
nString
nRoot::GetFullName() const
{
    // build stack of pointers leading from me to root
    const int maxDepth = 128;
    const nRoot* stack[maxDepth];
    const nRoot* cur = this;
    int i = 0;
    do
    {
        stack[i++] = cur;
    }
    while ((cur = cur->GetParent()) && i < maxDepth);

    // traverse stack in reverse order and build filename
    nString str;
    i--;
    for (; i >= 0; i--)
    {
        const char* curName = stack[i]->GetName();
        str.Append(curName);

        // add slash if not hierarchy root, and not last element in path
        if ((curName[0] != '/') && (i > 0))
        {
            str.Append("/");
        }
    }
    return str;
}

//------------------------------------------------------------------------------
/**
    Return shortest relative path leading from 'this' to 'other' object.
    This is a slow operation, unless one object is the parent of
    the other (this is a special case optimization).

     - 06-Mar-00    floh    created
     - 21-Feb-04    floh    now accepts "other == this" (returns a dot)
     - 24-May-04    floh    rewritten to nString
*/
nString
nRoot::GetRelPath(const nRoot* other) const
{
    n_assert(other);

    nString str;
    if (other == this)
    {
        str = ".";
    }
    else if (other == this->GetParent())
    {
        str = "..";
    }
    else if (other->GetParent() == this)
    {
        str = other->GetName();
    }
    else
    {
        // normal case
        nArray<const nRoot*> thisHierarchy;
        nArray<const nRoot*> otherHierarchy;

        // for both objects, create lists of all parents up to root
        const nRoot* o = this;
        do
        {
            thisHierarchy.Insert(0, o);
        }
        while ((o = o->GetParent()));
        o = other;
        do
        {
            otherHierarchy.Insert(0, o);
        }
        while ((o = o->GetParent()));

        // remove identical parents
        bool running = true;
        do
        {
            if ((thisHierarchy.Size() > 0) && (otherHierarchy.Size() > 0))
            {
                const nRoot* o0 = thisHierarchy[0];
                const nRoot* o1 = otherHierarchy[0];
                if (o0 == o1)
                {
                    thisHierarchy.Erase(0);
                    otherHierarchy.Erase(0);
                }
                else running = false;
            }
            else running = false;
        }
        while (running);

        // create path leading upward from this to the identical parent
        while (thisHierarchy.Size() > 0)
        {
            str.Append("../");
            thisHierarchy.Erase(thisHierarchy.Size() - 1);
        }
        while (otherHierarchy.Size() > 0)
        {
            str.Append(otherHierarchy[0]->GetName());
            str.Append("/");
            otherHierarchy.Erase(0);
        }

        // eliminate trailing '/'
        str.StripTrailingSlash();
    }

    // done
    return str;
}

//------------------------------------------------------------------------------
/**
    Compare-Hook for qsort() in nRoot::Sort()

     - 18-May-99   floh    created
*/
int __cdecl
child_cmp(const void* e0, const void* e1)
{
    nRoot* r0 = *((nRoot**)e0);
    nRoot* r1 = *((nRoot**)e1);
    return strcmp(r1->GetName(), r0->GetName());
}

//------------------------------------------------------------------------------
/**
    Sort child objects alphabetically. This is a slow operation.

     - 18-May-99   floh    created
*/
void
nRoot::Sort()
{
    int num,i;
    nRoot* c;

    // count child objects
    for (num = 0, c = this->GetHead(); c; c = c->GetSucc(), num++);

    if (num > 0)
    {
        nRoot** c_array = (nRoot**)n_malloc(num * sizeof(nRoot*));
        n_assert(c_array);
        for (i = 0, c = this->GetHead(); c; c = c->GetSucc(), i++)
        {
            c_array[i] = c;
        }
        qsort(c_array, num, sizeof(nRoot*), child_cmp);

        for (i = 0; i < num; i++)
        {
            c_array[i]->Remove();
            this->AddHead(c_array[i]);
        }
        n_free(c_array);
    }
}

//------------------------------------------------------------------------------
/**
     - 04-Nov-98   floh    created
*/
bool
nRoot::Save()
{
    return this->SaveAs(this->GetName());
}

//------------------------------------------------------------------------------
/**
*/
bool
nRoot::SaveAs(const char* name)
{
    n_assert(name);
    nPersistServer* ps = kernelServer->GetPersistServer();
    n_assert(ps);

    bool retval = false;
    if (ps->BeginObject(this, name, true))
    {
        // ...the usual behavior...
        nRoot* c;
        if (this->saveModeFlags & N_FLAG_SAVEUPSIDEDOWN)
        {
            // upside down: save children first, then own status
            if (!(this->saveModeFlags & N_FLAG_SAVESHALLOW))
            {
                for (c = this->GetHead(); c; c = c->GetSucc())
                {
                    c->Save();
                }
            }
            retval = this->SaveCmds(ps);
        }
        else
        {
            // normal: save own status first, then children
            retval = this->SaveCmds(ps);
            if (!(this->saveModeFlags & N_FLAG_SAVESHALLOW))
            {
                for (c = this->GetHead(); c; c = c->GetSucc())
                {
                    c->Save();
                }
            }
        }
        ps->EndObject(true);
    }
    else
    {
        n_error("nRoot::SaveAs(): BeginObject() failed!");
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nRoot::Clone(const char* name)
{
    n_assert(name);
    nObject* clone = NULL;
    nPersistServer* ps = kernelServer->GetPersistServer();
    n_assert(ps);

    nPersistServer::nSaveMode oldMode = ps->GetSaveMode();
    ps->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
    if (ps->BeginObject(this, name, true))
    {
        // ...the usual behavior...
        nRoot* c;
        if (this->saveModeFlags & N_FLAG_SAVEUPSIDEDOWN)
        {
            // upside down: save children first, then own status
            if (!(this->saveModeFlags & N_FLAG_SAVESHALLOW))
            {
                for (c = this->GetHead(); c; c = c->GetSucc())
                {
                    c->Clone(c->GetName());
                }
            }
            this->SaveCmds(ps);
        }
        else
        {
            // normal: save own status first, then children
            this->SaveCmds(ps);
            if (!(this->saveModeFlags & N_FLAG_SAVESHALLOW))
            {
                for (c = this->GetHead(); c; c = c->GetSucc())
                {
                    c->Clone(c->GetName());
                }
            }
        }
        ps->EndObject(true);
        clone = ps->GetClone();

    }
    else
    {
        n_error("nRoot::Clone(): BeginObject() failed!");
    }
    ps->SetSaveMode(oldMode);

    return clone;
}

//------------------------------------------------------------------------------
/**
    Recursive version of GetInstanceSize(). Returns the summed the size of
    all children.
*/
int
nRoot::GetTreeSize() const
{
    // get size of child objects
    int size = 0;
    nRoot* curChild;
    for (curChild = this->GetHead(); curChild; curChild = curChild->GetSucc())
    {
        size += curChild->GetTreeSize();
    }

    // add own size
    size += this->GetInstanceSize();
    return size;
}

//------------------------------------------------------------------------------
/**
    Lock the object's main mutex.
*/
void
nRoot::LockMutex()
{
    this->mutex.Lock();
}

//------------------------------------------------------------------------------
/**
    Unlock the object's main mutex.
*/
void
nRoot::UnlockMutex()
{
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
