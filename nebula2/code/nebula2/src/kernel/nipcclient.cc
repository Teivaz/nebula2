#define N_IMPLEMENTS nIpcClient
#define N_KERNEL
//------------------------------------------------------------------------------
//  nipcclient.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nipcclient.h"

//------------------------------------------------------------------------------
/**
*/
nIpcClient::nIpcClient() :
    serverPortNum(0),
    sock(0)
{
#   ifdef __WIN32__
    struct WSAData wsa_data;
    WSAStartup(0x101, &wsa_data);
#   endif

    memset(&(this->serverAddr), 0, sizeof(this->serverAddr));
}

//------------------------------------------------------------------------------
/**
*/
nIpcClient::~nIpcClient()
{
    if (this->sock) 
    {
        shutdown(this->sock, 2);
        closesocket(this->sock);
        this->sock = 0;
    }

#   ifdef __WIN32__
    WSACleanup();
#   endif
}

//------------------------------------------------------------------------------
/**
    Sets the address of the server. 
    See nIpcClient::Connect() for more information.
*/
bool 
nIpcClient::FillServerAddr(const char* name)
{
    n_assert(name);

    char nameBuf[128];
    char *hostname = NULL;
    char *portname = NULL;
    char *tmp;
    struct hostent *he;
    
    // split host and port name
    n_strncpy2(nameBuf, name, sizeof(nameBuf));
    if ((tmp = strchr(nameBuf, ':'))) 
    {
        *tmp = 0;
        hostname = nameBuf;
        portname = tmp + 1;
    } else {
        hostname = "localhost";
        portname = nameBuf;
    }

    this->serverHostName = hostname;
    this->serverPortName = portname;
    n_assert(hostname[0] != 0);
    n_assert(portname[0] != 0);

    he = gethostbyname(hostname);
    if (!he) {
        n_printf("nIpcClient(): unknown host!");
        return false;
    }
    this->serverPortNum = this->GetPortNumFromName(portname);
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port   = htons(this->serverPortNum);
#if defined(__WIN32__)
    this->serverAddr.sin_addr.S_un.S_addr = inet_addr(hostname);
#elif defined(__LINUX__)
    this->serverAddr.sin_addr.s_addr = inet_addr(hostname);
#endif
    this->serverAddr.sin_addr   = *((struct in_addr *)he->h_addr);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
short 
nIpcClient::GetPortNumFromName(const char *portName)
{
    n_assert(portName);
    short pnum = ((short)hash(portName, N_SOCKET_PORTRANGE)) + N_SOCKET_MIN_PORTNUM;
    return pnum;
} 

//------------------------------------------------------------------------------
/**
    Connects to the given server. The server name has the following structure:
  
    [hostname:]portname

    Hostname is the usual address or IP of the server, this can be left blank
    for the local machine. Portname is the string name of the port as set by the
    server.
*/
bool 
nIpcClient::Connect(const char* portName)
{
    n_assert(this->sock == 0);
    bool retval = false;
   
    // resolve portname
    if (this->FillServerAddr(portName)) 
    {
        int res;
        char msg_buf[128];
        bool connected = false;
        bool error     = false;
        int num_retries = 0;
        
        while ((!connected) && (!error) && (num_retries < 4)) 
        {    
            if (this->sock) 
            {
                shutdown(this->sock,2);
                closesocket(this->sock);
                this->sock = 0;
                num_retries++;
            }
            
            // intialise new socket
            this->sock = socket(AF_INET, SOCK_STREAM, 0);
            if (this->sock == INVALID_SOCKET) 
            {
                n_printf("nIpcClient::nIpcClient(): could not create socket!");
                return false;        
            }

            n_printf("nIpcClient: trying %s:%s, port %d...\n",
                      this->serverHostName.Get(), this->serverPortName.Get(),
                      this->serverPortNum);

            res = connect(this->sock, (struct sockaddr *) &(this->serverAddr),
                          sizeof(struct sockaddr));
            if (res != -1) 
            {
                // check for correct port
                sprintf(msg_buf, "~handshake %s", this->serverPortName.Get());
                res = send(this->sock, msg_buf, strlen(msg_buf)+1, 0);
            
                // wait for response
                if (res != -1) 
                {
                    n_printf("nIpcClient: sending handshake... ");
                    res = recv(this->sock, msg_buf, sizeof(msg_buf), 0);
                    if ((res==-1)||(res==0)) 
                    {
                        n_printf("nIpcClient::Connect(): ~handshake recv() failed!\n");
                        error = true;
                    } 
                    else 
                    {                    
                        // correct response
                        if (strcmp(msg_buf,"~true") == 0) 
                        {
                            connected = true;
                            n_printf("accepted.\n");
                        } 
                        else 
                        {
                            // wrong response, try next port
                            this->serverPortNum++;
                            this->serverAddr.sin_port = htons(this->serverPortNum);
                            n_printf("wrong portname.\n");
                        }
                    }
                } 
                else 
                {
                    n_printf("nIpcClient::Connect(): ~handshake send() failed!");
                    error = true;
                }
            } 
            else 
            {
                // no response, try next port
                this->serverPortNum++;
                this->serverAddr.sin_port = htons(this->serverPortNum);
            }
        }
        if (connected) 
        {
            n_printf("nIpcClient: connected.\n");
            retval = true;
        } 
        else 
        {
            n_printf("nIpcClient: Could not connect to a valid server!\n");
            shutdown(this->sock,2);
            closesocket(this->sock);
            this->sock = 0;
        }
    }
    return retval;
}                                                                  
        
//------------------------------------------------------------------------------
/**
    Disconnect from the current server.
*/
void 
nIpcClient::Disconnect()
{
    n_assert(this->sock);
    int res;
    char *cmd = "~close";
    
    // send ~close cmd
    res = send(this->sock, cmd, strlen(cmd)+1, 0);
    if (res != -1) 
    {
        // get response
        char buf[128];
        res = recv(this->sock, buf, sizeof(buf), 0);
        if ((res == -1) || (res == 0)) 
        {
            n_printf("nIpcClient::Disconnect(): recv(~close) failed!\n");    
        }
    } 
    else 
    {
        n_printf("nIpcClient::Disconnect(): send(~close) failed!\n");
    } 

    // kill the socket
    shutdown(this->sock,2);
    closesocket(this->sock);
    this->sock = 0;
}

//------------------------------------------------------------------------------
/**
    Sends a message to the current server.
    
    @param buf pointer to data to send
    @param size size of data
    
    @return a pointer to a new nMsgNode with the response
*/
nMsgNode*
nIpcClient::SendMsg(void* buf, int size)
{
    n_assert(buf);
    n_assert(size > 0);

    nMsgNode *nd = NULL;
    int res;
    res = send(this->sock, (char *) buf, size, 0);
    if (res == -1) 
    {
        n_printf("nIpcClient::SendMsg(): send() failed!");
    } 
    else 
    {
        // wait for an answer...
        res = recv(this->sock, this->receiveBuffer, RECEIVEBUFFERSIZE, 0);
        if ((res == -1) || (res == 0)) 
        {
            n_printf("nIpcClient::SendMsg(): recv() failed!\n");
        } 
        else if (res > 0) 
        {
            nd = new nMsgNode(this->receiveBuffer, res);
        }
    }
    return nd;
}

//------------------------------------------------------------------------------
/**
    Frees the reply nMsgNode. Should be called once you have finished processing a
    response from nIpcClient::SendMsg()
    
    @param nd pointer to the nMsgNode you want to free.
*/
void 
nIpcClient::FreeReplyMsgNode(nMsgNode *nd)
{
    delete nd;
}

//------------------------------------------------------------------------------
/**
    Get the current server's hostname.
    
    @return hostname
*/
const char*
nIpcClient::GetHostName() const
{
    return this->serverHostName.IsEmpty() ? 0 : this->serverHostName.Get();
}

//------------------------------------------------------------------------------
/**
    Get the current server's portname.
    
    @return portname
*/
const char*
nIpcClient::GetPortName() const
{
    return this->serverPortName.IsEmpty() ? 0 : this->serverPortName.Get();
}

