#ifndef N_SESSIONATTRPOOL_H
#define N_SESSIONATTRPOOL_H
//------------------------------------------------------------------------------
/**
    @class nSessionAttrPool
    @ingroup Network
    @brief An attribute pool class for the network subsystem.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nautoref.h"
#include "variable/nvariableserver.h"
#include "variable/nvariablecontext.h"

//------------------------------------------------------------------------------
class nSessionAttrPool
{
public:
    /// constructor
    nSessionAttrPool();
    /// destructor
    ~nSessionAttrPool();
    /// set an attribute
    void SetAttr(const char* name, const char* val);
    /// get number of attributes
    int GetNumAttrs() const;
    /// get attribute by index
    void GetAttrAt(int index, const char*& name, const char*& val);
    /// get an attribute value by name
    const char* GetAttr(const char* name);

private:
    nAutoRef<nVariableServer> refVarServer;
    nVariableContext attrs;
};

//------------------------------------------------------------------------------
/**
*/
inline
nSessionAttrPool::nSessionAttrPool() :
    refVarServer("/sys/servers/variable")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nSessionAttrPool::~nSessionAttrPool()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionAttrPool::SetAttr(const char* name, const char* val)
{
    n_assert(name && val);

    // get a handle for the variable name
    nVariable::Handle varHandle = this->refVarServer->GetVariableHandleByName(name);
   
    // update existing variable or create new one
    nVariable* var = this->attrs.GetVariable(varHandle);
    if (var)
    {
        var->SetString(val);
    }
    else
    {
        nVariable newVar(varHandle, val);
        this->attrs.AddVariable(newVar);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSessionAttrPool::GetAttr(const char* name)
{
    n_assert(name);
    nVariable::Handle varHandle = this->refVarServer->FindVariableHandleByName(name);
    if (varHandle != nVariable::InvalidHandle)
    {
        nVariable* var = this->attrs.GetVariable(varHandle);
        if (var)
        {
            return var->GetString();
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSessionAttrPool::GetNumAttrs() const
{
    return this->attrs.GetNumVariables();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSessionAttrPool::GetAttrAt(int index, const char*& name, const char*& val)
{
    nVariable::Handle varHandle = this->attrs.GetVariableAt(index).GetHandle();
    name = this->refVarServer->GetVariableName(varHandle);
    val = this->attrs.GetVariableAt(index).GetString();
}

//------------------------------------------------------------------------------
#endif    

