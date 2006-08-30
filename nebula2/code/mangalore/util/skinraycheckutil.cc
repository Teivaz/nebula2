//------------------------------------------------------------------------------
//  util/skinraycheckutil.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------

#include "util/skinraycheckutil.h"
#include "graphics/resource.h"
#include "character/ncharacter2.h"
#include "managers/timemanager.h"


namespace Util
{
    using namespace Graphics;
//------------------------------------------------------------------------------
/**
*/
SkinRayCheckUtil::SkinRayCheckUtil():
    charSkeleton(0),
    charSkeletonDirty(false),
    initialized(false),
    lastUpdateFrameId(0),
    segmentationWidth(0),
    useLastFaceOptimization(true),
    lastFace(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SkinRayCheckUtil::~SkinRayCheckUtil()
{
    //this->refSkinnedMesh->Unload();
    this->refSkinnedMesh = 0;
    this->refOriginalMesh = 0;
    this->skinShapeNode = 0;
    this->charEntity = 0;
    this->activeEntity = 0;
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::SetGraphicsEntity(Graphics::Entity* entity)
{
    n_assert(entity);

    this->activeEntity = entity;
    nTransformNode* node = entity->GetResource().GetNode();
    nArray<nShapeNode*> shapeNodes;
    // find all subordinated shapenodes
    this->FindShapeNodes(node, shapeNodes);
    // handle a skinned mesh
    if (entity->IsA(Graphics::CharEntity::RTTI) &&
        shapeNodes.Size() > 0 &&
        shapeNodes[0]->IsA(nKernelServer::Instance()->FindClass("nskinshapenode")))
    {
        this->meshType = SkinRayCheckUtil::Skinned;

        CharEntity* charEntity = (CharEntity*) entity;
        nCharacter2* nebCharacter = charEntity->GetCharacterPointer();
        // FIXME: only the first shapenode will be used
        this->skinShapeNode = (nSkinShapeNode*) shapeNodes[0];

        this->charEntity = charEntity;

        charEntity->EvaluateSkeleton();
        this->charSkeleton = &nebCharacter->GetSkeleton();
        this->meshGroupIndex = this->skinShapeNode->GetGroupIndex();
        this->refOriginalMesh = this->skinShapeNode->GetMeshObject();
        this->resourceName = charEntity->GetResourceName();

        //initialize rsistent mesh
        this->InitResistentMesh();
        // initialize skinned mesh
        this->InitSkinnedMesh();
        // update skinned mesh
        this->UpdateSkinning();
        // initialize face array
        this->InitFaceArray();
        // create the face normals
        this->UpdateFaces();

        this->initialized = true;
    }
    // handle a static Mesh
    else if(entity->IsA(Graphics::Entity::RTTI) &&
            shapeNodes.Size() > 0)
    {
        this->meshType = SkinRayCheckUtil::Static;

        // FIXME: only the first shapenode will be used
        nShapeNode* shapeNode = shapeNodes[0];
        this->meshGroupIndex = shapeNode->GetGroupIndex();
        this->refOriginalMesh = shapeNode->GetMeshObject();

        //initialize rsistent mesh
        this->InitResistentMesh();
        this->refSkinnedMesh = this->refResistentMesh.get();
        //initialize face array
        this->InitFaceArray();
        //create the face normals
        this->UpdateFaces();

        this->initialized = true;
    }
    else
    {
        n_error("Unsupported type of GraphicsEntity or nShapeNode!");
    }
}

//------------------------------------------------------------------------------
/**
    Initialize the resistent mesh from the original mesh
*/
void
SkinRayCheckUtil::InitResistentMesh()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    // create the skinned mesh, this will be filled by the CPU with skinned data
    nString resistentMeshName = this->resourceName + "r";
    nMesh2* resistentMesh = gfxServer->NewMesh(resistentMeshName.Get());
    this->refResistentMesh = resistentMesh;
    nMesh2* originalMesh = this->refOriginalMesh;
    if (!resistentMesh->IsLoaded())
    {
        resistentMesh->SetVertexUsage(nMesh2::ReadOnly);
        resistentMesh->SetIndexUsage(nMesh2::ReadOnly);
        resistentMesh->SetVertexComponents(originalMesh->GetVertexComponents());
        resistentMesh->SetNumVertices(originalMesh->GetNumVertices());
        resistentMesh->SetNumIndices(originalMesh->GetNumIndices());
        resistentMesh->SetNumEdges(originalMesh->GetNumEdges());
        int numGroups = originalMesh->GetNumGroups();
        resistentMesh->SetNumGroups(numGroups);
        int groupIndex;
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            resistentMesh->Group(groupIndex) = originalMesh->Group(groupIndex);
        }
        resistentMesh->Load();

        // transfer vertices from bindPoseMesh
        int numVertices = originalMesh->GetNumVertices();
        int srcVertexWidth = originalMesh->GetVertexWidth();
        float* srcVertices = originalMesh->LockVertices();
        float* dstVertices = resistentMesh->LockVertices();
        int vertexWidth = originalMesh->GetVertexWidth();
        int vertexIndex;
        for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
        {
            int srcOffset = vertexIndex * srcVertexWidth;
            int i;
            for (i=0; i < vertexWidth; i++)
            {
                *dstVertices++ = srcVertices[srcOffset + i];
            }
        }
        originalMesh->UnlockVertices();
        resistentMesh->UnlockVertices();

        // transfer indices into skinned mesh and fix them according to the new vertex layout
        int numIndices = originalMesh->GetNumIndices();
        ushort* srcIndices = originalMesh->LockIndices();
        ushort* dstIndices = resistentMesh->LockIndices();
        int ii;
        for (ii = 0; ii < numIndices; ii++)
        {
            dstIndices[ii] = srcIndices[ii];
        }
        originalMesh->UnlockIndices();
        resistentMesh->UnlockIndices();
    }
    if (!this->refSkinnedMesh.isvalid()) this->refSkinnedMesh = this->refResistentMesh;
}

//------------------------------------------------------------------------------
/**
    Face list will be updated from the skinned or static mesh
*/
void
SkinRayCheckUtil::UpdateFaces()
{
    n_assert(this->refSkinnedMesh);
    nMesh2* mesh = this->refSkinnedMesh;

    const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
    ushort startIndex = meshGroup.GetFirstIndex();
    float* vertices = mesh->LockVertices();
    ushort* indices = mesh->LockIndices();

    int vertexWidth = mesh->GetVertexWidth();
    int numFaces = this->faces.Size();
    n_assert(meshGroup.GetNumIndices() == (numFaces * 3));
    matrix44 m = this->activeEntity->GetTransform();

    int faceIndex;
    for (faceIndex = 0; faceIndex < numFaces; faceIndex++)
    {
        /// each face gets 3 own vertices
        int ii = startIndex + faceIndex * 3;
        this->faces[faceIndex].p0 = *(vector3*)(vertices + indices[ii] * vertexWidth);
        this->faces[faceIndex].p1 = *(vector3*)(vertices + indices[ii + 1] * vertexWidth);
        this->faces[faceIndex].p2 = *(vector3*)(vertices + indices[ii + 2] * vertexWidth);

        ///----begin preparation for segmentation optimization----
        // check if segmentationIndex is still correct
        if (this->faces[faceIndex].segmentationIndex > -1)
        {
            if (this->segmentationBoxes[this->faces[faceIndex].segmentationIndex].contains(this->faces[faceIndex].p0) &&
                this->segmentationBoxes[this->faces[faceIndex].segmentationIndex].contains(this->faces[faceIndex].p1) &&
                this->segmentationBoxes[this->faces[faceIndex].segmentationIndex].contains(this->faces[faceIndex].p2))
            {
                // do nothing
            }
            else this->faces[faceIndex].segmentationIndex = -1;
        }
        // if segmentationIndex is not correct compute new
        if (this->faces[faceIndex].segmentationIndex == -1)
        {
            int i;
            for(i=0; i<this->segmentationBoxes.Size(); i++)
            {
                if (this->segmentationBoxes[i].contains(this->faces[faceIndex].p0) &&
                    this->segmentationBoxes[i].contains(this->faces[faceIndex].p1) &&
                    this->segmentationBoxes[i].contains(this->faces[faceIndex].p2))
                {
                    this->faces[faceIndex].segmentationIndex = i;
                    break;
                }
            }
        }
    }
    mesh->UnlockIndices();
    mesh->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
    Prepare the face array and the segmentation boxes
*/
void
SkinRayCheckUtil::InitFaceArray()
{
    // allocate the faces array
    const nMeshGroup& meshGroup = this->refSkinnedMesh->Group(this->meshGroupIndex);
    face emptyFace;
    emptyFace.segmentationIndex = -1;
    this->faces.SetSize(meshGroup.GetNumIndices() / 3);
    this->faces.Clear(emptyFace);

    // prepare segmentation
    // FIXME: has to be checked! Somewhere may be a mistake...
    int numSegementations = this->segmentationWidth * this->segmentationWidth * this->segmentationWidth;
    this->segmentationBoxes.SetSize(numSegementations);
    bbox3 bbox = this->activeEntity->GetLocalBox();
    //matrix44 m;
    //m.ident();
    //bbox.transform(m);
    //m = this->activeEntity->GetTransform();

    int i;
    int j;
    int k;
    float xLen = bbox.size().x / this->segmentationWidth;
    float yLen = bbox.size().y / this->segmentationWidth;
    float zLen = bbox.size().z / this->segmentationWidth;
    vector3 mainCenter = bbox.center();
    vector3 subCenter;
    vector3 extend;
    extend.x = xLen*0.5f;
    extend.y = yLen*0.5f;
    extend.z = zLen*0.5f;
    for (i=0; i < this->segmentationWidth; i++)
    {
        subCenter.x = mainCenter.x - (xLen * this->segmentationWidth * 0.5f) + ((i + 0.5f) * xLen);
        for (j=0; j < this->segmentationWidth; j++)
        {
            subCenter.y = mainCenter.y - (yLen * this->segmentationWidth * 0.5f) + ((j + 0.5f) * yLen);
            for (k=0; k < this->segmentationWidth; k++)
            {
                subCenter.z = mainCenter.z - (zLen * this->segmentationWidth * 0.5f) + ((k + 0.5f) * zLen);
                int index = (i*this->segmentationWidth) + (j*this->segmentationWidth) + k;
                this->segmentationBoxes[index].set(subCenter,extend);
            }
        }
    }

    this->vertices.SetSize(meshGroup.GetNumIndices());  //DEBUG

    /*this->UpdateFaces();
    for (i=0; i < this->faces.Size(); i++)
    {
        this->FindNeighbourFaces(i);
    }*/
}

//------------------------------------------------------------------------------
/**
    Initialize the skinned mesh from the bindposemesh
*/
void
SkinRayCheckUtil::InitSkinnedMesh()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    // create the skinned mesh, this will be filled by the CPU with skinned data
    nString skinnedMeshName = this->resourceName + "s";
    nMesh2* skinnedMesh = gfxServer->NewMesh(skinnedMeshName.Get());
    this->refSkinnedMesh = skinnedMesh;
    nMesh2* bindPoseMesh = this->refResistentMesh;
    if (!skinnedMesh->IsLoaded())
    {
        //skinnedMesh->SetVertexUsage(nMesh2::ReadWrite | nMesh2::NeedsVertexShader);
        skinnedMesh->SetVertexUsage(nMesh2::ReadOnly);
        skinnedMesh->SetIndexUsage(nMesh2::ReadOnly);
        skinnedMesh->SetVertexComponents(nMesh2::Coord4);
        skinnedMesh->SetNumVertices(bindPoseMesh->GetNumVertices());
        skinnedMesh->SetNumIndices(bindPoseMesh->GetNumIndices());
        skinnedMesh->SetNumEdges(bindPoseMesh->GetNumEdges());
        int numGroups = bindPoseMesh->GetNumGroups();
        skinnedMesh->SetNumGroups(numGroups);
        int groupIndex;
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            skinnedMesh->Group(groupIndex) = bindPoseMesh->Group(groupIndex);
        }
        skinnedMesh->Load();

        // transfer vertices from bindPoseMesh
        int numVertices = bindPoseMesh->GetNumVertices();
        int srcVertexWidth = bindPoseMesh->GetVertexWidth();
        float* srcVertices = bindPoseMesh->LockVertices();
        float* dstVertices = skinnedMesh->LockVertices();
        int vertexIndex;
        for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
        {
            int srcOffset = vertexIndex * srcVertexWidth;

            *dstVertices++ = srcVertices[srcOffset];
            *dstVertices++ = srcVertices[srcOffset + 1];
            *dstVertices++ = srcVertices[srcOffset + 2];
            *dstVertices++ = 1.0f;
        }
        bindPoseMesh->UnlockVertices();
        skinnedMesh->UnlockVertices();

        // transfer indices into skinned mesh and fix them according to the new vertex layout
        int numIndices = bindPoseMesh->GetNumIndices();
        ushort* srcIndices = bindPoseMesh->LockIndices();
        ushort* dstIndices = skinnedMesh->LockIndices();
        int ii;
        for (ii = 0; ii < numIndices; ii++)
        {
            dstIndices[ii] = srcIndices[ii];
        }
        bindPoseMesh->UnlockIndices();
        skinnedMesh->UnlockIndices();

        // transfer indices into skinned mesh and fix their vertex indices according to the new vertex layout
        int numEdges = bindPoseMesh->GetNumEdges();
        if (numEdges)
        {
            nMesh2::Edge* srcEdges = bindPoseMesh->LockEdges();
            nMesh2::Edge* dstEdges = skinnedMesh->LockEdges();
            int edgeIndex;
            for (edgeIndex = 0; edgeIndex < numEdges; edgeIndex++)
            {
                dstEdges[edgeIndex] = srcEdges[edgeIndex];
            }
        }
        bindPoseMesh->UnlockEdges();
        skinnedMesh->UnlockEdges();
    }
}

