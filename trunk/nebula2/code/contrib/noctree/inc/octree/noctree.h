#ifndef N_OCTREE_H
#define N_OCTREE_H
//--------------------------------------------------------------------
/**
    @class nOctree
    @ingroup NOctreeContribModule
    @brief Base octree space partitioning class.

    Administers a number nOctElement's, which are defined by
    position, radius and optionally a boundingbox. nOctree is composed
    of nOctNode's, which can also contain nOctNodes. Each nOctElement
    lives in the nOctNode, in which it fits in completely. If the
    number of elements in an nOctNode exceeds an adjustable threshold,
    the nOctNode attempts to partition itself and divide the elements
    among the new sub-nodes.  However it is possible that with a set
    of nodes with large radii, that there may be more elements within
    a node than the theshold would allow.

    The interface is simple: One adds new items to the tree with
    nOctree::AddElement(), update their position or radius with
    nOctree::UpdateElement() and removes them with nOctree::RemElement().
    There are different methods for collecting a subset of the items,
    e.g. all items which are within the view volume.

    - 14-May-04   child  Added support to modify the bounding box of an
                         element for more exact culling and to minimize
                         overlaps of objects. See the additional
                         UpdateElement(nOctElement *oe,
                                        const vector3& p,
                                        const bbox3& box)
    - 14-May-04   child  Cleaned up all nOctVisitors. Put an end to all this
                         nOctree overloading and added a general nOctree *octree
                         to the nOctVisitor base class. Added Set/Get functions
                         for it, too.
                         nOctree::collect(nOctElement *oe) has been moved to
                         nOctVisitor (Visitor State).
    - 02-Jan-04   Kim    CollectByViewvol() is removed.
                         (use CollectByFrustum() function)
    - 02-Jan-04   Kim    ported from N1.
                         change the name RemElements() to RemoveElement() and
                         allocnode() to alloc_node().
*/
//--------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "gfx2/ngfxserver2.h"
#include "mathlib/vector.h"
#include "mathlib/bbox.h"

//--------------------------------------------------------------------
/**
   @class nOctElement

   @brief The interface for elements contained within an nOctree.

   nOctElements are the items administered by the nOctree. They
   are defined by their position and radius, which must NOT be
   directly manipulated.  The position and radius must be updated
   through the nOctree::UpdateElement() because each attribute
   modification could cause the tree to be reorganized.
*/
//--------------------------------------------------------------------
//class nPrimitiveServer;
class nCamera2;
class nOctNode;
class nOctElement : public nNode {
public:
    nOctNode *octnode;          ///< The containing nOctNode
    vector3 pos;                ///< The position of the element
    vector3 minCorner;
    vector3 maxCorner;
    float radius;               ///< The radius of the element
    int collect_flags;

    enum {
        N_COLLECT_VIEWVOL     = (1<<0),
        N_COLLECT_BBOX        = (1<<1),
        N_COLLECT_CLIP_PLANES = (1<<2),
    };

    //----------------------------------------------------------------
    nOctElement() {
        octnode = NULL;
        radius = 0.0f;
        collect_flags = 0;
    };
    //----------------------------------------------------------------
    ~nOctElement() {
        n_assert(octnode == NULL);
    };
    //----------------------------------------------------------------
    void Set(const vector3& p, float r) {
        n_assert(r > 0.0f);
        pos    = p;
        radius = r;
        minCorner.x = pos.x - r;
        maxCorner.x = pos.x + r;
        minCorner.y = pos.y - r;
        maxCorner.y = pos.y + r;
        minCorner.z = pos.z - r;
        maxCorner.z = pos.z + r;
    };
    //----------------------------------------------------------------
    void Set(const vector3& p, float r, const vector3& min, const vector3& max) {
        n_assert(r > 0.0f);
        pos    = p;
        radius = r;
        minCorner = min;
        maxCorner = max;
    };
    //----------------------------------------------------------------
    void SetCollectFlags(int f) {
        this->collect_flags = f;
    };
    int GetCollectFlags(void) {
        return this->collect_flags;
    };
};

//--------------------------------------------------------------------
/**
   @class nOctNode

   @brief An internal implementation class for nOctree.

   - 02-Jan-04   Kim    change the name RemElm() to RemoveElement().
*/
//--------------------------------------------------------------------
class nOctNode {
public:
    union {
        nOctNode *parent;
        nOctNode *next;
    };
    nOctNode *c[8];             ///< Children nodes
    vector3 minCorner;          ///< Bounding Box 0
    vector3 maxCorner;          ///< Bounding Box 1
    short num_elms;             ///< Number of elements in elm_list for this node.
    short all_num_elms;         ///< Total number of elements in elm_list, in this code and all children, recursively.
    nList elm_list;             ///< List of nOctElement's

