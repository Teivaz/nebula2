//------------------------------------------------------------------------------
//  nglmesh_main.cc
//  Haron
//------------------------------------------------------------------------------
#include "opengl/nglmesh.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "gfx2/nn3d2loader.h"
#include "gfx2/nnvx2loader.h"

#include "opengl/ngltexture.h"

nNebulaClass(nGLMesh, "nmesh2");

#ifdef __WIN32__
//WGL_ARB_vertex_buffer_object functions
#define glBindBufferARB             this->refGfxServer->procBindBufferARB
#define glGenBuffersARB             this->refGfxServer->procGenBuffersARB
#define glDeleteBuffersARB          this->refGfxServer->procDeleteBuffersARB
#define glMapBufferARB              this->refGfxServer->procMapBufferARB
#define glUnmapBufferARB            this->refGfxServer->procUnmapBufferARB
#define glBufferDataARB             this->refGfxServer->procBufferDataARB
#define glGetBufferParameterivARB   this->refGfxServer->procGetBufferParameterivARB
#define glGetBufferPointervARB      this->refGfxServer->procGetBufferPointervARB
#endif

const ushort nGLMesh::componentSize[nGLMesh::ComponentsNum] = {3,3,2,2,2,2,4,3,3,4,4};

//------------------------------------------------------------------------------
/**
*/
nGLMesh::nGLMesh() :
    refGfxServer("/sys/servers/gfx"),
    VBMapFlag(false),
    IBMapFlag(false),
    vertexBuffer(0),
    indexBuffer(0),
    privVertexBuffer(0),
    privIndexBuffer(0),
    texCoordNum(0),
    texCoordFirst(Uv0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nGLMesh::~nGLMesh()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    nD3D9Mesh support asynchronous resource loading.
*/
bool
nGLMesh::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    This method is either called directly from the nResource::Load() method
    (in synchronous mode), or from the loader thread (in asynchronous mode).
    The method must try to validate its resources, set the valid and pending
    flags, and return a success code.
    This method may be called from a thread.
*/
bool
nGLMesh::LoadResource()
{
    n_assert(!this->IsValid());
    n_assert(0 == this->vertexBuffer && 0 == this->privVertexBuffer);
    n_assert(0 == this->indexBuffer  && 0 == this->privIndexBuffer );

    n_assert(!this->refGfxServer->getGLErrors("nGLMesh::LoadResource"));

    nPathString filename(this->GetFilename().Get());
    bool success = false;
    if (filename.IsEmpty())
    {
        // no filename, just create empty vertex and/or index buffers
        success = this->CreateEmpty();
    }
    else if (refResourceLoader.isvalid())
    {
        n_printf("DEBUG: nGLMesh::LoadResource(), resource loader.\n");
        // if the resource loader reference is valid, let it take a stab at the file
        success = refResourceLoader->Load(filename.Get(), this);
    }
    else if (filename.CheckExtension("nvx2"))
    {
        n_printf("DEBUG: nGLMesh::LoadResource(), nvx2.\n");
        // load from nvx2 file
        success = this->LoadNvx2File();
    }
    else if (filename.CheckExtension("n3d2"))
    {
        n_printf("DEBUG: nGLMesh::LoadResource(), n3d2.\n");
        // load from n3d2 file
        success = this->LoadN3d2File();
    }
    else
    {
        n_printf("nGLMesh: file extension not recognized (must be .nvx2 or .n3d2)\n");
        success = false;
    }
    this->SetValid(success);

    // fail hard if loading failed
    if (!this->IsValid())
    {
        n_error("Could not open mesh '%s'!\n", filename.Get());
    }
    return success;
}

//------------------------------------------------------------------------------
/**
    Unload the d3d resources. Make sure that the resource are properly
    disconnected from the graphics server. This method is called from
    nResource::Unload() which serves as a wrapper for synchronous and
    asynchronous mode. This method will NEVER be called from a thread
    though.
*/
void
nGLMesh::UnloadResource()
{
    n_assert(this->IsValid());

    nMesh2::UnloadResource();

    //TODO: whats with buffers that got invalid during mapping?
    nGLServer2 *gfxServer = this->refGfxServer.get();

    // check if I'm the current mesh in the gfx server, if yes, unlink
    if (gfxServer->GetMesh() == this)
    {
        gfxServer->SetMesh(0);
    }

    // release the resources
    if (gfxServer->support_GL_ARB_vertex_buffer_object)
    {    
        if (this->vertexBuffer)
        {
            glDeleteBuffersARB(1,&this->vertexBuffer);
            this->vertexBuffer = 0;
        }
        if (this->indexBuffer)
        {
            glDeleteBuffersARB(1,&this->indexBuffer);
            this->indexBuffer = 0;
        }
    }
    // release private buffers (if this is a ReadOnly mesh)
    if (this->privVertexBuffer)
    {
        n_free(this->privVertexBuffer);
        this->privVertexBuffer = 0;
    }
    if (this->privIndexBuffer)
    {
        n_free(this->privIndexBuffer);
        this->privIndexBuffer = 0;
    }

    this->SetValid(false);
}

//-----------------------------------------------------------------------------
/**
    init mesh state
    Feb-2004    Haron    created
*/
bool
nGLMesh::BeginRender(int vertexStart, bool useIndex, int indexStart)
{
    nGLServer2 *gfxServer = this->refGfxServer.get();

    //n_assert(!this->getGLErrors("nGLServer2::BeginGLState - test"));
    if (gfxServer->support_GL_ARB_vertex_buffer_object)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, this->vertexBuffer);
        if (useIndex)
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, this->indexBuffer);
    }

    int vstride = this->vertexWidth * sizeof(GLfloat);
    int istride = sizeof(GLushort);//this->numIndices * sizeof(GLushort);

    if (this->vertexComponentMask & Coord)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(this->componentSize[nGLMesh::CoordIndex],//3,
            GL_FLOAT, vstride, this->vertexOffset(vertexStart,Coord));
    }

    if (this->vertexComponentMask & Normal)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT,vstride,this->vertexOffset(vertexStart,Normal));
    }

    // we work with texture only if mesh has tex coordinates
    if (this->texCoordNum > 0)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // binding textures
        int stage;
        // TODO: check this for situation when textures not placed one by one in gfx->refTextures
        for (stage = 0; stage < this->texCoordNum; stage++)
        {
            if (gfxServer->refTextures[stage].isvalid())
            {
                nGLTexture* tex = (nGLTexture*)gfxServer->GetTexture(stage);
                
                int res = tex->ApplyCoords(
                    stage, this->componentSize[nGLMesh::Uv0Index + stage],
                    vstride, this->vertexOffset(vertexStart,this->texCoordFirst + stage));

                if (res == -1) return false;
                if (res == 0) break;
            }
        }

    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (this->vertexComponentMask & Color)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(this->componentSize[nGLMesh::ColorIndex],//4,
            GL_FLOAT,vstride,this->vertexOffset(vertexStart,Color));
    }

    if (useIndex)
    {
        glEnableClientState(GL_INDEX_ARRAY);
        glIndexPointer(GL_UNSIGNED_BYTE,istride,this->indexOffset(indexStart));
    }

    //n_printf("VStride(%s) IStride(%s)\n", vstride, istride);
    return !gfxServer->getGLErrors("nGLMesh::BeginState");
}