//------------------------------------------------------------------------------
/**
    This method performs the actual skinning on the cpu and writes the
    skinned vertices into the refSkinnedMesh. A valid and uptodate character
    skeleton must be set through

    FIXME: performance optimization: convert float joint indices to integer
    during load time?
*/
void
SkinRayCheckUtil::UpdateSkinning()
{
    n_assert(this->charSkeleton);
    n_assert(this->skinShapeNode);
    nMesh2* srcMesh = this->refResistentMesh;
    nMesh2* dstMesh = this->refSkinnedMesh;
    float* srcVertexBase = srcMesh->LockVertices();
    float* dstVertexBase = dstMesh->LockVertices();
    int numFrags = this->skinShapeNode->GetNumFragments();
    int numGroups = srcMesh->GetNumGroups();

    n_assert(numFrags <= numGroups);

    int curFrag;
    for (curFrag=0; curFrag < numFrags; curFrag++)
    {
        int groupIndex = this->skinShapeNode->GetFragGroupIndex(curFrag);
        int paletteSize = this->skinShapeNode->GetJointPaletteSize(curFrag);

        const nMeshGroup& srcGroup = srcMesh->Group(groupIndex);
        const nMeshGroup& dstGroup = dstMesh->Group(groupIndex);
        int numSrcVertices = srcGroup.GetNumVertices();
        int numDstVertices = dstGroup.GetNumVertices();
        n_assert(numSrcVertices == numDstVertices);

        float* srcVertices = srcVertexBase + srcGroup.GetFirstVertex();
        float* dstVertices = dstVertexBase + dstGroup.GetFirstVertex();
        vector3 v;
        int index;
        for (index = 0; index < numSrcVertices; index++)
        {
            // perform weighted skinning
            const vector3& srcVec = *((vector3*)srcVertices);
            float* weights = &srcVertices[srcMesh->GetVertexComponentOffset(nMesh2::Weights)];
            float* indices = &srcVertices[srcMesh->GetVertexComponentOffset(nMesh2::JIndices)];
            v.set(0.0f, 0.0f, 0.0f);
            int i;
            for (i = 0; i < 4; i++)
            {
                if (weights[i] > 0.0f)
                {
                    const matrix44& skinMatrix = this->charSkeleton->GetJointAt(this->skinShapeNode->GetJointIndex(curFrag, int(indices[i]))).GetSkinMatrix44();
                    //const matrix44& skinMatrix = this->charSkeleton->GetJointAt(int(indices[i])).GetSkinMatrix44();
                    skinMatrix.weighted_madd(srcVec, v, weights[i]);
                }
            }

            int x = this->charSkeleton->GetNumJoints();
            int y = srcMesh->GetNumGroups();

            // write skinned vertex to vertex position, note, we are filling
            // a dynamic vertex buffer which will be discarded after rendering, so we NEED to write
            // seeminigly constant data (the extrude weights) as well!
            *dstVertices++ = v.x; *dstVertices++ = v.y; *dstVertices++ = v.z; *dstVertices++ = 1.0f;

            // set to next src vertex
            srcVertices += srcMesh->GetVertexWidth();
        }
    }
    dstMesh->UnlockVertices();
    srcMesh->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
    Does a raycheck, returns if an intersection occured
    and fills an array of faceIntersections
*/
bool
SkinRayCheckUtil::DoRayCheck(const line3 &ray, nArray<faceIntersection> &intersectedFaces)
{
    n_assert(this->initialized);

    matrix44 trans = this->activeEntity->GetTransform();
    vector3 transPos = trans.pos_component();
    trans.translate(-transPos);
    trans.rotate(vector3(0,1,0), N_PI);
    trans.translate(transPos);
    matrix44 invTrans = trans;
    invTrans.invert();
    line3 tRay;
    tRay.set(invTrans.transform_coord(ray.start()),invTrans.transform_coord(ray.end()));

    /// Checks if ray intersects boundingbox
    /// FIXME: not really exact...
    bbox3 bbox = this->activeEntity->GetBox();
    if (ray.distance(bbox.center()) > (bbox.diagonal_size()*0.5f)) return false;

    uint currentFrameId = Managers::TimeManager::Instance()->GetFrameId();

    bool updateFaces = false;


    // Only update once a frame
    // only skinned meshes have to be updated
    if (currentFrameId != this->lastUpdateFrameId &&
        this->meshType == Skinned)
    {
        this->charEntity->EvaluateSkeleton();
        this->UpdateSkinning();
        this->lastUpdateFrameId = currentFrameId;
        updateFaces = true;
    }

    bool intersected = false;
    int numFaces = this->faces.Size();
    faceIntersection newFaceIntersection;

    if (this->useLastFaceOptimization && this->lastFace >= 0)
    {
        // Optimization: Before updating all faces/vertices,
        // just update the last intersected one and check it.
        if (updateFaces) this->UpdateSingleFace(this->lastFace);
        if (this->Intersects(this->faces[this->lastFace], tRay, newFaceIntersection))
        {
            // collect information about face intersection and append to array
            newFaceIntersection.faceIndex = this->lastFace;
            newFaceIntersection.distance = ray.start().distance(ray.start(), newFaceIntersection.intersectionPoint);
            newFaceIntersection.intersectionPoint = trans.transform_coord(newFaceIntersection.intersectionPoint);
            newFaceIntersection.normal = trans.transform_coord(newFaceIntersection.normal);
            newFaceIntersection.normal = newFaceIntersection.normal - trans.pos_component();
            newFaceIntersection.normal.norm();
            intersectedFaces.Append(newFaceIntersection);
            intersected = true;
        }
    }

    // check all faces
    if (!intersected)
    {
        // if faces are not up to date update them!
        if (updateFaces) this->UpdateFaces();

        int i;
        for (i=0; i < numFaces; i++)
        {
            if (this->Intersects(this->faces[i], tRay, newFaceIntersection))
            {
                // collect information about face intersection and append to array
                newFaceIntersection.faceIndex = i;
                newFaceIntersection.distance = ray.start().distance(ray.start(), newFaceIntersection.intersectionPoint);
                newFaceIntersection.intersectionPoint = trans.transform_coord(newFaceIntersection.intersectionPoint);
                newFaceIntersection.normal = trans.transform_coord(newFaceIntersection.normal);
                newFaceIntersection.normal = newFaceIntersection.normal - trans.pos_component();
                newFaceIntersection.normal.norm();
                intersectedFaces.Append(newFaceIntersection);
                intersected = true;
            }
        }
    }

    if (intersected)
    {
        int nearest = 0;
        int i;
        for (i=1; i < intersectedFaces.Size(); i++)
        {
            if (intersectedFaces[i].distance < intersectedFaces[nearest].distance) nearest = i;
        }
        this->lastFace = intersectedFaces[nearest].faceIndex;
    }
    else this->lastFace = -1;

    return intersected;
}

//------------------------------------------------------------------------------
/**
    returns a face by index
*/
SkinRayCheckUtil::face&
SkinRayCheckUtil::GetFace(int faceIndex)
{
    n_assert(this->faces.Size() > faceIndex);
    return this->faces[faceIndex];
}

//------------------------------------------------------------------------------
/**
    searches recursivly for shapenodes and returns them in an array
*/
void
SkinRayCheckUtil::FindShapeNodes(nRoot *parent, nArray<nShapeNode*> &shapeNodes)
{
    if (parent->IsA(nKernelServer::Instance()->FindClass("nshapenode")))
    {
        shapeNodes.Append((nShapeNode*) parent);
    }
    nRoot* node;
    for (node = parent->GetHead();
         node;
         node = node->GetSucc())
    {
        this->FindShapeNodes(node, shapeNodes);
    }
}

//------------------------------------------------------------------------------
/**
    Checks if a face gets intersected by a ray
    FIXME: segmentation optimization may not work proper!
*/
bool
SkinRayCheckUtil::Intersects(face &poly, const line3 &line, faceIntersection& intersectionData)
{
    // segmentation optimization:
    // if ray doesn't intersects the faces segmentation, it doesn't has be checked
    //matrix44 m = this->activeEntity->GetTransform();
    //line3 tRay(m.transform_coord(line.start()), m.transform_coord(line.end()));
    /*if (poly.segmentationIndex == -1 ||
        tRay.distance(this->segmentationBoxes[poly.segmentationIndex].center())
        <= (this->segmentationBoxes[poly.segmentationIndex].diagonal_size() * 0.5f))
    {*/

        vector3 n;
        vector3 pa = poly.p0;
        vector3 pb = poly.p1;
        vector3 pc = poly.p2;

        vector3 p2 = line.start();
        vector3 p1 = line.end();

        vector3 pa1, pa2, pa3;
        vector3 ip; // intersection point


        // Calculate the normal vector of the plane
        n.x = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
        n.y = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
        n.z = (pb.x - pa.x)*(pc.y - pa.y) - (pb.y - pa.y)*(pc.x - pa.x);
        n.norm();
        intersectionData.normal = n;
        vector3 lineVec = line.vec();
        lineVec.norm();
        if (n.dot(lineVec)> 0) return false;
        double d = - (n.x * pa.x) - (n.y * pa.y) - (n.z * pa.z);
        double denom = n.x * (p2.x - p1.x) + n.y * (p2.y - p1.y) + n.z * (p2.z - p1.z);
        if (abs(denom) < 0.00001f) return false; // Line and plane don't intersect

        double mu = - (d + n.x * p1.x + n.y * p1.y + n.z * p1.z) / denom;
        if (mu < 0 || mu > 1) return false;   // Intersection not along line segment

        // compute intersection point
        ip.x = (float)(p1.x + mu * (p2.x - p1.x));
        ip.y = (float)(p1.y + mu * (p2.y - p1.y));
        ip.z = (float)(p1.z + mu * (p2.z - p1.z));
        intersectionData.intersectionPoint = ip;

        /* Determine whether or not the intersection point is bounded by pa,pb,pc */
        float a1, a2, a3;
        pa1 = pa - ip;
        pa1.norm();
        pa2 = pb - ip;
        pa2.norm();
        pa3 = pc - ip;
        pa3.norm();
        a1 = pa1.x*pa2.x + pa1.y*pa2.y + pa1.z*pa2.z;
        a2 = pa2.x*pa3.x + pa2.y*pa3.y + pa2.z*pa3.z;
        a3 = pa3.x*pa1.x + pa3.y*pa1.y + pa3.z*pa1.z;
        double total = n_acos(a1) + n_acos(a2) + n_acos(a3);
        if (abs(total - (2*N_PI)) > 0.00001f) return false;
        else return true;
    /*}
    else
    {
        return false;
    }*/
}

//------------------------------------------------------------------------------
/**
    determines the uv coordinate of an intersection point.
*/
bool
SkinRayCheckUtil::GetIntersectionUV(int uvNr, faceIntersection &intersection, vector2& resultUV)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* skinnedMesh = this->refSkinnedMesh.get();
    nMesh2* resistentMesh = this->refResistentMesh.get();

    bool exists = false;
    int offset;

    switch(uvNr)
    {
        case -1: //----DEBUG----
            {
                if(resistentMesh->HasAllVertexComponents(nMesh2::Color))
                {
                    exists = true;
                    offset = resistentMesh->GetVertexComponentOffset(nMesh2::Color);
                }
                break;
            }  //-------------
        case 0:
            {
                if(resistentMesh->HasAllVertexComponents(nMesh2::Uv0))
                {
                    exists = true;
                    offset = resistentMesh->GetVertexComponentOffset(nMesh2::Uv0);
                }
                break;
            }
        case 1:
            {
                if(resistentMesh->HasAllVertexComponents(nMesh2::Uv1))
                {
                    exists = true;
                    offset = resistentMesh->GetVertexComponentOffset(nMesh2::Uv1);
                }
                break;
            }
        case 2:
            {
                if(resistentMesh->HasAllVertexComponents(nMesh2::Uv2))
                {
                    exists = true;
                    offset = resistentMesh->GetVertexComponentOffset(nMesh2::Uv2);
                }
                break;
            }
        case 3:
            {
                if(resistentMesh->HasAllVertexComponents(nMesh2::Uv3))
                {
                    exists = true;
                    offset = resistentMesh->GetVertexComponentOffset(nMesh2::Uv3);
                }
                break;
            }
        default: exists = false; break;
    }

    if (exists)
    {

        // read vertex positions from skinned mesh
        const nMeshGroup& skinnedMeshGroup = skinnedMesh->Group(this->meshGroupIndex);
        ushort startIndex = skinnedMeshGroup.GetFirstIndex();
        float* vertices = skinnedMesh->LockVertices();
        ushort* indices = skinnedMesh->LockIndices();
        int vertexWidth = skinnedMesh->GetVertexWidth();

        int ii = startIndex + intersection.faceIndex * 3;

        vector3 p0 = *(vector3*)(vertices + indices[ii] * vertexWidth);
        vector3 p1 = *(vector3*)(vertices + indices[ii + 1] * vertexWidth);
        vector3 p2 = *(vector3*)(vertices + indices[ii + 2] * vertexWidth);

        skinnedMesh->UnlockIndices();
        skinnedMesh->UnlockVertices();


        // read information from resistent mesh
        const nMeshGroup& resistentMeshGroup = resistentMesh->Group(this->meshGroupIndex);
        startIndex = resistentMeshGroup.GetFirstIndex();
        vertices = resistentMesh->LockVertices();
        indices = resistentMesh->LockIndices();
        vertexWidth = resistentMesh->GetVertexWidth();

        ii = startIndex + intersection.faceIndex * 3;

        vector2 p0uv = *(vector2*)(vertices + indices[ii] * vertexWidth + offset);
        vector2 p1uv = *(vector2*)(vertices + indices[ii + 1] * vertexWidth + offset);
        vector2 p2uv = *(vector2*)(vertices + indices[ii + 2] * vertexWidth + offset);

        vector4 col;

        col = *(vector4*)(vertices + indices[ii] * vertexWidth + offset);
        col.y += 0.05f;
        n_clamp(col.y,0,1);
        *(vector4*)(vertices + indices[ii] * vertexWidth + offset) = col;   //DEBUG

        col = *(vector4*)(vertices + indices[ii+1] * vertexWidth + offset);
        col.y += 0.05f;
        n_clamp(col.y,0,1);
        *(vector4*)(vertices + indices[ii + 1] * vertexWidth + offset) = col;   //DEBUG

        col = *(vector4*)(vertices + indices[ii+2] * vertexWidth + offset);
        col.y += 0.05f;
        n_clamp(col.y,0,1);
        *(vector4*)(vertices + indices[ii + 2] * vertexWidth + offset) = col;   //DEBUG

        vector3 ip = intersection.intersectionPoint;
        vector3 weight;
        weight.x = ip.distance(ip, p0);
        weight.y = ip.distance(ip, p1);
        weight.z = ip.distance(ip, p2);
        float weightSum = weight.x + weight.y + weight.z;
        weight = weight / weightSum;
        resultUV = (p0uv * weight.x) + (p1uv * weight.y) + (p2uv * weight.z);

        resistentMesh->UnlockIndices();
        resistentMesh->UnlockVertices();

        this->UpdateOriginalMesh();

        return true;
    }
    else return false;
}

