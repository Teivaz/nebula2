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

*/
#include "export2/nmaxnotetrack.h"

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

    // @name Primary methods for handling bones.
    // @{
    bool BuildBones(INode* node);
    bool Export(const char* animFileName);
    // @}

    /// Retrieves the number of bones which are actually used in the animation.
    int GetNumBones() const;

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
    const Bone& GetBone(int index);
    /// Retrieves bone array.
    nArray<nMaxBoneManager::Bone>& GetBoneArray();
    /// Retrievs note track.
    nMaxNoteTrack& GetNoteTrack();

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

    INode* GetRootBone(INode *sceneRoot, nArray<INode*> &boneNodeArray);
    void ReconstructBoneHierarchy(int parentID, INode* node, nArray<INode*> &boneNodeArray);
    // @}

protected:
    /// array for collected bones of the scene.
    nArray<Bone>      boneArray;

    /// note track object which we retrieves states and clips.
    nMaxNoteTrack noteTrack;

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
inline
nArray<nMaxBoneManager::Bone>& 
nMaxBoneManager::GetBoneArray()
{
    return this->boneArray;
}
//---------------------------------------------------------------------------
inline
nMaxNoteTrack& 
nMaxBoneManager::GetNoteTrack()
{
    return this->noteTrack;
}
//---------------------------------------------------------------------------
#endif
