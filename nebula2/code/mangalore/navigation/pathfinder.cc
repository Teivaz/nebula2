//------------------------------------------------------------------------------
//  navigation/pathfinder.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "navigation/pathfinder.h"
#include "mathlib/line.h"
#include "foundation/factory.h"
#include <limits.h>

namespace Navigation
{
ImplementRtti(Navigation::PathFinder, Foundation::RefCounted);
ImplementFactory(Navigation::PathFinder);

float PathFinder::smoothFactor = 1.00f;

//------------------------------------------------------------------------------
/**
*/
PathFinder::PathFinder()
{
    this->queueCount = 0;
    this->queueCapacity = 32;
    this->queue = (int*)n_malloc(sizeof(int) * this->queueCapacity);
    n_assert(0 != this->queue);
}

//------------------------------------------------------------------------------
/**
*/
PathFinder::~PathFinder()
{
    n_assert(this->queue);
    n_free(this->queue);
    this->queue = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
PathFinder::SetMap(Map* map)
{
    n_assert(0 != map);

    this->map = map;
    this->nodes.SetFixedSize(map->GetNodes().Size());
}

//------------------------------------------------------------------------------
/**
*/
Path3D*
PathFinder::FindPath(const vector3& start, const vector3& target)
{
    Path3D* path = Path3D::Create();

    // begin path at start vector
    //path->Extend(start);

    // no path finding without navigation mesh
    if (! this->map.isvalid())
    {
        path->Extend(target);
        return path;
    }

    // find start and target nodes on navigation mesh
    int startNode, targetNode;
    this->FindStartEndNodes(start, target, targetNode, startNode); // reversed to simplify path generation

    // direct path if we're out of the navigation mesh
    if (startNode == -1 || targetNode == -1)
    {
        path->Extend(target);
        return path;
    }

    n_assert(startNode >= 0 && startNode < this->map->nodes.Size());
    n_assert(targetNode >= 0 && targetNode < this->map->nodes.Size());
    
    // prepare for search
    this->PrepareSearch(target);
    this->AddCandidate(startNode, 0.0f);

    // keep looking until all options are exhausted or we found our target
    int currentNode;
    while ((-1 != (currentNode = this->GetNextCandidate())) && (currentNode != targetNode))
    {
        const Map::Node& currentMapNode = this->map->nodes[currentNode];
        PathNode& currentPathNode = this->nodes[currentNode];

        // visit neighbors
        //n_assert(currentMapNode.neighbors.Size() > 0);
        if (currentMapNode.neighbors.Size() == 0)
        {
            continue;
        }
        for (int i = 0; i < currentMapNode.neighbors.Size(); ++i)
        {
            int neighborNode = currentMapNode.neighbors[i];
            PathNode& neighborPathNode = this->nodes[neighborNode];

            // calculate cost to reach neighbor node
            float cost = currentPathNode.cost + this->TraverseCost(currentNode, neighborNode);

            // move neighbor node to open list if it is unvisited or if we found a shorter path
            if (neighborPathNode.cost > cost)
            {
                n_assert(neighborNode != currentPathNode.from);

                neighborPathNode.from = currentNode;
                this->AddCandidate(neighborNode, cost);
            }
        }
    }

    // no path found
    if (-1 == currentNode)
    {
        path->Extend(target);
        return path;
    }

    n_assert(targetNode == currentNode);

    // path optimization: drop first and last node
    {
        int node = targetNode;

        targetNode = this->nodes[node].from;
        this->nodes[node].from = -1;

        node = targetNode;
        while (node != -1)
        {
            if (this->nodes[node].from == startNode)
            {
                startNode = node;
                this->nodes[node].from = -1;
            }

            node = this->nodes[node].from;
        }
    }

    // build path over navigation mesh
    // note that the path on the mesh is reversed, so appending elements 
    // from target to start is actually the right order
    vector3 prev;
    prev = start;

    int nodeIndex = targetNode;
    while (nodeIndex != -1)
    {
        const Map::Node& curNode = this->map->nodes[nodeIndex];
        const vector3 next = nodeIndex != startNode
            ? this->map->nodes[this->nodes[nodeIndex].from].position
            : target;

        // get "ideal" next point
        line3 shortest(prev, next);
        vector3 ideal = shortest.ipol(shortest.closestpoint(curNode.position));

        // find best neighbor
        vector3 bestNeighbor;
        float bestAngle = FLT_MAX;
        for (int n = 0; n < curNode.neighbors.Size(); ++n)
        {
            vector3 neighbor = this->map->nodes[curNode.neighbors[n]].position;
            float angle = fabsf(_vector3::angle(ideal - curNode.position, neighbor - curNode.position));
            if (angle < n_deg2rad(90.0f) && angle < bestAngle)
            {
                line3 toNeighbor(curNode.position, neighbor);

                bestAngle = angle;
                bestNeighbor = toNeighbor.ipol(toNeighbor.closestpoint(ideal)); // move ideal point onto a valid "walk-line"
            }
        }

        // move point if it smoothes the path
        vector3 ipol = bestAngle < n_deg2rad(90.0f)
            ? bestNeighbor * smoothFactor + curNode.position * (1.0f - smoothFactor)
            : curNode.position;

        path->Extend(ipol);

        prev = ipol;
        nodeIndex = this->nodes[nodeIndex].from;
    }

    // finish path by adding the target vector
    path->Extend(target);

    return path;
}

//------------------------------------------------------------------------------
/**
*/
void
PathFinder::PrepareSearch(const vector3& target)
{
    n_assert(this->nodes.Size() == this->map->nodes.Size());

    for (int i = 0; i < this->nodes.Size(); ++i)
    {
        PathNode& node = this->nodes[i];

        node.from = -1;

        node.cost = FLT_MAX;
        node.estimate = (target - this->map->nodes[i].position).len();

        node.open = false;
    }

    this->queueCount = 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PathFinder::FindQueueIndex(float cost)
{
    int low = 0, high = this->queueCount;

    while (high != low)
    {
        int pivot = (high + low) / 2;

        if (this->nodes[this->queue[pivot]].cost > cost)
        {
            low = pivot + 1;
        }
        else
        {
            high = pivot;
        }
    }

    return low;
}

//------------------------------------------------------------------------------
/**
*/
void
PathFinder::AddCandidate(int node, float cost)
{
    // remove node if already in open list (needs new position)
    if (this->nodes[node].open)
    {
        int index = this->FindQueueIndex(this->nodes[node].cost);

        if (this->queue[index] != node)
        {
            n_printf("looking for: node %d with cost %f\n", node, this->nodes[node].cost);
            n_printf("but found:   node %d with cost %f (at index %d)\n", this->queue[index], this->nodes[this->queue[index]].cost, index);

            // if there are multiple entries with the same cost, FindQueueIndex returns the index of the first entry
            while (this->queue[index] != node && index < this->queueCount) // && this->nodes[this->queue[index]].cost == cost
            {
                ++index;
            }

            if (index < this->queueCount)
            {
                n_printf("corrected to node %d with cost %f (at index %d)\n", this->queue[index], this->nodes[this->queue[index]].cost, index);
            }
            else
            {
                n_printf("correction tried but correct element not found in queue\n");
            }
        }

        // DEBUG CODE
        if (index >= this->queueCount || this->queue[index] != node)
        {
            n_printf("ERROR IN PRIORITY QUEUE\n");
            n_printf("looking for: node %d with cost %f\n", node, this->nodes[node].cost);
            n_printf("but found:   node %d with cost %f (at index %d)\n", this->queue[index], this->nodes[this->queue[index]].cost, index);
            n_printf("queue was:\n");
            for (int i = 0; i < this->queueCount; ++i)
            {
                int cur_node = this->queue[i];
                float cur_cost = this->nodes[cur_node].cost;

                n_printf(" %4d: %5d (%f)\n", i, cur_node, cur_cost);
            }
            n_error("Fehler im Pathfinding. Die Logdatei (C:\\Dokumente und Einstellungen\\<Benutzername>\\Lokale Einstellungen\\Anwendungsdaten\\RadonLabs\\Nebula2\\Schwarzenberg.log) bitte an Enno.");
        }
        //n_assert(index < this->queueCount && this->queue[index] == node);

        --this->queueCount;
        memmove(this->queue + index, this->queue + index + 1, sizeof(int) * (this->queueCount - index));
    }

    // grow queue as necessary
    if (this->queueCount == this->queueCapacity)
    {
        this->queueCapacity *= 2;
        this->queue = (int*)n_realloc(this->queue, sizeof(int) * this->queueCapacity);
        n_assert(0 != this->queue);
    }

    // update node
    this->nodes[node].cost = cost;
    this->nodes[node].open = true;

    // add node to queue
    int index = this->FindQueueIndex(cost);

    n_assert(index == 0                || this->nodes[this->queue[index - 1]].cost > cost);
    n_assert(index == this->queueCount || this->nodes[this->queue[index    ]].cost <= cost);

    memmove(this->queue + index + 1, this->queue + index, sizeof(int) * (this->queueCount - index));
    ++this->queueCount;

    this->queue[index] = node;
}

//------------------------------------------------------------------------------
/**
*/
int
PathFinder::GetNextCandidate()
{
    int node = this->queueCount > 0
        ? this->queue[--this->queueCount]
        : -1;

    if (node != -1)
        this->nodes[node].open = false;

    return node;
}

//------------------------------------------------------------------------------
/**
*/
float
PathFinder::TraverseCost(int mapNodeIndex1, int mapNodeIndex2) const
{
    vector3 v1 = this->map->nodes[mapNodeIndex1].position;
    vector3 v2 = this->map->nodes[mapNodeIndex2].position;

    return (v1 - v2).len();
}

//------------------------------------------------------------------------------
/**
*/
void
PathFinder::FindStartEndNodes(const vector3& from, const vector3& to, int& fromNode, int& toNode) const
{
    n_assert(!this->map->nodes.Empty());

    float bestFrom = FLT_MAX;
    float bestTo   = FLT_MAX;

    float airLineDist = vector3::distance(to, from);

    fromNode = -1;
    toNode = -1;

    int num = this->map->nodes.Size();
    for (int i = 0; i < num; ++i)
    {
        const Map::Node& mapNode = this->map->nodes[i];

        float fromDist = vector3::distance(mapNode.position, from);
        float toDist = vector3::distance(mapNode.position, to);
        float penalty = fromDist + toDist - airLineDist;

        float fromWeight = fromDist + penalty;
        float toWeight = toDist + penalty;

        if (fromWeight < bestFrom)
        {
            fromNode = i;
            bestFrom = fromWeight;
        }

        if (toWeight < bestTo)
        {
            toNode = i;
            bestTo = toWeight;
        }
    }
}

} // namespace Navigation