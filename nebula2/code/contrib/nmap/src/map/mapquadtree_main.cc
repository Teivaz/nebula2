//-------------------------------------------------------------------
//  noctree_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nenv.h"
#include "gfx2/ngfxserver2.h"
#include "map/mapquadtree.h"
#include "map/nmap.h"

//-------------------------------------------------------------------
/**
    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
MapQuadtree::MapQuadtree(nMap* map) :
    tree_root(NULL),
    num_collected(0),
    array_size(N_OCT_MAXNUMCOLLECT),
    collect_array(),
    clipPlanes(),
    max_height(map->GetHeightRangeMax()),
    min_height(map->GetHeightRangeMin())
{
    this->subdiv_num = 4;
    this->free_root  = NULL;

    // link node to free_pool
    for (int i = 0;  i < (N_OCT_MAXNUMNODES-1); i++)
        this->free_pool[i].next = &(this->free_pool[i+1]);
    this->free_root = this->free_pool;

    // Figure out bounding box
    float map_size = map->GetDimension() * map->GetGridInterval();

    // create root-node
    this->tree_root = this->allocnode(NULL,
                                      0.0f, map_size,
                                      min_height, max_height,
                                      0.0f, map_size);
}

//-------------------------------------------------------------------
/**
    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
MapQuadtree::~MapQuadtree()
{
    
    // make sure that there are no more elements in the tree
    n_assert(0 == this->tree_root->all_num_elms);
}

//-------------------------------------------------------------------
/**
    @brief Set the maxinum number of elements allowed within an
    MapQuadNode before the node will be subdivided and its elements
    spread out over the new subcubes.

    This will rebalance the tree.

    - 01-Jun-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::SetSubdivNum(int s)
{
    this->subdiv_num = s;
    this->BalanceTree();
}

//-------------------------------------------------------------------
/**
    @return the maximun number of elements allowed within an
    MapQuadNode.

    - 01-Jun-99   floh    created
*/
//-------------------------------------------------------------------
int MapQuadtree::GetSubdivNum(void)
{
    return this->subdiv_num;
}

