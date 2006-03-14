#ifndef MESSAGE_ID_H
#define MESSAGE_ID_H
//------------------------------------------------------------------------------
/**
    @class Message::Id
    
    A message identifier. This is automatically implemented in message classes
    using the DeclareMsgId and ImplementMsgId macros.
   
    (C) 2005 Radon Labs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
namespace Message
{
class Id
{
public:
    /// constructor
    Id();
    /// equality operator
    bool operator==(const Id& rhs) const;
};

//------------------------------------------------------------------------------
/**
*/
inline
Id::Id()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Id::operator==(const Id& rhs) const
{
    return (this == &rhs);
}

}; // namespace Message
//------------------------------------------------------------------------------
#endif