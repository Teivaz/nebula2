#define N_IMPLEMENTS nRoot
#define N_KERNEL
//------------------------------------------------------------------------------
//  kernel/nrootscript.cc
//  This file was machine generated.
//  (C) 2000 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/nroot.h"
#include "kernel/nref.h"

static void n_getrefcount(void *, nCmd *);
static void n_getclass(void *, nCmd *);
static void n_isa(void *, nCmd *);
static void n_isinstanceof(void *, nCmd *);
static void n_getcmds(void *, nCmd *);
static void n_save(void *, nCmd *);
static void n_saveas(void *, nCmd *);
static void n_clone(void *, nCmd *);
static void n_getparent(void *, nCmd *);
static void n_gethead(void *, nCmd *);
static void n_gettail(void *, nCmd *);
static void n_getsucc(void *, nCmd *);
static void n_getpred(void *, nCmd *);
static void n_getchildren( void *, nCmd *);
static void n_setname(void *, nCmd *);
static void n_getname(void *, nCmd *);
static void n_getfullname(void *, nCmd *);
static void n_getrelpath(void *, nCmd *);
static void n_find(void *, nCmd *);
static void n_sort(void *, nCmd *);
static void n_getinstancesize(void*, nCmd*);
static void n_gettreesize(void*, nCmd*);

//-------------------------------------------------------------------
//  CLASS
//  nroot
//  SUPERCLASS
//  ---
//  INCLUDE
//  kernel/nroot.h
//  INFO
//  nroot is the superclass of all higher level Nebula class
//  and defines the basic behaviour and properties for all 
//  nroot derived classes:
//  - linkage into hierarchical name space of objects
//  - runtime type information
//  - object persistency
//  - language independent scripting interface 
//-------------------------------------------------------------------
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("i_getrefcount_v",       'GRCT', n_getrefcount);
    cl->AddCmd("s_getclass_v",          'GCLS', n_getclass);
    cl->AddCmd("b_isa_s",               'ISA_', n_isa);
    cl->AddCmd("b_isinstanceof_s",      'ISIO', n_isinstanceof);
    cl->AddCmd("l_getcmds_v",           'GMCD', n_getcmds);
    cl->AddCmd("b_save_v",              'SAVE', n_save);
    cl->AddCmd("b_saveas_s",            'SVAS', n_saveas);
    cl->AddCmd("o_clone_s",             'CLON', n_clone);
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
    cl->AddCmd("i_getinstancesize_v",   'GISZ', n_getinstancesize);
    cl->AddCmd("i_gettreesize_v",       'GTSZ', n_gettreesize);
    cl->EndCmds();
}

//-------------------------------------------------------------------
//  CMD
//  getrefcount
//  INPUT
//  v
//  OUTPUT
//  i (Refcount)
//  INFO
//  Return current ref count of object.
//-------------------------------------------------------------------
static void n_getrefcount(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetI(self->GetRefCount());
}

//-------------------------------------------------------------------
//  CMD
//  getclass
//  INPUT
//  v
//  OUTPUT
//  s (Classname)
//  INFO
//  Return name of class which the object is an instance of.
//-------------------------------------------------------------------
static void n_getclass(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetS(self->GetClass()->GetName());
}

//-------------------------------------------------------------------
//  CMD
//  isa
//  INPUT
//  s (Classname)
//  OUTPUT
//  b (Success)
//  INFO
//  Check whether the object is instantiated or derived from the
//  class given by 'Classname'.
//-------------------------------------------------------------------
static void n_isa(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    const char *arg0 = cmd->In()->GetS();
    nClass *cl = nRoot::kernelServer->FindClass(arg0);
    if (cl) cmd->Out()->SetB(self->IsA(cl));
    else    cmd->Out()->SetB(false);
}

//-------------------------------------------------------------------
//  CMD
//  isinstanceof
//  INPUT
//  s (Classname)
//  OUTPUT
//  b (Success)
//  INFO
//  Check whether the object is an instance of the class given
//  by 'Classname'.
//-------------------------------------------------------------------
static void n_isinstanceof(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    const char *arg0 = cmd->In()->GetS();
    nClass *cl = nRoot::kernelServer->FindClass(arg0);
    if (cl) cmd->Out()->SetB(self->IsInstanceOf(cl));
    else    cmd->Out()->SetB(false);
}

//-------------------------------------------------------------------
//  CMD
//  getcmds
//  INPUT
//  v
//  OUTPUT
//  s (Commands)
//  INFO
//  Return a list of all script command prototypes the object accepts.
//-------------------------------------------------------------------
static void n_getcmds(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    {
        nHashList *cmd_list = n_new nHashList;
        nCmdProto *act_cmdproto;                
        int num_cmds = 0;
        
        self->GetCmdProtos(cmd_list);
        // count commands
        for (act_cmdproto = (nCmdProto *) cmd_list->GetHead();
             act_cmdproto;
             act_cmdproto = (nCmdProto *) act_cmdproto->GetSucc())
        {
            num_cmds++;
        }
        
        nArg* args = new nArg[num_cmds];
        int i = 0;
        while ((act_cmdproto = (nCmdProto *) cmd_list->RemHead()))
        {
            args[i++].SetS(act_cmdproto->GetProtoDef());
            n_delete act_cmdproto;
        }
        cmd->Out()->SetL(args, num_cmds);
        n_delete cmd_list;
    }
}

//-------------------------------------------------------------------
//  CMD
//  save
//  INPUT
//  v
//  OUTPUT
//  b (Success)
//  INFO
//  Save the object under its current name into a filesystem
//  file. The filename will be 'name_of_object.n'. Such a
//  persistent object file must be read back with the
//  nsh 'get' command.
//-------------------------------------------------------------------
static void n_save(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetB(self->Save());                    
}

