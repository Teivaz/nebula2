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
/**
    @class nMaxSkinMeshData
    @ingroup NebulaMaxExport2Contrib

    @brief Storage which contains skinned shape node and its group map id in its
           mesh data. It is needed when we do partitioning the mesh to specifies
           the fragments and joint palette for the skinned shape node.
*/
struct nMaxSkinMeshData
{
    /// skinned shape node.
    nSkinShapeNode* node;
    /// group map index.
    int groupIndex;
};

//-----------------------------------------------------------------------------
/**
    @class nMaxMesh
    @ingroup NebulaMaxExport2Contrib

    @brief A class for exporting nShapeNode and its derive classes.

    It extracts mesh data such as vertex, normal, color, uvs and so on
    from the 3dsmax mesh geometry node then stores that data to the given
    mesh builder.

*/
class nMaxMesh : public nMaxNode
{
public:
    /// mesh types
    enum Type {
        None      = 0,  /// 0,  none of selected in radiobutton of max script ui
        Shape     = 1,  /// static or skinned mesh 
        Swing     = 2,  /// swing shape node
        Shadow    = 3,  /// shadow mesh
        Collision = 4,  /// collision mesh only
        Particle2 = 5,  /// nparticleshapenode2
        Sky       = 6,  /// sky node, this will contain some sky elements
    };

    /// Constructor.
    nMaxMesh();
    /// Destructor.
    virtual ~nMaxMesh();

    nSceneNode* Export(INode* inode);

    ///
    int GetNumGroupMeshes() const;
    ///
    const nMaxSkinMeshData& GetGroupMesh(const int index);

    /// Retrieve mesh type.
    Type GetType() const;

    ///
    nMeshBuilder& GetMeshBuilder();
    ///
    void SetBaseGroupIndex(int baseGroupIndex);

    ///
    bool BuildMeshTangentNormals(nMeshBuilder &meshBuilder);
    ///
    static void CheckGeometryErrors(nMeshBuilder& meshBuilder, nString& meshName);

    /// Retrieve directory path where the mesh will be located.
    nString GetMeshPath() const;

    /// Retrieve vertex component of the first vertex.(position at 0)
    int GetVertexComponent() const;

    /// Retrieve 3dsmax node name for this mesh.
    nString GetNodeName() const;

    /// Retrieve skynode link element's name
    nString GetSkyElementNameToLink() const;

    ///
    static nString AppendMeshPostfixByType(Type type, bool isSkinned);

    ///
    bool IsSkinned();
    ///
    bool IsPhysique();

protected:
    Mesh* LockMesh(INode* node);
    void UnlockMesh();

    Mesh* GetMeshFromRenderMesh(INode* inode, BOOL &needDelete);
    Mesh* GetMeshFromTriObject(INode* inode, BOOL &needDelete);

    int GetMesh(INode* inode, nMeshBuilder* meshBuilder, const int matIdx, const int numMats, bool worldCoords = false);

    void GetMaterial(INode* inode, nShapeNode* shapeNode, int matID);

    nArray<int> GetUsedMapChannels(Mesh* mesh);

    bool HasSameMaterial(Mesh* mesh, int faceIdx, const int matIdx, const int numMats);
    bool HasNegativeScale(Matrix3& m);
    bool HasNegativeScale(INode* inode);

    bool UsePerVertexAlpha(Mesh* mesh);

    ///@name mesh extraction functions.
    //@{
    nMeshBuilder::Vertex GetVertex(Mesh* mesh, Face& face, int faceNo, int vIdx);

    vector3 GetVertexPosition(Mesh* mesh, int index);
    vector3 GetVertexNormal(Mesh* mesh, Face& face, int faceNo, int vtxIdx);
    Point3 GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
    vector4 GetVertexColor(Mesh* mesh, int faceNo, int vtxIdx);
    vector2 GetVertexUv(Mesh* mesh, int faceNo, int vtxIdx, int m);
    void GetVertexWeight(int vertexIdx, vector4 &jointIndices, vector4 &weights);
    //@}

    void SetMeshFile(nSceneNode* createdNode);

protected:
    int GetGroupIndex (nMeshBuilder *meshBuilder);

