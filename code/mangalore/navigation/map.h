#ifndef NAVIGATION_MAP_H
#define NAVIGATION_MAP_H
//------------------------------------------------------------------------------
/**
    @class Navigation::Map

    (C) 2005 RadonLabs GmbH
*/

#include "mathlib/vector.h"
#include "util/narray.h"
#include "util/nstring.h"
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class Map: public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Map);

public:
    friend class PathFinder;

    class Node
    {
        public:
            /// Default Constructor
            Node();
            /// Add a neighbor node
            void AddNeighbor(uint index);

            vector3 position;
            nArray<uint> neighbors;
    };
    struct Triangle;
    struct Corner
    {
        ushort vertex;
        int index;

        Triangle* face;
        ushort cornerIndex;
    };
    struct Edge
    {
        ushort vertices[2];
        int index;

        Triangle* face;
        ushort edgeIndex;
    };
    struct Triangle
    {
        const Corner* corners[3];
        const Edge* edges[3];
    };

    /// constructor
    Map();
    /// initialize the map object
    bool Open(const nString& filename);
    /// Get name of map file
    const nString& GetFilename() const;
    /// cleanup the map object
    void Close();
    /// is map initialized?
    bool IsOpen() const;
    /// get the map nodes
    const nArray<Node>& GetNodes() const;
    /// render debug visualization
    void RenderDebug();

private:
    void Connect(int node0, int node1);

    /// qsort hooks for sorting corners and edges
    static int __cdecl CornerCompare(const void* corner0, const void* corner1);
    static int __cdecl EdgeCompare(const void* edge0, const void* edge1);

    nString filename;
    nArray<Node> nodes;
    bool isOpen;
};

RegisterFactory(Map);

//------------------------------------------------------------------------------
/**
*/
inline
bool
Map::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Map::GetFilename() const
{
    return this->filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<Map::Node>&
Map::GetNodes() const
{
    return this->nodes;
}

} // namespace Navigation

//------------------------------------------------------------------------------
#endif
