//------------------------------------------------------------------------------
//  nsessionclientcontext_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsessionclientcontext.h"

nNebulaScriptClass(nSessionClientContext, "kernel::nroot");

//------------------------------------------------------------------------------
/**
*/
nSessionClientContext::nSessionClientContext() :
    ipcClientId(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSessionClientContext::~nSessionClientContext()
{
    // empty
}

