#ifndef NAVIGATION_COVERPOINT_H
#define NAVIGATION_COVERPOINT_H
//------------------------------------------------------------------------------
/**
    @class Navigation::CoverPoint

    (C) 2005 RadonLabs GmbH
*/

#include "mathlib/vector.h"
#include "mathlib/polar.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class CoverPoint
{
public:
    enum Type {
        Crouching,
        Standing
    };

    /// Constructor
    CoverPoint();
    /// Destructor
    ~CoverPoint();

    /// Set position
    void SetPosition(const vector3& position);
    /// Get position
    const vector3& GetPosition() const;

    /// Set heading
    void SetHeading(float heading);
    /// Get heading
    float GetHeading() const;

    /// Set cover point type
    void SetType(Type type);
    /// Get cover point type
    Type GetType() const;

    /// Claim cover point
    void Claim();
    /// Release cover point
    void Release();
    /// Check whether 
    bool IsClaimed() const;

    // Check whether cover point provides cover against a give point of attack
    bool ProvidesCoverAgainst(const vector3& attacker) const;

private:
    vector3 position;
    float heading;
    Type type;

    bool claimed;
};


//------------------------------------------------------------------------------
/**
*/
inline
void
CoverPoint::SetPosition(const vector3& position)
{
    this->position = position;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
CoverPoint::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CoverPoint::SetHeading(float heading)
{
    this->heading = heading;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CoverPoint::GetHeading() const
{
    return this->heading;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CoverPoint::SetType(Type type)
{
    this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
CoverPoint::Type
CoverPoint::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CoverPoint::Claim()
{
    n_assert(!this->claimed);
    this->claimed = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CoverPoint::Release()
{
    n_assert(this->claimed);
    this->claimed = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
CoverPoint::IsClaimed() const
{
    return this->claimed;
}
//------------------------------------------------------------------------------
/**
*/
inline
bool 
CoverPoint::ProvidesCoverAgainst(const vector3& attacker) const
{
    polar2 attackAngle(attacker - this->position);
    float delta = fabsf(this->heading - attackAngle.rho);

    if (delta > n_deg2rad(180.0f))
        delta = n_deg2rad(360.0f) - delta;

    return delta < n_deg2rad(60.0f);
}


} // namespace Navigation

//------------------------------------------------------------------------------
#endif