//-------------------------------------------------------------------
//  CMD
//  saveas
//  INPUT
//  s (Name)
//  OUTPUT
//  b (Success)
//  INFO
//  Save the object under a given name into a file. A trailing
//  '.n' will be appended.
//-------------------------------------------------------------------
static void n_saveas(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetB(self->SaveAs(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
//  CMD
//  clone
//  INPUT
//  s (CloneName)
//  OUTPUT
//  o (CloneHandle)
//  INFO
//  Creates a clone with the name 'CloneName' of the object.
//  If the original has child objects they will be cloned as well.
//-------------------------------------------------------------------
static void n_clone(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->Clone(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
//  CMD
//  getparent
//  INPUT
//  v
//  OUTPUT
//  o (ObjectHandle)
//  INFO
//  Return the full pathname of the parent object. If the object
//  doesn't have a parent object (this is only valid for the
//  root object '/'), 'null' will be returned.
//-------------------------------------------------------------------
static void n_getparent(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetParent());
}

//-------------------------------------------------------------------
//  CMD
//  gethead
//  INPUT
//  v
//  OUTPUT
//  o (ObjectHandle)
//  INFO
//  Return full pathname of first child object, or 'null'
//  if no child objects exist.
//-------------------------------------------------------------------
static void n_gethead(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetHead());
}

//-------------------------------------------------------------------
//  CMD
//  gettail
//  INPUT
//  v
//  OUTPUT
//  o (ObjectHandle)
//  INFO
//  Return full pathname of last child object, or 'null'
//  if no child objects exist.
//-------------------------------------------------------------------
static void n_gettail(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetTail());
}

//-------------------------------------------------------------------
//  CMD
//  getsucc
//  INPUT
//  v
//  OUTPUT
//  o (ObjectHandle)
//  INFO
//  Return full pathname of next object in the same hierarchie
//  level, zero if no next object exists.
//-------------------------------------------------------------------
static void n_getsucc(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetSucc());
}

//-------------------------------------------------------------------
//  CMD
//  getpred
//  INPUT
//  v
//  OUTPUT
//  o (ObjectHandle)
//  INFO
//  Return full pathname of previous object in the same
//  hierarchie level, zero if no previous object exists. 
//-------------------------------------------------------------------
static void n_getpred(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetPred());
}

//-------------------------------------------------------------------
//  CMD
//  getchildren
//  INPUT
//  v
//  OUTPUT
//  l (ListArg)
//  INFO
//  Return a list of all children.
//-------------------------------------------------------------------
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
        nArg* children = new nArg[num_children];
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
//  CMD
//  setname
//  INPUT
//  s (Name)
//  OUTPUT
//  v
//  INFO
//  Give the object a new name ('Name' may not contain any path
//  components!).
//-------------------------------------------------------------------
static void n_setname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    self->SetName(cmd->In()->GetS());
}

//-------------------------------------------------------------------
//  CMD
//  getname
//  INPUT
//  v
//  OUTPUT
//  s (Name)
//  INFO
//  Return the object's name without path.
//-------------------------------------------------------------------
static void n_getname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetS(self->GetName());
}

//-------------------------------------------------------------------
//  CMD
//  getfullname
//  INPUT
//  v
//  OUTPUT
//  s (Name)
//  INFO
//  Return the full pathname of the object.
//-------------------------------------------------------------------
static void n_getfullname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    char buf[N_MAXPATH];
    cmd->Out()->SetS(self->GetFullName(buf,sizeof(buf)));
}

//-------------------------------------------------------------------
//  CMD
//  getrelpath
//  INPUT
//  o (OtherObject)
//  OUTPUT
//  s (Path)
//  INFO
//  Return the shortest relative path leading from this object
//  to the given object. 
//-------------------------------------------------------------------
static void n_getrelpath(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    char buf[N_MAXPATH];
    cmd->Out()->SetS(self->GetRelPath((nRoot *)cmd->In()->GetO(),buf,sizeof(buf)));
}

//-------------------------------------------------------------------
//  CMD
//  find
//  INPUT
//  s (Name)
//  OUTPUT
//  o (PathName)
//  INFO
//  Find a child object with name 'Name'. Return full path
//  of child if found, or 'null' if not found.
//-------------------------------------------------------------------
static void n_find(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->Find(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
//  CMD
//  sort
//  INPUT
//  v
//  OUTPUT
//  v
//  INFO
//  Sort all child objects alphabetically.
//-------------------------------------------------------------------
static void n_sort(void *o, nCmd *)
{
    nRoot *self = (nRoot *) o;
    self->Sort();
}

//-------------------------------------------------------------------
//  CMD
//  getinstancesize
//  INPUT
//  v
//  OUTPUT
//  i (InstanceSize)
//  INFO
//  Get byte size of this object. This may or may not accurate,
//  depending on whether the object uses external allocated memory,
//  and if the object's class takes this into account.
//-------------------------------------------------------------------
static void n_getinstancesize(void* o, nCmd* cmd)
{
    nRoot* self = (nRoot*) o;
    cmd->Out()->SetI(self->GetInstanceSize());
}

//-------------------------------------------------------------------
//  CMD
//  gettreesize
//  INPUT
//  v
//  OUTPUT
//  i (TreeInstanceSize)
//  INFO
//  This is the recursive version of getinstancesize.
//-------------------------------------------------------------------
static void n_gettreesize(void* o, nCmd* cmd)
{
    nRoot* self = (nRoot*) o;
    cmd->Out()->SetI(self->GetTreeSize());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
