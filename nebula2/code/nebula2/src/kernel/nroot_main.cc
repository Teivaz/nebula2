#define N_IMPLEMENTS nRoot
#define N_KERNEL
//------------------------------------------------------------------------------
//  nroot_main.cc
//  (C) 2000 A.Weissflog
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"
#include "kernel/npersistserver.h"

#include <stdlib.h>

nNebulaRootClass(nRoot);

//------------------------------------------------------------------------------
/**
    08-Oct-98   floh    created
    11-Dec-98   floh    + Objekt-Flags
    26-May-99   floh    + ks->num_objects
    02-May-00   floh    + parse file name
*/
nRoot::nRoot() :
    instanceClass(0),
    parent(0),
//    childList(0),
    refCount(1),
    saveModeFlags(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    08-Oct-98   floh    created
    01-Dec-98   floh    wiederholt jetzt Release() auf Child solange,
                        bis es wirklich tot ist (es kann sein, dass
                        dessen RefCount nicht 1 ist).
    13-May-99   floh    + Cwd-Safe-Code. Wenn dieses Objekt das
                          Cwd ist, wird das Cwd auf '/' gesetzt,
                          damit der Kernel keinen ungueltigen Pointer
                          mit sich rumschleppt
    17-May-99   floh    + Listenhandling umgeschrieben
    26-May-99   floh    + ks->num_objects
    14-May-99   floh    + gibt alle Childs frei, die noch uebrig
                          sind...
    02-May-00   floh    + release parse file name
    12-Sep-00   floh    + InvalidateAllRefs() moved here
    12-Dec-00   floh    + BUG! when removing child objects, the loop
                          used a RemHead() to remove the child object
                          from the list, which in turn invalidates
                          the parent pointer, this caused the "am I the
                          current cwd" check to fail!
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
    nRoot *child;
    while ((child = this->GetHead())) 
    {
        while (!child->Release());
    }

    // invalidate all refs to me
    this->InvalidateAllRefs();

    // if I am a child, remove from parent
    if (this->parent) 
    {
        this->Remove();   
    }
        
    // clean up
/*
    if (this->childList) 
    {
        n_delete this->childList;
    }
*/
}

//------------------------------------------------------------------------------
/**
    Will be called by the kernel after the object has been
    constructed and linked into the name space.

    13-Nov-00   floh    created
*/
void 
nRoot::Initialize() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    08-Oct-98   floh    created
    11-Nov-98   floh    Rueckgabewert von long auf bool
    10-Dec-98   floh    + wertet jetzt zuerst Objekt-Flags aus
    24-Apr-99   floh    + Release() auf Childobjekts (war vorher
                          im Destruktor)
    14-Jun-99   floh    + Irgendwie ham se mir wahrscheinlich ins
                          Gehirn geschissen. Das Killen der Subobjekte
                          SOLLTE im Destruktor passieren, weil damit
                          Subklassen die Objekte selbst auf
                          spezialisierte Weise wegraeumen koennen. Das
                          sei hiermit definiert!
    16-Jul-99   floh    + N_FLAGS_PROTECTED gekillt
                        + InvalidateAllRefs()
    12-Sep-00   floh    + InvalidateAllRefs() has been moved to
                          destructor
