//------------------------------------------------------------------------------
//  nsessionclient_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsessionclient.h"
#include "network/nsessionservercontext.h"

nNebulaScriptClass(nSessionClient, "nroot");

//------------------------------------------------------------------------------
/**
*/
nSessionClient::nSessionClient() :
    time(0.0),
    isOpen(false),
    isJoined(false),
    ipcSessionSniffer(0),
    uniqueNumber(0),
    clientAttrsDirty(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSessionClient::~nSessionClient()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Set a client attribute, a new attribute will be created if it does
    not exist yet. Client attributes are distributed to the connected server,
    which in turn distributes them to all other joined clients.

    @param  name        an attribute name
    @param  val         the attribute value as string
*/
void
nSessionClient::SetClientAttr(const char* name, const char* val)
{
    this->clientAttrs.SetAttr(name, val);
    this->clientAttrsDirty = true;
}

//------------------------------------------------------------------------------
/**
    Get a client attribute by name. Returns 0 if client attribute doesn't
    exist.

    @param  name    an attribute name
    @return         value string, or 0
*/
const char*
nSessionClient::GetClientAttr(const char* name)
{
    return this->clientAttrs.GetAttr(name);
}

//------------------------------------------------------------------------------
/**
    Get number of client attributes.

    @return     current number of client attributes
*/
int
nSessionClient::GetNumClientAttrs() const
{
    return this->clientAttrs.GetNumAttrs();
}

//------------------------------------------------------------------------------
/**
    Get value of client attribute at index.

    @param  index   [in] attribute index (0 .. numServerAttrs)
    @param  name    [out] attribute name
    @param  value   [out] attribute value
*/
void
nSessionClient::GetClientAttrAt(int index, const char*& name, const char*& value)
{
    this->clientAttrs.GetAttrAt(index, name, value);
}

//------------------------------------------------------------------------------
/**
    Open the client. This will start discovering servers on the net,
    allow to connect the client to a server and exchange attributes with
    the server.
*/
bool
nSessionClient::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->isJoined);
    n_assert(this->GetAppName());
    n_assert(this->GetAppVersion());
    n_assert(0 == this->ipcSessionSniffer);
    n_assert(!this->refServerContexts.isvalid());
    n_assert(!this->refJoinServer.isvalid());

    // reset the unique number (used for naming server contexts)
    this->uniqueNumber = 0;

    // create location for session context objects
    this->refServerContexts = kernelServer->New("nroot", "/sys/share/session/servers");

    // initialize the various address
    nString portName(this->GetAppName());
    portName.Append("Broadcast");
    this->snifferIpcAddress.SetHostName("any");
    this->snifferIpcAddress.SetPortName(portName.Get());
    this->snifferIpcAddress.ValidateAddrStruct();

    // initialize the session sniffer which listens for server broadcasts
    this->ipcSessionSniffer = n_new(nIpcPeer(&(this->snifferIpcAddress), nIpcPeer::CanReceive));

    // initialize a new client guid
    this->clientGuid.Generate();

    this->isOpen = true;
    this->isJoined = false;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the client, this will stop discovering servers on the net.
