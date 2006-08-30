#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H
//------------------------------------------------------------------------------
/**
    @class Message::Dispatcher

    A message Dispatcher is a specialization of a message Port. A message
    Dispatcher distributes all messages it receives to the attached Ports
    which are interested in this message id.

    @verbatim
                                    +------+
                                +-->| Port |
                               /    +------+
                +------------+/     +------+
    --- Msg --->| Dispatcher |----->| Port |
                +------------+\     +------+
                               \    +------+
                                +-->| Port |
                                    +------+
    @endverbatim

    Dispatcher objects usually serve as front end message ports which hide
    a more complex message processing infrastructure underneath.
    Message sent to Dispatcher objects are immediately forwarded to
    Handlers, no messages are kept in the Dispatcher object. Thus,
    a Dispatcher always appears as an empty message Port.

    (C) 2005 RadonLabs GmbH
*/
#include "foundation/ptr.h"
#include "message/port.h"

//------------------------------------------------------------------------------
namespace Message
{
class Dispatcher : public Port
{
    DeclareRtti;
    DeclareFactory(Dispatcher);

public:
    /// constructor
    Dispatcher();
    /// return true if the port accepts a message id
    virtual bool Accepts(Msg* msg);
    /// immediately forward incoming messages to the Handlers
    virtual void Put(Msg* msg);
    /// handle a single message (distribute to ports)
    virtual void HandleMessage(Msg* msg);
    /// attach a message port
    void AttachPort(Port* port);
    /// remove a message port
    void RemovePort(Port* port);

protected:
    /// cleanup empty msg ports when not in handle message trigger
    virtual void CleanupEmptyPorts();

    /// begin handle message
    void BeginHandleMessage();
    /// is in handle message tigger
    bool IsInHandleMessage() const;
    /// end handle message
    void EndHandleMessage();
private:
    nArray<Ptr<Port> > portArray;
    /// in the handle message trigger
    int handleMsgLockCount;
};

RegisterFactory(Dispatcher);

} // namespace Message
//------------------------------------------------------------------------------
#endif
