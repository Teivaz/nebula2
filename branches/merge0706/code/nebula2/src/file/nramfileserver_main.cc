#define N_IMPLEMENTS nRamFileServer
//------------------------------------------------------------------------------
//  nramfileserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "file/nramfileserver.h"
#include "file/nramfile.h"

nNebulaScriptClass(nRamFileServer, "nfileserver2");


//------------------------------------------------------------------------------
/**
*/
nRamFileServer::nRamFileServer()
{
}

//------------------------------------------------------------------------------
/**
*/
nRamFileServer::~nRamFileServer()
{
}

//------------------------------------------------------------------------------
/**
*/
nFile*
nRamFileServer::NewFileObject() const
{
    nRamFile* result = n_new(nRamFile);
    n_assert(result != 0);
    return result;
}