    ///@name physique and skin functions.
    ///@{
    bool BeginSkin(INode *node);
    ///
    void EndSkin();
    ///
    IPhyVertexExport* GetVertexInterface(int vertexId);
    ///
    void ReleaseVertexInterface(IPhyVertexExport* phyVertexExport);
    ///
    ISkinContextData* GetSkinContextData();
    ///
    ISkin* GetSkin();
    ///@}

    ///
    int GetNumPoints();
    ///
    int GetNumBones();

    ///
    bool HasCustomAttribute();
    ///
    bool ExportNormals();
    ///
    bool ExportColors();
    ///
    bool ExportUvs();
    ///
    bool ExportTangents();

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

    ///
    bool GetCustAttrib(Animatable* obj);
    ///
    nSceneNode* CreateShapeNode(INode* inode, nString &name);

    ///
    void SetSkinAnimator(INode* inode, nSceneNode* createdNode, int numMaterials);
    ///
    nString GetSkinAnimatorName(const char* baseName, int skelIndex);

protected:

    Type meshType;

    // mesh's vertex options.
    enum
    {
        VertexNormal  = 0x01,
        VertexColor   = 0x02,
        VertexUvs     = 0x04,
        VertexTangent = 0x08,
    };

    /// vertex option of the mesh.
    bool hasCustomAttr;
    uint vertexFlag;

    /// 3dsmax geometry node which we extract the mesh data.
    INode* maxNode;

    ReferenceMaker *refMaker;
    Mesh *mesh;
    BOOL deleteMesh;
    TriObject* triObj;

    /// used for local mesh file.
    nMeshBuilder localMeshBuilder; 

    /// Array which contains meshes which extracted from this node.
    nArray<nMaxSkinMeshData> skinmeshArray;

    /// pivot matrix.
    Matrix3 pivMat;

    /// storage for all created shape or shadow nodes.
    nArray<nSceneNode*> sceneNodeArray;

    ///
    nString meshPath;

    /// 3dsmax node name.
    nString nodeName;

    /// 3dsmax node name which was picked for sky element to link.
    nString skyLinkElemName;

};
//-----------------------------------------------------------------------------
/**
*/
inline
int 
nMaxMesh::GetNumGroupMeshes() const
{
    return this->skinmeshArray.Size();
}
//-----------------------------------------------------------------------------
/**
*/
inline
const nMaxSkinMeshData&
nMaxMesh::GetGroupMesh(const int index)
{
    n_assert(index >= 0);
    return this->skinmeshArray[index];
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
nMaxMesh::Type nMaxMesh::GetType() const
{
    return this->meshType;
}
//-----------------------------------------------------------------------------
inline
nMeshBuilder& nMaxMesh::GetMeshBuilder()
{
    return this->localMeshBuilder;
}
//-----------------------------------------------------------------------------
inline
nString nMaxMesh::GetMeshPath() const
{
    return this->meshPath;
}
//-----------------------------------------------------------------------------
inline
int nMaxMesh::GetVertexComponent() const
{
    nMeshBuilder::Vertex v = this->localMeshBuilder.GetVertexAt(0);
    return v.compMask;
}
//-----------------------------------------------------------------------------
inline
nString nMaxMesh::GetNodeName() const
{
    return this->nodeName;
}
//-----------------------------------------------------------------------------
inline
nString nMaxMesh::GetSkyElementNameToLink() const
{
    return this->skyLinkElemName;
}
inline
bool nMaxMesh::HasCustomAttribute()
{
    return this->hasCustomAttr;
}
//-----------------------------------------------------------------------------
inline
bool nMaxMesh::ExportNormals()
{
    return (this->vertexFlag & VertexNormal) ? true : false;
}
//-----------------------------------------------------------------------------
inline
bool nMaxMesh::ExportColors()
{
    return (this->vertexFlag & VertexColor) ? true : false;
}
//-----------------------------------------------------------------------------
inline
bool nMaxMesh::ExportUvs()
{
    return (this->vertexFlag & VertexUvs) ? true : false;
}
//-----------------------------------------------------------------------------
inline
bool nMaxMesh::ExportTangents()
{
    return (this->vertexFlag & VertexTangent) ? true : false;
}

//-----------------------------------------------------------------------------
#endif
