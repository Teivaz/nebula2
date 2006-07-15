//------------------------------------------------------------------------------
//  nipcpeer.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nipcpeer.h"

//------------------------------------------------------------------------------
/**
    The local address must only be valid if the CanReceive flag is set,
    since this is the address definition the receive socket will be bound
    to. For a send-only socket, a null-pointer should be given for selfAddr,
    otherwise, the hostname MUST be set to "self" or "inetself":

    "self:portName"
*/
nIpcPeer::nIpcPeer(nIpcAddress* selfAddr, int peerFlags)
{
    if (peerFlags & CanSend)
    {
        // create the send socket
        this->sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
        n_assert(this->sendSocket != INVALID_SOCKET);

        // configure the send socket
        int trueAsInt = 1;
        int res = setsockopt(this->sendSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&trueAsInt, sizeof(trueAsInt)); 
        n_assert(res != -1);

        // enable/disable broadcast mode on sender socket
        if (peerFlags & CanBroadcast)
        {
            int trueAsInt = 1;
            res = setsockopt(this->sendSocket, SOL_SOCKET, SO_BROADCAST, (const char *)&trueAsInt, sizeof(trueAsInt));
        }
        else
        {
            int falseAsInt = 0;
            res = setsockopt(this->sendSocket, SOL_SOCKET, SO_BROADCAST, (const char *)&falseAsInt, sizeof(falseAsInt));
        }
        n_assert(res != -1);
    }
    else
    {
        this->sendSocket = INVALID_SOCKET;
    }

    if (peerFlags & CanReceive)
    {
        n_assert(selfAddr);

        // create the receiver socket
        this->recvSocket = socket(AF_INET, SOCK_DGRAM, 0);
        n_assert(this->recvSocket != INVALID_SOCKET);

        // configure the receiver socket
        int trueAsInt = 1;
        int res = setsockopt(this->recvSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&trueAsInt, sizeof(trueAsInt)); 
        n_assert(res != -1);

        // enable/disable blocking mode on the receiver socket
        if (peerFlags & Blocking)
        {
            #if defined(__WIN32__)
                u_long falseAsUlong = 0;
                res = ioctlsocket(this->recvSocket, FIONBIO, &falseAsUlong);
            #elif defined(__LINUX__) || defined(__MACOSX__)
                int flags;
                flags = fcntl(this->recvSocket, F_GETFL);
                flags |= O_NONBLOCK;
                res = fcntl(this->recvSocket, F_SETFL, flags);
            #endif
            n_assert(0 == res);
        }
        else
        {
            #if defined(__WIN32__)
                u_long trueAsUlong = 1;
                res = ioctlsocket(this->recvSocket, FIONBIO, &trueAsUlong);
            #elif defined(__LINUX__) || defined(__MACOSX__)
                int flags;
                flags = fcntl(this->recvSocket, F_GETFL);
                res = fcntl(this->recvSocket, F_SETFL, flags & ~O_NONBLOCK);
            #endif
            n_assert(0 == res);
        }

        // bind the receiver socket to the local address
        res = bind(this->recvSocket, (const sockaddr*) &(selfAddr->GetAddrStruct()), sizeof(selfAddr->GetAddrStruct()));
        n_assert(SOCKET_ERROR != res);
    }
    else
    {
        this->recvSocket = INVALID_SOCKET;
    }
}

//------------------------------------------------------------------------------
/**
*/
nIpcPeer::~nIpcPeer()
{
    // shutdown the receiver socket
    if (this->recvSocket != INVALID_SOCKET)
    {
        shutdown(this->recvSocket, 2);
        closesocket(this->recvSocket);
        this->recvSocket = INVALID_SOCKET;
    }

    // shutdown the send socket
    if (this->sendSocket != INVALID_SOCKET)
    {
        shutdown(this->sendSocket, 2);
        closesocket(this->sendSocket);
        this->sendSocket = INVALID_SOCKET;
    }
}

//------------------------------------------------------------------------------
/**
    Sends a byte buffer to the location defined by nIpcAddress. nIpcAddress
    may define a broadcast address (see nipcaddress.h for details).
*/
bool
nIpcPeer::SendTo(nIpcAddress& toAddr, nIpcBuffer& msg)
{
    n_assert(this->sendSocket != INVALID_SOCKET);
    int result;
    result = sendto(this->sendSocket, 
                    (const char*) msg.GetPointer(), 
                    msg.GetSize(), 
                    0, 
                    (const sockaddr*) &(toAddr.GetAddrStruct()), 
                    sizeof(toAddr.GetAddrStruct()));
    if (SOCKET_ERROR == result)
    {
        n_printf("nIpcPeer::SendTo(): sendto() failed!\n");
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Receives a message. In blocking mode, the method blocks until a message
    arrives (FIXME: a timeout should be defined in that case!). In non-blocking
    mode, the method returns immediately. In this case, a return value of
    0 indicates that no message was pending. If a message has been received,
    the fromAddr will contain the address definition of the sender, and the
    number of bytes received will be returned.
*/
bool 
nIpcPeer::ReceiveFrom(nIpcBuffer& msg, nIpcAddress& fromAddr)
{
    n_assert(this->recvSocket != INVALID_SOCKET);
    sockaddr_in from;
    socklen_t fromSize = sizeof(from);
    int result;
    result = recvfrom(this->recvSocket, msg.GetPointer(), msg.GetMaxSize(), 0, (sockaddr*) &from, &fromSize);
    if (result > 0)
    {
        // something has been received, fill out the fromAddr
        fromAddr.SetAddrStruct(from);
        msg.SetSize(result);
        return true;
    }
    else
    {
        // an error has occured, or the method would block
        msg.SetSize(0);
        return false;
    }
}