//------------------------------------------------------------------------------
/**

*/
bool
SkinRayCheckUtil::SetVertexColorOfMesh(const vector4& col)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        ushort startIndex = meshGroup.GetFirstIndex();
        float* vertices = mesh->LockVertices();
        ushort* indices = mesh->LockIndices();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        vector4 clampCol;
        clampCol.x = n_clamp(col.x, 0, 1);
        clampCol.y = n_clamp(col.y, 0, 1);
        clampCol.z = n_clamp(col.z, 0, 1);
        clampCol.w = n_clamp(col.w, 0, 1);

        int index;
        for (index = 0; index < meshGroup.GetNumIndices(); index++)
        {
            int ii = startIndex + index;
            *(vector4*)(vertices + indices[ii] * vertexWidth + offset) = clampCol;
        }
        mesh->UnlockIndices();
        mesh->UnlockVertices();
        this->UpdateOriginalMesh();
        return true;
    }
    else return false;
}

//------------------------------------------------------------------------------
/**

*/
vector4
SkinRayCheckUtil::GetAverageVertexColorOfMesh()
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    vector4 averageColor(0,0,0,0);
    int numVertices = 0;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        ushort startIndex = meshGroup.GetFirstIndex();
        float* vertices = mesh->LockVertices();
        ushort* indices = mesh->LockIndices();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        int index;
        for (index = 0; index < meshGroup.GetNumIndices(); index++)
        {
            int ii = startIndex + index;
            averageColor += *(vector4*)(vertices + indices[ii] * vertexWidth + offset);
            numVertices++;
        }
        mesh->UnlockIndices();
        mesh->UnlockVertices();
    }
    averageColor *= 1/(float)numVertices;
    return averageColor;
}

