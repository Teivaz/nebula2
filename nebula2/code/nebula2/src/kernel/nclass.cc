#define N_IMPLEMENTS nClass
#define N_KERNEL
//--------------------------------------------------------------------
//  nclass.cc
//  (C) 1998..2000 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>

#include "util/nhashlist.h"
#include "util/nkeyarray.h"
#include "kernel/nclass.h"
#include "kernel/nroot.h"

//--------------------------------------------------------------------
/**
    @param name               name of the class
    @param kserv              pointer to kernel server
    @param initFunc           pointer to n_init function in class package
    @param finiFunc           pointer to n_fini function in class package
    @param newFunc            pointer to n_create function in class package
*/
nClass::nClass(const char *name,
               nKernelServer *kserv,
               bool (*initFunc)(nClass *, nKernelServer *),
               void (*finiFunc)(void),
               void *(*newFunc)(void)) :
    nHashNode(name),
    kernelServer(kserv),
    superClass(0),
    cmdList(0),
    cmdTable(0),
    refCount(0),
    instanceSize(0),
    n_init_ptr(initFunc),
    n_fini_ptr(finiFunc),
    n_new_ptr(newFunc)
{
    // call the class module's init function
    this->n_init_ptr(this, this->kernelServer);
}

//--------------------------------------------------------------------
/**
    04-Aug-99   floh    boeser Bug: nCmdProto-Liste wurde als nCmd 
						Objekte freigegeben...
    18-Feb-00   floh    + cmd_table is now a nKeyArray
*/
nClass::~nClass(void)
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
        while (cmdProto = (nCmdProto*) this->cmdList->RemHead())
        {
            n_delete cmdProto;
        }
        n_delete this->cmdList;
    }

    if (this->cmdTable)
    {
        n_delete this->cmdTable;
    }
}

//--------------------------------------------------------------------
/**
    08-Aug-99   floh    created
*/
nRoot*
nClass::NewObject(void)
{
    nRoot *obj = (nRoot *) this->n_new_ptr();
    n_assert(obj);
    obj->SetClass(this);
    return obj;
}

//--------------------------------------------------------------------
/**
    08-Aug-99   floh    created
*/
void 
nClass::BeginCmds(void)
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

    this->cmdList = n_new nHashList(numCmds);
}

//--------------------------------------------------------------------
/**
    @param  proto_def   the command's prototype definition
    @param  id          the command's unique fourcc code 
    @param  cmd_proc    the command's stub function
*/
void 
nClass::AddCmd(const char *proto_def, uint id, void (*cmd_proc)(void *, nCmd *))
{
    n_assert(proto_def);
    n_assert(id);
    n_assert(cmd_proc);
    n_assert(this->cmdList);
    nCmdProto *cp = n_new nCmdProto(proto_def, id, cmd_proc);
    n_assert(cp);
    this->cmdList->AddTail(cp);
}

//--------------------------------------------------------------------
/**
    Build sorted array of attached cmds for a bsearch() by ID. 

    08-Aug-99   floh    created
    24-Oct-99   floh    checks for identical cmd ids and throws
                        an error 
    18-Feb-00   floh    cmd_table now a nKeyArray
*/
void 
nClass::EndCmds(void)
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
            nCmdProto *cp;
            for (cp = (nCmdProto *) cl->cmdList->GetHead();
                 cp;
                 cp = (nCmdProto *) cp->GetSucc())
            {
                num_cmds++;
            }
        }
    } while ((cl = cl->GetSuperClass()));

    // create and fill command table
    this->cmdTable = n_new nKeyArray<nCmdProto *>(num_cmds);
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
            n_error("Command id collission in class '%s'\n"
                     "cmd '%s' and cmd '%s' both have id '0x%lx'\n",
                     this->GetName(),
                     cp0->GetName(),
                     cp1->GetName(),
                     this->cmdTable->GetKeyAt(i));
        }
    }
}

//--------------------------------------------------------------------
/**
    @param  name    name of command to be found
    @return         pointer to nCmdProto object, or 0

    08-Aug-99   floh    Header
*/
nCmdProto*
nClass::FindCmdByName(const char *name)
{
    n_assert(name);

    nCmdProto *cp = 0;
    if (this->cmdList)
    {
        cp = (nCmdProto *) this->cmdList->Find(name);
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
    @param  id      the fourcc code of the command to be found
    @return         pointer to nCmdProto object, or 0

    08-Aug-99   floh    Header
    08-Aug-99   floh    + macht jetzt einen Binary-Search auf die
                          Cmd-Table, anstatt sich linear durch die
                          Cmd-Liste zu hangeln.
    25-Jan-99   floh    + bsearch() slightly optimized
    18-Feb-00   floh    + cmd_table now nKeyArray
*/
nCmdProto*
nClass::FindCmdById(uint id)
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
//  EOF
//--------------------------------------------------------------------