//-------------------------------------------------------------------
/**
    Takes a new MapQuadNode from the free_pool and initializes
    its parent pointer and bounding box 

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
MapQuadNode *MapQuadtree::allocnode(MapQuadNode *p,
                             float x0, float x1,
                             float y0, float y1,
                             float z0, float z1)
{
    MapQuadNode *n = this->free_root;
    n_assert(n);
    this->free_root = n->next;
    n->parent = p;
    n->p0.x=x0; n->p0.y=y0; n->p0.z=z0;
    n->p1.x=x1; n->p1.y=y1; n->p1.z=z1;
    //n->pos = (n->p0 + n->p1) / 2.0f;
	n->pos = (n->p0 + n->p1) * 0.5f;
    // n_printf("# new octnode %d\n",n);
    return n;
}

//-------------------------------------------------------------------
/**
    Adds an MapQuadNode back into the free_pool and makes sure it
    has been cleaned up properly

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::freenode(MapQuadNode *n)
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
    Returns true if the element completely fits into the MapQuadNode

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
bool MapQuadtree::elm_inside_node(MapQuadElement *oe, MapQuadNode *n)
{
    if (oe->p0.x < n->p0.x || oe->p1.x > n->p1.x ||
        oe->p0.y < n->p0.y || oe->p1.y > n->p1.y ||
        oe->p0.z < n->p0.z || oe->p1.z > n->p1.z)
        return false;
    return true;
}

//-------------------------------------------------------------------
/**
    Make sure that no child-nodes exist yet. If ok,
    create 4 nOctNodes and try to distribute all elements
    into them.

    Cause for a subdivide is, in general, that the number of
    elements in a MapQuadNode has exceeded 'subdiv_num'

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::subdivide(MapQuadNode *n)
{
    n_assert(NULL == n->c[0]);

    // n_printf("# subdividing octnode %d\n",n);

    // create new child-nodes
    float x = (n->p0.x + n->p1.x) * 0.5f;
    // float y = (n->p0.y + n->p1.y) * 0.5f;
    float y0 = n->p0.y;
    float y1 = n->p1.y;
    float z = (n->p0.z + n->p1.z) * 0.5f;
    for (int i = 0; i < 4; ++i) {
        float x0,x1,z0,z1;
        if (i & 1) { x0=n->p0.x; x1=x; }
        else       { x0=x; x1=n->p1.x; }
        if (i & 2) { z0=n->p0.z; z1=z; }
        else       { z0=z; z1=n->p1.z; }
        n->c[i] = this->allocnode(n,x0,x1,y0,y1,z0,z1);
    }

    // distribute elements to new childnodes
    MapQuadElement *oe = (MapQuadElement *) n->elm_list.GetHead();
    MapQuadElement *next_oe = NULL;
    if (oe) do {
        next_oe = (MapQuadElement *) oe->GetSucc();
        for (int j = 0; j < 4; ++j) {
            // If element entirely fits into child node,
            // move it to the child node
            if (this->elm_inside_node(oe,n->c[j])) {
                n->RemElm(oe);
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
void MapQuadtree::collapse(MapQuadNode *n)
{
    // n_printf("# collapsing octnode %d\n",n);

    // Abort recursion if no childs found
    if (n->c[0]) {   
        // for each child...
        for (int i=0; i<4; i++) {
            // First, collapse() the childs (recursion)
            this->collapse(n->c[i]);

            // Overtake all elements from the child nodes
            MapQuadElement *oe;
            while ((oe = (MapQuadElement *) n->c[i]->elm_list.GetHead())) {
                n->c[i]->RemElm(oe);
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
    Starting at the specified MapQuadNode, searches the first node
    towards the root which is able to entirely contain the 
    MapQuadElement.

    Will return NULL if the element doesn't even fit in the
    root node!

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
MapQuadNode *MapQuadtree::find_node_upward(MapQuadNode *n, MapQuadElement *oe)
{
    while (n && (!this->elm_inside_node(oe,n))) n = n->parent;
    return n;
}

//-------------------------------------------------------------------
/**
    Beginning at the specified MapQuadNode, looks for the smallest
    child that still is able to entirely contain the MapQuadElement.
    Search takes place until the current node doesn't contain
    any children anymore or none that could entirely contain the
    MapQuadElement

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
MapQuadNode *MapQuadtree::find_node_downward(MapQuadNode *n, MapQuadElement *oe)
{
    int i;
    // outer loop: abort if no children
    while (n->c[0]) {
        for (i=0; i<4; i++) {
            // If element entirely fits into current child-node,
            // go down into the node
            if (this->elm_inside_node(oe,n->c[i])) {
                n = n->c[i];
                break;
            }
        }
        
        // If we reach this point, then, althought there
        // are children, none of them is big enough to contain
        // the MapQuadElement
        if (i==4) break;
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
void MapQuadtree::insert_element(MapQuadElement *oe)
{
    n_assert(NULL == oe->octnode);
    MapQuadNode *on = this->find_node_downward(this->tree_root,oe);
    // n_printf("# adding elm %d to octnode %d\n",oe,on);
    on->AddElm(oe);
}

//-------------------------------------------------------------------
/**
    Ensured that the MapQuadElement is located at the right position
    in the tree. The element has to be in the tree already.
    First, does a find_node_upward() and from there, in general,
    a find_node_downward().

    If the resulting node doesn't match the current node of
    the element, the element is moved from its current node
    into the new node.

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::move_element(MapQuadElement *oe)
{
    n_assert(oe->octnode);
    
    MapQuadNode *on = oe->octnode;

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
        oe->octnode->RemElm(oe);
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
void MapQuadtree::balance(MapQuadNode *on)
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
        for (int i=0; i<4; i++) this->balance(on->c[i]);
    }
}

//-------------------------------------------------------------------
/**
    @brief Add a new empty element to the tree, which is initialized
    with position (0,0,0) and radius (N_OCT_MAXRADIUS).

    Until this is initialized with actual values by MapQuadtree::UpdateElement(),
    this node will live in the root node of the MapQuadtree.

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::AddElement(MapQuadElement *oe, const vector3& min, const vector3& max)
{
    n_assert(NULL == oe->octnode);
    oe->Set(min, max);
    this->insert_element(oe);
}

//-------------------------------------------------------------------
/**
    @brief Removes an element from the tree.

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::RemElement(MapQuadElement *oe)
{
    n_assert(oe->octnode);
    oe->octnode->RemElm(oe);
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
void MapQuadtree::UpdateElement(MapQuadElement* map_elm, const vector3& min, const vector3& max)
{
    n_assert(NULL != map_elm->octnode);
    map_elm->Set(min, max);
    this->move_element(map_elm);
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
void MapQuadtree::BalanceTree(void)
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
static void cube(nGfxServer2 *gs,
                 float x0, float y0, float z0,
                 float x1, float y1, float z1)
{
#if 0
    gs->Coord(x0,y0,z0); gs->Coord(x1,y0,z0);
    gs->Coord(x1,y0,z0); gs->Coord(x1,y1,z0);
    gs->Coord(x1,y1,z0); gs->Coord(x0,y1,z0);
    gs->Coord(x0,y1,z0); gs->Coord(x0,y0,z0);

    gs->Coord(x0,y0,z1); gs->Coord(x1,y0,z1);
    gs->Coord(x1,y0,z1); gs->Coord(x1,y1,z1);
    gs->Coord(x1,y1,z1); gs->Coord(x0,y1,z1);
    gs->Coord(x0,y1,z1); gs->Coord(x0,y0,z1);

    gs->Coord(x0,y0,z0); gs->Coord(x0,y0,z1);
    gs->Coord(x1,y0,z0); gs->Coord(x1,y0,z1);
    gs->Coord(x1,y1,z0); gs->Coord(x1,y1,z1);
    gs->Coord(x0,y1,z0); gs->Coord(x0,y1,z1);
#endif
}

//-------------------------------------------------------------------
/**
    Visualizes a node. For each MapQuadNode a green cube is drawn.
    All elements in the node are drawn as red boxes which are
    connected to the center of the node with an orange line.
    After that, recursively visualizes all of its children

    - 31-May-99   floh    created
*/
//-------------------------------------------------------------------
void MapQuadtree::visualize_node(nGfxServer2 *gs, MapQuadNode *on)
{
#if 0
    gs->Begin(N_PTYPE_LINE_LIST);
    float x0 = on->p0.x;
    float x1 = on->p1.x;
    float y0 = on->p0.y;
    float y1 = on->p1.y;
    float z0 = on->p0.z;
    float z1 = on->p1.z;
    float x = (x0 + x1)*0.5f;
    float y = (y0 + y1)*0.5f;
    float z = (z0 + z1)*0.5f;

    // MapQuadNode as green cube
    gs->Rgba(0.0f, 0.7f, 0.0f, 1.0f);
    cube(gs,x0,y0,z0,x1,y1,z1);

    // Draw each element...
    MapQuadElement *oe;
    for (oe = (MapQuadElement *) on->elm_list.GetHead();
         oe;
         oe = (MapQuadElement *) oe->GetSucc())
    {
        x0 = oe->p0.x;
        x1 = oe->p1.x;
        y0 = oe->p0.y;
        y1 = oe->p1.y;
        z0 = oe->p0.z;
        z1 = oe->p1.z;
        gs->Rgba(1.0f, 0.0f, 0.0f, 1.0f);
        cube(gs,x0,y0,z0,x1,y1,z1);
       
        // Line to the node's center      
        gs->Rgba(0.7f, 0.7f, 0.0f, 1.0f);
        gs->Coord(x,y,z); gs->Coord(x0,y0,z0);
        gs->Coord(x,y,z); gs->Coord(x1,y1,z1);
    }

    // and recurse all children
    if (on->c[0]) {
        for (int i=0; i<4; i++) this->visualize_node(gs,on->c[i]);
    }
    gs->End();
#endif
}

