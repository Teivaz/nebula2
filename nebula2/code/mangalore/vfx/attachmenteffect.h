#ifndef VFX_HARDPOINTEFFECT_H
#define VFX_HARDPOINTEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::AttachmentEffect

    An effect which attaches a graphics entity to a hardpoint while active.

    (C) 2006 Radon Labs GmbH
*/
#include "vfx/effect.h"

//------------------------------------------------------------------------------
namespace VFX
{
class AttachmentEffect : public Effect
{
    DeclareRtti;
    DeclareFactory(AttachmentEffect);
public:
    /// constructor
    AttachmentEffect();
    /// destructor
    virtual ~AttachmentEffect();
    /// set the joint- (hardpoint) name
    void SetJointName(const nString& s);
    /// get the joint-name
    const nString& GetJointName() const;
    /// set graphics resource name
    void SetResourceName(const nString& s);
    /// get graphics resource name
    const nString& GetResourceName() const;
    /// set optional offset matrix
    void SetOffsetMatrix(const matrix44& m);
    /// get optional offset matrix
    const matrix44& GetOffsetMatrix() const;
    /// start the effect
    virtual void OnStart();
    /// deactivate the effect
    virtual void OnDeactivate();

private:
    nString jointName;
    nString resourceName;
    matrix44 offsetMatrix;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
AttachmentEffect::SetJointName(const nString& s)
{
    n_assert(s.IsValid());
    this->jointName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
AttachmentEffect::GetJointName() const
{
    return this->jointName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
AttachmentEffect::SetResourceName(const nString& s)
{
    n_assert(s.IsValid());
    this->resourceName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
AttachmentEffect::GetResourceName() const
{
    return this->resourceName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
AttachmentEffect::SetOffsetMatrix(const matrix44& m)
{
    this->offsetMatrix = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
AttachmentEffect::GetOffsetMatrix() const
{
    return this->offsetMatrix;
}

} // namespace VFX
//------------------------------------------------------------------------------
#endif
