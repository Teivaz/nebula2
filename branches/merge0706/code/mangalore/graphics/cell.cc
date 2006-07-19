//------------------------------------------------------------------------------
//  graphics/cell.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/cell.h"
#include "graphics/server.h"
#include "graphics/cameraentity.h"
#include "graphics/lightentity.h"

namespace Graphics
{
ImplementRtti(Graphics::Cell, Foundation::RefCounted);
ImplementFactory(Graphics::Cell);

//------------------------------------------------------------------------------
/**
*/
Cell::Cell() :
    parentCell(0),
    numEntitiesInHierarchyAllTypes(0)
{
    // initialize dynamic entity arrays and entity counters
    int poolIndex;
    for (poolIndex = 0; poolIndex < Entity::NumTypes; poolIndex++)
    {
        this->entityPool[poolIndex].SetFlags(nArray<Ptr<Entity> >::DoubleGrowSize);
        this->numEntitiesInHierarchy[poolIndex] = 0;
    }

    // create a random debug color
    this->debugColor.set(n_rand(), n_rand(), n_rand(), 0.3f);
}

//------------------------------------------------------------------------------
/**
*/
Cell::~Cell()
{
    // release child cells
    this->childCellArray.Clear(0);

    // release attached graphics entities
    int poolIndex;
    for (poolIndex = 0; poolIndex < Entity::NumTypes; poolIndex++)
    {
        int num = this->entityPool[poolIndex].Size();
        for (int i = 0; i < num; i++)
        {
            this->UpdateNumEntitiesInHierarchy(this->entityPool[poolIndex][i]->GetType(), -1);
            this->entityPool[poolIndex][i]->OnRemovedFromCell();
            this->entityPool[poolIndex][i] = 0;
        }
    }
    n_assert(0 == this->numEntitiesInHierarchyAllTypes);
}

//------------------------------------------------------------------------------
/**
    This updates the numEntitiesInHierarchy member in this cell and all parent
    cells.
*/
void
Cell::UpdateNumEntitiesInHierarchy(Entity::Type type, int num)
{
    n_assert((type >= 0) && (type < Entity::NumTypes));

    this->numEntitiesInHierarchyAllTypes += num;
    this->numEntitiesInHierarchy[type] += num;
    n_assert(this->numEntitiesInHierarchyAllTypes >= 0);
    Cell* p = this->parentCell;
    if (p) do
    {
        p->numEntitiesInHierarchyAllTypes += num;
        p->numEntitiesInHierarchy[type] += num;
        n_assert(p->numEntitiesInHierarchyAllTypes >= 0);
    }
    while ((p = p->parentCell));
}

//------------------------------------------------------------------------------
/**
    Called when the cell is attached to a level.
*/
void
Cell::OnAttachToLevel(Level* l)
{
    // distribute to child cells
    int i;
    int num = this->GetNumChildCells();
    for (i = 0; i < num; i++)
    {
        this->GetChildCellAt(i)->OnAttachToLevel(l);
    }
}

//------------------------------------------------------------------------------
/**
    Called when cell is removed from level.
*/
void
Cell::OnRemoveFromLevel()
{
    // distribute to child cells
    int i;
    int num = this->GetNumChildCells();
    for (i = 0; i < num; i++)
    {
        this->GetChildCellAt(i)->OnRemoveFromLevel();
    }
}

//------------------------------------------------------------------------------
/**
    Start adding child cells. This should be called only once by the
    level loader. Child cells are owned by their parent cell. Adding child
    cells will increment the refcount of the child cell.

    @param  num     number of child cells which will be added
*/
void
Cell::BeginChildCells(int num)
{
    n_assert(num > 0);
    n_assert(0 == this->childCellArray.Size());
    
    this->childCellArray.SetSize(num);
    int i;
    for (i = 0; i < num; i++)
    {
        this->childCellArray[i] = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Add a child cell at a given index. Adding a child cell will increment
    the refcount of the child cell. Since child cells are owned by
    their parent cell, be sure to call release on a cell after adding it,
    otherwise memleaks will result.

    @param  index   index at which to add the child cell
    @param  cell    pointer to the child cell (refcount will be incremented)
*/
void
Cell::AddChildCell(int index, Cell* cell)
{
    n_assert(cell);
    n_assert(this->childCellArray[index] == 0);

    cell->SetParentCell(this);
    this->childCellArray[index] = cell;
}

//------------------------------------------------------------------------------
/**
    Finish adding child cells.
*/
void
Cell::EndChildCells()
{
    // do some validity checks
    int num = this->childCellArray.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        n_assert(this->childCellArray[i] != 0);
    }
}

//------------------------------------------------------------------------------
/**
    Attach a graphics entity to the cell. The refcount of the entity 
    will be incremented.
*/
void
Cell::AttachEntity(Entity* entity)
{
    n_assert(entity);
    n_assert(entity->GetCell() == 0);

    entity->OnAttachedToCell(this);
    this->entityPool[entity->GetType()].Append(entity);
    this->UpdateNumEntitiesInHierarchy(entity->GetType(), 1);
}

//------------------------------------------------------------------------------
/**
    Remove a graphics entity from the cell, this will decrement the
    refcount of the cell.

    FIXME: this may be a slow operation, consider linked list?
*/
void
Cell::RemoveEntity(Entity* entity)
{
    n_assert(entity);
    n_assert(entity->GetCell() == this);

    // remove entity pointer from array
    entity->OnRemovedFromCell();
    nArray<Ptr<Entity> >::iterator iter = this->entityPool[entity->GetType()].Find(entity);
    n_assert(0 != iter);
    this->UpdateNumEntitiesInHierarchy((*iter)->GetType(), -1);
    this->entityPool[entity->GetType()].Erase(iter);
}

//------------------------------------------------------------------------------
/**
    Starting from this cell, try to find the smallest cell which completely
    contains the given entity:

    - starting from initial cell:
        - if the entity does not fit into the cell, move up the
          tree until the first cell is found which the entity completely fits into
        - if the entity fits into a cell, check each child cell if the 
          entity fits completely into the cell

    The entity will not be attached! If the entity does not fit into the 
    root cell, the root cell will be returned, not 0.
    
    @param  entity      pointer of entity to find new cell for
    @return             cell which completely encloses the entity (the
                        root cell is an exception)
*/
Cell*
Cell::FindEntityContainmentCell(Entity* entity)
{
    // get the global space entity bounding box
    bbox3 entityBox = entity->GetBox();

    // find the first upward cell which completely contains the entity,
    // stop at tree root
    Cell* cell = this;
    while ((cell->GetParentCell() != 0) && (!cell->GetBox().contains(entityBox)))
    {
        cell = cell->GetParentCell();
    } 

    // find smallest downward cell which completely contains the entity
    int cellIndex, numCells;
    do
    {
        numCells = cell->GetNumChildCells();
        for (cellIndex = 0; cellIndex < numCells; cellIndex++)
        {
            Cell* childCell = cell->GetChildCellAt(cellIndex);
            if (childCell->GetBox().contains(entityBox))
            {
                cell = childCell;
                break;
            }
        }
        // check for loop fallthrough: this means that the current cell either has
        // no children, or that none of the children completely contains the entity
    }
    while (cellIndex != numCells);

    return cell;
}

//------------------------------------------------------------------------------
/**
    Insert a dynamic graphics entity into the cell tree. The entity
    will correctly be inserted into the smallest enclosing cell in the tree.
    The cell may not be currently attached to a cell, the refcount of the
    entity will be incremented.

    @param  entity      pointer to a graphics entity
*/
void
Cell::InsertEntity(Entity* entity)
{
    Cell* cell = this->FindEntityContainmentCell(entity);
    n_assert(cell);
    cell->AttachEntity(entity);
}

//------------------------------------------------------------------------------
/**
    Recursively clear all entity links of a specific type. Entity
    links MUST be cleared before calling the associated Cell::UpdateLinks()
    method!
*/
void
Cell::ClearLinks(Entity::LinkType linkType)
{
    // break if no entities in this cell and below
    if (0 == this->GetNumEntitiesInHierarchy())
    {
        return;
    }

    // for each entity pool...
    int pool;
    for (pool = 0; pool < Entity::NumTypes; pool++)
    {
        // for each entity...
        int numEntities = this->GetNumEntities(Entity::Type(pool));
        int entityIndex;
        for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
        {
            this->GetEntityAt(Entity::Type(pool), entityIndex)->ClearLinks(linkType);
        }
    }

    // recurse into child cells
    int numChildCells = this->GetNumChildCells();
    int childCellIndex;
    for (childCellIndex = 0; childCellIndex < numChildCells; childCellIndex++)
    {
        this->GetChildCellAt(childCellIndex)->ClearLinks(linkType);
    }
}

//------------------------------------------------------------------------------
/**
    Recursively gather all entities of a given entity type which are 
    visible by a observerEntity. A link will be established between the observer 
    and observed entity. 
    
    This method is actually the core of the whole visibility detection!

    @param  observerEntity      the observer entity
    @param  observedType        the type of entities to consider
    @param  linkType            type of link that will be established
    @param  clipStatus          the uplevel clip status 
*/
void
Cell::LinkVisibleEntities(Entity* observerEntity, 
                          Entity::Type observedType,
                          Entity::LinkType linkType,
                          Entity::ClipStatus clipStatus)
{
    n_assert(observerEntity);

    // break immediately if of wanted type in this cell or below
    if (this->GetNumEntitiesInHierarchyByType(observedType) == 0)
    {
        return;
    }

    // gather statistics
    Graphics::Server* graphicsServer = Graphics::Server::Instance();
    graphicsServer->AddNumVisitedCells(linkType, 1);

    // if clip status unknown or clipped, get clip status of this cell against observer entity
    if ((Entity::InvalidClipStatus == clipStatus) || (Entity::Clipped == clipStatus))
    {
        const bbox3& cellBox = this->GetBox();
        clipStatus = observerEntity->GetBoxClipStatus(cellBox);
    }
    
    // if we are outside the view volume, return immediately since none
    // of our entities or child cells will be visible
    if (Entity::Outside == clipStatus)
    {
        graphicsServer->AddNumOutsideCells(linkType, 1);
        return;
    }
    else if (Entity::Inside == clipStatus)
    {
        // gather statistics
        graphicsServer->AddNumVisibleCells(linkType, 1);

        // cell completely inside, gather ALL contained shape entities
        const nArray<Ptr<Entity> >& observedPool = this->entityPool[observedType];
        int numEntities = observedPool.Size();
        int entityIndex;
        for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
        {
            Entity* entity = observedPool[entityIndex];
            if (entity->GetVisible() && entity->TestLodVisibility())
            {
                observerEntity->AddLink(linkType, entity);
                entity->AddLink(linkType, observerEntity);
                entity->SetRenderFlag(nRenderContext::ShadowVisible, true);
                entity->SetRenderFlag(nRenderContext::ShapeVisible, true);

                // gather statistics
                graphicsServer->AddNumVisibleEntities(observedType, linkType, 1);
            }
        }
    }
    else
    {
        // gather statistics
        graphicsServer->AddNumVisibleCells(linkType, 1);

        // cell partially inside, check each entity individually
        const nArray<Ptr<Entity> >& observedPool = this->entityPool[observedType];
        int numEntities = observedPool.Size();
        int entityIndex;
        for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
        {
            Entity* entity = observedPool[entityIndex];
            if (entity->GetVisible() && entity->TestLodVisibility())
            {
                // check against extruded shadow bounding box, 
                // or canonical bounding box, depending on link type
                const bbox3* entityBox;
                bool setRenderContextHints = false;
                if ((Entity::CameraLink == linkType) && (Entity::Shape == observedType))
                {
                    entityBox = &(entity->GetShadowBox());
                    setRenderContextHints = true;
                }
                else
                {
                    entityBox = &(entity->GetBox());
                }
                if (observerEntity->GetBoxClipStatus(*entityBox) != Entity::Outside)
                {
                    observerEntity->AddLink(linkType, entity);
                    entity->AddLink(linkType, observerEntity);
                    if (setRenderContextHints)
                    {
                        entity->SetRenderFlag(nRenderContext::ShadowVisible, true);
                        if (observerEntity->GetBoxClipStatus(entity->GetBox()) != Entity::Outside)
                        {
                            entity->SetRenderFlag(nRenderContext::ShapeVisible, true);
                        }
                        else
                        {
                            entity->SetRenderFlag(nRenderContext::ShapeVisible, false);
                        }
                    }

                    // gather statistics
                    graphicsServer->AddNumVisibleEntities(observedType, linkType, 1);
                }
            }
        }
    }

    // recurse into child cells
    int numChildCells = this->GetNumChildCells();
    int childCellIndex;
    for (childCellIndex = 0; childCellIndex < numChildCells; childCellIndex++)
    {
        this->GetChildCellAt(childCellIndex)->LinkVisibleEntities(observerEntity, observedType, linkType, clipStatus);
    }
}

//------------------------------------------------------------------------------
/**
    Recursively creates links between observer and observed objects based
    on visibility status (a link will be created between an observer and
    an entity visible to that observer). This method is used to
    create links between camera and shape entities, or light and shape
    entities (just tell the method what you want).

    @param  observerEntity  the observer entity (usually a camera or a light)
    @param  observedType    the type of entities
    @param  linkType        link type (e.g. CameraLink, LightLink)
*/
void
Cell::UpdateLinks(Entity* observerEntity, Entity::Type observedType, Entity::LinkType linkType)
{
    // recursively gather all entities in this cell and
    // its children which can be seen by the light entity
    this->LinkVisibleEntities(observerEntity, observedType, linkType, Entity::InvalidClipStatus);
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization.
*/
void
Cell::RenderDebug()
{
    // render entities in cell
    int entityIndex;
    int numEntities = this->GetNumEntities(Entity::Shape);
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        this->GetEntityAt(Entity::Shape, entityIndex)->RenderDebug();
    }

    // advance to child cells
    int cellIndex;
    int numCells = this->GetNumChildCells();
    for (cellIndex = 0; cellIndex < numCells; cellIndex++)
    {
        this->GetChildCellAt(cellIndex)->RenderDebug();
    }
}

} // namespace Graphics
