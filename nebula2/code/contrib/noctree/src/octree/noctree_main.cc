#define N_IMPLEMENTS nOctree
//-------------------------------------------------------------------
//  noctree_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nenv.h"

#include "octree/noctree.h"
//#include "gfx2/nprimitiveserver.h"

nNebulaScriptClass(nOctree, "nroot");

//-------------------------------------------------------------------
/**
    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
nOctree::nOctree() :
    refGfxServer("/sys/servers/gfx")
    //clipPlanes()
{
    int i;
    this->visualize  = false;
    this->subdiv_num = 4;
    this->tree_root  = NULL;
    this->free_root  = NULL;
    this->num_collected = 0;
    this->ext_array_size = 0;
    this->ext_collect_array = NULL;

    // link node to free_pool
    for (i=0; i<(N_OCT_MAXNUMNODES-1); i++) {
        this->free_pool[i].next = &(this->free_pool[i+1]);
    }
    this->free_root = this->free_pool;

    // create root-node
    this->tree_root = this->alloc_node(NULL,
                      N_OCT_MINX, N_OCT_MAXX,
                      N_OCT_MINY, N_OCT_MAXY,
                      N_OCT_MINZ, N_OCT_MAXZ);
}

//-------------------------------------------------------------------
/**
    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
nOctree::~nOctree()
{
    
    // make sure that there are no more elements in the tree
    n_assert(0 == this->tree_root->all_num_elms);
}

//-------------------------------------------------------------------
/**
    @brief Set the maxinum number of elements allowed within an
    nOctNode before the node will be subdivided and its elements
    spread out over the new subcubes.

    This will rebalance the tree.

    - 01-Jun-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::SetSubdivNum(int s)
{
    this->subdiv_num = s;
    this->BalanceTree();
}

//-------------------------------------------------------------------
/**
    @return the maximun number of elements allowed within an
    nOctNode.

    - 01-Jun-99   floh    created
*/
//-------------------------------------------------------------------
int nOctree::GetSubdivNum(void)
{
    return this->subdiv_num;
}

//-------------------------------------------------------------------
/**
    @brief Enable or disable the visualization of the tree.

    See nOctree::Visualize() for further information.

    - 01-Jun-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::SetVisualize(bool v)
{
    this->visualize = v;
}

//-------------------------------------------------------------------
/**
    @return whether or not visualization of the tree is enabled.

    - 01-Jun-99   floh    created
*/
//-------------------------------------------------------------------
bool nOctree::GetVisualize(void)
{
    return this->visualize;
}

