#ifndef MESSAGE_PORT_H
#define MESSAGE_PORT_H
//------------------------------------------------------------------------------
/**
    @class Message::Port

    A message Port is a receiving point for messages. One message
    can be attach to several message ports at the same time.
    Refcounting makes sure that messages are not deleted while
    they are attached to any port.

    Message Ports are the basis for message Dispatchers and
    message Handlers.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class Port : public Foundation::RefCounted
{
    DeclareRtti;
public:
    /// constructor
    Port();
    /// destructor
    virtual ~Port();
    /// return true if the port accepts a message id
    virtual bool Accepts(Msg* msg);
    /// put a new message on the port
    virtual void Put(Msg* msg);
    /// handle all pending messages
    virtual void HandlePendingMessages();
    /// handle a single message
    virtual void HandleMessage(Msg* msg);

private:
    nArray<Ptr<Msg> > msgQueue;
};

//------------------------------------------------------------------------------
/**
*/
inline
Port::Port() :
    msgQueue(8, 8)
{
    this->msgQueue.SetFlags(nArray<Ptr<Msg> >::DoubleGrowSize);
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