*/
bool 
nRoot::Release()
{
    bool retval = false;
    this->refCount--;
    if (this->refCount == 0) 
    {
        n_delete this;
        retval = true;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
void 
nRoot::InvalidateAllRefs()
{
    nRef<nRoot> *r;
    while ((r = (nRef<nRoot> *) this->refList.GetHead())) 
    {
        r->invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    08-Oct-98   floh    created
    23-Jan-01   floh    why the f*ck was this method recursive???
*/
char *
nRoot::GetFullName(char *buf, int sizeof_buf)
{
    // build stack of pointers leading from me to root
    const int maxDepth = 128;
    nRoot *stack[maxDepth];
    nRoot *cur = this;
    int i = 0;
    do 
    {
        stack[i++] = cur;
    } while ((cur = cur->GetParent()) && (i<maxDepth));

    // traverse stack in reverse order and build filename    
    char tmp[N_MAXPATH];
    tmp[0] = 0;
    i--;
    for (; i>=0; i--) 
    {
        const char *curName = stack[i]->GetName();
        strcat(tmp,curName);
        
        // add slash if not hierarchie root, and not last element in path
        if ((curName[0] != '/') && (i>0)) 
        {
            strcat(tmp,"/");
        }
    }

    // copy result to provided buffer
    n_strncpy2(buf,tmp,sizeof_buf);
    return buf;
}

//------------------------------------------------------------------------------
/**
    Return shortest relative path leading from 'this' to 'other' object.
    This is a slow operation, unless one object is the parent of
    the other (this is a special case optimization).
    
    06-Mar-00   floh    created
*/
char *
nRoot::GetRelPath(nRoot *other, char *buf, int sizeof_buf)
{
    n_assert(other);
    n_assert(other != this);
    n_assert(buf);
    n_assert(sizeof_buf > 0);

    buf[0] = 0;
    if (other == this->GetParent()) 
    {
        // special case optimize: other is parent of this
        n_strcat(buf,"..",sizeof_buf);
    } 
    else if (other->GetParent() == this) 
    {
        // special case optimize: this is parent of other
        n_strcat(buf,other->GetName(),sizeof_buf);
    } 
    else 
    {
        // normal case
        nList this_hier;
        nList other_hier;
        nRoot *o;

        // for both objects, create lists of all parents up to root 
        o = this;
        do 
        {
            nNode *n = n_new nNode(o);
            this_hier.AddHead(n);
        } while ((o=o->GetParent()));
        o = other;
        do 
        {
            nNode *n = n_new nNode(o);
            other_hier.AddHead(n);
        } while ((o=o->GetParent()));

        // remove identical parents
        bool running = true;
        do 
        {
            nNode *n0 = this_hier.GetHead();
            nNode *n1 = other_hier.GetHead();
            if (n0 && n1) 
            {
                if (n0->GetPtr() == n1->GetPtr()) 
                {
                    n0->Remove();
                    n1->Remove();
                    n_delete n0;
                    n_delete n1;
                } 
                else running = false;
            } 
            else running = false;
        } while (running);

        // create path leading upward from this to the identical parent
        nNode *n;
        while ((n=this_hier.RemTail())) 
        {
            n_delete n;
            n_strcat(buf,"../",sizeof_buf);
        }
        // create path leading downward from parent to 'other'
        while ((n=other_hier.RemHead())) 
        {
            o = (nRoot *) n->GetPtr();
            n_delete n;
            n_strcat(buf,o->GetName(),sizeof_buf);
            n_strcat(buf,"/",sizeof_buf);
        }

        // eliminate trailing '/'
        buf[strlen(buf)-1] = 0;
    }

    // done
    return buf;
}

//------------------------------------------------------------------------------
/**
    02-Jan-00   floh    created
*/
bool 
nRoot::Dispatch(nCmd *cmd)
{
    void (*cmdProc)(void *, nCmd *) = cmd->GetProto()->cmdProc;
    n_assert(cmdProc);
    cmd->Rewind();
    cmdProc((void *)this, cmd);
    return true;
}

//------------------------------------------------------------------------------
/**
    19-Oct-98   floh    created
    03-Nov-98   floh    umbenannt nach GetCmdProtos()
    17-May-99   floh    neues Listenhandling
    05-Feb-01   floh    + simplified: no longer checks
                          for duplicate names, this is illegal
                          anyway
*/
void 
nRoot::GetCmdProtos(nHashList *cmd_list)
{
    // for each superclass attach it's command proto names
    // to the list
    nClass *cl = this->instanceClass;
    
    // for each superclass...
    do 
    {
        nHashList *cl_cmdprotos = cl->GetCmdList();
        if (cl_cmdprotos)
        {
            nCmdProto *cmd_proto;
            for (cmd_proto=(nCmdProto *) cl_cmdprotos->GetHead(); 
                 cmd_proto; 
                 cmd_proto=(nCmdProto *) cmd_proto->GetSucc()) 
            {
                nCmdProto *cmdproto_dup;
                cmdproto_dup = n_new nCmdProto(*cmd_proto);
                n_assert(cmdproto_dup);
                cmd_list->AddTail(cmdproto_dup);
            }
        }
    } while ((cl = cl->GetSuperClass()));
}

//------------------------------------------------------------------------------
/**
    Compare-Hook for qsort() in nRoot::Sort()

    18-May-99   floh    created
*/
int __cdecl
child_cmp(const void *e0, const void *e1)
{
    nRoot *r0 = *((nRoot **)e0);
    nRoot *r1 = *((nRoot **)e1);
    return strcmp(r1->GetName(),r0->GetName());
}

//------------------------------------------------------------------------------
/**
    Sort child objects alphabetically. This is a slow operation.

    18-May-99   floh    created
*/
void 
nRoot::Sort()
{
    int num,i;
    nRoot *c;

    // count child objects
    for (num=0, c=this->GetHead(); c; c=c->GetSucc(), num++);

    if (num > 0) 
    {
        nRoot **c_array = (nRoot **) n_malloc(num * sizeof(nRoot *));
        n_assert(c_array);\
        for (i = 0, c = this->GetHead(); c; c = c->GetSucc(), i++) 
        {
            c_array[i] = c;
        }
        qsort(c_array, num, sizeof(nRoot *), child_cmp);

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
    04-Nov-98   floh    created
*/
bool 
nRoot::Save()
{
    return this->SaveAs(this->GetName());
}

//------------------------------------------------------------------------------
/**
    04-Nov-98   floh    created
    11-Dec-98   floh    + beachtet jetzt das N_FLAG_SAVEUPSIDEDOWN,
                          und speichert in diesem Fall die
                          Child-Objekte zuerst ab
    02-May-00   floh    + support for Import(). if an import filename
                          is set, only the import statement will be
                          written instead of the sequence of commands.
                          Subobjects will be skipped as well.
    09-Aug-00   floh    + support for N_FLAG_SAVESHALLOW (don't save
                          child objects)
*/
bool 
nRoot::SaveAs(const char *name)
{
    n_assert(name);
    nPersistServer *ps = kernelServer->GetPersistServer();
    n_assert(ps);

    bool retval = false;
    if (ps->BeginObject(this, name)) 
    {
        // ...the usual behaviour...
        nRoot *c;
        if (this->saveModeFlags & N_FLAG_SAVEUPSIDEDOWN) 
        {
            // upsidedown: save children first, then own status
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
		ps->EndObject();
    } 
    else 
    {
        n_error("nRoot::SaveAs(): BeginObject() failed!");
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
    18-Dec-98   floh    created
    02-May-00   floh    + support for Import(). if an import filename
                          is set, only the import statement will be
                          written instead of the sequence of commands.
                          Subobjects will be skipped as well.
    09-Aug-00   floh    + support for shallow copy
*/
nRoot*
nRoot::Clone(const char *name)
{
    n_assert(name);
    nRoot *clone = NULL;
    nPersistServer *ps = kernelServer->GetPersistServer();
    n_assert(ps);

    nPersistServer::nSaveMode old_sm = ps->GetSaveMode();
    ps->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
    if (ps->BeginObject(this, name)) 
    {
        // ...the usual behaviour...
		nRoot *c;
        if (this->saveModeFlags & N_FLAG_SAVEUPSIDEDOWN) 
        {
            // upsidedown: save children first, then own status
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
        ps->EndObject();
        clone = ps->GetClone();

    } 
    else 
    {
        n_error("nRoot::Clone(): BeginObject() failed!");
    }
    ps->SetSaveMode(old_sm);

    return clone;
}

//------------------------------------------------------------------------------
/**
    This method is usually derived by subclasses to write their peristent 
    attributes to the file server.
  
    04-Nov-98   floh    created
*/
bool 
nRoot::SaveCmds(nPersistServer *)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Get byte size of this instance. For more accuracy, subclasses should
    add the size of allocated memory.
*/
int
nRoot::GetInstanceSize() const
{
    n_assert(this->instanceClass);
    return this->instanceClass->GetInstanceSize();
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
//  EOF
//------------------------------------------------------------------------------
