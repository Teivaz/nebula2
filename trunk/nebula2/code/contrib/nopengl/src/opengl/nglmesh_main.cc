//------------------------------------------------------------------------------
//  nglmesh_main.cc
//  (c) 2004-2006 Oleg Khryptul (Haron)
//------------------------------------------------------------------------------
#include "opengl/nglmesh.h"

#include "opengl/nglserver2.h"
#include "opengl/ngltexture.h"
#include "opengl/nglextensionserver.h"

nNebulaClass(nGLMesh, "nmesh2");

const ushort nGLMesh::componentSize[] = {3,3,2,2,2,2,4,3,3,4,4};

//------------------------------------------------------------------------------
/**
*/
inline
int firstSetBitPos(uint n)
{
    int i, res;
    res = -1; // there is no setted bits

    for(i = 0; i < 32; i++)
    {
        if (n & (1 << i))
        {
            res = i;
            break;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
/**
*/
nGLMesh::nGLMesh() :
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
    this->componentOffset = n_new_array(ushort, firstSetBitPos(nMesh2::AllComponents + 1));
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
    n_delete_array(this->componentOffset);
}

//------------------------------------------------------------------------------
/**
    nGLMesh support asynchronous resource loading.
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
    n_assert(!this->IsLoaded());
    n_assert(0 == this->vertexBuffer && 0 == this->privVertexBuffer);
    n_assert(0 == this->indexBuffer  && 0 == this->privIndexBuffer );

    bool success = nMesh2::LoadResource();
    
    if (success)
    {
        // create the vertex declaration from the vertex component mask
        this->CreateVertexDeclaration();
    }
    return success;
}

//------------------------------------------------------------------------------
/**
    Unload the gl resources. Make sure that the resource are properly
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

    // release the resources
    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
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
        n_gltrace("nGLMesh::UnloadResource().");
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

    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
    This method is called when the gl device is lost. We only need to
    react if our vertex and index buffers are not in GL's managed pool.
    In this case, we need to unload ourselves...
*/
void
nGLMesh::OnLost()
{
    if (WriteOnly & this->vertexUsage)
    {
        this->UnloadResource();
        this->SetState(Lost);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the gl device has been restored. If our
    buffers are in the GL's default pool, we need to restore ourselves
    as well, and we need to set our state to empty, because the buffers contain
    no data.
*/
void
nGLMesh::OnRestored()
{
    if (WriteOnly & this->vertexUsage)
    {
        this->SetState(Unloaded);
        this->LoadResource();
        this->SetState(Empty);
    }
}

//------------------------------------------------------------------------------
/**
    Create a gl vertex buffer.
*/
void
nGLMesh::CreateVertexBuffer()
{
    n_assert(this->vertexBufferByteSize > 0);
    n_assert(0 == this->privVertexBuffer);
    n_assert(0 == this->vertexBuffer);

    if ((ReadOnly & this->vertexUsage) || !N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        // this is a read-only mesh which will never be rendered
        // and only read-accessed by the CPU, allocate private
        // vertex buffer
        this->privVertexBuffer = n_malloc(this->vertexBufferByteSize);
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
        if (WriteOnly & this->vertexUsage)
        {
            bufferUsage = GL_DYNAMIC_DRAW_ARB;
        }

        // create the vertex buffer
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, this->vertexBufferByteSize, NULL, bufferUsage);

        int glVBufSize;
        glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &glVBufSize);
        n_assert(glVBufSize == this->vertexBufferByteSize);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL);
    }
    n_gltrace("nGLMesh::CreateVertexBuffer().");
}

//------------------------------------------------------------------------------
/**
    Create a index buffer.
*/
void
nGLMesh::CreateIndexBuffer()
{
    n_assert(this->indexBufferByteSize > 0);
    n_assert(0 == this->indexBuffer);
    n_assert(0 == this->privIndexBuffer);

    if ((ReadOnly & this->indexUsage) || !N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        n_printf("nGLMesh::CreateIndexBuffer: NO vbo_support.\n");
        // this is a read-only mesh which will never be rendered
        // and only read-accessed by the CPU, allocate private
        // index buffer
        this->privIndexBuffer = n_malloc(this->indexBufferByteSize);
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
        if (WriteOnly == this->indexUsage)
        {
            bufferUsage = GL_DYNAMIC_DRAW_ARB;
        }

        // create the index buffer
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, this->indexBufferByteSize, NULL, bufferUsage);
        
        int glIBufSize;
        glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &glIBufSize);
        n_assert(glIBufSize == this->indexBufferByteSize);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL);
    }
    n_gltrace("nGLMesh::CreateIndexBuffer().");
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
    for (i = 0; i < firstSetBitPos(nMesh2::AllComponents + 1); i++)
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
    Lock the gl vertex buffer and return pointer to it.