//-----------------------------------------------------------------------------
/**
    uninit mesh state
    Feb-2004    Haron    created
*/
void
nGLMesh::EndRender(bool useIndex)
{
    nGLServer2 *gfxServer = this->refGfxServer.get();

    // we work with texture only if mesh has tex coordinates
    if (this->texCoordNum > 0)
    {
        // clear the texture stage
        // release p-buffer
        int stage;
        for (stage = 0; stage < this->texCoordNum; stage++)
            if (gfxServer->refTextures[stage].isvalid())
            {
                nGLTexture* tex = (nGLTexture*)gfxServer->refTextures[stage].get();

                int res = tex->UnApplyCoords(stage);

                if (res <= 0) break;
            }
    }

    /*
    if (this->vertexComponentMask & Coord) glDisableClientState(GL_VERTEX_ARRAY);
    if (this->vertexComponentMask & Normal) glDisableClientState(GL_NORMAL_ARRAY);
    if (texCoordNum > 0) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    if (this->vertexComponentMask & Color) glDisableClientState(GL_COLOR_ARRAY);
    */
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);

    if (gfxServer->support_GL_ARB_vertex_buffer_object)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL);
        if (useIndex)
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL);
    }
}

//-----------------------------------------------------------------------------
/**
    calculate vertex element offset
*/
void*
nGLMesh::vertexOffset(int vertexStart, int component)
{
    //char* basePtr = this->refGfxServer->support_GL_ARB_vertex_buffer_object ? (char*)NULL : (char*)this->privVertexBuffer;
    //basePtr += vertexStart * this->vertexWidth * sizeof(GLfloat);
    
    float* basePtr = this->privVertexBuffer + vertexStart * this->vertexWidth;
    //n_printf("nGLMesh::vertexOffset(basePtr=%p", (void*)basePtr);

    switch(this->vertexComponentMask & component)
    {
    case Coord:
        basePtr += componentOffset[CoordIndex];
        //n_printf(", offset(CoordIndex)=%u, res=%p", componentOffset[CoordIndex], (void*)basePtr);
        break;
    case Normal:
        basePtr += componentOffset[NormalIndex];
        //n_printf(", offset(NormalIndex)=%u, res=%p", componentOffset[NormalIndex], (void*)basePtr);
        break;
    case Uv0:
        basePtr += componentOffset[Uv0Index];
        //n_printf(", offset(Uv0Index)=%u, res=%p", componentOffset[Uv0Index], (void*)basePtr);
        break;
    case Uv1:
        basePtr += componentOffset[Uv1Index];
        //n_printf(", offset(Uv1Index)=%u, res=%p", componentOffset[Uv1Index], (void*)basePtr);
        break;
    case Uv2:
        basePtr += componentOffset[Uv2Index];
        //n_printf(", offset(Uv2Index)=%u, res=%p", componentOffset[Uv2Index], (void*)basePtr);
        break;
    case Uv3:
        basePtr += componentOffset[Uv3Index];
        //n_printf(", offset(Uv3Index)=%u, res=%p", componentOffset[Uv3Index], (void*)basePtr);
        break;
    case Color:
        basePtr += componentOffset[ColorIndex];
        //n_printf(", offset(ColorIndex)=%u, res=%p", componentOffset[ColorIndex], (void*)basePtr);
        break;
    case Tangent:
        basePtr += componentOffset[TangentIndex];
        //n_printf(", offset(TangentIndex)=%u, res=%p", componentOffset[TangentIndex], (void*)basePtr);
        break;
    case Binormal:
        basePtr += componentOffset[BinormalIndex];
        //n_printf(", offset(BinormalIndex)=%u, res=%p", componentOffset[BinormalIndex], (void*)basePtr);
        break;
    case Weights:
        basePtr += componentOffset[WeightsIndex];
        //n_printf(", offset(WeightsIndex)=%u, res=%p", componentOffset[WeightsIndex], (void*)basePtr);
        break;
    case JIndices:
        basePtr += componentOffset[JIndicesIndex];
        //n_printf(", offset(JIndicesIndex)=%u, res=%p", componentOffset[JIndicesIndex], (void*)basePtr);
        break;
    default:
        n_printf("You have tryed to get a buffer offset for a not possible type!!! this is a hard fail!\n");
        n_assert(false);
    }

    //n_printf(")\n");
    return (void*) basePtr;
}

