//-----------------------------------------------------------------------------
//  nmaxscene.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXSCENE_H
#define N_MAXSCENE_H
//-----------------------------------------------------------------------------
/**
    @class nMaxScene
    @ingroup NebulaMaxExport2Contrib

    @brief
*/
#include "export2/nmaxnotetrack.h"
#include "tools/nanimbuilder.h"

class nMaxMesh;
class nMaxNoteTrack;
class nVariableServer;
class nSceneNode;
class nMaxBoneManager;

//-----------------------------------------------------------------------------
class nMaxScene
{
public:
    nMaxScene();
    virtual ~nMaxScene();

    bool Export();

    void AddNode(INode* inode);
    bool IsExistNode(INode* inode);

protected:
    bool Begin(INode* root);
    bool End();

    bool OpenNebula();
    bool CloseNebula();

    bool Preprocess(INode* root);
    bool Postprocess();

    void InitializeNodes(INode* inode);
    void UnInitializeNodes(INode* inode);

    // @name
    // @{
    void ExportNodes(INode* inode);

    void ExportLightObject(INode* inode);
    nSceneNode* ExportGeomObject(INode* inode);

    nSceneNode* ExportMorph();
    nSceneNode* ExportParticle();

    void ExportBackgroudColor();

    void ExportXForm(INode* inode, nSceneNode* sceneNode, TimeValue &animStart);

    bool ExportAnimation(const nString &filename);
    // @}

    void CreateAnimStates();
    bool CreateAnimation(nAnimBuilder &animBuilder);

    void CollectTopLevelNodes(INode* inode);

    void PartitionMesh();
    void CreateSkinAnimator(const nString& animatorName, const nString& animFileName);

    Point3 GetBackgroundColor() const;

protected:
    nRef<nVariableServer> varServer;
    
    // bone manager for all bones in the scene.
    nMaxBoneManager* boneManager;

    /// scene root node.
    INode* sceneRoot; 

    /// Nebula object name of the scene base object.
    nString nohBase; 

    /// array for exported nMaxMesh instances.
    nArray<nMaxMesh*> meshArray;

    nMeshBuilder globalMeshBuilder;

    nArray<INode*> topLevelNodes;

    nMaxNoteTrack noteTrack;

    Point3 backgroudCol;

};
//-----------------------------------------------------------------------------
inline
Point3 
nMaxScene::GetBackgroundColor() const
{
    return this->backgroudCol;
}
//-----------------------------------------------------------------------------
#endif