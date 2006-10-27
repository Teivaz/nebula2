//--------------------------------------------------------------------
//  nclass.cc
//  (C) 1998..2000 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>

#include "util/nhashlist.h"
#include "util/nkeyarray.h"
#include "kernel/nclass.h"
#include "kernel/nobject.h"
#include "kernel/ncmdprotonative.h"

//--------------------------------------------------------------------
/**
    @param name               name of the class
    @param kserv              pointer to kernel server
    @param initFunc           pointer to n_init function in class package
    @param newFunc            pointer to n_create function in class package
*/
nClass::nClass(const char *name,
               nKernelServer *kserv,
               bool (*initFunc)(nClass *, nKernelServer *),
               void *(*newFunc)()) :
    nSignalRegistry(),
    nHashNode(name),
    kernelServer(kserv),
    superClass(0),
    cmdList(0),
    cmdTable(0),
    scriptCmdList(0),
    refCount(0),
    instanceSize(0),
    n_init_ptr(initFunc),
    n_new_ptr(newFunc)
{
    // call the class module's init function
    this->n_init_ptr(this, this->kernelServer);
}

//--------------------------------------------------------------------
/**
     - 04-Aug-99   floh    boeser Bug: nCmdProto-Liste wurde als nCmd
                           Objekte freigegeben...
     - 18-Feb-00   floh    + cmd_table is now a nKeyArray
*/
nClass::~nClass()
{
    // if I'm still connected to a superclass, unlink from superclass
    if (this->superClass)
    {
        this->superClass->RemSubClass(this);
    }

    // make sure we're cleaned up ok
    n_assert(0 == this->superClass);
    n_assert(0 == this->refCount);

    // release cmd protos
    if (this->cmdList)
    {
        nCmdProto* cmdProto;
        while ((cmdProto = (nCmdProto*) this->cmdList->RemHead()))
        {
            n_delete(cmdProto);
        }
        n_delete(this->cmdList);
    }

    if (this->cmdTable)
    {
        n_delete(this->cmdTable);
    }

    if (this->scriptCmdList)
    {
        nCmdProto* cmdProto;
        while ((cmdProto = (nCmdProto *) this->scriptCmdList->RemHead()))
        {
            n_delete(cmdProto);
        }
        n_delete(this->scriptCmdList);
    }
}

//--------------------------------------------------------------------
/**
     - 08-Aug-99   floh    created
     - 07-Nov-04   enlight now returns nObject* instead of nRoot*
*/
nObject*
nClass::NewObject()
{
    nObject *obj = (nObject *) this->n_new_ptr();
    n_assert(obj);
    obj->AddRef();
    obj->SetClass(this);
    return obj;
}

//--------------------------------------------------------------------
/**
     - 08-Aug-99   floh    created
*/
void
nClass::BeginCmds()
{
    n_assert(0 == this->cmdTable);
    n_assert(0 == this->cmdList);

    // compute an average number of commands, assume an average
    // of 16 commands per subclass
    int numCmds = 0;
    nClass* cl = this;
    do
    {
        numCmds += 16;
    } while ((cl = cl->GetSuperClass()));

    this->cmdList = n_new(nHashList(numCmds));
}

//--------------------------------------------------------------------
/**
    @param  cmdProto    pointer to nCmdProto object to be added
*/
void
nClass::AddCmd(nCmdProto * cmdProto)
{
    n_assert(cmdProto);
    n_assert(this->cmdList);
    this->cmdList->AddTail(cmdProto);
}

//--------------------------------------------------------------------
/**
    @param  proto_def   the command's prototype definition
    @param  id          the command's unique fourcc code
    @param  cmd_proc    the command's stub function
*/
void
nClass::AddCmd(const char *proto_def, nFourCC id, void (*cmd_proc)(void *, nCmd *))
{
    n_assert(proto_def);
    n_assert(id);
    n_assert(cmd_proc);
    n_assert(this->cmdList);
    nCmdProtoNative *cp = n_new(nCmdProtoNative(proto_def, id, cmd_proc));
    n_assert(cp);
    this->AddCmd(cp);
}

//--------------------------------------------------------------------
/**
    Build sorted array of attached cmds for a bsearch() by ID.

     - 08-Aug-99   floh    created
     - 24-Oct-99   floh    checks for identical cmd ids and throws
                           an error
     - 18-Feb-00   floh    cmd_table now a nKeyArray
*/
void
nClass::EndCmds()
{
    n_assert(0 == this->cmdTable);
    n_assert(this->cmdList);

    nClass *cl;

    // count commands
    int num_cmds = 0;
    cl = this;
    do
    {
        if (cl->cmdList)
        {
            nHashNode *node;
            for (node = cl->cmdList->GetHead();
                 node;
                 node = node->GetSucc())
            {
                num_cmds++;
            }
        }
    } while ((cl = cl->GetSuperClass()));

    // create and fill command table
    this->cmdTable = n_new(nKeyArray<nCmdProto *>(num_cmds));
    cl = this;
    do
    {
        if (cl->cmdList)
        {
            nCmdProto *cp;
            for (cp = (nCmdProto *) cl->cmdList->GetHead();
                 cp;
                 cp = (nCmdProto *) cp->GetSucc())
            {
                this->cmdTable->Add(cp->GetId(),cp);
            }
        }
    } while ((cl = cl->GetSuperClass()));

    // check for identical cmd ids
    int i;
    for (i=0; i<(num_cmds-1); i++)
    {
        if (this->cmdTable->GetKeyAt(i) == this->cmdTable->GetKeyAt(i+1))
        {
            nCmdProto *cp0 = (nCmdProto *) this->cmdTable->GetElementAt(i);
            nCmdProto *cp1 = (nCmdProto *) this->cmdTable->GetElementAt(i+1);
            n_error("Command id collision in class '%s'\n"
                     "cmd '%s' and cmd '%s' both have id '0x%x'\n",
                     this->GetName(),
                     cp0->GetName(),
                     cp1->GetName(),
                     this->cmdTable->GetKeyAt(i));
        }
    }
}

