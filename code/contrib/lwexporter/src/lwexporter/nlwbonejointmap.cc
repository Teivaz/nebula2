//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwbonejointmap.h"
#include "lwwrapper/nlwglobals.h"

//----------------------------------------------------------------------------
/**
*/
void
nLWBoneJointMap::MapBonesToJointIndices(LWItemID boneId, int& nextJointIdx, 
                                        LWItemInfo* itemInfo)
{
    this->boneJointMap[boneId] = nextJointIdx++;
    LWItemID childId = itemInfo->firstChild(boneId);
    while (childId != LWITEM_NULL)
    {
        if (itemInfo->type(childId) == LWI_BONE)
        {
            this->MapBonesToJointIndices(childId, nextJointIdx, itemInfo);
        }
        childId = itemInfo->nextChild(boneId, childId);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWBoneJointMap::Fill(LWItemID rootBoneId)
{
    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid());
    if (itemInfo.IsValid())
    {
        int nextJointIdx = 0;
        this->MapBonesToJointIndices(rootBoneId, nextJointIdx, itemInfo.Get());
    }
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
