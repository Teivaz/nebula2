//------------------------------------------------------------------------------
//  nipcclient.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nipcclient.h"

//------------------------------------------------------------------------------
/**
*/
nIpcClient::nIpcClient() :
    sock(INVALID_SOCKET),
    isConnected(false),
    blocking(true)
{
#if __WIN32__
    struct WSAData wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}

//------------------------------------------------------------------------------
/**
*/
nIpcClient::~nIpcClient()
{
    if (this->isConnected)
    {
        this->Disconnect();
    }
#if __WIN32__
    WSACleanup();
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nIpcClient::DestroySocket()
{
    if (this->sock != INVALID_SOCKET)
    {
        shutdown(this->sock, 2);
        closesocket(this->sock);
        this->sock = INVALID_SOCKET;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nIpcClient::ApplyBlocking(bool b)
{
    if (b)
    {
        // enable blocking
        #ifdef __WIN32__
            u_long falseAsUlong = 0;
            int res = ioctlsocket(this->sock, FIONBIO, &falseAsUlong);
        #elif defined(__LINUX__) || defined(__MACOSX__)
            int flags;
            flags = fcntl(this->sock, F_GETFL);
            flags &= ~O_NONBLOCK;
            int res = fcntl(this->sock, F_SETFL, flags);
        #endif
        n_assert(0 == res);
    }
    else
    {
        // disable blocking
        #ifdef __WIN32__
            u_long trueAsUlong = 1;
            int res = ioctlsocket(this->sock, FIONBIO, &trueAsUlong);
        #elif defined(__LINUX__) || defined(__MACOSX__)
            int flags;
            flags = fcntl(this->sock, F_GETFL);
            flags |= O_NONBLOCK;
            int res = fcntl(this->sock, F_SETFL, flags);
        #endif
        n_assert(0 == res);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nIpcClient::SetBlocking(bool b)
{
    this->blocking = b;
    if (this->isConnected)
    {
        this->ApplyBlocking(b);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nIpcClient::GetBlocking() const
{
    return this->blocking;
}

//------------------------------------------------------------------------------
/**
    Establish a connection to the ipc server identified by the nIpcAddress
    object.
*/
bool
nIpcClient::Connect(nIpcAddress& addr)
{
    if (!this->isConnected)
    {
        this->serverAddr = addr;

        // create socket (in blocking mode)
        this->sock = socket(AF_INET, SOCK_STREAM, 0);
        n_assert(this->sock != INVALID_SOCKET);

        // configure the socket
        int trueAsInt = 1;
        int res = setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&trueAsInt, sizeof(trueAsInt));
        n_assert(res != -1);

        // try connection
        n_printf("nIpcClient: trying host %s port %d...\n", addr.GetHostName(), addr.GetPortNum());
        res = connect(this->sock, (const sockaddr*) &(addr.GetAddrStruct()), sizeof(addr.GetAddrStruct()));
        if (res == -1)
        {
            // connection failed.
            n_printf("nIpcClient: failed to connect to host %s port %d!\n", addr.GetHostName(), addr.GetPortNum());
            this->DestroySocket();
            return false;
        }

        // send handshake
        char msg[256];
        sprintf(msg, "~handshake %s", addr.GetPortName());
        res = send(this->sock, msg, strlen(msg) + 1, 0);
        if (res == -1)
        {
            // initial send failed!
            n_printf("nIpcClient: failed to send handshake to host %s port %d!\n", addr.GetHostName(), addr.GetPortNum());
            this->DestroySocket();
            return false;
        }

        // put socket into nonblocking mode?
        this->ApplyBlocking(this->blocking);

        // all ok
        this->isConnected = true;
        n_printf("\nConnected.");
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Disconnect from the server.
*/
void
nIpcClient::Disconnect()
{
    if (this->isConnected)
    {
        n_assert(INVALID_SOCKET != this->sock);

        // send a close message to the server
        const char* cmd = "~close";
        int res = send (this->sock, cmd, strlen(cmd) + 1, 0);
        if (res == -1)
        {
            n_printf("nIpcClient: failed to send close msg to host %s port %s\n",
                this->serverAddr.GetHostName(), this->serverAddr.GetPortName());
        }
        this->DestroySocket();
        this->isConnected = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nIpcClient::IsConnected() const
{
    return this->isConnected;
}

//------------------------------------------------------------------------------
/**
*/
bool
nIpcClient::Send(const nIpcBuffer& msg)
{
    if (this->isConnected)
    {
        n_assert(INVALID_SOCKET != this->sock);
        int res = send(this->sock, msg.GetPointer(), msg.GetSize(), 0);
        if (-1 == res)
        {
            n_printf("nIpcClient: Send() failed!\n");
            this->Disconnect();
            return false;
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Receive a message. This method may block if the connection has been
    created with blocking on.
*/
bool
nIpcClient::Receive(nIpcBuffer& msg)
{
    n_assert(INVALID_SOCKET != this->sock);
    int res = recv(this->sock, msg.GetPointer(), msg.GetMaxSize(), 0);


    if (res == 0 || N_SOCKET_LAST_ERROR == N_ECONNRESET)
    {
        // the connection has been closed
        n_printf("connection lost!\n");
        this->DestroySocket();
        this->isConnected = false;
    }
    if (res > 0)
    {
        msg.SetSize(res);
        return true;
    }
    else
    {
        // either an error occured, or the method would block
        if (N_SOCKET_LAST_ERROR == N_EWOULDBLOCK)
        {
            msg.SetSize(0);
        }
        else
        {
            // FIXME: some other error then WouldBlock
            n_printf("nIpcClient::Receive(): failed!\n");
        }
        return false;
    }
}