//------------------------------------------------------------------------------
/**

*/
bool
SkinRayCheckUtil::SetVertexColorOfFace(int faceIndex, const vector4& col)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        ushort startIndex = meshGroup.GetFirstIndex();
        float* vertices = mesh->LockVertices();
        ushort* indices = mesh->LockIndices();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        vector4 clampCol;
        clampCol.x = n_clamp(col.x, 0, 1);
        clampCol.y = n_clamp(col.y, 0, 1);
        clampCol.z = n_clamp(col.z, 0, 1);
        clampCol.w = n_clamp(col.w, 0, 1);

        int ii = startIndex + faceIndex * 3;
        *(vector4*)(vertices + indices[ii] * vertexWidth + offset) = clampCol;
        *(vector4*)(vertices + indices[ii+1] * vertexWidth + offset) = clampCol;
        *(vector4*)(vertices + indices[ii+2] * vertexWidth + offset) = clampCol;

        mesh->UnlockIndices();
        mesh->UnlockVertices();
        this->UpdateOriginalMesh();
        return true;
    }
    else return false;
}

//------------------------------------------------------------------------------
/**

*/
bool
SkinRayCheckUtil::AddVertexColorOfFace(int faceIndex, const vector4& col)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        ushort startIndex = meshGroup.GetFirstIndex();
        float* vertices = mesh->LockVertices();
        ushort* indices = mesh->LockIndices();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        int ii = startIndex + faceIndex * 3;
        vector4 tempCol;
        int k;
        for (k=0; k < 3; k++)
        {
            tempCol = *(vector4*)(vertices + indices[ii+k] * vertexWidth + offset);
            tempCol +=col;
            tempCol.x = n_clamp(tempCol.x, 0, 1);
            tempCol.y = n_clamp(tempCol.y, 0, 1);
            tempCol.z = n_clamp(tempCol.z, 0, 1);
            tempCol.w = n_clamp(tempCol.w, 0, 1);
            *(vector4*)(vertices + indices[ii+k] * vertexWidth + offset) = tempCol;
        }

        mesh->UnlockIndices();
        mesh->UnlockVertices();
        this->UpdateOriginalMesh();
        return true;
    }
    else return false;
}

