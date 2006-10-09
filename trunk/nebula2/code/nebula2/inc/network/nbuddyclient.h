#ifndef N_BUDDYCLIENT_H
#define N_BUDDYCLIENT_H
//------------------------------------------------------------------------------
/**
    @class nBuddyClient
    @ingroup Network

    A client in a multiplayer session. Works together with nNetServer,
    should be subclassed for a specific application or protocol.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nguid.h"
#include "kernel/nipcaddress.h"
#include "kernel/nipcclient.h"
#include "network/nservermessageinterpreter.h"
#include "util/narray.h"
#include "network/ncommand.h"
#include "network/nlogin.h"
#include "network/ncreateuser.h"
#include "network/ngetbuddylist.h"
#include "network/naddbuddy.h"
#include "network/nupdate.h"
#include "network/nsendmessage.h"
#include "network/nsetprofileattr.h"
#include "network/ngetprofileattr.h"


//------------------------------------------------------------------------------
class nBuddyClient : public nRoot
{
public:

    /// client status enum
    enum ClientStatus
    {
        Invalid,
        Connecting,         // trying to establish server connection
        Connected,          // connected, waiting for authentification
        Timeout,            // connection timed out
    };

    enum BuddySystemOnlineStatus
    {
        none,
        Online,
        Offline,
        Playing,
    };

    /// constructor
    nBuddyClient();
    /// destructor
    virtual ~nBuddyClient();
    /// get instance pointer
    static nBuddyClient* Instance();
    /// set the client's guid
    void SetClientGuid(const char* guid);
    /// get the client's guid
    const char* GetClientGuid() const;
    /// set the server's host name
    void SetServerHostName(const char* name);
    /// get the server's host name
    const char* GetServerHostName() const;
    /// set the server's port
    void SetServerPortNum(short port);
    /// get the server's port
    short GetServerPortNum();
    /// set the current time
    void SetTime(double t);
    /// get the current time
    double GetTime() const;
    /// open the client
    bool Open();
    /// close the client
    void Close();
    /// return true if open
    bool IsOpen() const;
    /// per-frame-trigger
    void Trigger();
    /// get the client's connection status
    ClientStatus GetClientStatus() const;
    /// get number of retries
    int GetNumRetries() const;

    /// get the client's online status
    BuddySystemOnlineStatus GetOnlineStatus() const;
    /// set the client's online status
    void SetOnlineStatus(BuddySystemOnlineStatus status);


    /// creates an user
    void CreateUser(const char* user,const char* pass);
    void Login(const char* user,const char* pass);
    void SendMessage(const char* user,const char* message);
    void AddBuddy(const char* buddy);
    void GetBuddylist();

    void AddUpdate(nUpdate* update);
    void DeleteLastUpdates();

    nArray<nUpdate*>& GetUpdates();

    bool SendCommand(nCommand* command);

protected:

    /// set current client status
    void SetClientStatus(ClientStatus status);

    nGuid clientGuid;
    nIpcAddress ipcAddress;
    nIpcClient* ipcClient;
    bool isOpen;
    ClientStatus clientStatus;
    BuddySystemOnlineStatus onlineStatus;
    double time;
    double retryTime;
    int numRetries;

    nServerMessageInterpreter CommandInterpreter;

private:

    static nBuddyClient* Singleton;

    nArray<nUpdate*> Updates; // unrequested updates from server

    nLogin* login;
    nCreateUser* createuser;
    nAddBuddy* addbuddy;
    nGetBuddylist* buddylist;
    nSendMessage* sendmessage;
    nSetProfileAttr* setprofileattr;
    nGetProfileAttr* getprofileattr;

};

//------------------------------------------------------------------------------
/**
*/
inline
nArray<nUpdate*>&
nBuddyClient::GetUpdates()
{
    return this->Updates;
}

//------------------------------------------------------------------------------
/**
*/
inline
nBuddyClient*
nBuddyClient::Instance()
{
    n_assert(Singleton);
    return Singleton;
}



//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddyClient::SetClientGuid(const char* guid)
{
    n_assert(guid);
    this->clientGuid.Set(guid);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nBuddyClient::GetClientGuid() const
{
    return this->clientGuid.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddyClient::SetServerHostName(const char* name)
{
    this->ipcAddress.SetHostName(name);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nBuddyClient::GetServerHostName() const
{
    return this->ipcAddress.GetHostName();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddyClient::SetServerPortNum(short port)
{
    this->ipcAddress.SetPortNum(port);
}

//------------------------------------------------------------------------------
/**
*/
inline
short
nBuddyClient::GetServerPortNum()
{
    return this->ipcAddress.GetPortNum();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddyClient::SetTime(double t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
double
nBuddyClient::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddyClient::SetClientStatus(ClientStatus status)
{
    this->clientStatus = status;
}

//------------------------------------------------------------------------------
/**
*/
inline
nBuddyClient::ClientStatus
nBuddyClient::GetClientStatus() const
{
    return this->clientStatus;
}



//------------------------------------------------------------------------------
/**
*/
inline
nBuddyClient::BuddySystemOnlineStatus
nBuddyClient::GetOnlineStatus() const
{
    return this->onlineStatus;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBuddyClient::SetOnlineStatus(BuddySystemOnlineStatus status)
{
    this->onlineStatus = status;
}



//------------------------------------------------------------------------------
/**
*/
inline
int
nBuddyClient::GetNumRetries() const
{
    return this->numRetries;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBuddyClient::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
#endif