//-----------------------------------------------------------------------------
/**
    calculate index offset
*/
void*
nGLMesh::indexOffset(int indexStart)
{
    //char* basePtr = this->refGfxServer->support_GL_ARB_vertex_buffer_object ? (char*)NULL : (char*)this->privIndexBuffer;
    //basePtr += indexStart * 2; // 2 ?? nMeshLoader::IndexType -> Index16:Index32
    
    //return (void*)basePtr;
    //n_printf("nGLMesh::indexOffset(basePtr=%p, offset(Index)=%u)\n",
    //    (void*)this->privIndexBuffer, indexStart * 2);
    return (void*)(this->privIndexBuffer + indexStart);
}

void
nGLMesh::UnlockVertices()
{
    n_assert(this->vertexBuffer || this->privVertexBuffer);
    n_assert(this->VBMapFlag);

    this->VBMapFlag = false;
    if (this->refGfxServer->support_GL_ARB_vertex_buffer_object)
    {
        //glBindBufferARB(GL_ARRAY_BUFFER_ARB, this->vertexBuffer);
        
        //UnmapBuffer returns false if the memory was violated while mapped.
        this->SetValid(glUnmapBufferARB(GL_ARRAY_BUFFER_ARB) == GL_TRUE);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL);
    }
    n_assert(!this->refGfxServer->getGLErrors("nGLMesh::UnlockVertices"));
}


