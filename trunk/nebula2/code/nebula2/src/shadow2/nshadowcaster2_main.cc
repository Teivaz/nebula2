//------------------------------------------------------------------------------
//  nshadowcaster2_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "shadow2/nshadowcaster2.h"

nNebulaClass(nShadowCaster2, "nresource");

//------------------------------------------------------------------------------
/**
*/
nShadowCaster2::nShadowCaster2() :
    indexBufferStart(0),
    indexBufferCurrent(0),
    drawNumIndices(0),
    srcMesh(0),
    srcIndices(0),
    srcEdges(0),
    prevLightType(nLight::InvalidType)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowCaster2::~nShadowCaster2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method allocates internal buffers which are required for all
    types of shadow casters. It's usually called from LoadResource()
    of a subclass.
*/
bool
nShadowCaster2::AllocateBuffers(nMesh2* mesh)
{
    n_assert(mesh);
    n_assert(!this->refIndexBuffer.isvalid());
    n_assert(this->faces.Size() == 0);

    // allocate the faces array
    const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
    Face emptyFace;
    emptyFace.lit = false;
    this->faces.SetSize(meshGroup.GetNumIndices() / 3);
    this->faces.Clear(emptyFace);

    // create the face normals
    this->UpdateFaceNormalsAndMidpoints(mesh);

    // allocate the shared index buffer for the dark cap, light cap and shadow volume indices
    // we assume that in the worst case there will never be more then 4 times the original
    // indices required for rendering the shadow volume
    nString ibName = this->GetFilename() + "_ib";
    nMesh2* newMesh = nGfxServer2::Instance()->NewMesh(ibName.Get());
    this->refIndexBuffer = newMesh;
    if (!newMesh->IsLoaded())
    {
        newMesh->SetUsage(nMesh2::WriteOnly | nMesh2::NeedsVertexShader);
        newMesh->SetVertexComponents(0);
        newMesh->SetNumVertices(0);
        newMesh->SetNumIndices(mesh->GetNumIndices() * 4);
        newMesh->SetNumEdges(0);
        newMesh->Load();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Release internal buffers. This method is usually called from
    UnloadResource() of a derived class.
*/
void
nShadowCaster2::ReleaseBuffers()
{
    n_assert(this->refIndexBuffer.isvalid());
    n_assert(this->faces.Size() > 0);

    // free the faces array
    this->faces.SetSize(0);

    // release the index buffer mesh object
    if (this->refIndexBuffer.isvalid())
    {
        this->refIndexBuffer->Release();
        this->refIndexBuffer.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    This updates the stored face normals and midpoints from a mesh.

    FIXME: eliminate temporary vector3 objects in loop.
*/
void
nShadowCaster2::UpdateFaceNormalsAndMidpoints(nMesh2* mesh)
{
    n_assert(mesh);

    const nMeshGroup& meshGroup = mesh->Group(this->meshGroupIndex);
    ushort startIndex = meshGroup.GetFirstIndex();
    float* vertices = mesh->LockVertices();
    ushort* indices = mesh->LockIndices();
    int vertexWidth = mesh->GetVertexWidth();
    int numFaces = this->faces.Size();
    n_assert(meshGroup.GetNumIndices() == (numFaces * 3));
    int faceIndex;
    for (faceIndex = 0; faceIndex < numFaces; faceIndex++)
    {
        int ii = startIndex + faceIndex * 3;
        const vector3& v0 = *(vector3*)(vertices + indices[ii] * vertexWidth);
        const vector3& v1 = *(vector3*)(vertices + indices[ii + 1] * vertexWidth);
        const vector3& v2 = *(vector3*)(vertices + indices[ii + 2] * vertexWidth);
        this->faces[faceIndex].normal = (v1 - v0) * (v2 - v0);
        this->faces[faceIndex].normal.norm();
        this->faces[faceIndex].point  = v0;
    }
    mesh->UnlockIndices();
    mesh->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
    This method checks if the lighting situation has been changed since the
    last time the method was called. If yes, the method will return true,
    and the render-buffer must be updated. If no, everything can remain
    as is, and rendering can happen without computations.
*/
bool
nShadowCaster2::LightingChanged(const nLight& light, const matrix44& invModelLight)
{
    nLight::Type curLightType = light.GetType();
    vector3 curLightPosOrDir;

    if (nLight::Directional == curLightType)
    {
        curLightPosOrDir = invModelLight.z_component();
    }
    else
    {
        curLightPosOrDir = invModelLight.pos_component();
    }

    if ((curLightType != this->prevLightType) || (!curLightPosOrDir.isequal(this->prevLightPosOrDir, 0.001f)))
    {
        this->prevLightType = curLightType;
        this->prevLightPosOrDir = curLightPosOrDir;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    This method computes the lit/unlit status of faces and updates
    the "lit" flag of all faces in the face array.
*/
void
nShadowCaster2::UpdateFaceLitFlags(const nLight& light, const matrix44& invModelLight)
{
    nLight::Type lightType = light.GetType();
    if (nLight::Directional == lightType)
    {
        // handle directional lights
        const vector3& modelLightDir = invModelLight.z_component();
        int faceIndex;
        int numFaces = this->faces.Size();
        for (faceIndex = 0; faceIndex < numFaces; faceIndex++)
        {
            Face& curFace = this->faces[faceIndex];
            if (curFace.normal.dot(modelLightDir) > 0.0f)
            {
                curFace.lit = true;
            }
            else
            {
                curFace.lit = false;
            }
        }
    }
    else if (nLight::Point == lightType)
    {
        // handle point lights
        const vector3& modelLightPos = invModelLight.pos_component();
        int faceIndex;
        int numFaces = this->faces.Size();
        for (faceIndex = 0; faceIndex < numFaces; faceIndex++)
        {
            Face& curFace = this->faces[faceIndex];
            if (curFace.normal.dot(curFace.point - modelLightPos) > 0.0f)
            {
                curFace.lit = true;
            }
            else
            {
                curFace.lit = false;
            }
        }
    }
    else
    {
        n_error("nShadowCaster2::UpdateFaceLitFlags(): unsupported light type!");
    }
}

//------------------------------------------------------------------------------
/**
    This method starts writing the indices for the dark cap, light cap
    and/or shadow volume sides into the shadow volume index buffer.
*/
void
nShadowCaster2::BeginWriteIndices(nMesh2* mesh)
{
    n_assert(0 == this->srcMesh);
    n_assert(0 == this->srcIndices);
    n_assert(0 == this->srcEdges);
    n_assert(0 == this->indexBufferStart);
    n_assert(0 == this->indexBufferCurrent);
    n_assert(mesh);

    this->indexBufferStart = this->refIndexBuffer->LockIndices();
    this->indexBufferCurrent = this->indexBufferStart;
    this->drawNumIndices = 0;
    this->srcMesh = mesh;
    this->srcIndices = mesh->LockIndices();
    this->srcEdges   = mesh->LockEdges();
}

//------------------------------------------------------------------------------
/**
    This writes the indices to render the dark or light cap into the shadow
    index buffer. If the 'lit' argument is true, the light cap will be
    rendered, otherwise the dark cap.
*/
void
nShadowCaster2::WriteCapIndices(bool lit)
{
    n_assert(this->indexBufferCurrent);
    n_assert(this->srcIndices);
    n_assert(this->srcMesh);

    ushort startIndex = this->srcMesh->Group(this->meshGroupIndex).GetFirstIndex();

    // for each face...
    if (lit)
    {
        // write light cap indices, these get extruded away from the light source
        int faceIndex;
        int numFaces = this->faces.Size();
        for (faceIndex = 0; faceIndex < numFaces; faceIndex++)
        {
            const Face& curFace = this->faces[faceIndex];
            if (!curFace.lit)
            {
                int ii = startIndex + faceIndex * 3;
                *this->indexBufferCurrent++ = this->srcIndices[ii] + 1;
                *this->indexBufferCurrent++ = this->srcIndices[ii + 2] + 1;
                *this->indexBufferCurrent++ = this->srcIndices[ii + 1] + 1;
            }
        }
    }
    else
    {
        // write dark cap indices, these remain at their original position
        int faceIndex;
        int numFaces = this->faces.Size();
        for (faceIndex = 0; faceIndex < numFaces; faceIndex++)
        {
            const Face& curFace = this->faces[faceIndex];
            if (curFace.lit)
            {
                int ii = startIndex + faceIndex * 3;
                *this->indexBufferCurrent++ = this->srcIndices[ii];
                *this->indexBufferCurrent++ = this->srcIndices[ii + 2];
                *this->indexBufferCurrent++ = this->srcIndices[ii + 1];
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    This writes a quad made of 2 triangles into the index buffer. This
    is used for rendering the sides of the shadow volume.
*/
void
nShadowCaster2::WriteQuad(ushort vIndex0, ushort vIndex1)
{
    ushort extrudedIndex0 = vIndex0 + 1;
    ushort extrudedIndex1 = vIndex1 + 1;

    // write first triangle of quad
    *this->indexBufferCurrent++ = vIndex0;
    *this->indexBufferCurrent++ = vIndex1;
    *this->indexBufferCurrent++ = extrudedIndex0;

    // write second triangle of quad
    *this->indexBufferCurrent++ = vIndex1;
    *this->indexBufferCurrent++ = extrudedIndex1;
    *this->indexBufferCurrent++ = extrudedIndex0;
}

//------------------------------------------------------------------------------
/**
    This writes the side faces of the shadow volume into the shadow index
    buffer. The method iterates through the edge array, and for each silhouette
    edge 2 triangles forming a quad will be written.
*/
void
nShadowCaster2::WriteSideIndices()
{
    n_assert(this->indexBufferCurrent);
    n_assert(this->srcMesh);
    n_assert(this->srcEdges);

    // for each edge...
    const nMeshGroup& meshGroup = this->srcMesh->Group(this->meshGroupIndex);
    ushort startEdge = meshGroup.GetFirstEdge();
    ushort numEdges  = meshGroup.GetNumEdges();
    ushort startFace = meshGroup.GetFirstIndex() / 3;
    ushort edgeIndex;
    for (edgeIndex = startEdge; edgeIndex < (startEdge + numEdges); edgeIndex++)
    {
        const nMesh2::Edge& edge = this->srcEdges[edgeIndex];

        if (edge.fIndex[0] != nMesh2::InvalidIndex)
        {
            const Face& face0 = this->faces[edge.fIndex[0] - startFace];
            if (edge.fIndex[1] != nMesh2::InvalidIndex)
            {
                // this is a normal edge
                const Face& face1 = this->faces[edge.fIndex[1] - startFace];
                if (face0.lit != face1.lit)
                {
                    // a normal silhouette edge
                    if (face0.lit) this->WriteQuad(edge.vIndex[0], edge.vIndex[1]);
                    else           this->WriteQuad(edge.vIndex[1], edge.vIndex[0]);
                }
            }
            else
            {
                // a border edge, first case
                if (face0.lit) this->WriteQuad(edge.vIndex[0], edge.vIndex[1]);
                else           this->WriteQuad(edge.vIndex[1], edge.vIndex[0]);
            }
        }
        else
        {
            // a border edge, second case
            const Face& face1 = this->faces[edge.fIndex[1] - startFace];
            if (face1.lit) this->WriteQuad(edge.vIndex[0], edge.vIndex[1]);
            else           this->WriteQuad(edge.vIndex[1], edge.vIndex[0]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Finish writing indices to the shadow volume index buffer. This updated
    the drawNumIndices member which indicates how many indices from the
    index buffer should be drawn.
*/
void
nShadowCaster2::EndWriteIndices()
{
    n_assert(0 != this->srcMesh);
    n_assert(0 != this->srcIndices);
    n_assert(0 != this->srcEdges);
    n_assert(0 == this->drawNumIndices);
    n_assert(0 != this->indexBufferStart);
    n_assert(0 != this->indexBufferCurrent);

    this->srcMesh->UnlockEdges();
    this->srcMesh->UnlockIndices();
    this->srcMesh = 0;
    this->srcEdges = 0;
    this->srcIndices = 0;

    this->refIndexBuffer->UnlockIndices();
    this->drawNumIndices = this->indexBufferCurrent - this->indexBufferStart;
    this->indexBufferStart = 0;
    this->indexBufferCurrent = 0;
}

//------------------------------------------------------------------------------
/**
    Setup the shadow volume for this caster. This method is called by
    nShadowServer2 right before rendering the shadow volume. Implement this
    method in a derived class.
*/
void
nShadowCaster2::SetupShadowVolume(const nLight& /*light*/, const matrix44& /*invModelMatrix*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Actually render the shadow volume. This method is called by nShadowServer2
    after setting up the shadow volume. The method may be called 1 or 2 times
    depending on whether 1-pass or 2-pass shadow volume rendering is enabled.
    Implement this method in a subclass.
*/
void
nShadowCaster2::RenderShadowVolume()
{
    // empty
}
