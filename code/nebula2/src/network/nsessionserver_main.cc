//------------------------------------------------------------------------------
//  nsessionserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsessionserver.h"
#include "network/nsessionclientcontext.h"
#include "network/nnetserver.h"
#include "network/nnetclient.h"

nNebulaScriptClass(nSessionServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nSessionServer::nSessionServer() :
    refNetServer("/sys/servers/netserver"),
    refNetClient("/sys/servers/netclient"),
    maxNumClients(0),
    isOpen(false),
    ipcServer(0),
    ipcBroadcaster(0),
    time(0.0),
    broadcastTimeStamp(0.0),
    serverAttrsDirty(true),
    uniqueNumber(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSessionServer::~nSessionServer()
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
nSessionServer::SetServerAttr(const char* name, const char* val)
{
    this->serverAttrs.SetAttr(name, val);
    this->serverAttrsDirty = true;
}

//------------------------------------------------------------------------------
/**
    Get a server attribute by name. Returns 0 if server attribute doesn't 
    exist.

    @param  name    an attribute name
    @return         value string, or 0
*/
const char*
nSessionServer::GetServerAttr(const char* name)
{
    return this->serverAttrs.GetAttr(name);
}

//------------------------------------------------------------------------------
/**
    Get number of server attributes.

    @return     current number of server attributes
*/
int
nSessionServer::GetNumServerAttrs() const
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
nSessionServer::GetServerAttrAt(int index, const char*& name, const char*& value)
{
    this->serverAttrs.GetAttrAt(index, name, value);
}

//------------------------------------------------------------------------------
/**
    Open the session server. This makes the session visible in the network.
    After opening the session, the server must be triggered frequently
    to process messages (i.e. once per frame). An open server will
    generally answer any requests from clients and potential clients.

    See the protocol spec in nsessionserver.h!
*/
bool
nSessionServer::Open()
{
    n_assert(!this->isOpen);
    n_assert(this->GetAppName());
    n_assert(this->GetAppVersion());
    n_assert(this->GetMaxNumClients() > 0);
    n_assert(0 == this->ipcServer);
    n_assert(0 == this->ipcBroadcaster);
    n_assert(!this->refClientContexts.isvalid());

    // create location for client context objects
    this->refClientContexts = kernelServer->New("nroot", "/sys/share/session/clients");

    // initialize the various addresses
    nString portName(this->GetAppName());
    portName.Append("Session");
    this->sessionIpcAddress.SetHostName("self");
    this->sessionIpcAddress.SetPortName(portName.Get());
    this->sessionIpcAddress.ValidateAddrStruct();

    portName.Set(this->GetAppName());
    portName.Append("Broadcast");
    this->broadcastIpcAddress.SetHostName("broadcast");
    this->broadcastIpcAddress.SetPortName(portName.Get());
    this->broadcastIpcAddress.ValidateAddrStruct();

    // initialize nIpcServer object
    nIpcAddress ipcServerAddress("any", this->sessionIpcAddress.GetPortName());
    this->ipcServer = n_new(nIpcServer(ipcServerAddress));

    // initialize session info broadcaster
    this->ipcBroadcaster = n_new(nIpcPeer(0, (nIpcPeer::CanBroadcast | nIpcPeer::CanSend)));

    // create a unique identifier for this session
    this->sessionGuid.Generate();

    // initialize server attributes
    this->UpdateNumPlayersAttr();
    this->UpdateServerClientAttrs();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the session server. This removes the session from the network.
*/
void
nSessionServer::Close()
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    // release client context objects
    this->refClientContexts->Release();
    n_assert(!this->refClientContexts.isvalid());

    // kill the broadcaster object
    n_delete(this->ipcBroadcaster);
    this->ipcBroadcaster = 0;

    // send a proper close message to all connected clients before we disappear
    this->ipcServer->SendAll(nIpcBuffer("~closesession"));

    // kill ipc server object
    n_delete(this->ipcServer);
    this->ipcServer = 0;

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Process pending messages. This must be called frequently (i.e.
    once per frame) while the session server is open.
*/
void
nSessionServer::Trigger()
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    // poll ipc server and get pending messages
    if (this->ipcServer->Poll())
    {
        nIpcBuffer recvMsg(4096);
        int fromClientId;
        while (this->ipcServer->GetMsg(recvMsg, fromClientId))
        {
            // one received message could contain several strings
            const char* curMsg = recvMsg.GetFirstString();
            if (curMsg) do
            {
                n_printf("nSessionServer: msg on server channel: %s\n", curMsg);

                nString tokenString = curMsg;
                const char* cmd = tokenString.GetFirstToken(" ");
                if (cmd)
                {
                    if (0 == strcmp(cmd, "~queryserverattrs"))
                    {
                        // send server attributes to client 
                        this->SendServerAttrs(fromClientId);
                    }
                    else if (0 == strcmp(cmd, "~joinsession"))
                    {
                        // a join session request
                        this->HandleJoinSessionRequest(fromClientId, tokenString.GetNextToken(" "));
                    }
                    else if (0 == strcmp(cmd, "~leavesession"))
                    {
                        // a leave session request
                        this->HandleLeaveSessionRequest(fromClientId, tokenString.GetNextToken(" "));
                    }
                    else if (0 == strcmp(cmd, "~clientattr"))
                    {
                        // a client attribute message
                        const char* clientGuid = tokenString.GetNextToken(" ");
                        const char* attrName = tokenString.GetNextToken(" ");
                        const char* attrValue = tokenString.GetNextToken("[]");
                        this->HandleClientAttribute(fromClientId, clientGuid, attrName, attrValue);
                    }
                }
            } 
            while ((curMsg = recvMsg.GetNextString()));
        }
    }

    // periodically send a broadcast with server data
    this->BroadcastIdentity();

    // kick clients above MaxNumClients
    while (this->GetNumClients() > this->GetMaxNumClients())
    {
        this->KickClient(this->GetNumClients() - 1);
    }

    // if server attributes dirty, send them to 
    // all connected clients
    if (this->serverAttrsDirty)
    {
        this->SendServerAttrs(AllClients);
        this->serverAttrsDirty = false;
    }
}

//------------------------------------------------------------------------------
/**
    Returns the number of joined clients.
*/
int
nSessionServer::GetNumClients() const
{
    int numClients = 0;
    if (this->refClientContexts.isvalid())
    {
        nRoot* curClient;
        for (curClient = this->refClientContexts->GetHead(); curClient; curClient = curClient->GetSucc())
        {
            numClients++;
        }
    }
    return numClients;
}

//------------------------------------------------------------------------------
/**
    Private helper method which returns the pointer to a client context
    object by its index. Fails hard if index is invalid.
*/
nSessionClientContext*
nSessionServer::GetClientAt(int index) const
{
    int curClientIndex = 0;
    nRoot* curClient;
    for (curClient = this->refClientContexts->GetHead(); curClient; curClient = curClient->GetSucc())
    {
        if (curClientIndex == index)
        {
            return (nSessionClientContext*) curClient;
        }
        curClientIndex++;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Kick a joined client from the session.
*/
bool
nSessionServer::KickClient(int clientIndex)
{
    nSessionClientContext* clientContext = this->GetClientAt(clientIndex);
    if (clientContext)
    {
        int ipcClientId = clientContext->GetIpcClientId();
        this->ipcServer->Send(ipcClientId, nIpcBuffer("~kick"));

        clientContext->Release();
        this->UpdateNumPlayersAttr();
        this->UpdateServerClientAttrs();

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Start the session. This will send the "~start" message
    to all joined clients, configure and open the
    game network server object.
*/
bool
nSessionServer::Start()
{
    // build the port name for the game server
    char gamePortName[1024];
    sprintf(gamePortName, "%sGame", this->GetAppName());

    // build the start message string
    char msgBuf[1024];
    sprintf(msgBuf, "~start %s %s", this->sessionIpcAddress.GetIpAddrString(), gamePortName);
    nIpcBuffer startMsg(msgBuf);

    // send start signal to all joined clients
    int numClients = this->GetNumClients();
    int clientIndex;
    for (clientIndex = 0; clientIndex < numClients; clientIndex++)
    {
        nSessionClientContext* clientContext = this->GetClientAt(clientIndex);
        this->ipcServer->Send(clientContext->GetIpcClientId(), startMsg);
    }

    // configure and open the game net server
    nNetServer* netServer = this->refNetServer.get();
    nNetClient* netClient = this->refNetClient.get();

    // configure the local client
    nGuid localClientGuid;
    localClientGuid.Generate();
    netClient->SetClientGuid(localClientGuid.Get());
    netClient->SetServerHostName("localhost");
    netClient->SetServerPortName(gamePortName);

    // configure the local server
    netServer->SetPortName(gamePortName);
    netServer->BeginClients(numClients + 1);
    
    // note: the first game client is always the local client
    netServer->SetClientGuid(0, localClientGuid.Get());
    netServer->SetClientPlayerName(0, this->GetServerAttr("PlayerName"));

    // configure the remote clients
    for (clientIndex = 0; clientIndex < numClients; clientIndex++)
    {
        nSessionClientContext* clientContext = this->GetClientAt(clientIndex);
        n_assert(clientContext);
        netServer->SetClientGuid(clientIndex + 1, clientContext->GetClientGuid());
        netServer->SetClientPlayerName(clientIndex + 1, clientContext->GetClientAttr("PlayerName"));
    }
    netServer->EndClients();

    // open the net server and the local client
    netServer->Open();
    netClient->Open();

    return true;
}

//------------------------------------------------------------------------------
/**
    This broadcasts a server info message into the LAN (once per second
    or so). Interested clients use this info to connect to potential hosts
    in order to receive more detailed infos about the session in the 
    form of server attributes.
*/
void
nSessionServer::BroadcastIdentity()
{
    n_assert(this->ipcBroadcaster);

    double timeSinceLastBroadcast = this->time - this->broadcastTimeStamp;
    if (timeSinceLastBroadcast > 1.0f)
    {
        this->broadcastTimeStamp = this->time;

        // build the broadcast message
        char msg[512];
        sprintf(msg, "~session %s %s %s %s %s", 
                this->sessionGuid.Get(),
                this->GetAppName(),
                this->GetAppVersion(),
                this->sessionIpcAddress.GetIpAddrString(),
                this->sessionIpcAddress.GetPortName());

        // send the broadcast message
        nIpcBuffer ipcBroadcastMsg(msg, strlen(msg) + 1);
        this->ipcBroadcaster->SendTo(this->broadcastIpcAddress, ipcBroadcastMsg);
    }
}

//------------------------------------------------------------------------------
/**
    Send server attributes to one or all connected clients. Either provide
    the client id, or the "AllClients" value as argument.
*/
void
nSessionServer::SendServerAttrs(int clientId)
{
    int numAttrs = this->serverAttrs.GetNumAttrs();
    int attrIndex;
    nIpcBuffer msg(1024);
    for (attrIndex = 0; attrIndex < numAttrs; attrIndex++)
    {
        const char* attrName;
        const char* attrValue;
        this->serverAttrs.GetAttrAt(attrIndex, attrName, attrValue);
        char buf[N_MAXPATH];
        snprintf(buf, sizeof(buf), "~serverattr %s [%s]", attrName, attrValue);
        msg.SetString(buf);
        if (AllClients == clientId)
        {
            n_printf("nSessionServer: Sending server attr '%s = %s' to all clients\n", attrName, attrValue);
            this->ipcServer->SendAll(msg);
        }
        else
        {
            n_printf("nSessionServer: Sending server attr '%s = %s' to client %d\n", attrName, attrValue, clientId);
            this->ipcServer->Send(clientId, msg);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This updates the MaxNumPlayers server attribute.
*/
void
nSessionServer::UpdateMaxNumPlayersAttr()
{
    char attrValue[64];
    sprintf(attrValue, "%d", this->GetMaxNumClients() + 1);
    this->SetServerAttr("MaxNumPlayers", attrValue);
}

//------------------------------------------------------------------------------
/**
    This updates the NumPlayers server attribute. Must be called when
    the number of joined clients changes.
*/
void
nSessionServer::UpdateNumPlayersAttr()
{
    char attrValue[64];
    sprintf(attrValue, "%d", this->GetNumClients() + 1);
    this->SetServerAttr("NumPlayers", attrValue);
}

//------------------------------------------------------------------------------
/**
    Handle a join session request. This checks first if the join can be 
    accepted (the only reason for not accepting is a full session). If the
    join is accepted, a new session client context will be created.
*/
void
nSessionServer::HandleJoinSessionRequest(int ipcClientId, const char* clientGuid)
{
    n_assert(clientGuid);
n_printf("nSessionServer::HandleJoinSessionRequest(%d, %s)\n", ipcClientId, clientGuid);

    if (this->GetNumClients() >= this->GetMaxNumClients())
    {
        // deny the request
        this->ipcServer->Send(ipcClientId, nIpcBuffer("~joindenied"));
    }
    else
    {
        // create a session client context
        char clientName[N_MAXNAMELEN];
        sprintf(clientName, "client%d", this->uniqueNumber++);
        kernelServer->PushCwd(this->refClientContexts.get());
        nSessionClientContext* clientContext = (nSessionClientContext*) kernelServer->New("nsessionclientcontext", clientName);
        kernelServer->PopCwd();

        // initialize it
        clientContext->SetIpcClientId(ipcClientId);
        clientContext->SetClientGuid(clientGuid);
        clientContext->SetKeepAliveTime(this->GetTime());

        // accept the request
        this->ipcServer->Send(ipcClientId, nIpcBuffer("~joinaccepted"));

        // update the num players server attribute
        this->UpdateNumPlayersAttr();
        this->UpdateServerClientAttrs();
    }
}

//------------------------------------------------------------------------------
/**
    Find a client context by the client guid.
*/
nSessionClientContext*
nSessionServer::FindClientContextByGuid(const char* guid)
{
    n_assert(guid);
    nSessionClientContext* cur;
    for (cur = (nSessionClientContext*) this->refClientContexts->GetHead();
         cur;
         cur = (nSessionClientContext*) cur->GetSucc())
    {
        if (strcmp(cur->GetClientGuid(), guid) == 0)
        {
            return cur;
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    Handle a leave session request. This removes the session identified by
    the provided client guid.
*/
void
nSessionServer::HandleLeaveSessionRequest(int ipcClientId, const char* clientGuid)
{
    n_assert(clientGuid);
n_printf("nSessionServer::HandleLeaveSessionRequest(%d, %s)\n", ipcClientId, clientGuid);

    nSessionClientContext* clientContext = this->FindClientContextByGuid(clientGuid);
    if (clientContext)
    {
        clientContext->Release();
        this->UpdateNumPlayersAttr();
        this->UpdateServerClientAttrs();
    }
}

//------------------------------------------------------------------------------
/**
    Handle a client attribute update.
*/
void
nSessionServer::HandleClientAttribute(int /*ipcClientId*/, const char* clientGuid, const char* attrName, const char* attrValue)
{
    n_assert(clientGuid && attrName && attrValue);
    nSessionClientContext* clientContext = this->FindClientContextByGuid(clientGuid);
    if (clientContext)
    {
        clientContext->SetClientAttr(attrName, attrValue);
    }

    // convert the client attributes into a server attribute
    this->UpdateServerClientAttrs();
}

//------------------------------------------------------------------------------
/**
    Convert client attributes into server attributes. Client attributes
    are redistributed to all clients as server attributes with a ClientX_
    prefix.
*/
void
nSessionServer::UpdateServerClientAttrs()
{
    // for each joined client...
    char serverAttrName[128];
    int clientIndex;
    int numClients = this->GetNumClients();
    for (clientIndex = 0; clientIndex < numClients; clientIndex++)
    {
        nSessionClientContext* clientContext = this->GetClientAt(clientIndex);
        n_assert(clientContext);

        // set the Status attribute
        sprintf(serverAttrName, "Client%d_Status", clientIndex);
        this->SetServerAttr(serverAttrName, "joined");

        // update the attributes for this client
        int numAttrs = clientContext->GetNumClientAttrs();
        int attrIndex;
        for (attrIndex = 0; attrIndex < numAttrs; attrIndex++)
        {
            const char* attrName;
            const char* attrValue;
            clientContext->GetClientAttrAt(attrIndex, attrName, attrValue);
            sprintf(serverAttrName, "Client%d_%s", clientIndex, attrName);
            this->SetServerAttr(serverAttrName, attrValue);
        }
    }

    // fill the remaining Client status attributes with the value "free"
    for (; clientIndex < this->GetMaxNumClients(); clientIndex++)
    {
        sprintf(serverAttrName, "Client%d_Status", clientIndex);
        this->SetServerAttr(serverAttrName, "open");
    }
}
