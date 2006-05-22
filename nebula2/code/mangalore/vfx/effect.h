#ifndef VFX_EFFECT_H
#define VFX_EFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::Effect

    A visual effect.
    
    (C) 2003 RadonLabs GmbH  
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "graphics/entity.h"

//------------------------------------------------------------------------------
namespace VFX
{
class Effect : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Effect);

public:
    /// constructor
    Effect();
    /// copy constructor
    Effect(const Effect& rhs);
    /// destructor
    virtual ~Effect();
    /// assignment operator, asserts that effect is not active while being copied
    void operator=(const Effect& rhs);
    /// set the effect's name
    void SetName(const nString& n);
    /// get the effect's name
    const nString& GetName() const;
    /// set graphics resource name of the effect
    void SetResourceName(const nString& n);
    /// get graphics resource name of the effect
    const nString& GetResourceName() const;
    /// set the world transform of the effect
    virtual void SetTransform(const matrix44& m);
    /// get the world transform of the effect
    const matrix44& GetTransform() const;
    /// set the duration of the effect
    void SetDuration(nTime t);
    /// get the duration of the effect
    nTime GetDuration() const;
    /// set the current time
    void SetTime(nTime t);
    /// get the current time
    nTime GetTime() const;
    /// activate the effect
    virtual void Activate();
    /// deactivate the effect
    virtual void Deactivate();
    /// return true if currently active
    bool IsActive() const;
    /// trigger the effect, returns true as long as the effect is active
    virtual bool Update();

protected:
    /// copy contents
    void Copy(const Effect& rhs);

    nString name;
    nString resourceName;
    matrix44 transform;
    nTime duration;
    nTime curTime;
    nTime activationTime;
    Ptr<Graphics::Entity> graphicsEntity;
};

RegisterFactory(Effect);

//------------------------------------------------------------------------------
/**
*/
inline
bool
Effect::IsActive() const
{
    return this->graphicsEntity.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::Copy(const Effect& rhs)
{
    n_assert(!rhs.IsActive());
    n_assert(!this->IsActive());
    this->name           = rhs.name;
    this->resourceName   = rhs.resourceName;
    this->transform      = rhs.transform;
    this->duration       = rhs.duration;
    this->curTime        = rhs.curTime;
    this->activationTime = rhs.activationTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
Effect::Effect(const Effect& rhs)
{
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::operator=(const Effect& rhs)
{
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Effect::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetResourceName(const nString& n)
{
    this->resourceName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Effect::GetResourceName() const
{
    return this->resourceName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetTransform(const matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Effect::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetDuration(nTime t)
{
    this->duration = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Effect::GetDuration() const
{
    return this->duration;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetTime(nTime t)
{
    this->curTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Effect::GetTime() const
{
    return this->curTime;
}

};
//------------------------------------------------------------------------------
#endif
