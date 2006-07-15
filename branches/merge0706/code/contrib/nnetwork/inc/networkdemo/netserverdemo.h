/**
   @file netserverdemo.h
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Declaration of NetServerDemo class.
   @brief $Id$ 

   This file is licensed under the terms of the Nebula License.
*/
#ifndef N_NETSERVERDEMO_H
#define N_NETSERVERDEMO_H
//------------------------------------------------------------------------------
#include "network/nnetserver.h"
//------------------------------------------------------------------------------
/**
    @class NetServerDemo
    @ingroup NetworkDemoContribModule

    @brief A server in a multiplayer session. Specific Subclass for the demo.
*/
class NetServerDemo : public nNetServer
{
public:
protected:
    /// handle a custom message
    bool HandleMessage(int fromClientId, const char* msg);
};
#endif//N_NETSERVERDEMO_H
//------------------------------------------------------------------------------
//   EOF 
//------------------------------------------------------------------------------