//-------------------------------------------------------------------
/**
    Takes a new nOctNode from the free_pool and initializes
    its parent pointer and bounding box 

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
nOctNode *nOctree::alloc_node(nOctNode *p,
                             float x0, float x1,
                             float y0, float y1,
                             float z0, float z1)
{
    nOctNode *n = this->free_root;
    n_assert(n);
    this->free_root = n->next;
    n->parent = p;
    n->p0.x=x0; n->p0.y=y0; n->p0.z=z0;
    n->p1.x=x1; n->p1.y=y1; n->p1.z=z1;
    // n_printf("# new octnode %d\n",n);
    return n;
}

//-------------------------------------------------------------------
/**
    Adds an nOctNode back into the free_pool and makes sure it
    has been cleaned up properly

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::freenode(nOctNode *n)
{
    n_assert(n->elm_list.IsEmpty());
    n_assert(0 == n->num_elms);
    n_assert(NULL == n->c[0]);
    n_assert(NULL == n->c[1]);
    n_assert(NULL == n->c[2]);
    n_assert(NULL == n->c[3]);
    n_assert(NULL == n->c[4]);
    n_assert(NULL == n->c[5]);
    n_assert(NULL == n->c[6]);
    n_assert(NULL == n->c[7]);

    n->next = this->free_root;
    this->free_root = n;
    // n_printf("# freed octnode %d\n",n);
}

//-------------------------------------------------------------------
/**
    Returns true if the element completely fits into the nOctNode

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
bool nOctree::elm_inside_node(nOctElement *oe, nOctNode *n)
{
    int clip = 0;
    if (oe->p0.x < n->p0.x) clip |= N_OCT_CLIPX0;
    if (oe->p1.x > n->p1.x) clip |= N_OCT_CLIPX1;
    if (oe->p0.y < n->p0.y) clip |= N_OCT_CLIPY0;
    if (oe->p1.y > n->p1.y) clip |= N_OCT_CLIPY1;
    if (oe->p0.z < n->p0.z) clip |= N_OCT_CLIPZ0;
    if (oe->p1.z > n->p1.z) clip |= N_OCT_CLIPZ1;
    return (0 == clip);
}

//-------------------------------------------------------------------
/**
    Make sure that no child-nodes exist yet. If ok,
    create 4 nOctNodes and try to distribute all elements
    into them.

    Cause for a subdivide is, in general, that the number of
    elements in a nOctNode has exceeded 'subdiv_num'

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::subdivide(nOctNode *n)
{
    n_assert(NULL == n->c[0]);

    // n_printf("# subdividing octnode %d\n",n);

    // create new child-nodes
    float x = (n->p0.x + n->p1.x) * 0.5f;
    float y = (n->p0.y + n->p1.y) * 0.5f;
    float z = (n->p0.z + n->p1.z) * 0.5f;
    int i;
    for (i=0; i<8; i++) {
        float x0,x1,y0,y1,z0,z1;
        if (i & 1) { x0=n->p0.x; x1=x; }
        else       { x0=x; x1=n->p1.x; }
        if (i & 2) { y0=n->p0.y; y1=y; }
        else       { y0=y; y1=n->p1.y; }
        if (i & 4) { z0=n->p0.z; z1=z; }
        else       { z0=z; z1=n->p1.z; }
        n->c[i] = this->alloc_node(n,x0,x1,y0,y1,z0,z1);
    }

    // distribute elements to new childnodes
    nOctElement *oe = (nOctElement *) n->elm_list.GetHead();
    nOctElement *next_oe = NULL;
    if (oe) do {
        next_oe = (nOctElement *) oe->GetSucc();
        int j;
        for (j=0; j<8; j++) {
            // If element entirely fits into child node,
            // move it to the child node
            if (this->elm_inside_node(oe,n->c[j])) {
                n->RemoveElement(oe);
                n->c[j]->AddElm(oe);
                break;
            }
        }
    } while ((oe = next_oe));
}

//-------------------------------------------------------------------
/**
    Merges all child-nodes recursively into the specified node.
    Any elements contained in the child nodes are moved up
    into node.

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::collapse(nOctNode *n)
{
    // n_printf("# collapsing octnode %d\n",n);

    // Abort recursion if no childs found
    if (n->c[0]) {   
        // for each child...
        int i;
        for (i=0; i<8; i++) {
            // First, collapse() the childs (recursion)
            this->collapse(n->c[i]);

            // Overtake all elements from the child nodes
            nOctElement *oe;
            while ((oe = (nOctElement *) n->c[i]->elm_list.GetHead())) {
                n->c[i]->RemoveElement(oe);
                n->AddElm(oe);
            }
            
            // destroy child
            this->freenode(n->c[i]);
            n->c[i] = NULL;
        }
    }
}

//-------------------------------------------------------------------
/**
    Starting at the specified nOctNode, searches the first node
    towards the root which is able to entirely contain the 
    nOctElement.

    Will return NULL if the element doesn't even fit in the
    root node!

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
nOctNode *nOctree::find_node_upward(nOctNode *n, nOctElement *oe)
{
    while (n && (!this->elm_inside_node(oe,n))) n = n->parent;
    return n;
}

//-------------------------------------------------------------------
/**
    Beginning at the specified nOctNode, looks for the smallest
    child that still is able to entirely contain the nOctElement.
    Search takes place until the current node doesn't contain
    any children anymore or none that could entirely contain the
    nOctElement

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
nOctNode *nOctree::find_node_downward(nOctNode *n, nOctElement *oe)
{
    // outer loop: abort if no children
    while (n->c[0]) {
        int i;
        for (i=0; i<8; i++) {
            // If element entirely fits into current child-node,
            // go down into the node
            if (this->elm_inside_node(oe,n->c[i])) {
                n = n->c[i];
                break;
            }
        }
        
        // If we reach this point, then, althought there
        // are children, none of them is big enough to contain
        // the nOctElement
        if (i==8) break;
    }
    return n;
}

//-------------------------------------------------------------------
/**
    Adds a new element into the tree by searching downwards
    for the first node entirely covering the element and
    linking it into the node.

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::insert_element(nOctElement *oe)
{
    n_assert(NULL == oe->octnode);
    nOctNode *on = this->find_node_downward(this->tree_root,oe);
    // n_printf("# adding elm %d to octnode %d\n",oe,on);
    on->AddElm(oe);
}

//-------------------------------------------------------------------
/**
    Ensured that the nOctElement is located at the right position
    in the tree. The element has to be in the tree already.
    First, does a find_node_upward() and from there, in general,
    a find_node_downward().

    If the resulting node doesn't match the current node of
    the element, the element is moved from its current node
    into the new node.

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::move_element(nOctElement *oe)
{
    n_assert(oe->octnode);
    
    nOctNode *on = oe->octnode;

    // Search for the first node towards root which
    // is able to entirely contain the element...
    on = this->find_node_upward(on,oe);

    // from the, search downward again...
    if (!on) on = this->tree_root;
    on = this->find_node_downward(on,oe);

    // 'on' ist now guaranteed to be the correct node.
    // Now look if the element has to be noved...
    if (on != oe->octnode) {
        // it is... Move element from its current into the new node
        // n_printf("# moving elm %d from octnode %d to octnode %d\n",oe,oe->octnode,on);
        oe->octnode->RemoveElement(oe);
        on->AddElm(oe);
    }
}

//-------------------------------------------------------------------
/**
    Rebalances the node and all of its child nodes:
      - if the node has child nodes and the total number of
        elements in the node is smaller then the threshold,
        it is collapsed with all of its subnodes
      - else, if it is a leaf node and the number of elements
        is larger then the threshold, the node gets subdivided
      - finally, balance() is recursively called on each child node

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::balance(nOctNode *on)
{
    if ((NULL != on->c[0]) && (on->all_num_elms <= this->subdiv_num)) {
        // collapse all child nodes
        this->collapse(on);
    } else if ((NULL == on->c[0]) && (on->num_elms > this->subdiv_num)) {
        // to many elements in leaf node -> subdivide
        this->subdivide(on);
    }

    // balance() recursively on all children
    if (on->c[0]) {
        int i;
        for (i=0; i<8; i++) this->balance(on->c[i]);
    }
}

//-------------------------------------------------------------------
/**
    @brief Add a new empty element to the tree, which is initialized
    with position (0,0,0) and radius (N_OCT_MAXRADIUS).

    Until this is initialized with actual values by nOctree::UpdateElement(),
    this node will live in the root node of the nOctree.

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::AddElement(nOctElement *oe)
{
    n_assert(NULL == oe->octnode);
    vector3 null;
    oe->Set(null,N_OCT_MAXRADIUS);
    this->insert_element(oe);
}

//-------------------------------------------------------------------
/**
    @brief Removes an element from the tree.

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::RemoveElement(nOctElement *oe)
{
    n_assert(oe->octnode);
    oe->octnode->RemoveElement(oe);
}

//-------------------------------------------------------------------
/**
    @brief Update position and/or radius of an item.
    
    The item is sorted into the tree, however the tree should
    be balanced again prior to executing another CollectX(), so that
    the collection process to some extent brings optimal results.

    - 31-May-99   floh    created
    - 04-Jun-99   floh    Radius 0.0 wird jetzt auf MaxRadius gesetzt
*/
//-------------------------------------------------------------------
void nOctree::UpdateElement(nOctElement *oe, const vector3& p, float r)
{
    n_assert(oe->octnode);
    if (r <= 0.0f) r=N_OCT_MAXRADIUS;
    oe->Set(p,r);
    this->move_element(oe);
}

