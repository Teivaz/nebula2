//------------------------------------------------------------------------------
//  nshadowcaster_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "shadow/nshadowcaster.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ndynamicshadermesh.h"
#include "gfx2/nmeshgroup.h"

nNebulaClass(nShadowCaster, "nresource");
const float nShadowCaster::shadowOffset = 0.025f;

//------------------------------------------------------------------------------
/**
*/
nShadowCaster::nShadowCaster() :
    lightPosition(0.0f, 0.0f, 0.0f),
    faces(0),
    edges(0),
    meshUsage(nMesh2::ReadOnly),
    silhouetteIndices(0, 64),
    lastGroupIndex(-1),
    dirty(true)
{
    this->silhouetteIndices.SetFlags(nArray<ushort>::DoubleGrowSize);
}

//------------------------------------------------------------------------------
/**
*/
nShadowCaster::~nShadowCaster()
{
    if (this->IsValid())
    {
        this->UnloadResource();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowCaster::ApplyShadow()
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowCaster::UnloadResource()
{
    this->SetValid(false);
    
    if (0 != this->faces)
    {
        n_delete_array(this->faces);
        this->faces = 0;
        this->numFaces = 0;
    }

    if (0 != this->edges)
    {
        n_delete_array(this->edges);
        this->edges = 0;
        this->numEdges = 0;
    }
    
    this->silhouetteIndices.Reset();
    this->numSilhouetteIndices = 0;
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowCaster::LoadResource()
{
    bool success = false;
    n_assert(!this->IsValid());
    n_assert(!this->GetFilename().IsEmpty());

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    
    // resource name
    nString meshName = this->GetFilename();
    meshName += "_s";

    // prepare geometry data load from file
    nMesh2* sourceMesh = gfxServer->NewMesh(meshName.Get());
    if (!sourceMesh->IsValid())
    {
        sourceMesh->SetUsage(this->GetMeshUsage());
        sourceMesh->SetFilename(this->GetFilename());
        sourceMesh->SetAsyncEnabled(false); //needs to be loaded directly
    }

    // load data    
    if (sourceMesh->Load())
    {
        this->LoadShadowData(sourceMesh);
        success = true;
    }
    
    sourceMesh->Release();
    this->SetValid(success);
    return success;
}


//------------------------------------------------------------------------------
/**
*/
void
nShadowCaster::LoadShadowData(nMesh2* sourceMesh)
{
    n_assert(sourceMesh);
    this->LoadEdges(sourceMesh);
    this->LoadFaces(sourceMesh);
    this->LoadGroups(sourceMesh);
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowCaster::SetMeshUsage(int usage)
{
    this->meshUsage = usage;
}

//------------------------------------------------------------------------------
/**
*/
int
nShadowCaster::GetMeshUsage() const
{
    return this->meshUsage;
}

//------------------------------------------------------------------------------
/**
    Read group information from provided mesh.
*/
void
nShadowCaster::LoadGroups(nMesh2* sourceMesh)
{
    n_assert(sourceMesh);
    this->meshGroups.Clear();
    int numGroups = sourceMesh->GetNumGroups();
    int i;
    for (i = 0; i < numGroups; i++)
    {
        this->meshGroups.PushBack(sourceMesh->GetGroup(i));
    }
}

//------------------------------------------------------------------------------
/**
    Read faces from provided mesh.
*/
void
nShadowCaster::LoadFaces(nMesh2* sourceMesh)
{
    n_assert(sourceMesh);
    n_assert(sourceMesh->IsValid());
    n_assert(0 == this->faces);
    
    n_assert2(sourceMesh->GetNumIndices() > 0, "The mesh used for shadow casting must have triangle data!\n" );
    n_assert2(sourceMesh->GetNumIndices() % 3 == 0, "The indices must be a triangle list!\n" );

    // begin of source data
    ushort* sourcePtr = sourceMesh->LockIndices();
    n_assert(sourcePtr);

    // allocate memory
    this->numFaces = sourceMesh->GetNumIndices() / 3;
    n_assert(this->numFaces > 0);
    this->faces = n_new_array(Face, this->numFaces);
    n_assert(0 != this->faces);

    int f;
    // read index data
    for (f = 0; f < this->numFaces; f++)
    {
        Face& face = this->faces[f];
        face.index[0] = *(sourcePtr++);
        face.index[1] = *(sourcePtr++);
        face.index[2] = *(sourcePtr++);

        // init face members
        face.lastUpdate = -1;
        face.lightFacing = 0;
        face.lightCapCycle = -1;
    }
    sourceMesh->UnlockIndices();
}

//------------------------------------------------------------------------------
/**
    Load the edges of the current group from provided mesh.
*/
void
nShadowCaster::LoadEdges(nMesh2* sourceMesh)
{
    n_assert(sourceMesh);
    n_assert(sourceMesh->IsValid());
    n_assert(0 == this->edges);

    n_assert2(sourceMesh->GetNumEdges() > 0, "The mesh used for shadow casting must have edges!\n" );
    
    // source data begin
    nMesh2::Edge* sourceEdgePtr = sourceMesh->LockEdges();
    n_assert(sourceEdgePtr);

    // allocate memory
    this->numEdges = sourceMesh->GetNumEdges();
    n_assert(this->numEdges > 0);

    this->edges = n_new_array(nMesh2::Edge, this->numEdges);
    n_assert(0 != this->edges);

    // data destiantion
    nMesh2::Edge* destEdgePtr = this->edges;
    n_assert(destEdgePtr);

    // copy
    memcpy(destEdgePtr, sourceEdgePtr, sizeof(nMesh2::Edge) * this->numEdges);
    sourceMesh->UnlockEdges();
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowCaster::RenderShadow(const matrix44& modelMatrix, int groupIndex)
{
	nShadowServer* shdServer = nShadowServer::Instance();

	// model space light
    const nLight::Type lightType = shdServer->GetLightType();
    const vector3& worldSpaceLight = shdServer->GetLight();
    
    vector3 modelSpaceLight;
    if (nLight::Directional == lightType)
    {
        // ignore pos compnent of model matrix, becuase dirctional light is only a direction vector
        matrix33 invModelMatrix33(modelMatrix.x_component(), modelMatrix.y_component(), modelMatrix.z_component());
        invModelMatrix33.transpose();
        invModelMatrix33.mult(worldSpaceLight, modelSpaceLight);
    }
    else
    {
        n_error("FIXME: Point light not supported yet.\n");
    }
    
    // find shadow silhouette
    this->ComputeSilhouetteEdges(lightType, modelSpaceLight, groupIndex);

    const vector4 red(1.0, 0.0, 0.0, 1.0);
    const vector4 green(0.0, 1.0, 0.0, 1.0);
    const vector4 blue(0.0, 0.0, 1.0, 1.0);

    // debug shadow geometry
    if (shdServer->GetDebugShadows())
    {
        float groupColor = 1.0f - (1.0f / this->meshGroups.Size() * groupIndex);
        nGfxServer2::Instance()->SetTransform(nGfxServer2::Model, modelMatrix);
        // unlit faces
        if (this->DebugSetupGeometry(true, groupIndex))
        {
            shdServer->DrawDebugShadows(vector4(0.0, groupColor, 1.0, 0.5));
        }
        // lit faces
        if (this->DebugSetupGeometry(false, groupIndex))
        {
            shdServer->DrawDebugShadows(vector4(1.0, groupColor, 0.0, 0.5));
        }

        // open edges
        if (this->DebugSetupEdges(true, groupIndex))
        {
            shdServer->DrawDebugEdges(red);
        }
        // closed edges
        if (this->DebugSetupEdges(false, groupIndex))
        {
            shdServer->DrawDebugEdges(green);
        }

        // lit/unlit edges
        if (this->DebugSetupSilhouetteEdges())
        {
            shdServer->DrawDebugEdges(vector4(groupColor, 0.0, 1.0, 1.0));
        }
    }

	//TODO: decide if this must be Z-PASS or Z-FAIL
    if (shdServer->GetUseZFail())
    {
	    this->DrawSides(nShadowServer::zFail, lightType, worldSpaceLight, modelMatrix);
        // this->DrawDarkCap(nShadowServer::zFail, lightType, groupIndex, worldSpaceLight, modelMatrix);
        this->DrawLightCap(nShadowServer::zFail, groupIndex, worldSpaceLight, modelMatrix);
    }
    else
    {
        // zPass - faster but does not work when viewer is in shadow volume
        this->DrawSides(nShadowServer::zPass, lightType, worldSpaceLight, modelMatrix);
    }
}

//------------------------------------------------------------------------------
/**
    Compute the silhouetteEdges for the light position.    
*/
void
nShadowCaster::ComputeSilhouetteEdges(const nLight::Type lightType, const vector3& modelSpaceLight, int groupIndex)
{   
	static int currentCycle = 0;
    // is dirty, or light position has changed
    if (this->dirty || ! this->lightPosition.isequal(modelSpaceLight, (float)TINY) )
    {
        this->dirty = true;
        this->lightPosition = modelSpaceLight;
        currentCycle++; // next cycle
    }

    // is dirty, or a new group id
    if (this->dirty || this->lastGroupIndex != groupIndex)
    {
        this->lastGroupIndex = groupIndex;
        this->dirty = false;
        
        // current meshgroup
        const nMeshGroup& meshGroup = this->meshGroups[groupIndex];
        int firstGroupEdge = meshGroup.GetFirstEdge();
        int numGroupEdges = meshGroup.GetNumEdges();

        // face normals
        vector3* faceNormals = this->GetFaceNormals();
        n_assert(faceNormals);
        
        // negative light position
        const vector3 negLight3(- this->lightPosition.x, - this->lightPosition.y, - this->lightPosition.z);
        
        int i, e;
        if (nLight::Directional == lightType)
        {
            for (e = 0; e < numGroupEdges; e++)
            {
                const nMesh2::Edge& edge = this->edges[firstGroupEdge + e];

                // determine if the triangles face the light or not.
                for (i = 0; i < 2; i++)
                {
                    if (edge.fIndex[i] != (ushort)nMesh2::InvalidIndex)
                    {
                        Face& face = this->faces[edge.fIndex[i]];
                        
                        // was this face calculated in the current cycle?
                        if (face.lastUpdate != currentCycle)
                        {
                            face.lastUpdate = currentCycle;

                            const vector3& n = faceNormals[edge.fIndex[i]];

                            if (n.dot(negLight3) > 0.0f)
                            {
                                face.lightFacing = true;
                            }
                            else
                            {
                                face.lightFacing = false;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            /* TODO:
            //point light
            vector3* coordiantes = this->GetCoords();
            n_assert(coordiantes);
            
            // determine if the triangles face the light or not.
            int i;
            for (i = 0; i < this->numFaces; i++)
            {
                // get the position of one point of the triangle
                // to compute the direction vector from light to face 
                
                Face& face = this->faces[i];
                const vector3& n = faceNormals[i];
                const vector3& v0 = coordiantes[face.index[0]];

                if (n.dot((v0 * this->lightPosition.w) + negLight3) > 0.0f)
                {
                    face.lightFacing = true;
                }
                else
                {
                    face.lightFacing = false;
                }
            }
            */
        }

        this->numSilhouetteIndices = 0;
        this->silhouetteIndices.Reset();
        int growSize = n_max(16, this->numEdges / 16);
        int index = 0;
        ushort* indices = this->silhouetteIndices.Reserve(growSize);

        // find silhouette edges
        for (e = 0; e < numGroupEdges; e++)
        {
            if (index + 2 > growSize)
            {
                this->numSilhouetteIndices += growSize;
                // get more space
                index = index - growSize;
                indices = this->silhouetteIndices.Reserve(growSize);
            }
            
            const nMesh2::Edge& edge = this->edges[firstGroupEdge + e];
            
            if (edge.fIndex[0] != (ushort)nMesh2::InvalidIndex)
            {
                const Face& face0 = this->faces[edge.fIndex[0]];
                
                if (edge.fIndex[1] != (ushort)nMesh2::InvalidIndex)
                {
                    const Face& face1 = this->faces[edge.fIndex[1]];
                    
                    if (face0.lightFacing != face1.lightFacing)
                    {
                        if (face0.lightFacing)
                        {
                            indices[index++] = edge.vIndex[0];
                            indices[index++] = edge.vIndex[1];
                        }
                        else
                        {
                            indices[index++] = edge.vIndex[1];
                            indices[index++] = edge.vIndex[0];
                        }
                    }
                }
                else
                {
                    //special case: the edge.fIndex[1] is a border.
                    if (face0.lightFacing)
                    {
                        indices[index++] = edge.vIndex[0];
                        indices[index++] = edge.vIndex[1];
                    }
                    else
                    {
                        indices[index++] = edge.vIndex[1];
                        indices[index++] = edge.vIndex[0];
                    }
                }
            }
            else
            {
                // face0 is not valid - than face1 must be valid!
                const Face& face1 = this->faces[edge.fIndex[1]];
                
                //special case: the edge.fIndex[0] is a border.
                if (face1.lightFacing)
                {
                    indices[index++] = edge.vIndex[0];
                    indices[index++] = edge.vIndex[1];
                }
                else
                {
                    indices[index++] = edge.vIndex[1];
                    indices[index++] = edge.vIndex[0];
                }
            }
        }        
        this->numSilhouetteIndices += index;

        //DEBUG
        /*for (int v = 0; v < this->numSilhouetteIndices; v++)
        {
            int index = this->silhouetteIndices[v];
            n_assert(index >= meshGroup.GetFirstVertex() && index < (meshGroup.GetFirstVertex() + meshGroup.GetNumVertices()));
        }*/
    }
}

//------------------------------------------------------------------------------
/**
    @param  [in]    type                Z-Pass or Z-Fail, to get the right buffer from the shadowserver
    @param  [in]    lightType           point or directional light
    @param  [in]    worldLightPosition  light position in worldspace
    @param  [in]    modelMatrix         model matrix
*/
void
nShadowCaster::DrawSides(nShadowServer::DrawType type, const nLight::Type lightType, const vector3& worldLightPosition, const matrix44& modelMatrix)
{
	nShadowServer* shdServer = nShadowServer::Instance();
    
    // data source
    vector3* srcCoords = this->GetCoords();
	n_assert(srcCoords);
    const int numSrcCoords = this->GetNumCoords();
    n_assert(numSrcCoords > 0);

    // make sure that we have an even number of indices (this must be, because both indices of each edge are added)
    n_assert(this->numSilhouetteIndices % 2 == 0);

    // data destination
    const int maxNumCoords = shdServer->GetMaxNumCoords(type);
    n_assert(maxNumCoords >= 3);
    vector3* dstCoords = 0;
	int numCoords = 0;
    bool success = shdServer->BeginDrawBuffer(type, dstCoords, numCoords);
    n_assert(success);

    if (nLight::Directional == lightType)
    {
	    // extruded vector to light
        vector3 v0, v1;
        const vector3 extrudedLight(-worldLightPosition * 16000000.0f);
        vector3 offset = -worldLightPosition;
        offset.norm();
        offset *= shadowOffset;
        
        int i;
        for (i = 0; i < this->numSilhouetteIndices - 1; i += 2)
        {
		    // swap if needed
            if (numCoords + 3 > maxNumCoords)
            {
                bool success = shdServer->SwapDrawBuffer(type, dstCoords, numCoords);
                n_assert(success);
            }

            //n_assert(this->silhouetteIndices[i] >= 0);
            //n_assert(this->silhouetteIndices[i+1] >= 0);

            // transform to model space
            modelMatrix.mult(srcCoords[this->silhouetteIndices[i]], v0);
            modelMatrix.mult(srcCoords[this->silhouetteIndices[i+1]], v1);
            *dstCoords++ = v0 + offset;
            *dstCoords++ = v1 + offset;

            // extruded light
		    *(dstCoords++) = extrudedLight;
		    numCoords += 3;
	    }
    }
    else
    {
        //FIXME: point light - draw quads
        //ushort index0 = 2 * this->silhouetteIndices[i];
        //ushort index1 = 2 * this->silhouetteIndices[i+1];
        //ushort index2 = index0 + 1;
        //ushort index3 = index1 + 1;
        //
        //*(basePtr + numAddedIndices++) = index0;
        //*(basePtr + numAddedIndices++) = index1;
        //*(basePtr + numAddedIndices++) = index2;

        //*(basePtr + numAddedIndices++) = index1;
        //*(basePtr + numAddedIndices++) = index3;
        //*(basePtr + numAddedIndices++) = index2;
    }

    shdServer->EndDrawBuffer(type, dstCoords, numCoords);
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowCaster::DrawDarkCap(nShadowServer::DrawType type, const nLight::Type lightType, int groupIndex, const vector3& worldLightPosition, const matrix44& modelMatrix)
{
    if (nLight::Point == lightType)
    {
        //FIXME: add point light
    }
    // don't draw dark cap for directional lights
}

//------------------------------------------------------------------------------
/**
    FIXME (Floh): should use indexed rendering, currently performs redundant
    transformations.
*/
void
nShadowCaster::DrawLightCap(nShadowServer::DrawType type, int groupIndex, const vector3& worldLightPosition, const matrix44& modelMatrix)
{
    static int currentCycle = 0;
    currentCycle++;
    
    nShadowServer* shdServer = nShadowServer::Instance();

    // data source
    vector3* srcCoords = this->GetCoords();
	n_assert(srcCoords);
    
    // current meshgroup
    const nMeshGroup& meshGroup = this->meshGroups[groupIndex];
    int firstGroupEdge = meshGroup.GetFirstEdge();
    int numGroupEdges = meshGroup.GetNumEdges();

    // data destination
    const int maxNumCoords = shdServer->GetMaxNumCoords(type);
    n_assert(maxNumCoords >= 3);
    vector3* dstCoords = 0;
	int numCoords = 0;
    bool success = shdServer->BeginDrawBuffer(type, dstCoords, numCoords);
    n_assert(success);

	// extruded vector to light (FIXME: handle point lights!)
    vector3 v0, v1, v2;
    vector3 offset = -worldLightPosition;
    offset.norm();
    offset *= shadowOffset;

    int e, i;
    for (e = 0; e < numGroupEdges; e++)
    {
        const nMesh2::Edge& edge = this->edges[firstGroupEdge + e];

        // determine if the triangles face the light or not.
        for (i = 0; i < 2; i++)
        {
            if (edge.fIndex[i] != (ushort)nMesh2::InvalidIndex)
            {
                Face& face = this->faces[edge.fIndex[i]];
                if (face.lightCapCycle < currentCycle)
                {
                    face.lightCapCycle = currentCycle;
                    if (!face.lightFacing)
                    {
                        // swap if needed
                        if (numCoords + 3 > maxNumCoords)
                        {
                            bool success = shdServer->SwapDrawBuffer(type, dstCoords, numCoords);
                            n_assert(success);
                        }
                        
                        // transform coordinates
                        modelMatrix.mult(srcCoords[face.index[0]], v0);
                        modelMatrix.mult(srcCoords[face.index[1]], v1);
                        modelMatrix.mult(srcCoords[face.index[2]], v2);
                        *dstCoords++ = v0 + offset;
                        *dstCoords++ = v1 + offset;
                        *dstCoords++ = v2 + offset;

                        numCoords += 3;
                    }
                }
            }
        }
    }
    
    shdServer->EndDrawBuffer(type, dstCoords, numCoords);
}


//------------------------------------------------------------------------------
/**
*/
bool
nShadowCaster::DebugSetupGeometry(bool litFaces, int groupIndex)
{
    if (this->dbgMesh.isvalid())
    {
        this->dbgMesh->Release();
    }

    nMeshGroup& meshGroup = this->meshGroups[groupIndex];
    const int firstFace = meshGroup.GetFirstIndex() / 3;
    const int lastFace  = (meshGroup.GetFirstIndex() + meshGroup.GetNumIndices()) / 3;
    int numGroupFaces = lastFace - firstFace;
    n_assert(numGroupFaces > 0);

    nMesh2* mesh = nGfxServer2::Instance()->NewMesh(0);
    mesh->SetAsyncEnabled(false);
    mesh->SetNumVertices(numGroupFaces * 3);
    mesh->SetVertexComponents(nMesh2::Coord);
    mesh->SetNumGroups(1);
    mesh->SetNumEdges(0);
    mesh->SetNumIndices(0);
    mesh->SetUsage(nMesh2::WriteOnce);
    mesh->Load();

    vector3* srcVtxPtr = (vector3*) this->GetCoords();
    vector3* dstVtxPtr = (vector3*) mesh->LockVertices();

    int i;
    int numValidVtx = 0;
    for (i = firstFace; i < lastFace; i++)
    {
        const Face& face = this->faces[i];
        if (face.lightFacing == litFaces)
        {
            int f;
            for (f = 0; f < 3; f++)
            {
                *(dstVtxPtr++) = srcVtxPtr[face.index[f]];
                numValidVtx++;
            }
        }
    }

    mesh->UnlockVertices();
    this->dbgMesh = mesh;
    
    if (numValidVtx > 0)
    {
        nGfxServer2::Instance()->SetMesh(mesh);
        nGfxServer2::Instance()->SetVertexRange(0, numValidVtx);
        nGfxServer2::Instance()->SetIndexRange(0, mesh->GetNumIndices());
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowCaster::DebugSetupEdges(bool openEdges, int groupIndex)
{
    if (this->dbgMesh.isvalid())
    {
        this->dbgMesh->Release();
    }

    const nMeshGroup& meshGroup = this->meshGroups[groupIndex];
    const int firstEdge = meshGroup.GetFirstEdge();
    const int lastEdge  = (meshGroup.GetFirstEdge() + meshGroup.GetNumEdges());
    int numGroupEdges = lastEdge - firstEdge;
    n_assert(numGroupEdges > 0);

    nMesh2* mesh = nGfxServer2::Instance()->NewMesh(0);
    mesh->SetAsyncEnabled(false);
    mesh->SetNumVertices(numGroupEdges * 2);
    mesh->SetVertexComponents(nMesh2::Coord);
    mesh->SetNumGroups(1);
    mesh->SetNumEdges(0);
    mesh->SetNumIndices(0);
    mesh->SetUsage(nMesh2::WriteOnce);
    mesh->Load();

    vector3* srcVtxPtr = (vector3*) this->GetCoords();
    vector3* dstVtxPtr = (vector3*) mesh->LockVertices();

    int i;
    int numValidVtx = 0;
    for (i = firstEdge; i < lastEdge; i++)
    {
        const nMesh2::Edge& edge = this->edges[i];
        
        if (edge.fIndex[0] != (ushort) nMesh2::InvalidIndex && edge.fIndex[1] != (ushort) nMesh2::InvalidIndex)
        {
            // closed edge
            if (!openEdges)
            {
                *(dstVtxPtr++) = srcVtxPtr[edge.vIndex[0]];
                *(dstVtxPtr++) = srcVtxPtr[edge.vIndex[1]];
                numValidVtx += 2;
            }
        }
        else
        {
            // open edge
            if (openEdges)
            {
                *(dstVtxPtr++) = srcVtxPtr[edge.vIndex[0]];
                *(dstVtxPtr++) = srcVtxPtr[edge.vIndex[1]];
                numValidVtx += 2;
            }
        }
    }
    mesh->UnlockVertices();
    this->dbgMesh = mesh;
    
    if (numValidVtx > 0)
    {
        nGfxServer2::Instance()->SetMesh(mesh);
        nGfxServer2::Instance()->SetVertexRange(0, numValidVtx);
        nGfxServer2::Instance()->SetIndexRange(0, mesh->GetNumIndices());
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowCaster::DebugSetupSilhouetteEdges()
{
    if (this->dbgMesh.isvalid())
    {
        this->dbgMesh->Release();
    }

    nMesh2* mesh = nGfxServer2::Instance()->NewMesh(0);
    mesh->SetAsyncEnabled(false);
    mesh->SetNumVertices(this->numSilhouetteIndices);
    mesh->SetVertexComponents(nMesh2::Coord);
    mesh->SetNumGroups(1);
    mesh->SetNumEdges(0);
    mesh->SetNumIndices(0);
    mesh->SetUsage(nMesh2::WriteOnce);
    mesh->Load();

    vector3* srcVtxPtr = (vector3*) this->GetCoords();
    vector3* dstVtxPtr = (vector3*) mesh->LockVertices();

    int i;
    int numValidVtx = 0;
    for (i = 0; i < this->numSilhouetteIndices - 1; i +=2)
    {
        const int index0 = this->silhouetteIndices[i];
        const int index1 = this->silhouetteIndices[i+1];
        *(dstVtxPtr++) = srcVtxPtr[index0];
        *(dstVtxPtr++) = srcVtxPtr[index1];
        numValidVtx +=2;
    }
    mesh->UnlockVertices();
    this->dbgMesh = mesh;
    
    if (numValidVtx > 0)
    {
        nGfxServer2::Instance()->SetMesh(mesh);
        nGfxServer2::Instance()->SetVertexRange(0, numValidVtx);
        nGfxServer2::Instance()->SetIndexRange(0, mesh->GetNumIndices());
        return true;
    }
    return false;
}
