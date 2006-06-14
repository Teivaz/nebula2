#ifndef PROPERTIES_ACTORGRAPHICSPROPERTY_H
#define PROPERTIES_ACTORGRAPHICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::ActorGraphicsProperty

    A specialized graphics property for actors. This creates a
    Graphics::CharEntity, knows how to switch animations and
    manages attachments.

    (C) 2005 Radon Labs GmbH
*/
#include "properties/graphicsproperty.h"
#include "graphics/charentity.h"

//------------------------------------------------------------------------------
namespace Properties
{
class ActorGraphicsProperty : public GraphicsProperty
{
    DeclareRtti;
	DeclareFactory(ActorGraphicsProperty);

public:
    /// destructor
    virtual ~ActorGraphicsProperty();

    /// called before rendering happens
    virtual void OnRender();

    /// return true if message is accepted by controller
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);

    /// setup default entity attributes
    virtual void SetupDefaultAttributes();

protected:
    /// get graphics entity cast to Graphics::CharEntity
    Graphics::CharEntity* GetGraphicsEntity() const;
    /// setup graphics entities
    virtual void SetupGraphicsEntities();
    /// cleanup graphics entities
    virtual void CleanupGraphicsEntities();
    /// set base or overlay animation
    void SetAnimation(const nString& baseAnim, const nString& overlayAnim, float baseAnimTimeOffset);
    /// add attachment definition
    void AddAttachment(const nString& jointName, const nString& gfxResName, const matrix44& offset);
    /// remove attachment definition
    void RemAttachment(const nString& jointName);
    /// find attachment index by joint name, return -1 if not found
    int FindAttachment(const nString& jointName);
    /// update attachment positions
    void UpdateAttachments();
    /// make skin visible on Character3
    void AddSkin(const nString& skinName);
    /// make skin visible on Character3
    void RemSkin(const nString& skinName);

    struct Attachment
    {
        int jointIndex;
        Ptr<Graphics::Entity> graphicsEntity;
        matrix44 offsetMatrix;
    };

    nArray<Attachment> attachments;
};

RegisterFactory(ActorGraphicsProperty);

//------------------------------------------------------------------------------
/**
*/
inline
Graphics::CharEntity*
ActorGraphicsProperty::GetGraphicsEntity() const
{
    n_assert(this->graphicsEntities[0]->IsA(Graphics::CharEntity::RTTI));
    return (Graphics::CharEntity*) this->graphicsEntities[0].get();
}

}; // namespace Properties
//------------------------------------------------------------------------------
#endif
