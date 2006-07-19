#ifndef MESSAGE_SERVER_H
#define MESSAGE_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Message::Server

    The server of the message subsystem is the central communication point
    where all events go through.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"

//------------------------------------------------------------------------------
namespace Message
{
class Port;
class Msg;

class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get instance pointer
    static Server* Instance();
    /// open the message server
    bool Open();
    /// close the message server
    void Close();
    /// register a broadcast message port with the server
    void RegisterPort(Port* port);
    /// unregister a broadcast message port from the server
    void UnregisterPort(Port* port);

private:
    static Server* Singleton;

    friend class Msg;

    /// send a synchronous message to a specific message port, use Msg::SendSync()!
    void SendSync(Port* port, Msg* msg);
    /// broadcast a synchronous message to all registered message ports, use Msg::BroadcastSync()!
    void BroadcastSync(Msg* msg);
    /// send an asynchronous message to a specific message port, use Msg::SendAsync()!
    void SendAsync(Port* port, Msg* msg);
    /// broadcast an asynchronous message to all registered message ports, use Msg::BroadcastAsync()!
    void BroadcastAsync(Msg* msg);

    /// cleanup empty msg ports when not in broadcasting trigger
    void CleanupEmptyPorts();

    /// begin broadcasting
    void BeginBroadcast();
    /// is in broadcasting trigger
    bool IsInBroadcast() const;
    /// end handle message
    void EndBroadcast();

    bool isOpen;
    nArray<Ptr<Port> > portArray;
    int broadcastLockCount; ///< in the handle message trigger
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
