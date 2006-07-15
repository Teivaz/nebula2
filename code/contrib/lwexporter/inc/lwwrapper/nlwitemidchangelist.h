#ifndef N_LW_ITEMID_CHANGE_LIST_H
#define N_LW_ITEMID_CHANGE_LIST_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwrender.h>
}

#include "util/narray.h"

//----------------------------------------------------------------------------
/**
    @class nLWItemIDChangeList
    @brief Converts the LWItemID list passed to the changeID callback of 
           handler plugins to a format that's a bit easier to digest.
*/
class nLWItemIDChangeList
{
public:
    struct Item
    {
        LWItemID oldId;
        LWItemID newId;
    };

    nLWItemIDChangeList();
    nLWItemIDChangeList(const LWItemID* idChangeList);

    /// convert and store the given change list
    void Populate(const LWItemID* idChangeList);
    /// get an item in the converted list
    const Item& GetItemAt(int index);
    /// get the number of items in the converted list
    int GetItemCount();
    /// get the new item id given the old one
    bool GetNewId(LWItemID oldId, LWItemID& newId);

private:
    /// find an item in the converted list by it's old id
    bool FindItemByOldId(LWItemID id, Item& item);

    nArray<Item> items;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWItemIDChangeList::nLWItemIDChangeList()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
inline
nLWItemIDChangeList::nLWItemIDChangeList(const LWItemID* idChangeList)
{
    this->Populate(idChangeList);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWItemIDChangeList::Populate(const LWItemID* idChangeList)
{
    this->items.Reset();
    
    if (!idChangeList)
        return;

    int i = 0;
    nLWItemIDChangeList::Item item;
    while (LWITEM_NULL != idChangeList[i])
    {
        if (this->FindItemByOldId(idChangeList[i], item))
        {
            item.newId = idChangeList[i + 1];
        }
        else
        {
            item.oldId = idChangeList[i];
            item.newId = idChangeList[i + 1];
            this->items.PushBack(item);
        }
        i += 2;
    }
}

//----------------------------------------------------------------------------
/**
*/
inline
const nLWItemIDChangeList::Item&
nLWItemIDChangeList::GetItemAt(int index)
{
    return this->items[index];
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWItemIDChangeList::GetItemCount()
{
    return this->items.Size();
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWItemIDChangeList::GetNewId(LWItemID oldId, LWItemID& newId)
{
    nLWItemIDChangeList::Item item;
    if (this->FindItemByOldId(oldId, item))
    {
        newId = item.newId;
        return false;
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWItemIDChangeList::FindItemByOldId(LWItemID id, nLWItemIDChangeList::Item& item)
{
    for (int i = 0; i < this->items.Size(); i++)
    {
        if (this->items[i].oldId == id)
        {
            item = this->items[i];
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------
#endif // N_LW_ITEMID_CHANGE_LIST_H