//------------------------------------------------------------------------------
/**

*/
vector4
SkinRayCheckUtil::GetVertexColorOfFace(int faceIndex)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        ushort startIndex = meshGroup.GetFirstIndex();
        float* vertices = mesh->LockVertices();
        ushort* indices = mesh->LockIndices();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        vector4 col1;
        vector4 col2;
        vector4 col3;
        vector4 resultCol;

        int ii = startIndex + faceIndex * 3;
        col1 = *(vector4*)(vertices + indices[ii] * vertexWidth + offset);
        col2 = *(vector4*)(vertices + indices[ii+1] * vertexWidth + offset);
        col3 = *(vector4*)(vertices + indices[ii+2] * vertexWidth + offset);

        resultCol = (col1 + col2 + col3)* 0.333333333f;

        mesh->UnlockIndices();
        mesh->UnlockVertices();
        return resultCol;
    }
    else return vector4(-1,-1,-1,-1);
}

//------------------------------------------------------------------------------
/**

*/
vector4
SkinRayCheckUtil::GetVertexColorOfFace(int faceIndex, const vector3& intersectionPoint)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        ushort startIndex = meshGroup.GetFirstIndex();
        float* vertices = mesh->LockVertices();
        ushort* indices = mesh->LockIndices();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        vector4 col0;
        vector4 col1;
        vector4 col2;
        vector4 resultCol;

        int ii = startIndex + faceIndex * 3;
        col0 = *(vector4*)(vertices + indices[ii] * vertexWidth + offset);
        col1 = *(vector4*)(vertices + indices[ii+1] * vertexWidth + offset);
        col2 = *(vector4*)(vertices + indices[ii+2] * vertexWidth + offset);

        float distP0 = intersectionPoint.distance(intersectionPoint, this->faces[faceIndex].p0);
        float distP1 = intersectionPoint.distance(intersectionPoint, this->faces[faceIndex].p1);
        float distP2 = intersectionPoint.distance(intersectionPoint, this->faces[faceIndex].p2);
        resultCol = (col0*distP0 + col1*distP1 + col2*distP2) * (1/(distP0 + distP1 + distP2));

        mesh->UnlockIndices();
        mesh->UnlockVertices();
        return resultCol;
    }
    else return vector4(-1,-1,-1,-1);
}

