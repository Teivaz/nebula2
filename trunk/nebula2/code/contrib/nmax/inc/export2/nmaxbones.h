//---------------------------------------------------------------------------
//  nmaxbones.h
//
//  (c)2004 Kim, Hyoun Woo
//---------------------------------------------------------------------------
#ifndef N_MAXBONEMANAGER_H
#define N_MAXBONEMANAGER_H
//---------------------------------------------------------------------------
/**
    @class nMaxBoneManager
    @ingroup NebulaMaxExport2Contrib

    @brief Class for handling bones in the given scene. 

    @note
        This class is singleton.

    The followings are supported.
      -# Support Character Studio's Biped and Physique.
      -# Support 3DS Max's Skin modifier.
      -# Support unlimited type of object for bones.
         (any type of object can be bone like 3DS Max does)
    
    nMaxBoneManager collect all bones even the bones are not actually used
    in the animation. 
    It happens at the start time of the export to provide easy access to the
    joint indexes when it is needed.

*/
#include "mathlib/vector.h"
#include "util/narray.h"
#include "util/nstring.h"

//---------------------------------------------------------------------------
class nMaxBoneManager
{
public:
    /// present individual bone.
    class Bone
    {
    public:
        Bone();

        int parentID;
        int id;
        nString name;
        INode* node;

        Matrix3 localTransform;
        Matrix3 worldTransform;

        bool dummy;
    };

    static nMaxBoneManager* Instance();

    virtual ~nMaxBoneManager();

    void BuildBoneList(INode* node);

    //bool ExistInBones(INode* inode);

    int GetNumBones() const;

    static bool IsBone(INode *inode);
    static bool IsDummy(INode* inode);
    static bool IsFootStep(INode* inode);

    int FindBoneIDByNode(INode* inode);

    int FindBoneIDByName(const nString &name);
    int FindBoneIndexByNodeId(int nodeID);
    INode* FindBoneNodeByIndex(int index);

    const Bone& GetBone(int index);

protected:
    void GetNodes(INode* node, nArray<INode*>& nodeArray);
    
    void GetBoneByModifier(const nArray<INode*>& nodeArray, nArray<INode*> &boneNodeArray);
    void GetBoneByClassID(const nArray<INode*>& nodeArray, nArray<INode*> &boneNodeArray);
    void GetRootBones(INode* sceneRoot, const nArray<INode*> &boneNodeArray, nArray<INode*> &rootBoneArray);

    void BuildBones(int parentID, INode* node);
    void BuildBoneArray(const nArray<INode*> &rootBoneArray);

    void ExtractPhysiqueBones(INode* node, Modifier* phyMod, ObjectState* os, nArray<INode*> &boneNodeArray);
    void ExtractSkinBones(INode* node, Modifier* skinMod,nArray<INode*> &boneNodeArray);
    bool IsGeomObject(INode *node);

protected:
    nArray<Bone>      boneArray;
    //nArray<INode*>    bones;

    typedef nArray<nMaxBoneManager::Bone> BONE_ARRAY;

private:
    nMaxBoneManager();
    nMaxBoneManager(const nMaxBoneManager&) {};

    static nMaxBoneManager* Singleton;

};
//---------------------------------------------------------------------------
inline
nMaxBoneManager* 
nMaxBoneManager::Instance()
{
    if (0 == Singleton)
    {
        return n_new(nMaxBoneManager);
    }
    return Singleton;
}
//---------------------------------------------------------------------------
/**
    Check the given node is exist in nodes array.
*/
/*
inline
bool 
nMaxBoneManager::ExistInBones(INode* inode)
{
    return (this->bones.Find(inode) != 0);
}
*/
//---------------------------------------------------------------------------
inline
int 
nMaxBoneManager::GetNumBones() const
{
    return this->boneArray.Size();
}
//---------------------------------------------------------------------------
inline
const nMaxBoneManager::Bone& 
nMaxBoneManager::GetBone(int index)
{
    n_assert(index >= 0);
    return this->boneArray[index];
}
//---------------------------------------------------------------------------
#endif