void
nGLMesh::UnlockIndices()
{
    n_assert(this->indexBuffer || this->privIndexBuffer);
    n_assert(this->IBMapFlag);

    this->IBMapFlag = false;
    if (this->refGfxServer->support_GL_ARB_vertex_buffer_object)
    {
        //glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, this->indexBuffer);
        
        //UnmapBuffer returns false if the memory was violated while mapped.
        this->SetValid(glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB) == GL_TRUE);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL);
    }
    n_assert(!this->refGfxServer->getGLErrors("nGLMesh::UnlockIndices"));
}

//------------------------------------------------------------------------------
/**
    Lock the gl vertex buffer and return pointer to it.
*/
float*
nGLMesh::LockVertices()
{
    //n_printf("nGLMesh::LockVertices(): vertexBuffer(%i) privVertexBuffer(%p).\n",this->vertexBuffer,this->privVertexBuffer);
    n_assert((0 != this->vertexBuffer) || (0 != this->privVertexBuffer));
    n_assert(!this->VBMapFlag);

    this->VBMapFlag = true;
    if (this->refGfxServer->support_GL_ARB_vertex_buffer_object)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB,this->vertexBuffer);
        void* ptr = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

        n_assert(ptr);
        //n_assert(this->refGfxServer->getGLErrors());
    
        return (float*) ptr;
    }
    n_assert(!this->refGfxServer->getGLErrors("nGLMesh::LockVertices"));
    return this->privVertexBuffer;
}

//------------------------------------------------------------------------------
/**
    Lock the gl index buffer and return pointer to it.
*/
ushort*
nGLMesh::LockIndices()
{
    //n_printf("nGLMesh::LockIndices():  indexBuffer(%i)  privIndexBuffer(%p).\n",this->indexBuffer,this->privIndexBuffer);
    n_assert((0 != this->indexBuffer) || (0 != this->privIndexBuffer));

    n_assert(!this->IBMapFlag);

    this->IBMapFlag = true;
    if (this->refGfxServer->support_GL_ARB_vertex_buffer_object)
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,this->indexBuffer);
        void* ptr = glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        
        n_assert(ptr);
        //n_assert(this->refGfxServer->getGLErrors());

        return (ushort*) ptr;
    }
    n_assert(!this->refGfxServer->getGLErrors("nGLMesh::LockIndices"));
    return this->privIndexBuffer;
}

//------------------------------------------------------------------------------
/**
    calculate vertex element offsets for loaded mesh
*/
void
nGLMesh::CreateVertexDeclaration()
{
    //n_printf("componentOffset[ ");
    int i, s = 0;
    for (i = 0; i < ComponentsNum; i++)
        if (this->vertexComponentMask & (1 << i))
        {
            componentOffset[i] = s;
            //n_printf("(%d):%d,%d ", i, s, componentSize[i]);
            s += componentSize[i];
        }

    //n_printf("]\n");
    if (this->vertexComponentMask & Uv3){texCoordNum=1; texCoordFirst = Uv3;}
    if (this->vertexComponentMask & Uv2){texCoordNum++; texCoordFirst = Uv2;}
    if (this->vertexComponentMask & Uv1){texCoordNum++; texCoordFirst = Uv1;}
    if (this->vertexComponentMask & Uv0){texCoordNum++; texCoordFirst = Uv0;}
}