//------------------------------------------------------------------------------
/**

*/
bool
SkinRayCheckUtil::HasMeshVertexColor()
{
    n_assert(this->refResistentMesh);
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color)) return true;
    else return false;
}

//------------------------------------------------------------------------------
/**
    update original mesh from resistent mesh
*/
void
SkinRayCheckUtil::UpdateOriginalMesh()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    nMesh2* resistentMesh = this->refResistentMesh;
    nMesh2* originalMesh = this->refOriginalMesh;

    // transfer vertices from bindPoseMesh
    int numVertices = originalMesh->GetNumVertices();
    int srcVertexWidth = originalMesh->GetVertexWidth();
    float* originalVertices = originalMesh->LockVertices();
    float* resistentVertices = resistentMesh->LockVertices();
    int vertexWidth = originalMesh->GetVertexWidth();
    int vertexIndex;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        int srcOffset = vertexIndex * srcVertexWidth;
        int i;
        for (i=0; i < vertexWidth; i++)
        {
            *originalVertices++ = resistentVertices[srcOffset + i];
        }
    }
    originalMesh->UnlockVertices();
    resistentMesh->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::FindNeighbourFaces(int faceIndex)
{
    int i;
    for (i=0; i < this->faces.Size(); i++)
    {
        if (this->faces[i].neighbourFaces.FindIndex(faceIndex) > -1)
        {
            this->faces[faceIndex].neighbourFaces.Append(i);
        }
        else
        if (((this->faces[faceIndex].p0.x == this->faces[i].p0.x &&
            this->faces[faceIndex].p0.y == this->faces[i].p0.y &&
            this->faces[faceIndex].p0.z == this->faces[i].p0.z) ||
            (this->faces[faceIndex].p0.x == this->faces[i].p1.x &&
            this->faces[faceIndex].p0.y == this->faces[i].p1.y &&
            this->faces[faceIndex].p0.z == this->faces[i].p1.z) ||
            (this->faces[faceIndex].p0.x == this->faces[i].p2.x &&
            this->faces[faceIndex].p0.y == this->faces[i].p2.y &&
            this->faces[faceIndex].p0.z == this->faces[i].p2.z)) ||

            ((this->faces[faceIndex].p1.x == this->faces[i].p0.x &&
            this->faces[faceIndex].p1.y == this->faces[i].p0.y &&
            this->faces[faceIndex].p1.z == this->faces[i].p0.z) ||
            (this->faces[faceIndex].p1.x == this->faces[i].p1.x &&
            this->faces[faceIndex].p1.y == this->faces[i].p1.y &&
            this->faces[faceIndex].p1.z == this->faces[i].p1.z) ||
            (this->faces[faceIndex].p1.x == this->faces[i].p2.x &&
            this->faces[faceIndex].p1.y == this->faces[i].p2.y &&
            this->faces[faceIndex].p1.z == this->faces[i].p2.z)) ||

            ((this->faces[faceIndex].p2.x == this->faces[i].p0.x &&
            this->faces[faceIndex].p2.y == this->faces[i].p0.y &&
            this->faces[faceIndex].p2.z == this->faces[i].p0.z) ||
            (this->faces[faceIndex].p2.x == this->faces[i].p1.x &&
            this->faces[faceIndex].p2.y == this->faces[i].p1.y &&
            this->faces[faceIndex].p2.z == this->faces[i].p1.z) ||
            (this->faces[faceIndex].p2.x == this->faces[i].p2.x &&
            this->faces[faceIndex].p2.y == this->faces[i].p2.y &&
            this->faces[faceIndex].p2.z == this->faces[i].p2.z)))
        {
            this->faces[faceIndex].neighbourFaces.Append(i);
        }
    }
    if (this->faces[faceIndex].neighbourFaces.Size() > 20)
    {
        int num = this->faces[faceIndex].neighbourFaces.Size();
        bool stop = true;
    }
}

