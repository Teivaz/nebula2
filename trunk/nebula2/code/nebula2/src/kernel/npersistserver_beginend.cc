//------------------------------------------------------------------------------
//  npersistentserver_beginend.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/npersistserver.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
    Write persistent file header.

     - 05-Nov-98   floh    created
     - 29-Mar-99   floh    + rewritten to FOpen()
     - 23-May-02   floh    + rewritten to nFile
*/
nFile *
nPersistServer::PutFoldedObjectHeader(nScriptServer *saver, const char *fname, nRoot* obj)
{
    n_assert(saver);
    n_assert(fname);
    n_assert(obj);
    return saver->BeginWrite(fname, obj);
}

//------------------------------------------------------------------------------
/**
    Start writing a persistent object into a persistent object file.

     - 04-Nov-98    floh     created
     - 11-Nov-98    floh    + Removed support for root dir,
                               now always saves to cwd
     - 20-Jan-00    floh    + rewritten for ref_ss
     - 29-Feb-00    floh    + optional constructor cmd object
     - 06-Mar-00    floh    + 'sel_only' argument
                            + 'owner' object handling, which is the
                              object at the top of stack
     - 16-Feb-04    floh    + no longer appends an ".n2" to path
*/
bool 
nPersistServer::BeginFoldedObject(nRoot *obj, nCmd *cmd, const char *name, bool selOnly)
{
    n_assert(obj);
    n_assert(name);

    // if no objects on stack, create a new file...
    bool fileOk = false;
    if (this->objectStack.IsEmpty()) 
    {
        n_assert(0 == this->file);
        this->file = this->PutFoldedObjectHeader(this->refSaver.get(), name, obj);
        if (this->file) 
        {
            fileOk = true;
        }
        else 
        {
            n_printf("nPersistServer: could not open file '%s' for writing!\n", name);
        }
    } 
    else 
    {
        // there are objects on stack, reuse existing file
        n_assert(this->file);
        if (selOnly) 
        {
            this->refSaver->WriteBeginSelObject(this->file, obj, this->objectStack.Top());
        } 
        else if (cmd) 
        {
            this->refSaver->WriteBeginNewObjectCmd(this->file, obj, this->objectStack.Top(), cmd);
        } 
        else 
        {
            this->refSaver->WriteBeginNewObject(this->file, obj, this->objectStack.Top());
        }
        fileOk = true;
    }
    
    // push object onto stack
    if (fileOk) 
    {
        this->objectStack.Push(obj);
    }
    return fileOk;
}

//------------------------------------------------------------------------------
/** 
    Finish writing persistent object.

     - 04-Nov-98   floh    created
     - 20-Jan-00   floh    + rewritten for ref_ss
     - 06-Mar-00   floh    + object_stack/fp_stack now templates
                           + owner object handling
*/
bool 
nPersistServer::EndFoldedObject()
{
    // get object from stack
    nRoot *obj = this->objectStack.Pop();
    n_assert(obj);
    if (this->objectStack.IsEmpty()) 
    {
        n_assert(this->file);

        // top object reached, close file
        this->refSaver->EndWrite(this->file);
        this->file = 0;
    } 
    else 
    {
        // ...a sub object
        this->refSaver->WriteEndObject(this->file, obj, this->objectStack.Top());
    }
    return true;
}

//------------------------------------------------------------------------------
/** 
    Begin an object clone operation.
 
     - 18-Dec-98   floh    created
*/
bool 
nPersistServer::BeginCloneObject(nRoot *obj, const char *name)
{
    n_assert(obj);
    n_assert(name);
    bool retval = false;
    const char *cl = obj->GetClass()->GetName();
    this->cloneTarget = kernelServer->New(cl, name);
    if (this->cloneTarget) 
    {
        n_assert(this->cloneTarget->GetRefCount() == 1);
        nRoot *actCwd = kernelServer->GetCwd();
        if (this->objectStack.IsEmpty()) 
        {
            this->origCwd = actCwd;
        }

        // push current object cwd onto stack, make clone target  the cwd object
        this->objectStack.Push(actCwd);
        kernelServer->SetCwd(this->cloneTarget);
        retval = true;
    }
    return retval;
}

//------------------------------------------------------------------------------
/** 
    Finish an object clone operation.
  
     - 18-Dec-98   floh    created
*/
bool 
nPersistServer::EndCloneObject(void)
{
    nRoot *cwd = this->objectStack.Pop();
    if (cwd != this->origCwd) 
    {
        this->cloneTarget = cwd;
    }
    kernelServer->SetCwd(cwd);
    return true;
}

//------------------------------------------------------------------------------
/** 
    Get the pointer to the cloned object.

     - 18-Dec-98   floh    created
*/
nRoot*
nPersistServer::GetClone(void)
{
    return this->cloneTarget;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
