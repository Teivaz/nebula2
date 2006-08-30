//------------------------------------------------------------------------------
//  util/skinraycheckutil.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------

#include "util/skinraycheckutil.h"
#include "graphics/resource.h"
#include "character/ncharacter2.h"
#include "managers/timemanager.h"
#include "kernel/nfileserver2.h"
#include "game/time/systemtimesource.h"

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
    useLastFaceOptimization(true),
    lastFace(-1),
    invertZ(true)
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
bool
SkinRayCheckUtil::SetGraphicsEntity(Graphics::Entity* entity, nString meshName)
{
    n_assert(entity);

    this->activeEntity = entity;
    nTransformNode* node = entity->GetResource().GetNode();
    nArray<nShapeNode*> shapeNodes;
    // find all subordinated shapenodes
    this->FindShapeNodes(node, shapeNodes);

    int shapeNodeIndex = -1;
    if (meshName.IsEmpty())
    {
        shapeNodeIndex = 0;
    }
    else
    {
        int i;
        for (i=0; i < shapeNodes.Size(); i++)
        {
            nString name = shapeNodes[i]->GetMesh();
            name = name.ExtractFileName();
            if (name.FindStringIndex(meshName,0) >= 0)
            {
                shapeNodeIndex = i;
                break;
            }
        }

        //if (shapeNodeIndex < 0) n_error("shapeNode with mesh: %s not found!", meshName.Get());
        if (shapeNodeIndex < 0) return false;
    }

    // handle a skinned mesh
    if (entity->IsA(Graphics::CharEntity::RTTI) &&
        shapeNodes.Size() > 0 &&
        shapeNodes[shapeNodeIndex]->IsA(nKernelServer::Instance()->FindClass("nskinshapenode")))
    {
        this->meshType = SkinRayCheckUtil::Skinned;

        CharEntity* charEntity = (CharEntity*) entity;
        nCharacter2* nebCharacter = charEntity->GetCharacterPointer();
        // FIXME: only the first shapenode will be used
        this->shapeNode = shapeNodes[shapeNodeIndex];
        this->skinShapeNode = (nSkinShapeNode*) this->shapeNode;

        this->charEntity = charEntity;

        charEntity->EvaluateSkeleton();
        this->charSkeleton = &nebCharacter->GetSkeleton();
        this->meshGroupIndex = this->skinShapeNode->GetGroupIndex();
        this->refOriginalMesh = this->skinShapeNode->GetMeshObject();
        this->resourceName = charEntity->GetResourceName();

        //initialize resistant mesh
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
        this->shapeNode = shapeNodes[shapeNodeIndex];
        this->meshGroupIndex = this->shapeNode->GetGroupIndex();
        this->refOriginalMesh = this->shapeNode->GetMeshObject();

        //initialize resistant mesh
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
    return true;
}

//------------------------------------------------------------------------------
/**
    Initialize the resistant mesh from the original mesh
*/
void
SkinRayCheckUtil::InitResistentMesh()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    // create the skinned mesh, this will be filled by the CPU with skinned data
    nString resistentMeshName = this->resourceName + "r";
    nMesh2* resistentMesh = gfxServer->NewMesh("");
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

        this->refResistentMeshResourceLoader = (nMeshCopyResourceLoader*) nKernelServer::Instance()->New("nmeshcopyresourceloader", "resmeshresloader");
        this->refResistentMeshResourceLoader->SetSourceMesh(this->refOriginalMesh.get());
        resistentMesh->SetResourceLoader(this->refResistentMeshResourceLoader->GetFullName());
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

        if (!this->invertZ)
        {
            this->faces[faceIndex].p0.rotate(vector3(0,1,0), N_PI);
            this->faces[faceIndex].p1.rotate(vector3(0,1,0), N_PI);
            this->faces[faceIndex].p2.rotate(vector3(0,1,0), N_PI);
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
    this->faces.SetSize(meshGroup.GetNumIndices() / 3);
    this->faces.Clear(emptyFace);

    this->UpdateFaces();
    this->DoMeshCleanUp();
    this->FillNeighbourFaceMap();
    this->InitVertexColorMap();
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
    //nMesh2* skinnedMesh = gfxServer->NewMesh(skinnedMeshName.Get());
    nMesh2* skinnedMesh = gfxServer->NewMesh("");
    this->refSkinnedMesh = skinnedMesh;
    nMesh2* bindPoseMesh = this->refOriginalMesh;
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

        this->refSkinnedMeshResourceLoader = (nMeshCopyResourceLoader*) nKernelServer::Instance()->New("nmeshcopyresourceloader", "skinmeshresloader");
        this->refSkinnedMeshResourceLoader->SetSourceMesh(this->refOriginalMesh.get());
        skinnedMesh->SetResourceLoader(this->refSkinnedMeshResourceLoader->GetFullName());
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
    if (this->invertZ) trans.rotate(vector3(0,1,0), N_PI);
    trans.translate(transPos);
    matrix44 invTrans = trans;
    invTrans.invert();
    line3 tRay;
    tRay.set(invTrans.transform_coord(ray.start()),invTrans.transform_coord(ray.end()));

    /// Checks if ray intersects boundingbox
    /// FIXME: not really exact...
    bbox3 bbox = this->activeEntity->GetBox();
    if (ray.distance(bbox.center()) > (bbox.diagonal_size()*0.5f)) return false;

    uint currentFrameId = Game::SystemTimeSource::Instance()->GetFrameId();
    bool updateFaces = false;

    if (this->refResistentMeshResourceLoader->HasBeenReloaded() ||
        this->refSkinnedMeshResourceLoader->HasBeenReloaded())
    {

        //this->refResistentMesh->Unload();
        //this->refSkinnedMesh->Unload();
        //initialize rsistent mesh
        //this->InitResistentMesh();
        // initialize skinned mesh
        //this->InitSkinnedMesh();
        // update skinned mesh
        this->UpdateSkinning();
        // create the face normals
        this->UpdateFaces();

        this->refResistentMeshResourceLoader->ResetReloadFlag();
        this->refSkinnedMeshResourceLoader->ResetReloadFlag();
    }

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

        nArray<int> faceList;
        this->FindNeighbourFaces(this->lastFace, faceList, 1);
        int i;
        for (i=0; i < faceList.Size(); i++)
        {
            if (updateFaces) this->UpdateSingleFace(faceList[i]);
            if (this->Intersects(this->faces[faceList[i]], tRay, newFaceIntersection))
            {
                // collect information about face intersection and append to array
                newFaceIntersection.faceIndex = faceList[i];
                newFaceIntersection.intersectionPoint = trans.transform_coord(newFaceIntersection.intersectionPoint);
                newFaceIntersection.distance = ray.start().distance(ray.start(), newFaceIntersection.intersectionPoint);
                newFaceIntersection.normal = trans.transform_coord(newFaceIntersection.normal);
                newFaceIntersection.normal = newFaceIntersection.normal - trans.pos_component();
                newFaceIntersection.normal.norm();
                intersectedFaces.Append(newFaceIntersection);
                intersected = true;
                break;
            }
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
                newFaceIntersection.intersectionPoint = trans.transform_coord(newFaceIntersection.intersectionPoint);
                newFaceIntersection.distance = ray.start().distance(ray.start(), newFaceIntersection.intersectionPoint);
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
    // make sure to hit only the front of a face
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

    // to compensate a rounding error at the face edges we move the
    // intersection point a little bit to the center of the face.
    vector3 center = (pa + pb + pc)/3;
    ip = ip + (center - ip) * 0.01f;

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
SkinRayCheckUtil::AddVertexColorOfFace(int faceIndex, const vector4& col, const vector3& intersectionPoint, float radius)
{
    nArray<int> vertexList;
    vector3 p, q;
    matrix44 trans = this->activeEntity->GetTransform();
    vector3 transPos = trans.pos_component();
    trans.translate(-transPos);
    if (this->invertZ) trans.rotate(vector3(0,1,0), N_PI);
    trans.translate(transPos);
    matrix44 invTrans = trans;
    invTrans.invert();
    q = invTrans.transform_coord(intersectionPoint);
    p = (this->faces[faceIndex].p0 + this->faces[faceIndex].p1 + this->faces[faceIndex].p2) * 0.33333333f;
    this->FindVerticesInRange(faceIndex, q, radius, vertexList);
    return this->PaintVertexList(vertexList, col, true);
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

    /// each face gets 3 own vertices
    int ii = startIndex + faceIndex * 3;
    this->faces[faceIndex].p0 = *(vector3*)(vertices + indices[ii] * vertexWidth);
    this->faces[faceIndex].p1 = *(vector3*)(vertices + indices[ii + 1] * vertexWidth);
    this->faces[faceIndex].p2 = *(vector3*)(vertices + indices[ii + 2] * vertexWidth);

    mesh->UnlockIndices();
    mesh->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
*/
bool
SkinRayCheckUtil::PaintVertexList(const nArray<int> &vertexList, const vector4 &col, bool addCol)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        ushort startIndex = meshGroup.GetFirstIndex();
        float* vertices = mesh->LockVertices();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        int i;
        for (i=0; i < vertexList.Size(); i++)
        {
            vector4 tempCol;
            if (addCol)
            {
                tempCol = *(vector4*)(vertices + vertexList[i] * vertexWidth + offset);
                tempCol +=col;
            }
            else
            {
                tempCol = col;
            }
            tempCol.x = n_clamp(tempCol.x, 0, 1);
            tempCol.y = n_clamp(tempCol.y, 0, 1);
            tempCol.z = n_clamp(tempCol.z, 0, 1);
            tempCol.w = n_clamp(tempCol.w, 0, 1);
            *(vector4*)(vertices + vertexList[i] * vertexWidth + offset) = tempCol;
        }

        mesh->UnlockVertices();
        this->UpdateOriginalMesh();
        return true;
    }
    else return false;
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::FindVerticesInRange(int faceIndex, const vector3& intersectionPoint, float radius, nArray<int>& vertexList)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
    ushort startIndex = meshGroup.GetFirstIndex();
    ushort* indices = mesh->LockIndices();

    int ii = startIndex + faceIndex * 3;

    int vertex0, vertex1, vertex2;

    vertex0 = indices[ii];
    vertex1 = indices[ii+1];
    vertex2 = indices[ii+2];

    if (intersectionPoint.distance(this->faces[faceIndex].p0, intersectionPoint) < radius)
    {
        if (!vertexList.Find(vertex0))
        {
            vertexList.Append(vertex0);
            int i;
            for (i=0; i < this->neighbourFacesMap[this->faces[faceIndex].v0].Size(); i++)
            {
                this->FindVerticesInRange(this->neighbourFacesMap[this->faces[faceIndex].v0][i], intersectionPoint, radius, vertexList);
            }
        }
    }

    if (intersectionPoint.distance(this->faces[faceIndex].p1, intersectionPoint) < radius)
    {
        if (!vertexList.Find(vertex1))
        {
            vertexList.Append(vertex1);
            int i;
            for (i=0; i < this->neighbourFacesMap[this->faces[faceIndex].v1].Size(); i++)
            {
                this->FindVerticesInRange(this->neighbourFacesMap[this->faces[faceIndex].v1][i], intersectionPoint, radius, vertexList);
            }
        }
    }

    if (intersectionPoint.distance(this->faces[faceIndex].p2, intersectionPoint) < radius)
    {
        if (!vertexList.Find(vertex2))
        {
            vertexList.Append(vertex2);
            int i;
            for (i=0; i < this->neighbourFacesMap[this->faces[faceIndex].v2].Size(); i++)
            {
                this->FindVerticesInRange(this->neighbourFacesMap[this->faces[faceIndex].v2][i], intersectionPoint, radius, vertexList);
            }
        }
    }

    mesh->UnlockIndices();
}

//------------------------------------------------------------------------------
/**
    FIXME: Not tested if it works right!
*/
void
SkinRayCheckUtil::FindNeighbourVertices(int vertexIndex, nArray<int>& vertexList, int depth)
{
    if (depth >= 0)
    {
        if (!vertexList.Find(vertexIndex))
        {
            vertexList.Append(vertexIndex);
            int i;
            for (i=0; i < this->neighbourFacesMap[vertexIndex].Size(); i++)
            {
                int v0, v1, v2;
                this->GetVerticesOfFace(this->neighbourFacesMap[vertexIndex][i], v0, v1, v2);
                this->FindNeighbourVertices(v0, vertexList, depth-1);
                this->FindNeighbourVertices(v1, vertexList, depth-1);
                this->FindNeighbourVertices(v2, vertexList, depth-1);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::FindNeighbourFaces(int faceIndex, nArray<int>& faceList, int depth)
{
    if (depth >= 0)
    {
        n_assert(this->refResistentMesh.isvalid());
        nMesh2* mesh = this->refResistentMesh;

        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        ushort startIndex = meshGroup.GetFirstIndex();
        ushort* indices = mesh->LockIndices();

        int ii = startIndex + faceIndex * 3;

        int vertex0, vertex1, vertex2;

        vertex0 = indices[ii];
        vertex1 = indices[ii+1];
        vertex2 = indices[ii+2];

        if (!faceList.Find(faceIndex))
        {
            faceList.Append(faceIndex);
            int i;
            for (i=0; i < this->neighbourFacesMap[this->faces[faceIndex].v0].Size(); i++)
            {
                this->FindNeighbourFaces(this->neighbourFacesMap[this->faces[faceIndex].v0][i], faceList, depth-1);
            }

            for (i=0; i < this->neighbourFacesMap[this->faces[faceIndex].v1].Size(); i++)
            {
                this->FindNeighbourFaces(this->neighbourFacesMap[this->faces[faceIndex].v1][i], faceList, depth-1);
            }

            for (i=0; i < this->neighbourFacesMap[this->faces[faceIndex].v2].Size(); i++)
            {
                this->FindNeighbourFaces(this->neighbourFacesMap[this->faces[faceIndex].v2][i], faceList, depth-1);
            }
        }
        mesh->UnlockIndices();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::GetVerticesOfFace(int faceIndex, int &v0, int &v1, int &v2)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
    ushort startIndex = meshGroup.GetFirstIndex();
    ushort* indices = mesh->LockIndices();

    int ii = startIndex + faceIndex * 3;

    v0 = indices[ii];
    v1 = indices[ii+1];
    v2 = indices[ii+2];
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::DoMeshCleanUp()
{
    n_assert(this->shapeNode);

    this->backupMesh.Clear();
    this->cleanMesh.Clear();

    nFileServer2* fileServer = nFileServer2::Instance();
    nString meshName(this->shapeNode->GetMeshObject()->GetFilename());
    this->backupMesh.Load(fileServer, meshName.Get());
    this->cleanMesh = this->backupMesh;
    this->cleanMesh.ForceVertexComponents(nMeshBuilder::Vertex::COORD +
                                          nMeshBuilder::Vertex::COLOR +
                                          nMeshBuilder::Vertex::WEIGHTS +
                                          nMeshBuilder::Vertex::JINDICES);
    this->cleanMesh.Cleanup(&this->collapseMap);
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::FillNeighbourFaceMap()
{
    nArray<nMeshBuilder::Group> groupMap;
    this->cleanMesh.BuildGroupMap(groupMap);
    int numTriangles = groupMap[this->meshGroupIndex].GetNumTriangles();
    int numVertices = this->cleanMesh.GetNumVertices();

    n_assert(numTriangles == this->faces.Size());

    // WORK ---


    n_assert(this->refResistentMesh.isvalid());
    this->neighbourFacesMapOverOriginalVertex.SetFixedSize(this->refResistentMesh->Group(this->meshGroupIndex).GetNumVertices());


    // WORK ---

    nMeshBuilder::Group* group = &groupMap[this->meshGroupIndex];
    this->neighbourFacesMap.SetFixedSize(numVertices);

    int faceIndex;
    int firstTriangle = group->GetFirstTriangle();
    for (faceIndex=0; faceIndex < numTriangles; faceIndex++)
    {
        this->cleanMesh.GetTriangleAt(firstTriangle + faceIndex).GetVertexIndices(this->faces[faceIndex].v0,
                                                                                   this->faces[faceIndex].v1,
                                                                                   this->faces[faceIndex].v2);
        this->neighbourFacesMap[this->faces[faceIndex].v0].Append(faceIndex);
        this->neighbourFacesMap[this->faces[faceIndex].v1].Append(faceIndex);
        this->neighbourFacesMap[this->faces[faceIndex].v2].Append(faceIndex);

        int i;
        for (i=0; i < this->collapseMap[this->faces[faceIndex].v0].Size(); i++)
        {
            this->neighbourFacesMapOverOriginalVertex[this->collapseMap[this->faces[faceIndex].v0][i]].Append(faceIndex);
        }
        for (i=0; i < this->collapseMap[this->faces[faceIndex].v1].Size(); i++)
        {
            this->neighbourFacesMapOverOriginalVertex[this->collapseMap[this->faces[faceIndex].v1][i]].Append(faceIndex);
        }
        for (i=0; i < this->collapseMap[this->faces[faceIndex].v2].Size(); i++)
        {
            this->neighbourFacesMapOverOriginalVertex[this->collapseMap[this->faces[faceIndex].v2][i]].Append(faceIndex);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::InitVertexColorMap()
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        int numVertices = meshGroup.GetNumVertices();
        int startVertex = meshGroup.GetFirstVertex();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        this->vertexColorMap.SetFixedSize(numVertices);
        float* vertices = mesh->LockVertices();

        int i;
        for (i=0; i < numVertices; i++)
        {
            this->vertexColorMap[i] = *(vector4*)(vertices + (startVertex + i) * vertexWidth + offset);
        }

        mesh->UnlockVertices();

        this->startVertex = startVertex;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::InitVertexPosMap()
{
    n_assert(this->refSkinnedMesh.isvalid());
    nMesh2* mesh = this->refSkinnedMesh;

    const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
    int numVertices = meshGroup.GetNumVertices();
    int startVertex = meshGroup.GetFirstVertex();
    int vertexWidth = mesh->GetVertexWidth();
    int offset = 0;

    this->vertexPosMap.SetFixedSize(numVertices);
    float* vertices = mesh->LockVertices();

    int i;
    for (i=0; i < numVertices; i++)
    {
        this->vertexPosMap[i] = *(vector3*)(vertices + (startVertex + i) * vertexWidth + offset);
    }

    mesh->UnlockVertices();

    this->startVertex = startVertex;
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::InitVertexNormalMap()
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Normal))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        int numVertices = meshGroup.GetNumVertices();
        int startVertex = meshGroup.GetFirstVertex();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Normal);

        this->vertexNormalMap.SetFixedSize(numVertices);
        float* vertices = mesh->LockVertices();

        int i;
        for (i=0; i < numVertices; i++)
        {
            this->vertexNormalMap[i] = *(vector3*)(vertices + (startVertex + i) * vertexWidth + offset);
        }

        mesh->UnlockVertices();

        this->startVertex = startVertex;
    }
}





//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::UpdateVertexColorMapByList(const nArray<int>& vertexList)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        int numVertices = meshGroup.GetNumVertices();
        int startVertex = meshGroup.GetFirstVertex();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        float* vertices = mesh->LockVertices();

        int i;
        for (i=0; i < vertexList.Size(); i++)
        {
            this->vertexColorMap[vertexList[i]] = *(vector4*)(vertices + (startVertex + vertexList[i]) * vertexWidth + offset);
        }

        mesh->UnlockVertices();

        this->startVertex = startVertex;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::UpdateVertexPosMapByList(const nArray<int>& vertexList)
{
    n_assert(this->refSkinnedMesh.isvalid());
    nMesh2* mesh = this->refSkinnedMesh;

    const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
    int numVertices = meshGroup.GetNumVertices();
    int startVertex = meshGroup.GetFirstVertex();
    int vertexWidth = mesh->GetVertexWidth();
    int offset = 0;

    float* vertices = mesh->LockVertices();

    int i;
    for (i=0; i < vertexList.Size(); i++)
    {
        this->vertexPosMap[vertexList[i]] = *(vector3*)(vertices + (startVertex + vertexList[i]) * vertexWidth + offset);
    }

    mesh->UnlockVertices();

    this->startVertex = startVertex;
}

//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::UpdateVertexNormalMapByList(const nArray<int>& vertexList)
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Normal))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        int numVertices = meshGroup.GetNumVertices();
        int startVertex = meshGroup.GetFirstVertex();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Normal);

        float* vertices = mesh->LockVertices();

        int i;
        for (i=0; i < vertexList.Size(); i++)
        {
            this->vertexNormalMap[vertexList[i]] = *(vector3*)(vertices + (startVertex + vertexList[i]) * vertexWidth + offset);
        }

        mesh->UnlockVertices();

        this->startVertex = startVertex;
    }
}













//------------------------------------------------------------------------------
/**
*/
void
SkinRayCheckUtil::UpdateFromVertexColorMap()
{
    n_assert(this->refResistentMesh.isvalid());
    nMesh2* mesh = this->refResistentMesh;

    if(mesh->HasAllVertexComponents(nMesh2::Color))
    {
        const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
        int numVertices = meshGroup.GetNumVertices();
        int startVertex = meshGroup.GetFirstVertex();
        int vertexWidth = mesh->GetVertexWidth();
        int offset = mesh->GetVertexComponentOffset(nMesh2::Color);

        float* vertices = mesh->LockVertices();

        int i;
        for (i=0; i < this->vertexColorMap.Size(); i++)
        {
            *(vector4*)(vertices + (startVertex + i) * vertexWidth + offset) = this->vertexColorMap[i];
        }
        mesh->UnlockVertices();
        this->UpdateOriginalMesh();
    }
}

//------------------------------------------------------------------------------
/**
    Heronische Flächenformel
*/
float
SkinRayCheckUtil::GetSizeOfFace(int faceIndex)
{
    vector3 a = this->faces[faceIndex].p0;
    vector3 b = this->faces[faceIndex].p1;
    vector3 c = this->faces[faceIndex].p2;

    float ab = a.distance(a, b);
    float bc = b.distance(b, c);
    float ca = c.distance(c, a);

    float s = (ab + bc + ca) / 2;
    return sqrt(s * (s-ab) * (s-bc) * (s-ca));
}

//------------------------------------------------------------------------------
/**

*/
float
SkinRayCheckUtil::GetSizeOfVertexFace(int vertexIndex)
{
    vertexIndex += this->startVertex;
    float size = 0;
    int i;
    for (i = 0; i < this->neighbourFacesMapOverOriginalVertex[vertexIndex].Size(); i++)
    {
        size += this->GetSizeOfFace(this->neighbourFacesMapOverOriginalVertex[vertexIndex][i]);
    }
    return size;
}

//------------------------------------------------------------------------------
/**

*/
int
SkinRayCheckUtil::GetVertexFromFace(int faceIndex, int vertexNr)
{
    if (vertexNr == 0) return this->collapseMap[this->faces[faceIndex].v0][0];
    else if (vertexNr == 1) return this->collapseMap[this->faces[faceIndex].v1][0];
    else if (vertexNr == 2) return this->collapseMap[this->faces[faceIndex].v2][0];
    n_error("Only vertexNr 0 - 2!");
    return -1;
}

};
