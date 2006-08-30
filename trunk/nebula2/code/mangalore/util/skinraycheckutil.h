#ifndef UTIL_SKINRAYCHECKUTIL_H
#define UTIL_SKINRAYCHECKUTIL_H
//------------------------------------------------------------------------------
/**
    @class Util::SkinRayCheckUtil

    Util class that provides a raycheck method for skinned and static meshes
    INFO:
    * Class is VERY untidy but works for tamagotchi! Needs a huge cleanup!
    * Search for FIXME and DEBUG tags to improve class...
    * The segmentation optimization doesn't work proper yet!

    (C) 2006 Radon Labs GmbH
*/

#include "graphics/entity.h"
#include "resource/nresource.h"
#include "util/nfixedarray.h"
#include "gfx2/nmesh2.h"
#include "character/ncharskeleton.h"
#include "mathlib/sphere.h"
#include "graphics/charentity.h"
#include "scene/nshapenode.h"
#include "scene/nskinshapenode.h"
#include "tools/nmeshbuilder.h"
#include "resource/nmeshcopyresourceloader.h"


//------------------------------------------------------------------------------
namespace Util
{
class SkinRayCheckUtil
{
public:

    /// mesh types
    enum MeshType
    {
        Static,
        Skinned,

        NumTypes,
        InvalidType,
    };

    /// information of an intersected face
    struct faceIntersection
    {
        int faceIndex;
        vector3 intersectionPoint;
        vector3 normal;
        float distance;
    };

    /// a triangle face
    struct face
    {
        vector3 p0, p1, p2;
        int v0, v1, v2;
        nArray<int> neighbourFaces;
    };

    /// constructor
    SkinRayCheckUtil();
    /// destructor
    virtual ~SkinRayCheckUtil();

    /// set graphics entity
    bool SetGraphicsEntity(Graphics::Entity* entity, nString meshName = "");

    /// make a raycheck and return all intersected faces as index
    bool DoRayCheck(const line3& ray, nArray<faceIntersection>& intersectedFaces);

    // get face by Index
    face& GetFace(int faceIndex);

    // set vertexcolor of all vertices
    bool SetVertexColorOfMesh(const vector4& col);
    // set vertexcolor of a face
    bool SetVertexColorOfFace(int faceIndex, const vector4& col);
    // get vertexcolor of a face
    vector4 GetVertexColorOfFace(int faceIndex);
    // get vertexcolor of a face at an intersectionPoint
    vector4 GetVertexColorOfFace(int faceIndex, const vector3& intersectionPoint);
    // get average vertexcolor of mesh
    vector4 GetAverageVertexColorOfMesh();
    // set vertexcolor of a face
    bool AddVertexColorOfFace(int faceIndex, const vector4& col, const vector3& intersectionPoint, float radius);
    // has vertexcolor
    bool HasMeshVertexColor();

    /// update original mesh from resistent mesh
    void UpdateOriginalMesh();
    ///
    nArray<vector4>& GetVertexColorMap();
    ///
    nArray<vector3>& GetVertexPosMap();
    ///
    nArray<vector3>& GetVertexNormalMap();
    ///
    void UpdateFromVertexColorMap();
    ///
    void InitVertexColorMap();
    ///
    void InitVertexPosMap();
    ///
    void InitVertexNormalMap();
    /// update skinned mesh from character skeleton and bind pose mesh
    void UpdateSkinning();

    ///
    void UpdateVertexColorMapByList(const nArray<int>& vertexList);
    ///
    void UpdateVertexPosMapByList(const nArray<int>& vertexList);
    ///
    void UpdateVertexNormalMapByList(const nArray<int>& vertexList);