//--------------------------------------------------------------------
/**
  @param numCmds The number of script cmds that will be defined.
*/
void nClass::BeginScriptCmds(int numCmds)
{
    n_assert(!this->scriptCmdList);
    this->scriptCmdList = n_new(nHashList(numCmds));
}

//--------------------------------------------------------------------
/**
  Can only be called between Begin/EndScriptCmds().
  @param cmdProto A cmd proto created by a script server.
  @note Script server cmd protos can't be looked up by their 4cc.
*/
void nClass::AddScriptCmd(nCmdProto* cmdProto)
{
    n_assert(this->scriptCmdList);
    n_assert(cmdProto);
    this->scriptCmdList->AddTail(cmdProto);
}

//--------------------------------------------------------------------
/**
*/
void nClass::EndScriptCmds()
{
    // empty
}

//--------------------------------------------------------------------
/**
    @param  name    name of command to be found
    @return         pointer to nCmdProto object, or 0

     - 08-Aug-99   floh    Header
*/
nCmdProto*
nClass::FindCmdByName(const char *name)
{
    n_assert(name);

    nCmdProto *cp = 0;
    // try the native cmd list first
    if (this->cmdList)
    {
        cp = (nCmdProto *) this->cmdList->Find(name);
    }

    // if that fails try the script cmd list
    if (this->scriptCmdList && !cp)
    {
        cp = (nCmdProto *) this->scriptCmdList->Find(name);
    }

    // if not found or no command list, recursively hand up to parent class
    if ((!cp) && (this->superClass))
    {
        cp = this->superClass->FindCmdByName(name);
    }
    return cp;
}

//--------------------------------------------------------------------
/**
  @param name The name of the command to be found
*/
nCmdProtoNative *nClass::FindNativeCmdByName(const char *name)
{
    n_assert(name);
    nCmdProtoNative *cp = 0;

    if (this->cmdList)
    {
        cp = (nCmdProtoNative *) this->cmdList->Find(name);
    }
    // if not found, recursively hand up to parent class
    if ((!cp) && (this->superClass))
    {
        cp = this->superClass->FindNativeCmdByName(name);
    }

    return cp;
}

//--------------------------------------------------------------------
/**
  @param name The name of the command to be found
*/
nCmdProto *nClass::FindScriptCmdByName(const char *name)
{
    n_assert(name);
    nCmdProto *cp = 0;
    if (this->scriptCmdList)
    {
        cp = (nCmdProto *) this->scriptCmdList->Find(name);
    }

    // if not found, recursively hand up to parent class
    if ((!cp) && (this->superClass))
    {
        cp = this->superClass->FindScriptCmdByName(name);
    }
    return cp;
}

//--------------------------------------------------------------------
/**
    @param  id      the fourcc code of the command to be found
    @return         pointer to nCmdProto object, or 0

     - 08-Aug-99   floh    Header
     - 08-Aug-99   floh    + macht jetzt einen Binary-Search auf die
                             Cmd-Table, anstatt sich linear durch die
                             Cmd-Liste zu hangeln.
     - 25-Jan-99   floh    + bsearch() slightly optimized
     - 18-Feb-00   floh    + cmd_table now nKeyArray
*/
nCmdProto*
nClass::FindCmdById(nFourCC id)
{
    // if the class has no commands, hand the request to the parent class.
    if (!this->cmdTable)
    {
        nClass *cl = this->GetSuperClass();
        if (cl)
        {
            return cl->FindCmdById(id);
        }
    }
    else
    {
        nCmdProto *cp = NULL;
        if (this->cmdTable->Find((int)id, cp))
        {
            return cp;
        }
    }
    return 0;
}

//--------------------------------------------------------------------
/**
    Checks if the given class is an ancestor of this class.

    @param className Name (all lowercase) of a class.
    @return true if className is an ancestor of this class, or if
            ancestorName is the name of this class.
            false otherwise.
*/
bool
nClass::IsA(const char* className) const
{
    n_assert(className);

    if (strcmp(this->GetName(), className) == 0)
        return true;

    nClass *ancestor = this->superClass;
    while (ancestor)
    {
        if (strcmp(ancestor->GetName(), className) == 0)
            return true;
        ancestor = ancestor->GetSuperClass();
    }

    return false;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
