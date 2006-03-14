#ifndef NAVIGATION_PATHFINDER_H
#define NAVIGATION_PATHFINDER_H
//------------------------------------------------------------------------------
/**
    @class Navigation::PathFinder

    (C) 2005 RadonLabs GmbH
*/

#include "mathlib/vector.h"
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "navigation/path3d.h"
#include "navigation/map.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class PathFinder : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(PathFinder);

public:
    /// Default Constructor.
    PathFinder();
    /// Destruct.
    virtual ~PathFinder();

    /// Set map nodes
    void SetMap(Map* map);
    /// Find the path
    Path3D* FindPath(const vector3& start, const vector3& target);

private:
    struct PathNode
    {
        int from;

        float cost;
        float estimate;

        bool open;
        //bool visited;
    };

    void PrepareSearch(const vector3& target);
    int FindQueueIndex(float cost);
    void AddCandidate(int node, float cost);
    int GetNextCandidate();

    /// find the closest map node to pos; returns true if pos is inside the map node
    void FindStartEndNodes(const vector3& from, const vector3& to, int& fromIndex, int& toIndex) const;
    /// calculate the cost duing traversing from node1 to node2
    float TraverseCost(int mapNodeIndex1, int mapNodeIndex2) const;

    Ptr<Map> map;
    nArray<PathNode> nodes;

    int* queue;
    int queueCount;
    int queueCapacity;

    static float smoothFactor;
};

RegisterFactory(PathFinder);

} // namespace Navigation

//------------------------------------------------------------------------------
#endif
