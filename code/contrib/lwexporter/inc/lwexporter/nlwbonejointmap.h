#ifndef N_LW_BONE_JOINT_MAP_H
#define N_LW_BONE_JOINT_MAP_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwrender.h>
}

#include <map>

//----------------------------------------------------------------------------
/**
    @class nLWBoneJointMap
    @brief Helper class that maps Lightwave bone ids to Nebula 2 joint indices.
*/
class nLWBoneJointMap
{
public:
    nLWBoneJointMap();

    void Fill(LWItemID rootBoneId);
    int GetJointIndex(LWItemID boneId) const;

private:
    // prevent copying
    nLWBoneJointMap(const nLWBoneJointMap& rhs);
    nLWBoneJointMap& operator=(const nLWBoneJointMap& rhs);

    void MapBonesToJointIndices(LWItemID boneId, int& nextJointIdx, 
                                LWItemInfo*);

    std::map<LWItemID, int> boneJointMap;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWBoneJointMap::nLWBoneJointMap()
{
    // empty
}

//----------------------------------------------------------------------------
/**
    @brief Get the joint index that corresponds to the given bone id.
    @return -1 if the corresponding joint index isn't found.

    You must call Fill() once to populate the map before trying to access it.
*/
inline
int 
nLWBoneJointMap::GetJointIndex(LWItemID boneId) const
{
    std::map<LWItemID, int>::const_iterator it = this->boneJointMap.find(boneId);
    if (it != this->boneJointMap.end())
    {
        return it->second;
    }
    return -1;
}

//----------------------------------------------------------------------------
#endif // N_LW_BONE_JOINT_MAP_H
