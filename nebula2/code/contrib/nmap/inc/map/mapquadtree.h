//------------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#ifndef N_MAPQUADTREE_H
#define N_MAPQUADTREE_H
//---------------------------------------------------------------------------
/**
    @class MapQuadtree
    @ingroup NMapContribModule
    @brief Base quadtree space partitioning class.

    Adapted from nOctree, same code and same interface although this
    class is optimised to be used privately by the nMapNode class.

    Administers a number MapQuadElement's, which are defined by
    position and radius (for the time being). MapQuadtree is composed
    of MapQuadNode's, which can also contain nOctNodes. Each MapQuadElement
    lives in the MapQuadNode, in which it fits in completely. If the
    number of elements in an MapQuadNode exceeds an adjustable threshold,
    the MapQuadNode attempts to partition itself and divide the elements
    among the new sub-nodes.  However it is possible that with a set
    of nodes with large radii, that there may be more elements within
    a node than the theshold would allow.

    The interface is simple: One adds new items to the tree with
    MapQuadtree::AddElement(), update their position or radius with
    MapQuadtree::UpdateElement() and removes them with MapQuadtree::RemElement().
    There are different methods for collecting a subset of the items,
    e.g. all items which are within the view volume.

    @todo Hard coded max of 1024 nodes, or a terrain with 32 x 32 terrain
    blocks.
*/
//---------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "util/nnode.h"
#include "util/nlist.h"
#include "mathlib/bbox.h"

//--------------------------------------------------------------------
/**
   @class MapQuadElement
   @ingroup NMapContribModule
   @brief The interface for elements contained within an MapQuadtree.

   nOctElements are the items administered by the MapQuadtree. They
   are defined by their position and radius, which must NOT be
   directly manipulated.  The position and radius must be updated
   through the MapQuadtree because each attribute modification could
   cause the tree to be reorganized.
*/
//--------------------------------------------------------------------
class MapBlock;
class MapQuadNode;

class MapQuadElement : public nNode
{
public:
    MapQuadNode *octnode;          // bin zur Zeit Mitglied dieser OctNode
    vector3 pos;
    vector3 p0;
    vector3 p1;
    int collect_flags;

    enum
    {
        N_COLLECT_VIEWVOL = (1<<0)
    };

    //----------------------------------------------------------------
    MapQuadElement(MapBlock* block) : nNode(block), octnode(NULL), p0(), p1(), collect_flags(0)
    {
    };
    //----------------------------------------------------------------
    ~MapQuadElement()
    {
        n_assert(octnode == NULL);
    };
    //----------------------------------------------------------------
    void Set(const vector3& min, const vector3& max)
    {
        p0 = min;
        p1 = max;
        pos = (max + min) * (1 / 2.0f);
    }
    //----------------------------------------------------------------
    void SetCollectFlags(int f)
    {
        this->collect_flags = f;
    };
    //----------------------------------------------------------------
    int GetCollectFlags(void)
    {
        return this->collect_flags;
    };
};

//--------------------------------------------------------------------
/**
   @class MapQuadNode
   @ingroup NMapContribModule
   @brief An internal implementation class for MapQuadtree.
*/
//--------------------------------------------------------------------
class MapQuadNode
{
public:
    union 
    {
        MapQuadNode *parent;
        MapQuadNode *next;
    };
    MapQuadNode* c[4];             // Child-Nodes
    vector3 pos;                ///< Centre
    vector3 p0;                 // Bounding-Box-0
    vector3 p1;                 // Bounding-Box-1
    short num_elms;             // Anzahl Elemente in elm_list
    short all_num_elms;         // Gesamt-Anzahl-Elemente in elm_list
    nList elm_list;             // List von MapQuadElement's

    //----------------------------------------------------------------
    MapQuadNode()
    {
        parent   = NULL;
        num_elms = 0;
        all_num_elms = 0;
        memset(c,0,sizeof(c));
    };    
    //----------------------------------------------------------------
    ~MapQuadNode()
    {
        n_assert(0 == all_num_elms);
        n_assert(elm_list.IsEmpty());
    };    
    //----------------------------------------------------------------
    void AddElm(MapQuadElement *oe)
    {
        num_elms++;
        MapQuadNode *on = this;
        do
        {
            on->all_num_elms++;
        } while ((on=on->parent)); 
        oe->octnode = this;
        elm_list.AddTail(oe);
    };
    //----------------------------------------------------------------
    void RemElm(MapQuadElement *oe)
    {
        num_elms--;
        MapQuadNode *on = this;
        do
        {
            on->all_num_elms--;
        } while ((on=on->parent));
        oe->octnode = NULL;
        oe->Remove();
    };
    //----------------------------------------------------------------
};

//---------------------------------------------------------------------------
class nGfxServer2;
class nMap;

class MapQuadtree
{
public:
    MapQuadtree(nMap* map);
    virtual ~MapQuadtree();

    void SetSubdivNum(int);
    int GetSubdivNum(void);

    void AddElement(MapQuadElement*, const vector3&, const vector3&);
    void RemElement(MapQuadElement*);
    void UpdateElement(MapQuadElement*, const vector3&, const vector3&);
    void BalanceTree(void);

    int CollectByFrustum(nGfxServer2* gfx_server, MapQuadElement**&);
    int GetCollect(MapQuadElement**&);

    void Visualize(nGfxServer2*);

private:
    enum 
    {
        N_OCT_MAXNUMNODES   = 2048,     // max. Anzahl Elemente in free_pool 
        N_OCT_MAXNUMCOLLECT = 1024,     // max. Anzahl collected Elemente
    };

    MapQuadNode *allocnode(MapQuadNode *p, float x0, float x1, float y0, float y1, float z0, float z1);
    void freenode(MapQuadNode *); 

    bool elm_inside_node(MapQuadElement *, MapQuadNode *);
    void subdivide(MapQuadNode *);
    void collapse(MapQuadNode *);
    MapQuadNode *find_node_upward(MapQuadNode *, MapQuadElement *);
    MapQuadNode *find_node_downward(MapQuadNode *, MapQuadElement *);
    void insert_element(MapQuadElement *);
    void move_element(MapQuadElement *);
    void balance(MapQuadNode *);
    void visualize_node(nGfxServer2 *, MapQuadNode *);

    int box_clip_viewvol(vector3&, vector3&);
    void collect(MapQuadElement *);
    void recurse_collect_nodes_with_flags(MapQuadNode *, int);

    // Frustum occlusion
    void InitClipPlanes(nGfxServer2* gfx_server);
    void recurse_collect_by_frustum(MapQuadNode* on, unsigned int clip_mask);
    bool box_clip_frustum(const vector3& centre, const vector3& p1,
                          vector4* planes,
                          unsigned int& out_clip_mask,
                          unsigned int in_clip_mask);
    void collect_nodes_in_frustum(MapQuadNode* on, unsigned int clip_mask);

    /// Root node of octree
    MapQuadNode* tree_root;
    /// Maximum elements in a bucket before subdivision
    int subdiv_num;

    MapQuadNode *free_root;
    MapQuadNode free_pool[N_OCT_MAXNUMNODES];

    int num_collected;
    int array_size;
    MapQuadElement* collect_array[N_OCT_MAXNUMCOLLECT];

    /// Clip planes for frustum collection
    vector4 clipPlanes[6];

    // For visibility occlusion
    void init_block_clip_planes(nGfxServer2* gfx_server,
                                MapQuadElement* elm);
    float max_height;       ///< Terrain's maximum height
    float min_height;       ///< Terrain's minimum height
};

//---------------------------------------------------------------------------

#endif