//------------------------------------------------------------------------------
/**
    Face list will be updated from the skinned or static mesh
*/
void
SkinRayCheckUtil::UpdateSingleFace(int faceIndex)
{
    n_assert(this->refSkinnedMesh);
    nMesh2* mesh = this->refSkinnedMesh;

    const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
    ushort startIndex = meshGroup.GetFirstIndex();
    float* vertices = mesh->LockVertices();
    ushort* indices = mesh->LockIndices();

    int vertexWidth = mesh->GetVertexWidth();
    int numFaces = this->faces.Size();
    n_assert(meshGroup.GetNumIndices() == (numFaces * 3));
    //matrix44 m = this->activeEntity->GetTransform();

    /// each face gets 3 own vertices
    int ii = startIndex + faceIndex * 3;
    this->faces[faceIndex].p0 = *(vector3*)(vertices + indices[ii] * vertexWidth);
    this->faces[faceIndex].p1 = *(vector3*)(vertices + indices[ii + 1] * vertexWidth);
    this->faces[faceIndex].p2 = *(vector3*)(vertices + indices[ii + 2] * vertexWidth);

    //this->faces[faceIndex].p0.rotate(vector3(0,1,0), N_PI);
    //this->faces[faceIndex].p1.rotate(vector3(0,1,0), N_PI);
    //this->faces[faceIndex].p2.rotate(vector3(0,1,0), N_PI);

    ////FIXME: better transform ray and retransform result while raychecking!!!
    //
    //this->faces[faceIndex].p0 = m.transform_coord(this->faces[faceIndex].p0);
    //this->faces[faceIndex].p1 = m.transform_coord(this->faces[faceIndex].p1);
    //this->faces[faceIndex].p2 = m.transform_coord(this->faces[faceIndex].p2);

    mesh->UnlockIndices();
    mesh->UnlockVertices();
}

};
