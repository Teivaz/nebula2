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

    @brief A class for 3dsmax scene which to be exported.

*/
#include "tools/nanimbuilder.h"

class nMaxMesh;
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

protected:
    // @name Helper functions for begin and end stage of export.
    // @{
    bool Begin(INode* root);
    bool End();

    bool OpenNebula();
    bool CloseNebula();

    bool Preprocess(INode* root);
    bool Postprocess();

    void InitializeNodes(INode* inode);
    void UnInitializeNodes(INode* inode);
    // @}

    // @name Functions for export
    // @{
    bool ExportNodes(INode* inode);

    nSceneNode* ExportGeomObject(INode* inode);
    nSceneNode* ExportMorph();
    nSceneNode* ExportParticle();

    void ExportBackgroudColor();

    void ExportXForm(INode* inode, nSceneNode* sceneNode, TimeValue &animStart);

    bool ExportAnimation(const nString &filename);

    Point3 GetBackgroundColor() const;
    // @}

    bool CreateAnimation(nAnimBuilder &animBuilder);

    void CollectTopLevelNodes(INode* inode);
    bool IsExportedNode(INode* inode);

protected:
    nRef<nVariableServer> varServer;
    
    // bone manager for all bones in the scene.
    nMaxBoneManager* boneManager;

    /// scene root node.
    INode* sceneRoot; 

    /// Nebula object name of the scene base object.
    //nString nohBase;
    nSceneNode* exportRoot;

    /// array for exported nMaxMesh instances.
    nArray<nMaxMesh*> meshArray;

    /// Mesh builder for meshes in the mesh array.
    nMeshBuilder globalMeshBuilder;

    /// array for top level nodes.
    nArray<INode*> topLevelNodes;

    /// viewport background color.
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