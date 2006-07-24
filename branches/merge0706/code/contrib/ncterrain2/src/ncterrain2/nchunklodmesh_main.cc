//------------------------------------------------------------------------------
//  nchunklodmesh_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "ncterrain2/nchunklodmesh.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ngfxserver2.h"

nNebulaClass(nChunkLodMesh, "nresource");

//------------------------------------------------------------------------------
/**
*/
nChunkLodMesh::nChunkLodMesh() :
    fileChunkPos(0),
    chuFile(0),
    extChuFile(false)
{
    this->matDiffuse.x = 1.0f;
    this->matDiffuse.y = 1.0f;
    this->matDiffuse.z = 1.0f;
    this->matDiffuse.w = 1.0f;

    // do NOT create the mesh object inside LoadResource(),
    // because NewMesh() may not be multithreading safe
    this->refMesh = nGfxServer2::Instance()->NewMesh(0);
    this->refMesh->SetRefillBuffersMode(nMesh2::DisabledOnce);
}

//------------------------------------------------------------------------------
/**
*/
nChunkLodMesh::~nChunkLodMesh()
{
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
    }
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    We support asynchronous resource loading.
*/
bool
nChunkLodMesh::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    FIXME: this method is NOT multithreading safe at the moment, because
    a nRoot object is created!
    Need to implement a "kernel lock" which protects the Nebula kernel
    and the object hierarchy!

*/
bool
nChunkLodMesh::LoadResource()
{
    n_assert(!this->IsValid());
    n_assert(this->refMesh.isvalid());

    // open file if not externally provided
    if (!this->extChuFile)
    {
        n_assert(0 == this->chuFile);
        this->chuFile = kernelServer->GetFileServer()->NewFileObject();
        if (!chuFile->Open(this->GetFilename().Get(), "rb"))
        {
            return false;
        }
    }
    else
    {
        n_assert(this->chuFile);
    }

    // seek to start of chunk data
    this->chuFile->Seek(this->fileChunkPos, nFile::START);

    // read number of vertices in chunk (16 bit int)
    ushort numVertices = this->chuFile->GetUShort();

    // read compressed vertices into a temp buffer
    // a compressed vertex contains 4 ushorts: x, y, z and morph delta
    short* vertexBuffer = n_new_array(short, numVertices * 4);
    this->chuFile->Read(vertexBuffer, numVertices * 4 * sizeof(short));

    // read number of indices (32 bit int)
    int numIndices = this->chuFile->GetInt();
    n_assert(numIndices > 0);

    // create a vertex buffer object
    nMesh2* mesh = this->refMesh.get();
    mesh->SetUsage(nMesh2::WriteOnce);
    mesh->SetNumVertices(numVertices);
    mesh->SetNumIndices(numIndices);
    mesh->SetVertexComponents(nMesh2::Coord);
    mesh->SetNumGroups(1);
    this->refMesh = mesh;
    bool meshCreated = mesh->Load();
    n_assert(meshCreated);

    // read indices directly into mesh object
    ushort* indices = mesh->LockIndices();
    n_assert(indices);
    this->chuFile->Read(indices, numIndices * sizeof(ushort));
    mesh->UnlockIndices();

    // load the "real" triangle count
    this->chuFile->GetInt();

    // uncompress the vertex data into the mesh's vertex buffer
    float* vertices = mesh->LockVertices();
    ushort i;
    short* srcPtr = vertexBuffer;
    float* dstPtr = vertices;
    for (i = 0; i < numVertices; i++)
    {
        *dstPtr++ = (float(*srcPtr++) * this->vertexScale.x) + this->vertexOffset.x;
        *dstPtr++ = (float(*srcPtr++) * this->vertexScale.y) + this->vertexOffset.y;
        *dstPtr++ = (float(*srcPtr++) * this->vertexScale.z) + this->vertexOffset.z;
        srcPtr++;
    }
    mesh->UnlockVertices();

    // free the tmp compressed vertex buffer
    n_delete(vertexBuffer);
    vertexBuffer = 0;

    // close chunk file if not external file
    if (!this->extChuFile)
    {
        this->chuFile->Close();
        this->chuFile->Release();
        this->chuFile = 0;
    }

    // set a random material diffuse color
    this->matDiffuse.x = 0.5f + n_rand() * 0.5f;
    this->matDiffuse.y = 0.5f + n_rand() * 0.5f;
    this->matDiffuse.z = 0.5f + n_rand() * 0.5f;
    this->matDiffuse.w = 1.0f;

    this->SetValid(true);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload the internal mesh resource object.
*/
void
nChunkLodMesh::UnloadResource()
{
    n_assert(this->IsValid());
    if (this->refMesh.isvalid())
    {
        this->refMesh->Unload();
    }

    // n_printf("nChunkLodMesh %s: unloading\n");

    this->SetValid(false);
}
