/**
   @file netserverdemo.cc
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Definition of NetServerDemo class.
   @brief $Id$

   This file is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#include "networkdemo/netserverdemo.h"
#include "kernel/nkernelserver.h"

nNebulaClass( NetServerDemo, "network::nnetserver" );

//------------------------------------------------------------------------------
/**
    Handle a custom message. Print the message and resend to the others clients
*/
bool
NetServerDemo::HandleMessage(int fromClientId, const char* msg)
{
    n_assert(msg);
    n_printf("NetServerDemo:message(%s)from(%d)\n", msg, fromClientId);
    int i;
    char msgString[1024];
    sprintf(msgString, "~msgfrom %d %s", fromClientId, msg);
    for (i = 0; i < this->GetNumClients(); i++)
    {
        if ( i!=fromClientId && (Connected == this->clientArray[i].GetClientStatus()))
        {
            this->ipcServer->Send(i, nIpcBuffer(msgString));
        }
    }
    this->ipcServer->Send(fromClientId, nIpcBuffer("~msg received"));
    return true;
}
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
