#ifndef MSG_GFXSETVISIBLE_H
#define MSG_GFXSETVISIBLE_H
//------------------------------------------------------------------------------
/**
    @class Msg::GfxSetVisible

    Shows or hides all graphics entities of a game entity.

    (C) 2005 Radon Labs GmbH
*/
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Message
{
class GfxSetVisible : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(GfxSetVisible);
    DeclareMsgId;

public:
    /// constructor
    GfxSetVisible();

    /// set visibility (true = show, false = hide)
    void SetVisible(bool visible);
    /// get visibility (true = show, false = hide)
    bool GetVisible() const;

private:
    bool visible;
};

RegisterFactory(GfxSetVisible);

//------------------------------------------------------------------------------
/**
*/
inline
GfxSetVisible::GfxSetVisible():
    visible(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GfxSetVisible::SetVisible(bool visible)
{
    this->visible = visible;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
GfxSetVisible::GetVisible() const
{
    return this->visible;
}

}; // namespace Msg
//------------------------------------------------------------------------------
#endif