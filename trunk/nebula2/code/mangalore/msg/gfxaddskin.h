#ifndef MSG_GFXADDSKIN_H
#define MSG_GFXADDSKIN_H
//------------------------------------------------------------------------------
/**
    @class Message::GfxAddSkin
    
    Makes the given skin visible on a Character3.
    
    (C) 2006 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class GfxAddSkin : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(GfxAddSkin);
    DeclareMsgId;
public:
    /// set skin name
    void SetSkinName(const nString& s);
    /// get skin name
    const nString& GetSkinName() const;
private:
    nString skinName;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxAddSkin::SetSkinName(const nString& s)
{
    this->skinName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GfxAddSkin::GetSkinName() const
{
    return this->skinName;
}

}; // namespace Message
//------------------------------------------------------------------------------
#endif