//------------------------------------------------------------------------------
/**
    Create a gl vertex buffer.
*/
void
nGLMesh::CreateVertexBuffer(/*float* data=NULL*/)
{
    //TODO: enable import of memory data, by pointer to data if already in memory.
    float* data = NULL;

    n_assert(this->vertexBufferByteSize > 0);
    n_assert(0 == this->privVertexBuffer);
    n_assert(0 == this->vertexBuffer);

    if (ReadOnly == this->usage || !this->refGfxServer->support_GL_ARB_vertex_buffer_object)
    {
        n_printf("nGLMesh::CreateVertexBuffer: NO vbo_support.\n");
        // this is a read-only mesh which will never be rendered
        // and only read-accessed by the CPU, allocate private
        // vertex buffer
        this->privVertexBuffer =(float*) n_malloc(this->vertexBufferByteSize);
        n_assert(this->privVertexBuffer);
    }
    else
    {
        glGenBuffersARB(1, &(this->vertexBuffer));
        //n_printf("nGLMesh::CreateVertexBuffer(): buffer:%i.\n",this->vertexBuffer);

        //bind the buffer, so that all following commands aply on this buffer
        glBindBufferARB(GL_ARRAY_BUFFER_ARB,this->vertexBuffer);

        //select usage type
        GLenum bufferUsage = GL_STATIC_DRAW_ARB; //default
        if (WriteOnly == this->usage)
        {
            bufferUsage = GL_DYNAMIC_DRAW_ARB;
        }

        // create the vertex buffer
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, this->vertexBufferByteSize,(void*) data, bufferUsage);

        int glVBufSize;
        glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &glVBufSize);
        n_assert(glVBufSize == this->vertexBufferByteSize);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL);
    }
    n_assert(!this->refGfxServer->getGLErrors("nGLMesh::CreateVertexBuffer"));
}

//------------------------------------------------------------------------------
/**
    Create a index buffer.
*/
void
nGLMesh::CreateIndexBuffer(/*ushort* data=NULL*/)
{
    //TODO: enable import of memory data, by pointer to data if already in memory.
    ushort* data = NULL;

    n_assert(this->indexBufferByteSize > 0);
    n_assert(0 == this->indexBuffer);
    n_assert(0 == this->privIndexBuffer);

    if (ReadOnly == this->usage || !this->refGfxServer->support_GL_ARB_vertex_buffer_object)
    {
        n_printf("nGLMesh::CreateIndexBuffer: NO vbo_support.\n");
        // this is a read-only mesh which will never be rendered
        // and only read-accessed by the CPU, allocate private
        // index buffer
        this->privIndexBuffer =(ushort *) n_malloc(this->indexBufferByteSize);
        n_assert(this->privIndexBuffer);
    }
    else
    {
        //create indexBuffer
        glGenBuffersARB(1, &(this->indexBuffer));
        //n_printf("nGLMesh::CreateIndexBuffer():  buffer:%i.\n",this->indexBuffer);

        //bind the buffer, so that all following commands aply on this buffer
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,this->indexBuffer);

        //select usage type
        GLenum bufferUsage = GL_STATIC_DRAW_ARB; //default
        if (WriteOnly == this->usage)
        {
            bufferUsage = GL_DYNAMIC_DRAW_ARB;
        }

        // create the index buffer
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, this->indexBufferByteSize,(void*) data, bufferUsage);
        
        int glIBufSize;
        glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &glIBufSize);
        n_assert(glIBufSize == this->indexBufferByteSize);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL);
    }
    n_assert(!this->refGfxServer->getGLErrors("nGLMesh::CreateIndexBuffer"));
}

