#ifndef N_OCTREE_H
#define N_OCTREE_H
//--------------------------------------------------------------------
/**
    @class nOctree

    @brief Base octree space partitioning class.    

    Administers a number nOctElement's, which are defined by
    position and radius (for the time being). nOctree is composed
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

    - 02-Jan-06   Kim    CollectByViewvol() is removed.
                         (use CollectByFrustum() function)
    - 02-Jan-04   Kim    ported from N1.
                         change the name RemElements() to RemoveElement() and
                         allocnode() to alloc_node().
*/
//--------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nOctree
#include "kernel/ndefdllclass.h"

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
    vector3 p0;
    vector3 p1;
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
        p0.x = pos.x - r;
        p1.x = pos.x + r;
        p0.y = pos.y - r;
        p1.y = pos.y + r;
        p0.z = pos.z - r;
        p1.z = pos.z + r;
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
    vector3 p0;                 ///< Bounding Box 0
    vector3 p1;                 ///< Bounding Box 1
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
class N_PUBLIC nOctree : public nRoot {
protected:
    enum {
        N_OCT_MAXNUMNODES   = 2048,     ///< max. number of elements in free_pool 
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

    matrix44 vp_matrix;
    bbox3 collect_bbox;
    int subdiv_num;                 ///< Subdivision threshold.  Subdivides when the number of elements reaches this value.
    bool visualize;

    nOctNode *tree_root;
    nOctNode *free_root;
    nOctNode free_pool[N_OCT_MAXNUMNODES];

    int num_collected;
    int ext_array_size;
    nOctElement **ext_collect_array;

    /// Clip planes for frustum collection
    vector4 clipPlanes[32];

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nOctree();
    virtual ~nOctree();

    void InitClipPlanes(const nCamera2* camera);

    virtual void SetSubdivNum(int);
    virtual int GetSubdivNum(void);
    virtual void SetVisualize(bool);
    virtual bool GetVisualize(void);

    virtual void AddElement(nOctElement *);
    virtual void RemoveElement(nOctElement *);
    virtual void UpdateElement(nOctElement *, const vector3&, float);
    virtual void BalanceTree(void);

    virtual int CollectByBBox(bbox3& bbox, nOctElement **, int);
    virtual int CollectByViewvolOrBBox(matrix44& mv, matrix44& p, bbox3& bbox, nOctElement **, int);
    virtual int CollectByFrustum(nGfxServer2* gfx_server, nOctElement**, int);

    virtual void Visualize(nGfxServer2 *);

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
    //void visualize_node(nOctNode *, nPrimitiveServer *);

    int box_clip_viewvol(vector3&, vector3&);
    int box_clip_box(vector3&, vector3&);
    void collect(nOctElement *);
    void recurse_collect_nodes_with_flags(nOctNode *, int);
    void collect_nodes_in_viewvol_or_bbox(nOctNode *);
    void recurse_collect_by_viewvol_or_bbox(nOctNode *);
    void collect_nodes_in_bbox(nOctNode *);
    void recurse_collect_by_bbox(nOctNode *);

    // CollectByFrustum methods
    void transform_clip_planes_for_frustum(nGfxServer2* gfx_server);
    void recurse_collect_within_clip_planes(nOctNode* on, unsigned int clip_mask);
    bool box_clip_against_clip_planes(vector3& p0, vector3& p1, vector4* planes,
                                      unsigned int& out_clip_mask,
                                      unsigned int in_clip_mask);
    void collect_nodes_within_clip_planes(nOctNode* on, unsigned int clip_mask);
};
//--------------------------------------------------------------------
#endif

