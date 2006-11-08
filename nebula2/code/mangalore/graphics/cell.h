#ifndef GRAPHICS_CELL_H
#define GRAPHICS_CELL_H
//------------------------------------------------------------------------------
/**
    @class Graphics::Cell

    A Cell object defines a culling group for cells and entities. If a cell
    is not visible, none of the enclosed cells and entities will be visible.

    Cells can be arranged hierarchically.

    Cells are placed into the world by position, orientation and size
    (basically, a simple 4x4 matrix).

    Cells have the complete rendering control over the enclosed objects.
    Subclasses of Cell can implement different culling algorithms.

    Cells can have dynamic graphics entities attached and removed.
    Dynamic entities represent all movable/destroyable graphics
    entities.

    Cells position and hierarchies are static.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "mathlib/matrix.h"
#include "mathlib/bbox.h"
#include "graphics/entity.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class LightEntity;
class CameraEntity;
class Level;

class Cell : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Cell);

public:
    /// visibility status
    enum ClipStatus
    {
        InvalidClipStatus,  ///< visibility status invalid
        Inside,             ///< cell is inside clip volume
        Outside,            ///< cell is outside clip volume
        Clipped,            ///< cell is partly in clip volume
    };

    /// constructor
    Cell();
    /// destructor
    virtual ~Cell();
    /// called when cell is attached to level
    virtual void OnAttachToLevel(Level* l);
    /// called when cell is removed from level
    virtual void OnRemoveFromLevel();
    /// get pointer to parent cell
    Cell* GetParentCell() const;
    /// begin adding child cells
    void BeginChildCells(int num);
    /// add a child cell (bumps refcount)
    void AddChildCell(int index, Cell* cell);
    /// finish adding child cells
    void EndChildCells();
    /// get number of child cells
    int GetNumChildCells() const;
    /// get child cell at index
    Cell* GetChildCellAt(int index) const;
    /// attach an entity to this cell
    void AttachEntity(Entity* entity);
    /// remove an entity from this cell
    void RemoveEntity(Entity* entity);
    /// insert an entity into the tree of cells
    void InsertEntity(Entity* entity);
    /// get number of entities in a pool
    int GetNumEntities(Entity::Type type) const;
    /// get entity by index
    Entity* GetEntityAt(Entity::Type, int index) const;
    /// get number of entities of all types in hierarchy (including self)
    int GetNumEntitiesInHierarchy() const;
    /// get number of entities in hierarchy by entity type
    int GetNumEntitiesInHierarchyByType(Entity::Type type) const;
    /// get number of entities in hierarchy by combined types field
    int GetNumEntitiesInHierarchyByCombinedTypes(uint ordTypes) const;
    /// set transform of cell
    void SetTransform(const matrix44& transform);
    /// get transform of cell
    const matrix44& GetTransform() const;
    /// set the world space bounding box of the cell
    void SetBox(const bbox3& b);
    /// get the world space bounding box of the cell
    const bbox3& GetBox() const;
    /// starting from this cell, find smallest containment cell in cell tree
    Cell* FindEntityContainmentCell(Entity* entity);
    /// recursively clear links
    void ClearLinks(Entity::LinkType linkType);
    /// recursively create links between observers and observed entities
    void UpdateLinks(Entity* observerEntity, Entity::Type observedType, Entity::LinkType linkType);
    /// render a debug visualization of cell and contained entities
    void RenderDebug();
    /// get the debug color for this cell
    const vector4& GetDebugColor() const;

private:
    /// create links between visible entities
    void LinkVisibleEntities(Entity* observerEntity, Entity::Type observedType, Entity::LinkType linkType, Entity::ClipStatus clipStatus);
    /// set parent cell pointer
    void SetParentCell(Cell* cell);
    /// increment/decrement the numEntitiesInHierarchy counter (including in all parent cells)
    void UpdateNumEntitiesInHierarchy(Entity::Type type, int num);

    Cell* parentCell;                           // note: normal pointer to avoid refcount cycle
    nFixedArray<Ptr<Cell> > childCellArray;
    matrix44 transform;
    bbox3 bbox;
    vector4 debugColor;
    int numEntitiesInHierarchyAllTypes;
    int numEntitiesInHierarchy[Entity::NumTypes]; // overall number of entities in this cell and all child cells
    nArray<Ptr<Entity> > entityPool[Entity::NumTypes];
};

RegisterFactory(Cell);

//------------------------------------------------------------------------------
/**
    Returns overall number of entities in this cell and all child cells.
*/
inline
int
Cell::GetNumEntitiesInHierarchy() const
{
    return this->numEntitiesInHierarchyAllTypes;
}

