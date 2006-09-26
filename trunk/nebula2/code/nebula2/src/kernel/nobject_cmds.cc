//------------------------------------------------------------------------------
//  (c) 2004 Vadim Macagon
//  Refactored out of nRoot.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/nobject.h"
#include "kernel/nref.h"

static void n_saveas(void *, nCmd *);
static void n_clone(void *, nCmd *);
static void n_getrefcount(void *, nCmd *);
static void n_getclass(void *, nCmd *);
static void n_getclasses(void *, nCmd *);
static void n_isa(void *, nCmd *);
static void n_isinstanceof(void *, nCmd *);
static void n_getcmds(void *, nCmd *);
static void n_getinstancesize(void*, nCmd*);

//-------------------------------------------------------------------
/**
    @scriptclass
    nobject

    @cppclass
    nObject

    @superclass
    ---

    @classinfo
    nobject is the superclass of all higher level Nebula classes
    and defines this basic behavior and properties for all
    nobject derived classes:
    - runtime type information
    - object persistency
    - language independent scripting interface
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_saveas_s",            'SVAS', n_saveas);
    cl->AddCmd("o_clone_s",             'CLON', n_clone);
    cl->AddCmd("i_getrefcount_v",       'GRCT', n_getrefcount);
    cl->AddCmd("s_getclass_v",          'GCLS', n_getclass);
    cl->AddCmd("l_getclasses_v",        'GCLL', n_getclasses);
    cl->AddCmd("b_isa_s",               'ISA_', n_isa);
    cl->AddCmd("b_isinstanceof_s",      'ISIO', n_isinstanceof);
    cl->AddCmd("l_getcmds_v",           'GMCD', n_getcmds);
    cl->AddCmd("i_getinstancesize_v",   'GISZ', n_getinstancesize);

    n_initcmds_nsignalemitter(cl);

    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    saveas

    @input
    s (Name)

    @output
    b (Success)

    @info
    Save the object under a given name into a file.
*/
static void n_saveas(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetB(self->SaveAs(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    clone

    @input
    s (CloneName)

    @output
    o (CloneHandle)

    @info
    Creates a clone of this object.
    - If the object's class hierarchy doesn't contain nroot then
    'CloneName' is ignored. Otherwise 'CloneName' is the name given
    to the new cloned object.
    - If the original object has child objects, they will be cloned
    as well.
*/
static void n_clone(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetO(self->Clone(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    getrefcount

    @input
    v

    @output
    i (Refcount)

    @info
    Return current ref count of object.
*/
static void n_getrefcount(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetI(self->GetRefCount());
}

//-------------------------------------------------------------------
/**
    @cmd
    getclass

    @input
    v

    @output
    s (Classname)

    @info
    Return name of class which the object is an instance of.
*/
static void n_getclass(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetS(self->GetClass()->GetName());
}

//-------------------------------------------------------------------
/**
    @cmd
    getclasses

    @input
    v

    @output
    l (ClassnameList)

    @info
    Return the list of classes which the object is an instance of.
*/
static void n_getclasses(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    nClass* classObject;

    // count classes
    int numClasses = 0;
    for (classObject = self->GetClass();
         classObject;
         classObject = classObject->GetSuperClass())
    {
        numClasses++;
    }
    // Allocate
    nArg* args = n_new_array(nArg, numClasses);
    // And fill
    int i = 0;
    classObject = self->GetClass();
    do
    {
        args[i++].SetS(classObject->GetName());
    }
    while ((classObject = classObject->GetSuperClass()));
    cmd->Out()->SetL(args, numClasses);
}

//-------------------------------------------------------------------
/**
    @cmd
    isa

    @input
    s (Classname)

    @output
    b (Success)

    @info
    Check whether the object is instantiated or derived from the
    class given by 'Classname'.
*/
static void n_isa(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    const char *arg0 = cmd->In()->GetS();
    nClass *cl = nRoot::kernelServer->FindClass(arg0);
    if (cl)
    {
        cmd->Out()->SetB(self->IsA(cl));
    }
    else
    {
        cmd->Out()->SetB(false);
    }
}

//-------------------------------------------------------------------
/**
    @cmd
    isinstanceof

    @input
    s (Classname)

    @output
    b (Success)

    @info
    Check whether the object is an instance of the class given
    by 'Classname'.
*/
static void n_isinstanceof(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    const char *arg0 = cmd->In()->GetS();
    nClass *cl = nRoot::kernelServer->FindClass(arg0);
    if (cl)
    {
        cmd->Out()->SetB(self->IsInstanceOf(cl));
    }
    else
    {
        cmd->Out()->SetB(false);
    }
}

//-------------------------------------------------------------------
/**
    @cmd
    getcmds

    @input
    v

    @output
    l (Commands)

    @info
    Return a list of all script command prototypes the object accepts.
*/
static void n_getcmds(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    nHashList cmdList;
    nHashNode* node;
    int numCmds = 0;

    self->GetCmdProtos(&cmdList);
    // count commands
    for (node = cmdList.GetHead(); node; node = node->GetSucc())
    {
        numCmds++;
    }

    nArg* args = n_new_array(nArg, numCmds);
    int i = 0;
    while ((node = cmdList.RemHead()))
    {
        args[i++].SetS(((nCmdProto*) node->GetPtr())->GetProtoDef());
        n_delete(node);
    }
    cmd->Out()->SetL(args, numCmds);
}

//-------------------------------------------------------------------
/**
    @cmd
    getinstancesize

    @input
    v

    @output
    i (InstanceSize)

    @info
    Get byte size of this object. This may or may not accurate,
    depending on whether the object uses external allocated memory,
    and if the object's class takes this into account.
*/
static void n_getinstancesize(void* o, nCmd* cmd)
{
    nObject* self = (nObject*) o;
    cmd->Out()->SetI(self->GetInstanceSize());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
