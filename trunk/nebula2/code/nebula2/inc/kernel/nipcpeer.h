#ifndef N_IPCPEER_H
#define N_IPCPEER_H
//------------------------------------------------------------------------------
/**
    @class nIpcPeer
    @ingroup Ipc

    @brief A low level interprocess communications port for unreliable
    communications (using UDP).

    Ipc peers are connectionless, which means, every nIpcPeer can send to
    and receive from any other nIpcPeer as long as the target nIpcPeer's
    address is known.  nIpcPeers can send to the broadcast address,
    which means, any other nIpcPeer in the LAN area on the same port
    receives the broadcast message.

    nIpcPeers can work in blocking or nonblocking mode. In blocking mode,
    Receive() will block until at least one message has arrived. In
    non-blocking mode, Receive() will return always immediately, a return
    code true indicates that a message has been received. In non-blocking
    mode, Receive() should be called very frequently (at least once per frame)
    to prevent lost messages due to internal buffer overflow.

    nIpcPeer uses nIpcAddress objects to identify communication points.

    nIpcPeers use the socket datagrams, which use the UDP protocol. This
    means communication over nIpcPeers is unreliable. Unreliable means, messages
    can get lost, arrive in different order, or arrive incomplete. Message
    size should be less then 500 Bytes, since the guaranteed min buffer size
    is somewhere around 550 bytes, this does not include communications
    overhead.

    For an reliable alternative (which cannot use broadcast however), check out
    the nIpcServer and nIpcClient classes.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nsocketdefs.h"
#include "kernel/nipcaddress.h"
#include "kernel/nipcbuffer.h"

//------------------------------------------------------------------------------
class nIpcPeer
{
public:
    /// peer flags
    enum
    {
        Blocking = (1<<0),
        CanBroadcast = (1<<1),
        CanSend = (1<<2),
        CanReceive = (1<<3)
    };
    /// constructor
    nIpcPeer(nIpcAddress* localAddr, int peerFlags);
    /// destructor
    ~nIpcPeer();
    /// send a message to any address
    bool SendTo(nIpcAddress& toAddr, nIpcBuffer& msg);
    /// receive a message, capture sender address
    bool ReceiveFrom(nIpcBuffer& msg, nIpcAddress& fromAddr);

private:
    SOCKET sendSocket;
    SOCKET recvSocket;
};
//------------------------------------------------------------------------------
#endif
