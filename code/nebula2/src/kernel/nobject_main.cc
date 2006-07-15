//------------------------------------------------------------------------------
//  (c) 2004 Vadim Macagon
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nobject.h"
#include "kernel/npersistserver.h"

nNebulaRootClass(nObject);

//------------------------------------------------------------------------------
/**
*/
nObject::nObject() :
    instanceClass(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nObject::~nObject()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
nObject::Dispatch(nCmd *cmd)
{
    n_assert(cmd->GetProto());
    cmd->Rewind();
    cmd->GetProto()->Dispatch((void*)this, cmd);
    cmd->Rewind();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
nObject::GetCmdProtos(nHashList *cmd_list)
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
                nHashNode* node = n_new(nHashNode(cmd_proto->GetName()));
                node->SetPtr((void*)cmd_proto);
                cmd_list->AddTail(node);
            }
        }
    } while ((cl = cl->GetSuperClass()));
}

//------------------------------------------------------------------------------
/**
*/
bool 
nObject::SaveAs(const char *name)
{
    n_assert(name);
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    bool retval = false;
    if (ps->BeginObject(this, name, false))
    {
        retval = this->SaveCmds(ps);
        ps->EndObject(false);
    } 
    else 
    {
        n_error("nObject::SaveAs(): BeginObject() failed!");
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nObject::Clone(const char *)
{
    nObject *clone = NULL;
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    nPersistServer::nSaveMode oldMode = ps->GetSaveMode();
    ps->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
    if (ps->BeginObject(this, 0, false)) 
    {
        this->SaveCmds(ps);
        ps->EndObject(false);
        clone = ps->GetClone();
    } 
    else 
    {
        n_error("nObject::Clone(): BeginObject() failed!");
    }
    ps->SetSaveMode(oldMode);

    return clone;
}

//------------------------------------------------------------------------------
/**
    This method is usually derived by subclasses to write their peristent 
    attributes to the file server.
*/
bool 
nObject::SaveCmds(nPersistServer *)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Get byte size of this instance. For more accuracy, subclasses should
    add the size of allocated memory.
*/
int
nObject::GetInstanceSize() const
{
    n_assert(this->instanceClass);
    return this->instanceClass->GetInstanceSize();
}

//------------------------------------------------------------------------------
/**
    @brief Checks if a class of the given name is part of the class hierarchy
           for this object.
*/
bool 
nObject::IsA(const char *className) const
{
    return this->IsA(nKernelServer::Instance()->FindClass(className));
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
