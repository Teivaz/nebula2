//------------------------------------------------------------------------------
//  kernel/nrootscript.cc
//  This file was machine generated.
//  (C) 2000 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/nroot.h"
#include "kernel/nref.h"

static void n_save(void *, nCmd *);
static void n_getparent(void *, nCmd *);
static void n_gethead(void *, nCmd *);
static void n_gettail(void *, nCmd *);
static void n_getsucc(void *, nCmd *);
static void n_getpred(void *, nCmd *);
static void n_getchildren(void *, nCmd *);
static void n_setname(void *, nCmd *);
static void n_getname(void *, nCmd *);
static void n_getfullname(void *, nCmd *);
static void n_getrelpath(void *, nCmd *);
static void n_find(void *, nCmd *);
static void n_sort(void *, nCmd *);
static void n_gettreesize(void*, nCmd*);

//-------------------------------------------------------------------
/**
    @scriptclass
    nroot

    @cppclass
    nRoot

    @superclass
    nobject

    @classinfo
    nroot is the superclass of all higher level NOH-dependent Nebula classes
    and defines this basic behavior and properties for all
    nroot derived classes:
    - linkage into hierarchical name space of objects
    - runtime type information (through nobject)
    - object persistency
    - language independent scripting interface
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_save_v",              'SAVE', n_save);
    cl->AddCmd("o_getparent_v",         'GPRT', n_getparent);
    cl->AddCmd("o_gethead_v",           'GHED', n_gethead);
    cl->AddCmd("o_gettail_v",           'GTAL', n_gettail);
    cl->AddCmd("o_getsucc_v",           'GSUC', n_getsucc);
    cl->AddCmd("o_getpred_v",           'GPRD', n_getpred);
    cl->AddCmd("l_getchildren_v",       'GCHD', n_getchildren);
    cl->AddCmd("v_setname_s",           'SNAM', n_setname);
    cl->AddCmd("s_getname_v",           'GNAM', n_getname);
    cl->AddCmd("s_getfullname_v",       'GFNM', n_getfullname);
    cl->AddCmd("s_getrelpath_o",        'GRPH', n_getrelpath);
    cl->AddCmd("o_find_s",              'FIND', n_find);
    cl->AddCmd("v_sort_v",              'SORT', n_sort);
    cl->AddCmd("i_gettreesize_v",       'GTSZ', n_gettreesize);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    save

    @input
    v

    @output
    b (Success)

    @info
    Save the object under its current name into a filesystem
    file. The filename will be 'name_of_object.n'. Such a
    persistent object file must be read back with the
    nsh 'get' command.
*/
static void n_save(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetB(self->Save());
}

//-------------------------------------------------------------------
/**
    @cmd
    getparent

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return the full pathname of the parent object. If the object
    doesn't have a parent object (this is only valid for the
    root object '/'), 'null' will be returned.
*/
static void n_getparent(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetParent());
}

//-------------------------------------------------------------------
/**
    @cmd
    gethead

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return full pathname of first child object, or 'null'
    if no child objects exist.
*/
static void n_gethead(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetHead());
}

//-------------------------------------------------------------------
/**
    @cmd
    gettail

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return full pathname of last child object, or 'null'
    if no child objects exist.
*/
static void n_gettail(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetTail());
}

//-------------------------------------------------------------------
/**
    @cmd
    getsucc

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return full pathname of next object in the same hierarchy
    level, zero if no next object exists.
*/
static void n_getsucc(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetSucc());
}

//-------------------------------------------------------------------
/**
    @cmd
    getpred

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return full pathname of previous object in the same
    hierarchy level, zero if no previous object exists.
*/
static void n_getpred(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetPred());
}

//-------------------------------------------------------------------
/**
    @cmd
    getchildren

    @input
    v

    @output
    l (ListArg)

    @info
    Return a list of all children.
*/
static void n_getchildren(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    {
        int num_children = 0;

        nRoot *child;
        for (child = (nRoot *) self->GetHead();
             child;
             child = (nRoot *) child->GetSucc())
        {
             num_children++;
        }
        nArg* children = n_new_array(nArg,num_children);
        n_assert(children);
        int i = 0;
        for (child = (nRoot *) self->GetHead();
             child;
             child = (nRoot *) child->GetSucc())
        {
            children[i++].SetO(child);
        }
        cmd->Out()->SetL(children, num_children);
    }
}
//-------------------------------------------------------------------
/**
    @cmd
    setname

    @input
    s (Name)

    @output
    v

    @info
    Give the object a new name ('Name' may not contain any path
    components!).
*/
static void n_setname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    self->SetName(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    getname

    @input
    v

    @output
    s (Name)

    @info
    Return the object's name without path.
*/
static void n_getname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetS(self->GetName());
}

//-------------------------------------------------------------------
/**
    @cmd
    getfullname

    @input
    v

    @output
    s (Name)

    @info
    Return the full pathname of the object.
*/
static void n_getfullname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    nString str = self->GetFullName();
    cmd->Out()->SetS(str.Get());
}

//-------------------------------------------------------------------
/**
    @cmd
    getrelpath

    @input
    o (OtherObject)

    @output
    s (Path)

    @info
    Return the shortest relative path leading from this object
    to the given object.
*/
static void n_getrelpath(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    nString str = self->GetRelPath((nRoot*) cmd->In()->GetO());
    cmd->Out()->SetS(str.Get());
}

//-------------------------------------------------------------------
/**
    @cmd
    find

    @input
    s (Name)

    @output
    o (PathName)

    @info
    Find a child object with name 'Name'. Return full path
    of child if found, or 'null' if not found.
*/
static void n_find(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->Find(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    sort

    @input
    v

    @output
    v

    @info
    Sort all child objects alphabetically.
*/
static void n_sort(void *o, nCmd *)
{
    nRoot *self = (nRoot *) o;
    self->Sort();
}

//-------------------------------------------------------------------
/**
    @cmd
    gettreesize

    @input
    v

    @output
    i (TreeInstanceSize)

    @info
    This is the recursive version of getinstancesize.
*/
static void n_gettreesize(void* o, nCmd* cmd)
{
    nRoot* self = (nRoot*) o;
    cmd->Out()->SetI(self->GetTreeSize());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
