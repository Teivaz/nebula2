/**
   @file netclientdemo.h
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Declaration of NetClientDemo class.
   @brief $Id$ 

   This file is licensed under the terms of the Nebula License.
*/
#ifndef N_NETCLIENTDEMO_H
#define N_NETCLIENTDEMO_H
//------------------------------------------------------------------------------
#include "network/nnetclient.h"
//------------------------------------------------------------------------------
/**
    @class NetClientDemo
    @ingroup NetworkDemoContribModule

    @brief A client in a multiplayer session. Specific Subclass for the demo.
*/
class NetClientDemo : public nNetClient
{
public:
    void SendMessage(const char* msg) const;
protected:
    /// handle a custom message
    void HandleMessage(const char* msg);
};
#endif//N_NETCLIENTDEMO_H
//------------------------------------------------------------------------------
//   EOF 
//------------------------------------------------------------------------------