*/
float*
nGLMesh::LockVertices()
{
    this->LockMutex();
    n_assert((this->vertexBuffer || this->privVertexBuffer) && !this->VBMapFlag);
    float* retval = 0;
    this->VBMapFlag = true;
    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB,this->vertexBuffer);
        void* ptr = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        n_gltrace("nGLMesh::LockVertices().");
        n_assert(ptr);    
        retval = (float*) ptr;
    }
    else
    {
        retval = (float*) this->privVertexBuffer;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the gl vertex buffer locked by nGLMesh::LockVertices().
*/
void
nGLMesh::UnlockVertices()
{
    this->LockMutex();
    n_assert((this->vertexBuffer || this->privVertexBuffer) && this->VBMapFlag);
    this->VBMapFlag = false;
    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL);
        n_gltrace("nGLMesh::UnlockVertices().");
    }
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Lock the gl index buffer and return pointer to it.
*/
ushort*
nGLMesh::LockIndices()
{
    this->LockMutex();
    n_assert((this->indexBuffer || this->privIndexBuffer) && !this->IBMapFlag);
    ushort* retval = 0;
    this->IBMapFlag = true;
    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,this->indexBuffer);
        void* ptr = glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        n_gltrace("nGLMesh::LockIndices().");
        n_assert(ptr);
        retval = (ushort*) ptr;
    }
    else
    {
        retval = (ushort*) this->privIndexBuffer;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the gl index buffer locked by nD3D9Mesh::LockIndices().
*/
void
nGLMesh::UnlockIndices()
{
    this->LockMutex();
    n_assert((this->indexBuffer || this->privIndexBuffer) && this->IBMapFlag);
    this->IBMapFlag = false;
    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL);
        n_gltrace("nGLMesh::UnlockIndices().");
    }
    this->UnlockMutex();
}

//-----------------------------------------------------------------------------
/**
    Set gl vertices
*/
bool
nGLMesh::SetVertices(int vertexStart)
{
    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, this->vertexBuffer);
    }

    int stride = this->GetVertexWidth() * sizeof(GLfloat);

    if (this->vertexComponentMask & Coord)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(this->componentSize[firstSetBitPos(Coord)],//3,
            GL_FLOAT, stride, this->VertexOffset(vertexStart, Coord));
    }

    if (this->vertexComponentMask & Normal)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, stride, this->VertexOffset(vertexStart, Normal));
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
            nGLTexture* tex = (nGLTexture*)nGfxServer2::Instance()->GetRenderTarget(stage);
            if (tex)
            {                
                int res = tex->ApplyCoords(
                    stage, this->componentSize[firstSetBitPos(Uv0 << stage)],
                    stride, this->VertexOffset(vertexStart, this->texCoordFirst + stage));

                if (res == -1)
                {
                    n_gltrace("nGLMesh::SetVertices().");
                    return false;
                }
                if (res == 0) break;
            }
        }
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (this->vertexComponentMask & Color)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(this->componentSize[firstSetBitPos(Color)],//4,
            GL_FLOAT, stride, this->VertexOffset(vertexStart, Color));
    }

    //n_printf("VStride(%s)\n", stride);
    n_gltrace("nGLMesh::SetVertices().");
    return true;
}

//-----------------------------------------------------------------------------
/**
    Unset gl vertices
*/
void
nGLMesh::UnsetVertices()
{
    // we work with texture only if mesh has tex coordinates
    if (this->texCoordNum > 0)
    {
        // clear the texture stage and release p-buffer
        int stage;
        for (stage = 0; stage < this->texCoordNum; stage++)
        {
            nGLTexture* tex = (nGLTexture*)nGfxServer2::Instance()->GetRenderTarget(stage);
            if (tex)
            {
                int res = tex->UnApplyCoords(stage);
                if (res <= 0) break;
            }
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

    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL);
    }
    n_gltrace("nGLMesh::UnsetVertices().");
}

//-----------------------------------------------------------------------------
/**
    Set gl indices
*/
bool
nGLMesh::SetIndices(int indexStart)
{
    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, this->indexBuffer);
    }

    int stride = sizeof(GLushort);//this->numIndices * sizeof(GLushort);

    glEnableClientState(GL_INDEX_ARRAY);
    glIndexPointer(GL_UNSIGNED_BYTE, stride, this->IndexOffset(indexStart));

    //n_printf("IStride(%s)\n", stride);
    n_gltrace("nGLMesh::SetIndices().");
    return true;
}

//-----------------------------------------------------------------------------
/**
    Unset gl indices
*/
void
nGLMesh::UnsetIndices()
{
    glDisableClientState(GL_INDEX_ARRAY);

    if (N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object))
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL);
    }
    n_gltrace("nGLMesh::UnsetIndices().");
}

//-----------------------------------------------------------------------------
/**
    calculate vertex element offset
*/
void*
nGLMesh::VertexOffset(int vertexStart, int component)
{
    int cm = this->vertexComponentMask & component;
    n_assert2(cm < AllComponents, "You have tried to get a buffer offset for a not possible type!!! this is a hard fail!\n");

    float* basePtr = (float*)this->privVertexBuffer + vertexStart * this->vertexWidth + componentOffset[firstSetBitPos(cm)];

    return (void*) basePtr;
}

//-----------------------------------------------------------------------------
/**
    calculate index offset
*/
void*
nGLMesh::IndexOffset(int indexStart)
{
    return (void*)((ushort*)this->privIndexBuffer + indexStart);
}

//------------------------------------------------------------------------------
/**
    Compute the byte size of the mesh data
*/
int
nGLMesh::GetByteSize()
{
    if (this->IsValid())
    {
        int vertexBufferSize = this->GetNumVertices() * this->GetVertexWidth() * sizeof(float);
        int indexBufferSize  = this->GetNumIndices() * sizeof(ushort);
        return vertexBufferSize + indexBufferSize + nMesh2::GetByteSize();
    }
    else
    {
        return 0;
    }
}
