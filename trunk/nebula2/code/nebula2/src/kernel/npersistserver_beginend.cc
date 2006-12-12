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
     - 08-Nov-04   enlight + now takes nObject* instead of nRoot*
*/
nFile *
nPersistServer::PutFoldedObjectHeader(nScriptServer* saver, const char* fname, nObject* obj)
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
     - 09-Nov-04    enlight + nObject support
*/
bool
nPersistServer::BeginFoldedObject(nObject* obj, nCmd* cmd, const char* name, bool selOnly,
                                  bool isObjNamed)
{
    n_assert(obj);
    n_assert(name);

    bool fileOk = false;

    if (isObjNamed)
    {
        n_assert2(obj->IsA("nroot"), "Set isObjNamed to false to save a non-nRoot object!");

        // if no objects on stack, create a new file...
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
            n_assert(this->objectStack.Top()->IsA("nroot"));
            if (selOnly)
            {
                this->refSaver->WriteBeginSelObject(this->file, (nRoot*)obj,
                                                    (nRoot*)this->objectStack.Top());
            }
            else if (cmd)
            {
                this->refSaver->WriteBeginNewObjectCmd(this->file, (nRoot*)obj,
                                                       (nRoot*)this->objectStack.Top(), cmd);
            }
            else
            {
                this->refSaver->WriteBeginNewObject(this->file, (nRoot*)obj,
                                                    (nRoot*)this->objectStack.Top());
            }
            fileOk = true;
        }
    }
    else
    {
        n_assert2(!obj->IsA("nroot"), "Set isObjNamed to true to save an nRoot object!");
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
     - 09-Nov-04   enlight + nObject support
*/
bool
nPersistServer::EndFoldedObject(bool isObjNamed)
{
    n_assert(!this->objectStack.IsEmpty());
    // get object from stack
    nObject *obj = this->objectStack.Pop();
    n_assert(obj);

    if (isObjNamed)
    {
        n_assert2(obj->IsA("nroot"),
                  "Set isObjNamed to false to save a non-nRoot object!");

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
            n_assert(this->objectStack.Top()->IsA("nroot"));
            this->refSaver->WriteEndObject(this->file, (nRoot *)obj, (nRoot *)this->objectStack.Top());
        }
    }
    else
    {
        n_assert2(!obj->IsA("nroot"),
                  "Set isObjNamed to true to save an nRoot object!");
        n_assert2(this->objectStack.IsEmpty(),
                  "Object stack should be empty, what the hell have you been up to?");
        n_assert(this->file);

        this->refSaver->EndWrite(this->file);
        this->file = 0;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Begin an object clone operation.

     - 18-Dec-98   floh    created
     - 09-Nov-04   enlight nObject support
*/
bool
nPersistServer::BeginCloneObject(nObject* obj, const char* name, bool isObjNamed)
{
    n_assert(obj);

    bool retval = false;
    const char* cl = obj->GetClass()->GetName();

    if (isObjNamed)
    {
        n_assert(name);
        n_assert2(obj->IsA("nroot"), "Set isObjNamed to false to clone non-nRoot object!");

        this->cloneTarget = kernelServer->New(cl, name);
        if (this->cloneTarget)
        {
            n_assert(this->cloneTarget->GetRefCount() == 1);
            nRoot* actCwd = kernelServer->GetCwd();
            if (this->objectStack.IsEmpty())
            {
                this->origCwd = actCwd;
            }

            // push current object cwd onto stack, make clone target  the cwd object
            this->objectStack.Push(actCwd);
            kernelServer->SetCwd((nRoot *)this->cloneTarget);
            retval = true;
        }
    }
    else
    {
        n_assert2(!obj->IsA("nroot"), "Set isObjNamed to true to clone nRoot object!");

        this->cloneTarget = nKernelServer::Instance()->New(cl);
        if (this->cloneTarget)
        {
            n_assert(this->cloneTarget->GetRefCount() == 1);
            retval = true;
        }
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
    Finish an object clone operation.

     - 18-Dec-98   floh    created
     - 08-Nov-04   enlight + nObject support
*/
bool
nPersistServer::EndCloneObject(bool isObjNamed)
{
    if (isObjNamed)
    {
        n_assert(!this->objectStack.IsEmpty())
        n_assert2(this->objectStack.Top()->IsA("nroot"),
                  "Set isObjNamed to false to clone non-nRoot object!");
        nRoot* cwd = (nRoot*)this->objectStack.Pop();
        if (cwd != this->origCwd)
        {
            this->cloneTarget = cwd;
        }
        kernelServer->SetCwd(cwd);
    }
    else
    {
        n_assert2(!this->cloneTarget->IsA("nroot"),
                  "Set isObjNamed to true to clone nRoot object!");
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Get the pointer to the cloned object.

     - 18-Dec-98   floh    created
     - 08-Nov-04   enlight now returns nObject* instead of nRoot*
*/
nObject*
nPersistServer::GetClone()
{
    return this->cloneTarget;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
