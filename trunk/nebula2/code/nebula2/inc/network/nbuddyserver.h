#ifndef N_BUDDYSERVER_H
#define N_BUDDYSERVER_H
//------------------------------------------------------------------------------
/**
    @class nBuddyServer
    @ingroup Network

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nguid.h"
#include "kernel/nipcserver.h"
#include "util/narray.h"
#include "network/nbuddycommandinterpreter.h"

//------------------------------------------------------------------------------
class nBuddyServer : public nRoot
{
public:
    /// client status enum
    enum ClientStatus
    {
        Invalid,
        Waiting,
        Connected,
        Timeout,
    };

    /// constructor
    nBuddyServer();
    /// destructor
    virtual ~nBuddyServer();
    /// get instance pointer
    static nBuddyServer* Instance();
    /// set the communication port name
    void SetPortNum(short port);
    /// get the communication port name
    short GetPortNum() const;
    /// open the network session
    bool Open();
    /// close the network session
    void Close();
    /// per-frame-trigger
    void Trigger();
    /// return true if server is open
    bool IsOpen() const;

    /// Sends a message to a client
    bool SendMessage(int& IpcClientID,nString& Message);

    //get lost connections
    nArray<int> GetLostConnections();

    //clear lost connections
    void ClearLostConnections();

protected:

    nString portName;
    short portNum;
    bool isOpen;
    nIpcServer* ipcServer;
    nBuddyCommandInterpreter CommandInterpreter;
private:

     static nBuddyServer* Singleton;
};


//------------------------------------------------------------------------------
/**
*/
inline
nBuddyServer*
nBuddyServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddyServer::SetPortNum(short port)
{
    this->portNum = port;
}

//------------------------------------------------------------------------------
/**
*/
inline
short
nBuddyServer::GetPortNum() const
{
    return this->portNum;
}

//------------------------------------------------------------------------------
/**
    Return true if net server is open.
*/
inline
bool
nBuddyServer::IsOpen() const
{
    return this->isOpen;
}


//------------------------------------------------------------------------------
/**
   gets lost connections
*/
inline
nArray<int>
nBuddyServer::GetLostConnections()
{
    return this->ipcServer->ClientsReseted;
}


//------------------------------------------------------------------------------
/**
   clears lost connection
*/
inline
void
nBuddyServer::ClearLostConnections()
{
    return this->ipcServer->ClientsReseted.Clear();
}

//------------------------------------------------------------------------------
#endif