    //----------------------------------------------------------------
    nOctNode() {
        parent   = NULL;
        num_elms = 0;
        all_num_elms = 0;
        memset(c,0,sizeof(c));
    };
    //----------------------------------------------------------------
    ~nOctNode() {
        n_assert(0 == all_num_elms);
        n_assert(elm_list.IsEmpty());
    };
    //----------------------------------------------------------------
    void AddElm(nOctElement *oe) {
        num_elms++;
        nOctNode *on = this;
        do {
            on->all_num_elms++;
        } while ((on=on->parent));
        oe->octnode = this;
        elm_list.AddTail(oe);
    };
    //----------------------------------------------------------------
    void RemoveElement(nOctElement *oe) {
        num_elms--;
        nOctNode *on = this;
        do {
            on->all_num_elms--;
        } while ((on=on->parent));
        oe->octnode = NULL;
        oe->Remove();
    };
    //----------------------------------------------------------------
};

//--------------------------------------------------------------------
#define N_OCT_MINX (-100000.0f)
#define N_OCT_MAXX (+100000.0f)
#define N_OCT_MINY (-100000.0f)
#define N_OCT_MAXY (+100000.0f)
#define N_OCT_MINZ (-100000.0f)
#define N_OCT_MAXZ (+100000.0f)
#define N_OCT_MAXRADIUS (199999.0f)

class nGfxServer2;
class nOctVisitor;

class nOctree : public nRoot {

    friend class nOctVisitor;

protected:
    enum {
        N_OCT_MAXNUMNODES   = 4096,     ///< max. number of elements in free_pool
        N_OCT_MAXNUMCOLLECT = 1024,     ///< max. number of collected elements
    };
    enum {
        N_OCT_CLIPX0 = (1<<0),
        N_OCT_CLIPX1 = (1<<1),
        N_OCT_CLIPY0 = (1<<2),
        N_OCT_CLIPY1 = (1<<3),
        N_OCT_CLIPZ0 = (1<<4),
        N_OCT_CLIPZ1 = (1<<5),
    };

    int subdiv_num;                 ///< Subdivision threshold.  Subdivides when the number of elements reaches this value.
    bool visualize;

    nOctNode *tree_root;
    nOctNode *free_root;
    nOctNode free_pool[N_OCT_MAXNUMNODES];

    int num_collected;
    int ext_array_size;
    nOctElement **ext_collect_array;

    nAutoRef<nGfxServer2> refGfxServer;

public:
    nOctree();
    virtual ~nOctree();

    virtual void SetSubdivNum(int);
    virtual int  GetSubdivNum(void);
    virtual void SetVisualize(bool);
    virtual bool GetVisualize(void);

    virtual void AddElement(nOctElement *);
    virtual void RemoveElement(nOctElement *);
    virtual void UpdateElement(nOctElement *, const vector3&, float);
    virtual void UpdateElement(nOctElement *oe, const vector3& p, const bbox3& box);
    virtual void BalanceTree(void);

    nOctNode* GetRoot() const;
    nGfxServer2* GetGfxServer2();

    virtual int Collect(nOctVisitor& culler, nOctElement** ext_array, int size);

    virtual void Visualize();

    void recurse_collect_nodes_with_flags(nOctNode *, int);

protected:
    nOctNode *alloc_node(nOctNode *p, float x0, float x1, float y0, float y1, float z0, float z1);
    void freenode(nOctNode *);

    bool elm_inside_node(nOctElement *, nOctNode *);
    void subdivide(nOctNode *);
    void collapse(nOctNode *);
    nOctNode *find_node_upward(nOctNode *, nOctElement *);
    nOctNode *find_node_downward(nOctNode *, nOctElement *);
    void insert_element(nOctElement *);
    void move_element(nOctElement *);
    void balance(nOctNode *);
    void visualize_node(nOctNode *);
};

//--------------------------------------------------------------------
/**
*/
inline
nOctNode* nOctree::GetRoot() const
{
    return this->tree_root;
}

//--------------------------------------------------------------------
/**
*/
inline
nGfxServer2* nOctree::GetGfxServer2()
{
    return refGfxServer.get();
}
//--------------------------------------------------------------------
#endif

