/**
   @file netclientdemo.cc
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Declaration of NetworkDemoApp class.
   @brief $Id$

   This file is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#include "networkdemo/netclientdemo.h"
#include "kernel/nkernelserver.h"

nNebulaClass( NetClientDemo, "nnetclient" );

//------------------------------------------------------------------------------
/**
    Send a message to the server.
*/
void
NetClientDemo::SendMessage(const char* msg) const
{
    char msgString[1024];
    sprintf(msgString, "~msg %s", msg);
    this->ipcClient->Send(nIpcBuffer(msgString));
}
//------------------------------------------------------------------------------
/**
    Handle a custom message.
*/
void
NetClientDemo::HandleMessage(const char* msg)
{
    n_assert(msg);
    n_printf("NetClientDemo::HandleMessage(%s)\n", msg);
}
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
