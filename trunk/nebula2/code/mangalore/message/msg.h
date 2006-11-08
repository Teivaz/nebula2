#ifndef MESSAGE_MSG_H
#define MESSAGE_MSG_H
//------------------------------------------------------------------------------
/**
    @class Message::Msg

    Base class for new-style messages. Messages are now vanilla C++ objects
    instanciated from proper C++ classes with default constructor, and
    proper setter/getter methods. This makes messages easier, safer and more
    intuitive to use.

    Messages are generally created and used like this now
    (using the MoveGoto message as an example):

    Ptr<MoveGoto> msg = Factory<MoveGoto>::Create();
    msg->SetPosition(targetPos);
    msg->Send(entity->GetDispatcher());

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "message/id.h"

//------------------------------------------------------------------------------
/**
    Message Id macros.
*/
#define DeclareMsgId \
public:\
    static Message::Id Id; \
    virtual const Message::Id& GetId() const; \
private:

#define ImplementMsgId(type) \
    Message::Id type::Id; \
    const Message::Id& type::GetId() const { return type::Id; }

//------------------------------------------------------------------------------
namespace Message
{
class Port;

class Msg : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareMsgId;
public:
    /// constructor
    Msg();
    /// destructor
    virtual ~Msg() = 0;
    /// return true if message is of the given id
    bool CheckId(const Message::Id& id) const;
    /// get message name
    const nString& GetName() const;
    /// send synchronous message to a specific message port
    virtual void SendSync(Port* port);
    /// broadcast synchronous message to the world
    virtual void BroadcastSync();
    /// send asynchronous message to a specific message port
    virtual void SendAsync(Port* port);
    /// broadcast asynchronous message to the world
    virtual void BroadcastAsync();
};

//------------------------------------------------------------------------------
/**
    NOTE: the constructor is private to enforce creation through the
    Factory<> template class.
*/
inline
Msg::Msg()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Msg::CheckId(const Message::Id& id) const
{
    return (id == this->GetId());
}

//------------------------------------------------------------------------------
/**
    Returns the message name. This is identical with the class name.
*/
inline
const nString&
Msg::GetName() const
{
    return this->GetClassName();
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
