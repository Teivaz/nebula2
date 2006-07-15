#ifndef NAVIGATION_WAYPOINT_H
#define NAVIGATION_WAYPOINT_H
//------------------------------------------------------------------------------
/**
    @class Navigation::WayPoint

    FIXME: why doesn't this class have a constructor!?!?!?

    (C) 2005 RadonLabs GmbH
*/

#include "mathlib/vector.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class WayPoint
{
public:
    enum Stance
    {
        Crouching,
        Standing
    };

    /// constructor
    WayPoint();
    /// Set position
    void SetPosition(const vector3& position);
    /// Get position
    const vector3& GetPosition() const;
    /// Set heading
    void SetHeading(float heading);
    /// Get heading
    float GetHeading() const;
    /// Set wait time
    void SetWaitTime(double waitTime);
    /// Get wait time
    double GetWaitTime() const;
    /// Set guard radius
    void SetGuardRadius(float guardRadius);
    /// Get guard radius
    float GetGuardRadius() const;
    /// Set stance
    void SetStance(Stance stance);
    /// Get stance
    Stance GetStance() const;
    /// Set activity
    void SetActivity(const nString& activity);
    /// Get activity
    const nString& GetActivity() const;

private:
    vector3 position;
    float heading;
    nTime waitTime;
    float guardRadius;
    Stance stance;
    nString activity;
};

//------------------------------------------------------------------------------
/**
*/
inline
WayPoint::WayPoint() :
    heading(0.0f),
    waitTime(0.0),
    guardRadius(0.0f),
    stance(Crouching)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WayPoint::SetPosition(const vector3& position)
{
    this->position = position;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
WayPoint::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WayPoint::SetHeading(float heading)
{
    this->heading = heading;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
WayPoint::GetHeading() const
{
    return this->heading;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WayPoint::SetWaitTime(double waitTime)
{
    this->waitTime = waitTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
double
WayPoint::GetWaitTime() const
{
    return this->waitTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WayPoint::SetGuardRadius(float guardRadius)
{
    this->guardRadius = guardRadius;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
WayPoint::GetGuardRadius() const
{
    return this->guardRadius;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WayPoint::SetStance(WayPoint::Stance stance)
{
    this->stance = stance;
}

//------------------------------------------------------------------------------
/**
*/
inline
WayPoint::Stance
WayPoint::GetStance() const
{
    return this->stance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WayPoint::SetActivity(const nString& activity)
{
    this->activity = activity;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
WayPoint::GetActivity() const
{
    return this->activity;
}

} // namespace Navigation

//------------------------------------------------------------------------------
#endif
