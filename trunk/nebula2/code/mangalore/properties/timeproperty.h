#ifndef PROPERTIES_TIMEPROPERTY_H
#define PROPERTIES_TIMEPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::TimeProperty

    The time property adds the attribute "Time" to the entity. This 
    attributes contains the time since the time property has been
    attached to the entity.

    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"

//------------------------------------------------------------------------------
namespace Properties
{
class TimeProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(TimeProperty);

public:
    /// constructor
    TimeProperty();
    /// destructor
    virtual ~TimeProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called on begin of frame
    virtual void OnBeginFrame();

private:
    nTime attachTime;
};

RegisterFactory(TimeProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif
