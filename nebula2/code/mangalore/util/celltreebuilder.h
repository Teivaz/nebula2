#ifndef UTIL_CELLTREEBUILDER_H
#define UTIL_CELLTREEBUILDER_H
//------------------------------------------------------------------------------
/**
    @class Util::CellTreeBuilder

    Attaches a cell structure to a graphics level.
  
    FIXME: It would certainly be more efficient to implement the actual quad 
    tree culling stuff withing a Cell class. The Nebula2 quadtree class
    is definitely better then a simple tree of Cells.
    
    (C) 2005 Radon Labs GmbH
*/
#include "kernel/nref.h"
#include "graphics/level.h"
#include "util/nquadtree.h"
#include "graphics/cell.h"

//------------------------------------------------------------------------------
namespace Util
{
class CellTreeBuilder
{
public:
    /// constructor
    CellTreeBuilder();
    /// build a quad tree
    void BuildQuadTree(Graphics::Level* level, uint depth, const bbox3& box);
    /// returns number of cells created
    int GetNumCellsBuilt() const;

private:
    /// create a quad tree and its children
    Graphics::Cell* CreateQuadTreeCell(Graphics::Cell* parentCell, uint curLevel, uint curCol, uint curRow);
   
    nQuadTree quadTree;
    int numCellsBuilt;
};

}; // namespace Util
//------------------------------------------------------------------------------
#endif