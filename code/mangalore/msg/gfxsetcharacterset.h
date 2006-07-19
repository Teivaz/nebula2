#ifndef MSG_GFXSETCHARACTERSET_H
#define MSG_GFXSETCHARACTERSET_H
//------------------------------------------------------------------------------
/**
    @class Msg::GfxSetCharacterSet
    
    Set the character set of an actor.
    
    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"
#include "graphics/charentity.h"

//------------------------------------------------------------------------------
namespace Message
{
class GfxSetCharacterSet : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(GfxSetCharacterSet);
    DeclareMsgId;

public:
    /// constructor
    GfxSetCharacterSet();
    
    /// set base animation
    void SetCharacterSet(const nString& n);
    /// get base animation
    const nString& GetCharacterSet() const;

private:
    nString characterSet;

};
RegisterFactory(GfxSetCharacterSet);

//------------------------------------------------------------------------------
/**
*/
inline
GfxSetCharacterSet::GfxSetCharacterSet() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetCharacterSet::SetCharacterSet(const nString& n)
{
    this->characterSet = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GfxSetCharacterSet::GetCharacterSet() const
{
    return this->characterSet;
}

}; // namespace Msg
//------------------------------------------------------------------------------
#endif
