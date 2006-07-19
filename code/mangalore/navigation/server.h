#ifndef NAVIGATION_SERVER_H
#define NAVIGATION_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Navigation::Server

    Server of the navigation subsystem.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "navigation/waypointlist.h"
#include "navigation/coverpoint.h"
#include "navigation/map.h"
#include "kernel/nprofiler.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class Path3D;

class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get server pointer
    static Server* Instance();
    /// initialize
    bool Open();
    /// cleanup
    void Close();
    /// Open?
    bool IsOpen() const;
    void OnBeginFrame();
    void OnEndFrame();
    /// New path from point `a' to point `b'.
    Path3D* MakePath(const vector3& a, const vector3& b);
    /// Update path with new target position `targetPos'.
    void UpdatePath(Path3D* path, const vector3& targetPos);
    /// Add a way point
    void AddWayPointList(WayPointList* list);
    /// Get way point list by entity location
    WayPointList* GetWayPointList(const nString& name) const;
    /// Add a cover point
    void AddCoverPoint(const CoverPoint& coverPoint);
    /// Find a cover point
    CoverPoint* FindCoverPoint(const vector3& position, const vector3& enemy, float range) const;
    /// set current navigation map
    void SetMap(Map* m);
    /// get current navigation map
    Map* GetMap() const;
    /// render debug visualization
    void RenderDebug();

private:
    static Server* Singleton;
    bool isOpen;
    nArray<Ptr<WayPointList> > wayPointLists;
    nArray<CoverPoint> coverPointList;
    Ptr<Map> map;
    PROFILER_DECLARE(profNavMakePath);
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Server::IsOpen() const
{
    return isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetMap(Map* m)
{
    this->map = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
Map*
Server::GetMap() const
{
    return this->map.get_unsafe();
}

} // namespace Navigation
//------------------------------------------------------------------------------
#endif
