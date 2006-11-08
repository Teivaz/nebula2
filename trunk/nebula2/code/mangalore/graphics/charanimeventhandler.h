#ifndef GRAPHICS_CHARANIMEVENTHANDLER_H
#define GRAPHICS_CHARANIMEVENTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Graphics::CharAnimEventHandler

    An animation event handler for the Graphics::CharEntity class.

    (C) 2005 Radon Labs GmbH
*/
#include "anim2/nanimeventhandler.h"
#include "graphics/entity.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class CharAnimEventHandler : public nAnimEventHandler
{
public:
    /// constructor
    CharAnimEventHandler();
    /// destructor
    virtual ~CharAnimEventHandler();
    /// handle an animation event
    virtual void HandleEvent(const nAnimEventTrack& track, int eventIndex);
    /// set graphics entity
    void SetEntity(Entity* e);
    /// get graphics entity
    Entity* GetEntity() const;

protected:
    /// fire a sound effect
    virtual void FireSound(const nString& sound, const vector3& position);
    /// fire a visual effect
    virtual void FireVFX(const nString& vfx, const matrix44& transform);

private:
    Ptr<Entity> entity;
    nTime lastSoundTime;
    nTime lastVfxTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
CharAnimEventHandler::SetEntity(Entity* e)
{
    this->entity = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
Entity*
CharAnimEventHandler::GetEntity() const
{
    return this->entity.get_unsafe();
}
};
//------------------------------------------------------------------------------
#endif