*/
void
nSessionClient::Close()
{
    n_assert(this->isOpen);
    n_assert(this->ipcSessionSniffer);

    // currently joined to a session?
    if (this->IsJoined())
    {
        this->LeaveSession();
    }

    // release server context objects
    this->refServerContexts->Release();
    n_assert(!this->refServerContexts.isvalid());

    // release the session sniffer
    n_delete(this->ipcSessionSniffer);
    this->ipcSessionSniffer = 0;

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Process pending messages. This must be called frequently (i.e.
    once per frame) while the session client is open.
*/
void
nSessionClient::Trigger()
{
    n_assert(this->isOpen);

    // let the sniffer discover new sessions
    this->DiscoverSessions();

    // remove session which have expired (which have not been updated
    // for some keepalive time).
    this->CleanupExpiredSessions();

    // trigger the server contexts
    this->TriggerServerContexts();

    // send client attributes?
    if (this->IsJoined())
    {
        if (this->clientAttrsDirty)
        {
            this->SendClientAttrs();
            this->clientAttrsDirty = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Send all client attributes to the joined server. This should only be called when
    the client attributes have actually changed!
*/
void
nSessionClient::SendClientAttrs()
{
    n_assert(this->IsJoined());
    n_assert(this->clientAttrsDirty);

    if (this->IsJoined())
    {
        int numAttrs = this->clientAttrs.GetNumAttrs();
        int attrIndex;
        for (attrIndex = 0; attrIndex < numAttrs; attrIndex++)
        {
            const char* attrName;
            const char* attrValue;
            this->clientAttrs.GetAttrAt(attrIndex, attrName, attrValue);
            char buf[N_MAXPATH];
            snprintf(buf, sizeof(buf), "~clientattr %s %s [%s]", this->clientGuid.Get(), attrName, attrValue);
            this->refJoinServer->Send(buf);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This checks for incoming messages on the sniffer channel, where
    all open session servers broadcast their existence frequently. Once a
    new session is found, a new server context will be created which will
    track the server state on the client side.
*/
void
nSessionClient::DiscoverSessions()
{
    n_assert(this->ipcSessionSniffer);

    nIpcBuffer recvMsg(4096);
    nIpcAddress fromAddress;
    while (this->ipcSessionSniffer->ReceiveFrom(recvMsg, fromAddress))
    {
        // extract the session data from the string, check if it is a valid
        // broadcast message, and extract the message data
        const char* curMsg = recvMsg.GetFirstString();
        if (curMsg) do
        {
            nString tokenString = curMsg;
            const char* cmd = tokenString.GetFirstToken(" ");
            if (strcmp(cmd, "~session") == 0)
            {
                const char* guid       = tokenString.GetNextToken(" ");
                const char* appName    = tokenString.GetNextToken(" ");
                const char* appVersion = tokenString.GetNextToken(" ");
                const char* hostName   = tokenString.GetNextToken(" ");
                const char* portName   = tokenString.GetNextToken(" ");

                if (!(guid && appName && appVersion && hostName && portName))
                {
                    // hmm, a broken message, do not fail hard, just continue
                    n_printf("nSessionClient Warning: broken message '%s'\n", curMsg);
                    continue;
                }

                // check if a session of that name has already been registered,
                // if yes, this simply updates the KeepAliveTimestamp
                nSessionServerContext* serverContext = this->FindServerContextByGuid(guid);
                if (serverContext)
                {
                    serverContext->SetKeepAliveTime(this->GetTime());
                    continue;
                }

                // check app name and app version
                if (!(strcmp(appName, this->GetAppName()) == 0) &&
                    (strcmp(appVersion, this->GetAppVersion()) == 0))
                {
                    // app name or version don't match
                    continue;
                }

                // a new valid session
                this->RegisterSession(guid, hostName, portName);
            }
        }
        while ((curMsg = recvMsg.GetNextString()));
    }
}

//------------------------------------------------------------------------------
/**
    See if a server context for the given session guid exists.
*/
nSessionServerContext*
nSessionClient::FindServerContextByGuid(const char* guid)
{
    n_assert(guid);

    nSessionServerContext* cur;
    for (cur = (nSessionServerContext*) this->refServerContexts->GetHead();
         cur;
         cur = (nSessionServerContext*) cur->GetSucc())
    {
        if (strcmp(cur->GetSessionGuid(), guid) == 0)
        {
            return cur;
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    Checks session contexts for dead sessions due to timeout. Normally,
    session servers will correctly close the connection as part of the
    network protocol, if this fails however, a timeout will make sure
    that dead session contexts don't pile up.
*/
void
nSessionClient::CleanupExpiredSessions()
{
    const float sessionTimeOut  = 5.0f;
    nSessionServerContext* cur  = (nSessionServerContext*) this->refServerContexts->GetHead();
    nSessionServerContext* next = 0;
    if (cur) do
    {
        next = (nSessionServerContext*) cur->GetSucc();
        double age = this->GetTime() - cur->GetKeepAliveTime();
        if (age > sessionTimeOut)
        {
            n_printf("nSessionClient: session timeout from '%s' on port '%s'\n",
                     cur->GetHostName(), cur->GetPortName());
            cur->Release();
        }
    } while ((cur = next));
}

//------------------------------------------------------------------------------
/**
    This registers a new session. A server context object is created,
    and a reliable connection is established to the session server.
*/
bool
nSessionClient::RegisterSession(const char* guidString, const char* hostName, const char* portName)
{
    n_assert(guidString && hostName && portName);

    n_printf("nSessionClient: new session from '%s' on port '%s'\n", hostName, portName);

    // create a session server object
    char serverName[N_MAXNAMELEN];
    sprintf(serverName, "server%d", this->uniqueNumber++);
    kernelServer->PushCwd(this->refServerContexts.get());
    nSessionServerContext* serverContext = (nSessionServerContext*) kernelServer->New("nsessionservercontext", serverName);
    kernelServer->PopCwd();

    // initialize it
    serverContext->SetSessionGuid(guidString);
    serverContext->SetClientGuid(this->clientGuid);
    serverContext->SetHostName(hostName);
    serverContext->SetPortName(portName);
    serverContext->SetKeepAliveTime(this->GetTime());

    // and establish the connection
    if (!serverContext->Open())
    {
        // oops, connection to server failed, release the server context
        serverContext->Release();
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Triggers all existing server contexts. Removes server contexts where
    the connection has been closed.
*/
void
nSessionClient::TriggerServerContexts()
{
    nSessionServerContext* cur  = (nSessionServerContext*) this->refServerContexts->GetHead();
    nSessionServerContext* next = 0;
    if (cur) do
    {
        next = (nSessionServerContext*) cur->GetSucc();
        if (!cur->Trigger())
        {
            // the server connection should be closed, and the server context released
            cur->Close();
            cur->Release();
        }
    }
    while ((cur = next));
}

//------------------------------------------------------------------------------
/**
    Get number of discovered servers.
*/
int
nSessionClient::GetNumServers() const
{
    int numServers = 0;
    if (this->refServerContexts.isvalid())
    {
        nSessionServerContext* context;
        for (context = (nSessionServerContext*) this->refServerContexts->GetHead();
            context;
            context = (nSessionServerContext*) context->GetSucc())
        {
            numServers++;
        }
    }
    return numServers;
}

//------------------------------------------------------------------------------
/**
    Get pointer to server context at index.
*/
nSessionServerContext*
nSessionClient::GetServerAt(int index) const
{
    int curIndex = 0;
    nSessionServerContext* context;
    for (context = (nSessionServerContext*) this->refServerContexts->GetHead();
         context;
         context = (nSessionServerContext*) context->GetSucc())
    {
        if (index == curIndex)
        {
            return context;
        }
        curIndex++;
    }
    // fallthrough: invalid index
    return 0;
}

//------------------------------------------------------------------------------
/**
    Get pointer to joined server, or 0 if not joined.
*/
nSessionServerContext*
nSessionClient::GetJoinedServer() const
{
    if (this->IsJoined())
    {
        return this->refJoinServer.get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Join a session. This will simply send a join request to the
    server defined by the server index and return immediately.
*/
bool
nSessionClient::JoinSession(const char* sessionGuid)
{
    // currently joined?
    if (this->IsJoined())
    {
        this->LeaveSession();
    }

    // dirtyfy client attributes
    this->clientAttrsDirty = true;

    // join to new session
    this->refJoinServer = this->FindServerContextByGuid(sessionGuid);
    if (this->refJoinServer.isvalid())
    {
        n_assert(!this->refJoinServer->IsJoined());
        return this->refJoinServer->JoinSession();
    }
    else
    {
        // invalid server index
        n_printf("nSessionClient::JoinSession(): join failed, invalid server index!\n");
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Leave the currently joined session.
*/
bool
nSessionClient::LeaveSession()
{
    bool retval = false;
    if (this->refJoinServer.isvalid())
    {
        if (this->refJoinServer->IsJoined())
        {
            retval = this->refJoinServer->LeaveSession();
        }
        this->refJoinServer.invalidate();
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Return true if currently joined to a session.
*/
bool
nSessionClient::IsJoined() const
{
    if (this->refJoinServer.isvalid())
    {
        return this->refJoinServer->IsJoined();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Return status of join accepted flag.
*/
bool
nSessionClient::IsJoinAccepted() const
{
    if (this->refJoinServer.isvalid())
    {
        return this->refJoinServer->IsJoinAccepted();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Return status of join denied flag.
*/
bool
nSessionClient::IsJoinDenied() const
{
    if (this->refJoinServer.isvalid())
    {
        return this->refJoinServer->IsJoinDenied();
    }
    return false;
}
