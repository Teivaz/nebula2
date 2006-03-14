//------------------------------------------------------------------------------
//  graphics/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "navigation/server.h"
#include "navigation/path3d.h"
#include "navigation/pathfinder.h"
#include "foundation/factory.h"
#include "mathlib/polar.h"

namespace Navigation
{
ImplementRtti(Navigation::Server, Foundation::RefCounted);
ImplementFactory(Navigation::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false)
{
    n_assert(Singleton == 0);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(Singleton != 0);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Initialize the graphics subsystem and open the display.
*/
bool
Server::Open()
{
    n_assert(!IsOpen());
    isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Shutdown the graphics subsystem.
*/
void
Server::Close()
{
    n_assert(IsOpen());

    // release stored data
    this->wayPointLists.Clear();
    this->coverPointList.Clear();
    this->map = 0;

    isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
Path3D*
Server::MakePath(const vector3& a, const vector3& b) const
{
    Path3D* result = 0;
    PathFinder pathFinder;
    
    if (this->map.isvalid())
    {
        pathFinder.SetMap(this->map);
        result = pathFinder.FindPath(a, b);
    }
    else
    {
        // no path finding map available, just return a string path
        result = Path3D::Create();
        result->Extend(a);
        result->Extend(b);
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::UpdatePath(Path3D* path, const vector3& targetPos)
{
    n_assert(path != 0);

    // TODO: Update via A* algorithm!
    path->Extend(targetPos);
}

//------------------------------------------------------------------------------
/**
*/
void
Server::AddWayPointList(WayPointList* list)
{
    this->wayPointLists.Append(list);
}

//------------------------------------------------------------------------------
/**
*/
WayPointList*
Server::GetWayPointList(const nString& name) const
{
    int numWayPointLists = this->wayPointLists.Size();
    int index;

    for(index = 0; index < numWayPointLists; index++)
    {
        n_assert(this->wayPointLists[index].isvalid());
        if(strcmp(this->wayPointLists[index]->GetName().Get(), name.Get()) == 0)
        {
            return this->wayPointLists[index].get();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::AddCoverPoint(const CoverPoint& coverPoint)
{
    this->coverPointList.Append(coverPoint);
}

//------------------------------------------------------------------------------
/**
*/
CoverPoint* 
Server::FindCoverPoint(const vector3& position, const vector3& enemy, float range) const
{
    CoverPoint* bestCover = 0;
    float bestDistance = FLT_MAX;

    for (int i = 0; i < this->coverPointList.Size(); ++i)
    {
        CoverPoint& cover = this->coverPointList[i];

        float distance = (cover.GetPosition() - position).len();

        if (
            !cover.IsClaimed()
            && (distance <= range) && (distance < bestDistance)
            && cover.ProvidesCoverAgainst(enemy)
        )
        {
            bestCover = &cover;
            bestDistance = distance;
        }
    }

    return bestCover;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::RenderDebug()
{
    if (this->map.isvalid())
    {
        this->map->RenderDebug();
    }
}

} // namespace Navigation