//-------------------------------------------------------------------
/**
    @brief Render a visualization of the MapQuadtree.
    
    The nOctNodes are drawn in green, with their nOctElements as red
    squares, which point with oranges lines to the center of the MapQuadNode,
    in which they're situated.

    This only appears under the OpenGL renderer.

    - 31-May-99   floh    created
    - 06-Jul-99   floh    stellt jetzt Renderstates etc. selbst ein
*/
//-------------------------------------------------------------------
void MapQuadtree::Visualize(nGfxServer2 *gs)
{
    n_assert(this->tree_root);
    matrix44 vwr;
//TODO set render state not implemented
#if 0
    nRState rs;
    rs.Set(N_RS_LIGHTING,N_FALSE); gs->SetState(rs);
    rs.Set(N_RS_ZFUNC, N_CMP_LESSEQUAL); gs->SetState(rs);
    gs->GetMatrix(N_MXM_VIEWER,vwr);
    vwr.invert_simple();
    gs->SetMatrix(N_MXM_MODELVIEW,vwr);
    this->visualize_node(gs,this->tree_root);
    rs.Set(N_RS_LIGHTING,N_TRUE); gs->SetState(rs);
#endif

	vwr = gs->GetTransform(nGfxServer2::View);
	vwr.invert_simple();
	gs->SetTransform(nGfxServer2::ModelView,vwr);
	this->visualize_node(gs, this->tree_root);
}
