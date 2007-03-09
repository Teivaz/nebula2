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
    
    nMaxBoneManager collect only bones which actually used in the animation.

    It happens at the start time of the export to provide easy access to the
    joint indexes when it is needed.

    -16-Feb-06  kims  Added exporting of several skinned models.
                      Thanks Ivan Tivonenko for the patch. 

*/
#include "export2/nmaxnotetrack.h"

#include "mathlib/vector.h"
#include "util/narray.h"
#include "util/nstring.h"
#include <map>

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

    // @name Primary methods for handling bones.
    // @{
    bool BuildBones(INode* node);
    bool Export(int skelIndex, const char* animFileName);
    // @}

    /// Retrieves the number of bones which are actually used in the animation.
    int GetNumBones() const;
    int GetNumBones(int skelIndex) const;
    int GetNumSkeletons() const;

    static bool IsBone(INode *inode);
    static bool IsDummy(INode* inode);
    static bool IsFootStep(INode* inode);

    // @name Find functions
    // @{
    int FindBoneIDByNode(INode* inode);
    int FindBoneIDByName(const nString &name);
    int FindBoneIndexByNodeId(int nodeID);
    INode* FindBoneNodeByIndex(int index);
    // @}

    /// Retrieves bone by the given index.
    const Bone& GetBone(int skelIndex, int index);
    /// Retrieves bone array.
    nArray<nMaxBoneManager::Bone>& GetBoneArray(int skelIndex);
    /// Retrieves note track.
    nMaxNoteTrack& GetNoteTrack(int skelIndex);

    int GetSkelForNode(INode* inode);

    const nString& GetAnimPath() const;

protected:
    // @name Helper functions to collect bones in the scene.
    // @{
    void GetNodes(INode* node, nArray<INode*>& nodeArray);
    
    void GetBoneByModifier(const nArray<INode*>& nodeArray, nArray<INode*> &boneNodeArray);
    void GetBoneByClassID(const nArray<INode*>& nodeArray, nArray<INode*> &boneNodeArray);

    void ExtractPhysiqueBones(INode* node, Modifier* phyMod, ObjectState* os, 
                              nArray<INode*> &boneNodeArray);
    void ExtractSkinBones(INode* node, Modifier* skinMod,nArray<INode*> &boneNodeArray);
    bool IsGeomObject(INode *node);

    int GetRootBones(INode *sceneRoot, nArray<INode*> &boneNodeArray, nArray<INode*> &rootBonesNodeArray);
    void ReconstructBoneHierarchy(int parentID, int skeletion, INode* node, nArray<INode*> &boneNodeArray);
    // @}

    void AddBoneToNode(INode* inode, INode* bone);

    bool GetCustAttrib(Animatable* obj);

protected:
    /// array for collected bones of the scene.
    typedef nArray<Bone>  Skeleton;
    nArray<Skeleton>      skeletonsArray;
    std::map<INode*, INode*>    nodeToBone;
    std::map<INode*, int>       nodeToSkel;
    std::map<INode*, int>       boneToSkel;

    /// note track object which we retrieves states and clips.
    nArray<nMaxNoteTrack> noteTracksArray;

    /// A directory where exported animation for bones will be put.
    nString animPath;

private:
    // @name Singleton
    // @{
    nMaxBoneManager();
    nMaxBoneManager(const nMaxBoneManager&) {};

    static nMaxBoneManager* Singleton;
    // @}

};
//---------------------------------------------------------------------------
inline
nMaxBoneManager* nMaxBoneManager::Instance()
{
    if (0 == Singleton)
    {
        return n_new(nMaxBoneManager);
    }
    return Singleton;
}
//---------------------------------------------------------------------------
inline
int nMaxBoneManager::GetNumBones() const
{
    int num = 0;
    for (int i=0; i<skeletonsArray.Size(); i++) 
    {
        num += skeletonsArray[i].Size();
    }
    return num;
}
//---------------------------------------------------------------------------
inline 
int nMaxBoneManager::GetNumBones(int skelIndex) const 
{
    n_assert(skelIndex >= 0 && skelIndex < this->skeletonsArray.Size());
    return this->skeletonsArray[skelIndex].Size();
}
//---------------------------------------------------------------------------
inline 
const nMaxBoneManager::Bone& nMaxBoneManager::GetBone(int skelIndex, int index) 
{
    n_assert(index >= 0);
    n_assert(skelIndex >= 0);
    return this->skeletonsArray[skelIndex][index];
}
//---------------------------------------------------------------------------
inline 
nArray<nMaxBoneManager::Bone>& nMaxBoneManager::GetBoneArray(int skelIndex) 
{
    n_assert(skelIndex >= 0);
    return this->skeletonsArray[skelIndex];
}
//---------------------------------------------------------------------------
inline 
nMaxNoteTrack& nMaxBoneManager::GetNoteTrack(int skelIndex) 
{
    n_assert(skelIndex >= 0);
    return this->noteTracksArray[skelIndex];
}
//---------------------------------------------------------------------------
inline 
int nMaxBoneManager::GetNumSkeletons() const 
{
    return this->skeletonsArray.Size();
}

inline
const nString& nMaxBoneManager::GetAnimPath() const
{
    return this->animPath;
}
//---------------------------------------------------------------------------
#endif