//-------------------------------------------------------------------
/**
    @brief Balances the tree.

    This should always be run directly before a collection is made,
    so that the collection process runs in optimal time and the
    smallest possible set is returned. 

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void nOctree::BalanceTree(void)
{
    n_assert(this->tree_root);
    this->balance(this->tree_root);
}

//-------------------------------------------------------------------
/**
    Renders a wireframe cube

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
/*
static void cube(nPrimitiveServer *prim,
                 float x0, float y0, float z0,
                 float x1, float y1, float z1)
{
    prim->Coord(x0,y0,z0); prim->Coord(x1,y0,z0);
    prim->Coord(x1,y0,z0); prim->Coord(x1,y1,z0);
    prim->Coord(x1,y1,z0); prim->Coord(x0,y1,z0);
    prim->Coord(x0,y1,z0); prim->Coord(x0,y0,z0);

    prim->Coord(x0,y0,z1); prim->Coord(x1,y0,z1);
    prim->Coord(x1,y0,z1); prim->Coord(x1,y1,z1);
    prim->Coord(x1,y1,z1); prim->Coord(x0,y1,z1);
    prim->Coord(x0,y1,z1); prim->Coord(x0,y0,z1);

    prim->Coord(x0,y0,z0); prim->Coord(x0,y0,z1);
    prim->Coord(x1,y0,z0); prim->Coord(x1,y0,z1);
    prim->Coord(x1,y1,z0); prim->Coord(x1,y1,z1);
    prim->Coord(x0,y1,z0); prim->Coord(x0,y1,z1);
}
*/
//-------------------------------------------------------------------
/**
    Visualizes a node. For each nOctNode a green cube is drawn.
    All elements in the node are drawn as red boxes which are
    connected to the center of the node with an orange line.
    After that, recursively visualizes all of its children

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
/*
void nOctree::visualize_node(nOctNode *on, nPrimitiveServer *prim)
{
    float x0 = on->p0.x;
    float x1 = on->p1.x;
    float y0 = on->p0.y;
    float y1 = on->p1.y;
    float z0 = on->p0.z;
    float z1 = on->p1.z;
    float x = (x0 + x1)*0.5f;
    float y = (y0 + y1)*0.5f;
    float z = (z0 + z1)*0.5f;

    // nOctNode as green cube
    prim->Rgba(0.0f, 0.7f, 0.0f, 1.0f);
    cube(prim, x0,y0,z0,x1,y1,z1);

    // Draw each element...
    nOctElement *oe;
    for (oe = (nOctElement *) on->elm_list.GetHead();
         oe;
         oe = (nOctElement *) oe->GetSucc())
    {
        x0 = oe->p0.x;
        x1 = oe->p1.x;
        y0 = oe->p0.y;
        y1 = oe->p1.y;
        z0 = oe->p0.z;
        z1 = oe->p1.z;

        prim->Rgba(1.0f, 0.0f, 0.0f, 1.0f);
        cube(prim, x0,y0,z0,x1,y1,z1);
       
        // Line to the node's center      
        prim->Rgba(0.7f, 0.7f, 0.0f, 1.0f);
        prim->Coord(x,y,z); prim->Coord(x0,y0,z0);
        prim->Coord(x,y,z); prim->Coord(x1,y1,z1);
    }

    // and recurse all children
    if (on->c[0]) {
        int i;
        for (i=0; i<8; i++) this->visualize_node(on->c[i], prim);
    }
}
*/
//-------------------------------------------------------------------
/**
    @brief Render a visualization of the nOctree.
    
    The nOctNodes are drawn in green, with their nOctElements as red
    squares, which point with oranges lines to the center of the nOctNode,
    in which they're situated.

    This only appears under the OpenGL renderer.

    - 31-May-99   floh    created
    - 06-Jul-99   floh    stellt jetzt Renderstates etc. selbst ein
*/
//-------------------------------------------------------------------
/*
void nOctree::Visualize(nGfxServer *gs)
{
    if (this->visualize) {
        n_assert(this->tree_root);
        nPrimitiveServer *prim;
        prim = (nPrimitiveServer *)this->ks->Lookup("/sys/servers/primitive");
        if (prim) {
            prim->EnableLighting(false);
            prim->Begin(N_PTYPE_LINE_LIST);
            matrix44 vwr;
            gs->GetMatrix(N_MXM_VIEWER,vwr);
            vwr.invert_simple();
            gs->SetMatrix(N_MXM_MODELVIEW,vwr);
            this->visualize_node(this->tree_root, prim);
            prim->End();
        }
    }
}
*/
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

