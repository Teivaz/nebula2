//-----------------------------------------------------------------------------
//  nmaxmesh.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXMESH_H
#define N_MAXMESH_H

#include "export2/nmaxnode.h"

class nMeshBuilder;
class nSceneNode;
class nTransformNode;
class nShapeNode;
class nSkinShapeNode;

//-----------------------------------------------------------------------------
class nMaxGroupMesh
{
    nSkinShapeNode* node;
    int groupIndex;
public:
    nMaxGroupMesh() : node(0), groupIndex(-1) {}

    void SetNode(nSkinShapeNode* shape) { 
        n_assert(shape);
        this->node= shape; 
    }

    nSkinShapeNode* GetNode() const { 
        return this->node; 
    }

    void SetGroupIndex (const int& index) { 
        this->groupIndex = index; 
    }

    int GetGroupIndex() const { 
        return this->groupIndex; 
    }

};

//-----------------------------------------------------------------------------
class nMaxMeshFragment
{
public:
    struct Fragment
    {
        int groupMapIndex;
        nArray<int> bonePaletteArray;
    };

    nSkinShapeNode* node;

    nArray<Fragment> fragmentArray;

};

//-----------------------------------------------------------------------------
/**
    @class nMaxMesh
    @ingroup NebulaMaxExport2Contrib

    @brief
*/
class nMaxMesh : public nMaxNode
{
public:
    nMaxMesh();
    virtual ~nMaxMesh();

    nSceneNode* Export(INode* inode, nMeshBuilder* globalMeshBuilder, bool useIndivisualMesh);

    int GetNumGroupMeshes() const;
    const nMaxGroupMesh& GetGroupMesh(const int index);

    const nMeshBuilder& GetLocalMeshBuilder();

    static void BulldMeshFragments(nArray<nMaxMeshFragment>& meshFragmentArray);

    static bool BuildMeshTangentNormals(nMeshBuilder &meshBuilder);

protected:
    Mesh* LockMesh(INode* node);
    void UnlockMesh();

    Mesh* GetMeshFromRenderMesh(INode* inode, BOOL &needDelete);
    Mesh* GetMeshFromTriObject(INode* inode, BOOL &needDelete);

    int GetMesh(INode* inode, nMeshBuilder* meshBuilder, const int matIdx, const int numMats);

    void GetMaterial(INode* inode, nShapeNode* shapeNode, int matID);

    nArray<int> GetUsedMapChannels(Mesh* mesh);

    bool HasSameMaterial(Mesh* mesh, int faceIdx, const int matIdx, const int numMats);
    bool HasNegativeScale(Matrix3& m);
    bool HasNegativeScale(INode* inode);

    bool UsePerVertexAlpha(Mesh* mesh);

    ///@name mesh extraction functions.
    //@{
    nMeshBuilder::Vertex GetVertex(Mesh* mesh, Face& face, int faceNo, int vtxIdx, int index);

    vector3 GetVertexPosition(Mesh* mesh, int index);
    vector3 GetVertexNormal(Mesh* mesh, Face& face, int faceNo, int vtxIdx);
    Point3 GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
    vector4 GetVertexColor(Mesh* mesh, int faceNo, int vtxIdx);
    vector2 GetVertexUv(Mesh* mesh, int faceNo, int vtxIdx, int m);
    void GetVertexWeight(int vertexIdx, vector4 &jointIndices, vector4 &weights);
    //@}

    //void SaveMeshFile(const char* nodeName);
    void SetMeshFile(nShapeNode* shapeNode, nString &nodeName, bool useIndivisualMesh);

    void PartitionMesh();

protected:
    int GetGroupIndex (nMeshBuilder *meshBuilder);

    ///@name physique and skin functions.
    ///@{
    bool BeginSkin(INode *node);
    void EndSkin();

    bool IsSkinned();
    bool IsPhysique();

    IPhyVertexExport* GetVertexInterface(int vertexId);
    void ReleaseVertexInterface(IPhyVertexExport* phyVertexExport);

    ISkinContextData* GetSkinContextData();
    ISkin* GetSkin();
    ///@}

    int GetNumPoints();
    int GetNumBones();

    ///@name physique and skin variables.
    ///@{
    bool beginSkin;
    bool isSkinned;
    bool isPhysique;

    Modifier* modifier;
    IPhysiqueExport *phyExport;
    IPhyContextExport *phyContextExport;

    ISkin* skin;
    ISkinContextData* skinContext;
    Object* object;
    ///@}

    bool GetCustAttrib(INode* inode);
    nSceneNode* CreateNebulaNode(INode* inode);

    void SetShapeGroup(nShapeNode* createdNode, int baseGroupIndex, int numMaterials);

protected:
    // mesh types
    enum Type {
        None      = 0,  // 0,  none of selected in radiobutton of max script ui
        Shape     = 1,  // static or skinned mesh 
        Swing     = 2,  // swing shape node
        Shadow    = 3,  // shadow mesh
        Collision = 4,  // collision mesh only
    };

    Type meshType;

    // mesh's vertex options.
    enum
    {
        VertexNormal  = 0x01,
        VertexColor   = 0x02,
        VertexUvs     = 0x04,
        VertexTangent = 0x08,
    };

    uint vertexFlag;

    bool isAttachedNode;

    INode* maxNode;

    ReferenceMaker *refMaker;
    Mesh *mesh;
    BOOL deleteMesh;
    TriObject* triObj;

    nMeshBuilder localMeshBuilder; /// used for local mesh file.

    nArray<nMaxGroupMesh> groupMeshArray;

    Matrix3 pivMat;

};
//-----------------------------------------------------------------------------
/**
*/
inline
int 
nMaxMesh::GetNumGroupMeshes() const
{
    return this->groupMeshArray.Size();
}
//-----------------------------------------------------------------------------
/**
*/
inline
const nMaxGroupMesh&
nMaxMesh::GetGroupMesh(const int index)
{
    n_assert(index >= 0);
    return this->groupMeshArray[index];
}
//-----------------------------------------------------------------------------
/**
    Retrieves group index of given mesh.
*/
inline
int 
nMaxMesh::GetGroupIndex (nMeshBuilder *meshBuilder)
{
    n_assert(meshBuilder);

    nArray<nMeshBuilder::Group> groupMap;
    meshBuilder->BuildGroupMap(groupMap);

    //int baseGroupIndex = groupMap.Size();
    return groupMap.Size();
}
//-----------------------------------------------------------------------------
inline
const nMeshBuilder& nMaxMesh::GetLocalMeshBuilder()
{
    return this->localMeshBuilder;
}
//-----------------------------------------------------------------------------
#endif