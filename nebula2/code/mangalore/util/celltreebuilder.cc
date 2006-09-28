//------------------------------------------------------------------------------
//  util/celltreebuilder.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "util/celltreebuilder.h"
#include "util/nquadtree.h"
#include "graphics/level.h"
#include "graphics/cell.h"
#include "foundation/factory.h"

namespace Util
{

//------------------------------------------------------------------------------
/**
*/
CellTreeBuilder::CellTreeBuilder() :
    numCellsBuilt(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create a quad tree of Graphics::Cells and attach them to the
    provided level.
*/
void
CellTreeBuilder::BuildQuadTree(Graphics::Level* level, uint depth, const bbox3& box)
{
    // construct a Nebula2 quad tree object
    this->quadTree.Initialize(depth, box);
    this->numCellsBuilt = 0;

    // walk the tree and construct cells
    Ptr<Graphics::Cell> rootCell = this->CreateQuadTreeCell(0, 0, 0, 0);
    level->SetRootCell(rootCell);
}

//------------------------------------------------------------------------------
/**
    Initialize one quad tree level. Will be called recursively.
*/
Graphics::Cell*
CellTreeBuilder::CreateQuadTreeCell(Graphics::Cell* parentCell, uint curLevel, uint curCol, uint curRow)
{
    // create a new cell
	Graphics::Cell* cell = Graphics::Cell::Create();
    int nodeIndex = this->quadTree.GetNodeIndex(curLevel, curCol, curRow);
    const nQuadTree::Node& node = this->quadTree.GetNodeByIndex(nodeIndex);
    matrix44 cellTransform;
    cellTransform.translate(node.GetBoundingBox().center());
    cell->SetTransform(cellTransform);
    cell->SetBox(node.GetBoundingBox());
    this->numCellsBuilt++;

    // create child cells
    uint childLevel = curLevel + 1;
    if (childLevel < this->quadTree.GetDepth())
    {
        cell->BeginChildCells(4);
        for (int i = 0; i < 4; i++)
        {
            uint childCol = 2 * curCol + (i & 1);
            uint childRow = 2 * curRow + ((i & 2) >> 1);
            Ptr<Graphics::Cell> childCell = this->CreateQuadTreeCell(cell, childLevel, childCol, childRow);
            cell->AddChildCell(i, childCell);
        }
        cell->EndChildCells();
    }
    return cell;
}

//------------------------------------------------------------------------------
/**
    Returns the number of cells that have been created.
*/
int
CellTreeBuilder::GetNumCellsBuilt() const
{
    return this->numCellsBuilt;
}

}
