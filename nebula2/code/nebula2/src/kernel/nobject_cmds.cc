//------------------------------------------------------------------------------
//  (c) 2004 Vadim Macagon
//  Refactored out of nRoot.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/nobject.h"
#include "kernel/nref.h"

static void n_getrefcount(void *, nCmd *);
static void n_getclass(void *, nCmd *);
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
    and defines this basic behaviour and properties for all 
    nobject derived classes:
    - runtime type information
    - object persistency
    - language independent scripting interface 
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("i_getrefcount_v",       'GRCT', n_getrefcount);
    cl->AddCmd("s_getclass_v",          'GCLS', n_getclass);
    cl->AddCmd("b_isa_s",               'ISA_', n_isa);
    cl->AddCmd("b_isinstanceof_s",      'ISIO', n_isinstanceof);
    cl->AddCmd("l_getcmds_v",           'GMCD', n_getcmds);
    cl->AddCmd("i_getinstancesize_v",   'GISZ', n_getinstancesize);
    cl->EndCmds();
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
    if (cl) cmd->Out()->SetB(self->IsA(cl));
    else    cmd->Out()->SetB(false);
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
    if (cl) cmd->Out()->SetB(self->IsInstanceOf(cl));
    else    cmd->Out()->SetB(false);
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
    {
        nHashList *cmd_list = n_new(nHashList);
        nHashNode* node;
        int num_cmds = 0;
        
        self->GetCmdProtos(cmd_list);
        // count commands
        for (node = cmd_list->GetHead();
             node;
             node = node->GetSucc())
        {
            num_cmds++;
        }
        
        nArg* args = n_new_array(nArg,num_cmds);
        int i = 0;
        while ((node = cmd_list->RemHead()))
        {
            args[i++].SetS(((nCmdProto*) node->GetPtr())->GetProtoDef());
            n_delete(node);
        }
        cmd->Out()->SetL(args, num_cmds);
        n_delete(cmd_list);
    }
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