    // sets or adds a color to every vertex in the list
    bool PaintVertexList(const nArray<int>& vertexList, const vector4& col, bool addCol = false);
    // finds all vertices in range of a faces intersectionpoint
    void FindVerticesInRange(int faceIndex, const vector3& intersectionPoint, float radius, nArray<int>& vertexList);
    // finds all vertices next to the actual vertex
    void FindNeighbourVertices(int vertexIndex, nArray<int>& vertexList, int depth = 1);
    // finds all faces next to the actual face
    void FindNeighbourFaces(int faceIndex, nArray<int>& faceList, int depth = 1);
    //
    void GetVerticesOfFace(int faceIndex, int& v0, int& v1, int& v2);
    //
    float GetSizeOfFace(int faceIndex);
    //
    float GetSizeOfVertexFace(int vertexIndex);

    //
    bool IsDirty();
    ///
    int GetVertexFromFace(int faceIndex, int vertexNr);




private:
    /// initialize resistent mesh
    void InitResistentMesh();
    /// initialize the skinned mesh
    void InitSkinnedMesh();
    /// initialize the face array
    void InitFaceArray();
    /// update the face normals and midpoints from a mesh object
    void UpdateFaces();

    ///
    void UpdateSingleFace(int faceIndex);
    /// find a shapenode child of a transformnode
    void FindShapeNodes(nRoot* parent, nArray<nShapeNode*>& shapeNodes);
    /// intersection check of a triangle
    bool Intersects(face& poly, const line3& line, faceIntersection& intersectionData);
    ///
    void DoMeshCleanUp();
    ///
    void FillNeighbourFaceMap();



    MeshType meshType;
    nString resourceName;
    bool initialized;
    uint lastUpdateFrameId;
    nFixedArray<face> faces;            // face status


    // information of the actual Entity to raycheck
    int meshGroupIndex;
    Ptr<Graphics::CharEntity> charEntity;
    Ptr<Graphics::Entity> activeEntity;

    bool useLastFaceOptimization;
    int lastFace;
    int startVertex;

    // for skinning
    const nCharSkeleton* charSkeleton;  // the character skeleton
    nSkinShapeNode* skinShapeNode;
    nShapeNode* shapeNode;
    bool charSkeletonDirty;             // only need to update skinned mesh when skeleton dirty
    nRef<nMesh2> refOriginalMesh;       // the original mesh that will be rendered
    nRef<nMesh2> refResistentMesh;      // a copy of the original mesh
    nRef<nMeshCopyResourceLoader> refResistentMeshResourceLoader;
    nRef<nMesh2> refSkinnedMesh;        // the skinned mesh, written to analyse
    nRef<nMeshCopyResourceLoader> refSkinnedMeshResourceLoader;

    // for mesh-cleaning
    nMeshBuilder backupMesh;
    nMeshBuilder cleanMesh;
    nArray<nArray<int> > collapseMap;
    nArray<nArray<int> > neighbourFacesMap;
    nArray<nArray<int> > neighbourFacesMapOverOriginalVertex;
    nArray<vector4> vertexColorMap;
    nArray<vector3> vertexPosMap;
    nArray<vector3> vertexNormalMap;

    bool invertZ;

};

//------------------------------------------------------------------------------
/**
*/
inline
nArray<vector4>&
SkinRayCheckUtil::GetVertexColorMap()
{
    return this->vertexColorMap;
}

//------------------------------------------------------------------------------
/**
*/
inline
nArray<vector3>&
SkinRayCheckUtil::GetVertexPosMap()
{
    return this->vertexPosMap;
}

//------------------------------------------------------------------------------
/**
*/
inline
nArray<vector3>&
SkinRayCheckUtil::GetVertexNormalMap()
{
    return this->vertexNormalMap;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
SkinRayCheckUtil::IsDirty()
{
    if (this->refResistentMeshResourceLoader->HasBeenReloaded() ||
        (!this->refSkinnedMeshResourceLoader.isvalid() || this->refSkinnedMeshResourceLoader->HasBeenReloaded()))
    {
        return true;
    }
    else return false;
}

} // namespace Util
//------------------------------------------------------------------------------
#endif
