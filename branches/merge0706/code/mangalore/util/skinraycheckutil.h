#ifndef UTIL_SKINRAYCHECKUTIL_H
#define UTIL_SKINRAYCHECKUTIL_H
//------------------------------------------------------------------------------
/**
    @class Util::SkinRayCheckUtil

    Util class that provides a raycheck method for skinned and static meshes
    INFO:
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
        int segmentationIndex;
        vector3 p0, p1, p2;
        nArray<int> neighbourFaces;
    };

    /// constructor
    SkinRayCheckUtil();
    /// destructor
    virtual ~SkinRayCheckUtil();

    /// set graphics entity
    void SetGraphicsEntity(Graphics::Entity* entity);

    /// make a raycheck and return all intersected faces as index
    bool DoRayCheck(const line3& ray, nArray<faceIntersection>& intersectedFaces);

    // get face by Index
    face& GetFace(int faceIndex);

    // get uv of face intersection
    bool GetIntersectionUV(int uvNr, faceIntersection& intersection, vector2& resultUV);

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
    bool AddVertexColorOfFace(int faceIndex, const vector4& col);
    // has vertexcolor
    bool HasMeshVertexColor();

    /// update original mesh from resistent mesh
    void UpdateOriginalMesh();

    nFixedArray<vector3> vertices;      //DEBUG

private:
    /// initialize resistent mesh
    void InitResistentMesh();
    /// initialize the skinned mesh
    void InitSkinnedMesh();
    /// initialize the face array
    void InitFaceArray();
    /// update the face normals and midpoints from a mesh object
    void UpdateFaces();
    /// update skinned mesh from character skeleton and bind pose mesh
    void UpdateSkinning();
    ///
    void UpdateSingleFace(int faceIndex);
    /// find a shapenode child of a transformnode
    void FindShapeNodes(nRoot* parent, nArray<nShapeNode*>& shapeNodes);
    /// intersection check of a triangle
    bool Intersects(face& poly, const line3& line, faceIntersection& intersectionData);
    /// find neighbour faces of a face
    void FindNeighbourFaces(int faceIndex);


    MeshType meshType;
    nString resourceName;
    bool initialized;
    uint lastUpdateFrameId;
    nFixedArray<face> faces;            // face status

    // for the segmentation optimization
    nFixedArray<bbox3> segmentationBoxes;
    int segmentationWidth;

    // information of the actual Entity to raycheck
    int meshGroupIndex;
    Ptr<Graphics::CharEntity> charEntity;
    Ptr<Graphics::Entity> activeEntity;

    bool useLastFaceOptimization;
    int lastFace;

    // for skinning
    const nCharSkeleton* charSkeleton;  // the character skeleton
    nSkinShapeNode* skinShapeNode;
    bool charSkeletonDirty;             // only need to update skinned mesh when skeleton dirty
    nRef<nMesh2> refOriginalMesh;       // the original mesh that will be rendered
    nRef<nMesh2> refResistentMesh;      // a copy of the original mesh
    nRef<nMesh2> refSkinnedMesh;        // the skinned mesh, written to analyse
};

} // namespace Util
//------------------------------------------------------------------------------
#endif