//------------------------------------------------------------------------------
/**
    Returns overall number of entities in this cell and all child cells for
    a given type.
*/
inline
int
Cell::GetNumEntitiesInHierarchyByType(Entity::Type type) const
{
    n_assert((type >= 0) && (type < Entity::NumTypes));
    return this->numEntitiesInHierarchy[type];
}

//------------------------------------------------------------------------------
/**
    Returns overall number of entities in this cell and all child cells for
    a combination of types (encode type by (1<<type) and OR them together).
*/
inline
int
Cell::GetNumEntitiesInHierarchyByCombinedTypes(uint ordTypes) const
{
    int curType;
    int numEntities = 0;
    for (curType = 0; curType < Entity::NumTypes; curType++)
    {
        if (0 != ((1<<curType) & ordTypes))
        {
            numEntities += this->GetNumEntitiesInHierarchyByType((Entity::Type) curType);
        }
    }
    return numEntities;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
Cell::GetDebugColor() const
{
    return this->debugColor;
}

//------------------------------------------------------------------------------
/**
    Set the parent cell pointer. This is a private method called
    from within AddChildCell() only!

    @param  cell    pointer to parent cell
*/
inline
void
Cell::SetParentCell(Cell* cell)
{
    n_assert(0 != cell);
    this->parentCell = cell;
}

//------------------------------------------------------------------------------
/**
    Get pointer to the parent cell. The pointer can be 0 if this is a root
    level cell.

    @return     pointer to parent cell or 0
*/
inline
Cell*
Cell::GetParentCell() const
{
    return this->parentCell;
}

//------------------------------------------------------------------------------
/**
    Get number of child cells.

    @return     number of child cells
*/
inline
int
Cell::GetNumChildCells() const
{
    return this->childCellArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get pointer to child cell at index.

    @param  index   child cell index
    @return         pointer to child cell
*/
inline
Cell*
Cell::GetChildCellAt(int index) const
{
    n_assert(childCellArray[index] != 0)
    return this->childCellArray[index];
}

//------------------------------------------------------------------------------
/**
    Get number of entities by type

    @param  type    the entity type
    @return         number of entities in pool
*/
inline
int
Cell::GetNumEntities(Entity::Type type) const
{
    return this->entityPool[type].Size();
}

//------------------------------------------------------------------------------
/**
    Get pointer to entity by type at index.

    @param  type    the entity type
    @param  index   static entity index
    @return         pointer to static entity
*/
inline
Entity*
Cell::GetEntityAt(Entity::Type type, int index) const
{
    n_assert(entityPool[type][index] != 0);
    return this->entityPool[type][index];
}

//------------------------------------------------------------------------------
/**
    Set the transformation matrix of the cell. Since Cells are by
    definition static, this should only be called once by the level loader.

    @param  m   the transformation matrix
*/
inline
void
Cell::SetTransform(const matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
    Get the transformation matrix of the cell.

    @return     the transformation matrix
*/
inline
const matrix44&
Cell::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
    Set the bounding box of the cell in global space. This should only be called
    once by the level loader.

    @param  b   the cell's bounding box
*/
inline
void
Cell::SetBox(const bbox3& b)
{
    this->bbox = b;
}

//------------------------------------------------------------------------------
/**
    Get the bounding box of the cell in global space.

    @return     the cell's bounding box
*/
inline
const bbox3&
Cell::GetBox() const
{
    return this->bbox;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
