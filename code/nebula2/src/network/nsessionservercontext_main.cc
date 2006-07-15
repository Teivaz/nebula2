//------------------------------------------------------------------------------
//  nsessionservercontext_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsessionservercontext.h"
#include "network/nnetclient.h"

nNebulaScriptClass(nSessionServerContext, "nroot");

//------------------------------------------------------------------------------
/**
*/
nSessionServerContext::nSessionServerContext() :
    refNetClient("/sys/servers/netclient"),
    ipcClient(0),
    isOpen(false),
    isJoined(false),
    isJoinAccepted(false),
    isJoinDenied(false),
    keepAliveTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSessionServerContext::~nSessionServerContext()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Set a server attribute, a new attribute will be created if it does
    not exist yet. Server attributes are distributed to all connected clients.

    @param  name        an attribute name
    @param  val         the attribute value as string
*/
void
nSessionServerContext::SetServerAttr(const char* name, const char* val)
{
    this->serverAttrs.SetAttr(name, val);
}

//------------------------------------------------------------------------------
/**
    Get a server attribute by name. Returns 0 if server attribute doesn't 
    exist.

    @param  name    an attribute name
    @return         value string, or 0
*/
const char*
nSessionServerContext::GetServerAttr(const char* name)
{
    return this->serverAttrs.GetAttr(name);
}

//------------------------------------------------------------------------------
/**
    Get number of server attributes.

    @return     current number of server attributes
*/
int
nSessionServerContext::GetNumServerAttrs() const
{
    return this->serverAttrs.GetNumAttrs();
}

//------------------------------------------------------------------------------
/**
    Get value of server attribute at index.

    @param  index   [in] a server attribute index (0 .. numServerAttrs)
    @param  name    [out] attribute name
    @param  value   [out] attribute value
*/
void
nSessionServerContext::GetServerAttrAt(int index, const char*& name, const char*& value)
{
    this->serverAttrs.GetAttrAt(index, name, value);
}

