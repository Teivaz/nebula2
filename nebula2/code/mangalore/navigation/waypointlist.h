#ifndef NAVIGATION_WAYPOINTLIST_H
#define NAVIGATION_WAYPOINTLIST_H
//------------------------------------------------------------------------------
/**
    @class Navigation::WayPointList

    (C) 2005 RadonLabs GmbH
*/

#include "foundation/refcounted.h"
#include "navigation/waypoint.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class WayPointList : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(WayPointList);

public:
    /// Destruct.
    virtual ~WayPointList();
    /// Add way point
    void AddWayPoint(const WayPoint& waypoint);
    /// Get way points
    const nArray<WayPoint>& GetWayPoints() const;
    /// Set entity loc
    void SetName(const nString& name);
    /// Get entity loc
    const nString& GetName() const;

private:
    nArray<WayPoint> wayPoints;
    nString name;
};

RegisterFactory(WayPointList);

//------------------------------------------------------------------------------
/**
*/
inline
void
WayPointList::AddWayPoint(const WayPoint& waypoint)
{
    this->wayPoints.Append(waypoint);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<WayPoint>&
WayPointList::GetWayPoints() const
{
    return this->wayPoints;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WayPointList::SetName(const nString& name)
{
    this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
WayPointList::GetName() const
{
    return this->name;
}

} // namespace Navigation
//------------------------------------------------------------------------------
#endif