//------------------------------------------------------------------------------
/**
    Update the group bounding boxes. This is a slow operation (since the
    GL vertex buffer must be locked and read). It should only be called
    once after loading.
*/
void
nGLMesh::UpdateGroupBoundingBoxes()
{
    bbox3 groupBox;
    int groupIndex;

    float* vertexBufferData = this->LockVertices();
    ushort* indexBufferData = this->LockIndices();
    for (groupIndex = 0; groupIndex < this->numGroups; groupIndex++)
    {
        groupBox.begin_extend();

        const nMeshGroup& group = this->GetGroup(groupIndex);
        ushort* indexPointer = indexBufferData + group.GetFirstIndex();
        int i;
        for (i = 0; i < group.GetNumIndices(); i++)
        {
            float* vertexPointer = vertexBufferData + (indexPointer[i] * this->vertexWidth);
            groupBox.extend(vertexPointer[0], vertexPointer[1], vertexPointer[2]);
        }
    }
    this->UnlockIndices();
    this->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
    Read an .nvx2 file (Nebula's binary mesh file format).
*/
bool
nGLMesh::LoadNvx2File()
{
    n_assert(!this->IsValid());
    bool res;
    nString filename = this->GetFilename();

    // configure a mesh loader and load header
    nNvx2Loader meshLoader;
    meshLoader.SetFilename(filename.Get());
    meshLoader.SetIndexType(nMeshLoader::Index16);
    if (!meshLoader.Open(kernelServer->GetFileServer()))
    {
        n_error("nGLMesh: could not open file '%s'!\n", filename.Get());
        return false;
    }
    
    // transfer header data
    this->SetNumGroups(meshLoader.GetNumGroups());
    this->SetNumVertices(meshLoader.GetNumVertices());
    this->SetVertexComponents(meshLoader.GetVertexComponents());
    this->SetNumIndices(meshLoader.GetNumIndices());
    int groupIndex;
    int numGroups = meshLoader.GetNumGroups();
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        nMeshGroup& group = this->GetGroup(groupIndex);
        group = meshLoader.GetGroupAt(groupIndex);
    }
    n_assert(this->GetVertexWidth() == meshLoader.GetVertexWidth());

    // allocate vertex and index buffers
    int vbSize = meshLoader.GetNumVertices() * meshLoader.GetVertexWidth() * sizeof(float);
    int ibSize = meshLoader.GetNumIndices() * sizeof(ushort);
    this->SetVertexBufferByteSize(vbSize);
    this->SetIndexBufferByteSize(ibSize);
    this->CreateVertexBuffer();
    this->CreateIndexBuffer();

    // load vertex buffer
    float* vertexBufferPtr = this->LockVertices();
    res = meshLoader.ReadVertices(vertexBufferPtr, vbSize);
    n_assert(res);
    this->UnlockVertices();

    // load indices
    ushort* indexBufferPtr = this->LockIndices();
    res = meshLoader.ReadIndices(indexBufferPtr, ibSize);
    n_assert(res);
    this->UnlockIndices();

    // close the meshloader
    meshLoader.Close();

    // update the group bounding box data
    this->UpdateGroupBoundingBoxes();

    // create the vertex declaration from the vertex component mask
    this->CreateVertexDeclaration();

    return true;
}

//------------------------------------------------------------------------------
/**
    Read an .n3d2 file (Nebula ascii mesh file format).
*/
bool
nGLMesh::LoadN3d2File()
{
    n_assert(!this->IsValid());
    bool res;
    nString filename = this->GetFilename();

    // configure a mesh loader and load header
    nN3d2Loader meshLoader;
    meshLoader.SetFilename(filename.Get());
    meshLoader.SetIndexType(nMeshLoader::Index16);
    if (!meshLoader.Open(kernelServer->GetFileServer()))
    {
        n_error("nD3D9Mesh: could not open file '%s'!\n", filename.Get());
        return false;
    }

    // transfer header data
    this->SetNumGroups(meshLoader.GetNumGroups());
    this->SetNumVertices(meshLoader.GetNumVertices());
    this->SetVertexComponents(meshLoader.GetVertexComponents());
    this->SetNumIndices(meshLoader.GetNumIndices());
    int groupIndex;
    int numGroups = meshLoader.GetNumGroups();
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        nMeshGroup& group = this->GetGroup(groupIndex);
        group = meshLoader.GetGroupAt(groupIndex);
    }
    n_assert(this->GetVertexWidth() == meshLoader.GetVertexWidth());

    // allocate vertex and index buffers
    int vbSize = meshLoader.GetNumVertices() * meshLoader.GetVertexWidth() * sizeof(float);
    int ibSize = meshLoader.GetNumIndices() * sizeof(ushort);
    this->SetVertexBufferByteSize(vbSize);
    this->SetIndexBufferByteSize(ibSize);
    this->CreateVertexBuffer();
    this->CreateIndexBuffer();

    // load vertex buffer
    float* vertexBufferPtr = this->LockVertices();
    res = meshLoader.ReadVertices(vertexBufferPtr, vbSize);
    n_assert(res);
    this->UnlockVertices();

    // load indices
    ushort* indexBufferPtr = this->LockIndices();
    res = meshLoader.ReadIndices(indexBufferPtr, ibSize);
    n_assert(res);
    this->UnlockIndices();

    // close the meshloader
    meshLoader.Close();

    // update the group bounding box data
    this->UpdateGroupBoundingBoxes();

    // create the vertex declaration from the vertex component mask
    n_printf("Loading mesh: %s\n", filename.Get());
    this->CreateVertexDeclaration();

    return true;
}