//------------------------------------------------------------------------------
/**
    Open a reliable connection to the server.
*/
bool 
nSessionServerContext::Open()
{
    n_assert(!this->isOpen);
    n_assert(0 == this->ipcClient);

    // reset status flags
    this->isJoined = false;
    this->isJoinAccepted = false;
    this->isJoinDenied = false;

    // create an ipc client object and establish the connection
    this->ipcClient = n_new(nIpcClient);
    this->ipcClient->SetBlocking(false);
    if (this->ipcClient->Connect(this->serverIpcAddress))
    {
        // success...
        n_printf("nSessionServerContext: connected to server %s\n", this->serverIpcAddress.GetHostName());
        this->isOpen = true;

        // query server for a current set of server attributes
        this->ipcClient->Send(nIpcBuffer("~queryserverattrs"));
    }
    else
    {
        // failure
        n_printf("Failed to connect to server %s\n", this->serverIpcAddress.GetHostName());
        n_delete(this->ipcClient);
        this->ipcClient = 0;
    }

    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Close the connection to the server.
*/
void
nSessionServerContext::Close()
{
    n_assert(this->isOpen);

    // leave session if currently joined
    if (this->isJoined)
    {
        this->LeaveSession();
    }

    // disconnect communication channel
    if (this->ipcClient->IsConnected())
    {
        this->ipcClient->Disconnect();
    }
    n_delete(this->ipcClient);
    this->ipcClient = 0;

    // reset status flags
    this->isOpen = false;
    this->isJoined = false;
    this->isJoinAccepted = false;
    this->isJoinDenied = false;
}

//------------------------------------------------------------------------------
/**
    Return true if currently open.
*/
bool
nSessionServerContext::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Return true if currently joined to session. This will not immediately be
    the case after the JoinSession() method returns, but at some later
    time, when a "~joinaccepted" message is received from the server.
*/
bool
nSessionServerContext::IsJoined() const
{
    return this->isJoined;
}

//------------------------------------------------------------------------------
/**
    Return state of the join accepted flag.
*/
bool
nSessionServerContext::IsJoinDenied() const
{
    return this->isJoinDenied;
}

//------------------------------------------------------------------------------
/**
    Return state of the join accepted flag.
*/
bool
nSessionServerContext::IsJoinAccepted() const
{
    return this->isJoinAccepted;
}

//------------------------------------------------------------------------------
/**
    Per-frame-trigger method. The method returns false if the server
    connection should be closed.
*/
bool
nSessionServerContext::Trigger()
{
    n_assert(this->isOpen);
    n_assert(this->ipcClient);

    bool closeSessionReceived = false;

    // check if any messages arrived on my communications channel...
    nIpcBuffer recvMsg(4096);
    while (this->ipcClient->Receive(recvMsg))
    {
        // there may be multiple strings in one received msg buffer
        const char* curMsg = recvMsg.GetFirstString();
        if (curMsg) do
        {
            n_printf("Message from server %s: %s\n", this->serverIpcAddress.GetHostName(), curMsg);

            nString tokenString = curMsg;
            const char* cmd = tokenString.GetFirstToken(" ");
            if (cmd)
            {
                if (0 == strcmp(cmd, "~serverattr"))
                {
                    // a server attribute update
                    const char* attrName = tokenString.GetNextToken(" ");
                    const char* attrValue = tokenString.GetNextToken("[]");
                    if (attrName && attrValue)
                    {
                        this->SetServerAttr(attrName, attrValue);
                    }
                }
                else if (0 == strcmp(cmd, "~closesession"))
                {
                    // the server is about to close the session
                    closeSessionReceived = true;
                }
                else if (0 == strcmp(cmd, "~joinaccepted"))
                {
                    // our join request has been accepted!
                    this->isJoinAccepted = true;
                    this->isJoinDenied = false;
                    this->isJoined = true;
                }
                else if (0 == strcmp(cmd, "~joindenied"))
                {
                    // our join request has been denied!
                    this->isJoinAccepted = false;
                    this->isJoinDenied = true;
                    this->isJoined = false;
                }
                else if (0 == strcmp(cmd, "~kick"))
                {
                    // the server kicks us from the session
                    this->isJoined = false;
                    this->isJoinAccepted = false;
                    this->isJoinDenied = false;
                }
                else if (0 == strcmp(cmd, "~start"))
                {
                    const char* gameServerHostName = tokenString.GetNextToken(" ");
                    const char* gameServerPortName = tokenString.GetNextToken(" ");
                    this->HandleStartMessage(gameServerHostName, gameServerPortName);
                    
                    // a start session also causes the session to be closed
                    closeSessionReceived = true;
                }
            }
        }
        while ((curMsg = recvMsg.GetNextString()));
    }
    return (!closeSessionReceived);
}

//------------------------------------------------------------------------------
/**
    Join a session. This will send a join request to the server and
    immediately return. The object will go into join mode as soon
    as a positive reply by the server has arrived.
*/
bool
nSessionServerContext::JoinSession()
{
    n_assert(this->isOpen);
    n_assert(!this->isJoined);

    // NOTE: do not set the isJoined flag to true here, this happens
    // only when a "joinaccepted" message is received during trigger

    this->isJoinAccepted = false;
    this->isJoinDenied = false;

    // send a join request to the server
    char msg[N_MAXPATH];
    snprintf(msg, sizeof(msg), "~joinsession %s", this->clientGuid.Get());
    return this->ipcClient->Send(nIpcBuffer(msg));
}

//------------------------------------------------------------------------------
/**
    Leave a joined session. This will send a leave session request to the
    server, this client object will immediately leave the session, no
    reply from the server is required.
*/
bool
nSessionServerContext::LeaveSession()
{
    n_assert(this->isOpen);
    if (this->isJoined)
    {
        this->isJoined = false;
        this->isJoinAccepted = false;
        this->isJoinDenied = false;

        char msg[N_MAXPATH];
        snprintf(msg, sizeof(msg), "~leavesession %s", this->clientGuid.Get());
        return this->ipcClient->Send(nIpcBuffer(msg));
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Send a text message to the server on the other end.
*/
bool
nSessionServerContext::Send(const char* msg)
{
    if (this->isJoined)
    {
        n_assert(this->ipcClient);
        return this->ipcClient->Send(nIpcBuffer(msg));
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Handle a start message from the session server. This will configure
    the net client object and open it. The job of the session client
    is finished, and it will gracefully exit.
*/
void
nSessionServerContext::HandleStartMessage(const char* gameServerHostName, const char* gameServerPortName)
{
    n_assert(gameServerHostName);
    n_assert(gameServerPortName);

    nNetClient* netClient = this->refNetClient.get();

    // configure the net client object
    netClient->SetClientGuid(this->GetClientGuid().Get());
    netClient->SetServerHostName(gameServerHostName);
    netClient->SetServerPortName(gameServerPortName);
    netClient->Open();
}
