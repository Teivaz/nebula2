//------------------------------------------------------------------------------
//  nfilenode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfilenode.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

nNebulaScriptClass(nFileNode, "nroot");

//------------------------------------------------------------------------------
/**
*/
nFileNode::nFileNode() :
    file(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nFileNode::~nFileNode()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileNode::Open(const char* filename, const char* accessMode)
{
    n_assert(!this->file);
    n_assert(filename && accessMode);
    this->file = kernelServer->GetFileServer()->NewFileObject();
    n_assert(this->file);
    if (!this->file->Open(filename, accessMode))
    {
        this->file->Release();
        this->file = 0;
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nFileNode::Close()
{
    n_assert(this->file);
    this->file->Close();
    this->file->Release();
    this->file = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileNode::IsOpen() const
{
    return (0 != this->file);
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileNode::PutS(const char* buffer)
{
    n_assert(this->file);
    n_assert(buffer);
    return this->file->PutS(buffer);
}



