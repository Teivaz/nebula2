#ifndef MSG_GFXREMSKIN_H
#define MSG_GFXREMSKIN_H
//------------------------------------------------------------------------------
/**
    @class Message::GfxRemSkin

    Makes the given skin invisibe on a Character3.

    (C) 2006 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class GfxRemSkin : public Msg
{
    DeclareRtti;
    DeclareFactory(GfxRemSkin);
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
GfxRemSkin::SetSkinName(const nString& s)
{
    this->skinName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
GfxRemSkin::GetSkinName() const
{
    return this->skinName;
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
