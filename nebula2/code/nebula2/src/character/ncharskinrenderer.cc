//------------------------------------------------------------------------------
//  ncharskinrenderer.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character/ncharskinrenderer.h"
#include "gfx2/nshader2.h"

//------------------------------------------------------------------------------
/**
*/
nCharSkinRenderer::nCharSkinRenderer() :
    gfxServer(0),
    mesh(0),
    shader(0),
    jointPaletteVarHandle(nVariable::INVALID_HANDLE),
    charSkeleton(0),
    inBegin(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nCharSkinRenderer::Begin(nGfxServer2* gfxServer,
                         nMesh2* mesh,
                         nVariable::Handle jointPaletteVarHandle,
                         const nCharSkeleton* charSkeleton)
{
    n_assert(gfxServer && mesh && charSkeleton);
    n_assert(jointPaletteVarHandle != nVariable::INVALID_HANDLE);

    this->gfxServer = gfxServer;
    this->mesh = mesh;
    this->jointPaletteVarHandle = jointPaletteVarHandle;
    this->charSkeleton = charSkeleton;
    this->inBegin = true;

    // set the current mesh in the gfx server
    gfxServer->SetMesh(0, mesh);

    // get the current shader from the gfx server
    this->shader = gfxServer->GetShader();
    n_assert(this->shader);
}

//------------------------------------------------------------------------------
/**
*/
void
nCharSkinRenderer::End()
{
    n_assert(this->inBegin);
    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nCharSkinRenderer::Render(int meshGroupIndex, const nCharJointPalette& jointPalette)
{
    n_assert(inBegin);
    static const int maxJointPaletteSize = 64;
    static float4 jointArray[maxJointPaletteSize * 3];

    // extract the current joint palette from the skeleton in the
    // right format for the skinning shader (each joint matrix is described 
    // by 3 vector4's)
    int paletteSize = jointPalette.GetNumJoints();
    n_assert(paletteSize < maxJointPaletteSize);
    int paletteIndex;
    int jointArrayIndex = 0;
    for (paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
    {
        const nCharJoint& joint = this->charSkeleton->GetJointAt(jointPalette.GetJointIndexAt(paletteIndex));
        const matrix44& m = joint.GetSkinMatrix44();
        int i;
        for (i = 0; i < 3; i++, jointArrayIndex++)
        {
            jointArray[jointArrayIndex][0] = m.m[0][i];
            jointArray[jointArrayIndex][1] = m.m[1][i];
            jointArray[jointArrayIndex][2] = m.m[2][i];
            jointArray[jointArrayIndex][3] = m.m[3][i];
        }
    }

    // transfer the joint palette to the current shader
    this->shader->SetVectorArray(jointPaletteVarHandle, jointArray, jointArrayIndex);

    // set current vertex and index range and draw mesh
    const nMeshGroup& meshGroup = this->mesh->GetGroup(meshGroupIndex);
    this->gfxServer->SetVertexRange(meshGroup.GetFirstVertex(), meshGroup.GetNumVertices());
    this->gfxServer->SetIndexRange(meshGroup.GetFirstIndex(), meshGroup.GetNumIndices());
    this->gfxServer->Draw();
}

//------------------------------------------------------------------------------
/**
*/
/*
void
nCharSkinRenderer::RenderCPU(int meshGroupIndex, const nCharJointPalette& jointPalette)
{
    n_assert(this->inBegin);

    // extract joint palette matrices from the skeleton
    const int maxJointPaletteSize = 64;
    const matrix44* jointMatrixArray44[maxJointPaletteSize];
    const matrix33* jointMatrixArray33[maxJointPaletteSize];
    int paletteSize = jointPalette.GetNumJoints();
    n_assert(paletteSize < maxJointPaletteSize);
    int paletteIndex;
    for (paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
    {
        const nCharJoint& joint = this->charSkeleton->GetJointAt(jointPalette.GetJointIndexAt(paletteIndex));
        jointMatrixArray44[paletteIndex] = &(joint.GetSkinMatrix44());
        jointMatrixArray33[paletteIndex] = &(joint.GetSkinMatrix33());
    }

    // get vertex information from mesh
    const nMeshGroup& meshGroup = this->mesh->GetGroup(meshGroupIndex);
    int srcMinVertex   = meshGroup.GetFirstVertex();
    int srcNumVertices = meshGroup.GetNumVertices();
    int srcMinIndex    = meshGroup.GetFirstIndex();
    int srcNumIndices  = meshGroup.GetNumIndices();

    int srcVertexWidth = this->mesh->GetVertexWidth();
    float* srcVertices = this->mesh->LockVertices() + (srcMinVertex * srcVertexWidth);
    ushort* srcIndices = this->mesh->LockIndices()  + srcMinIndex;

    nDynamicMesh dynMesh;
    float* dstVertices;
    ushort* dstIndices;
    int dstNumVertices;
    int dstNumIndices;

    // start rendering to the dynamic mesh
    dynMesh.Begin(this->gfxServer, 
                  this->mesh->GetPrimitiveType(), 
                  nMesh2::Coord | nMesh2::Normal | nMesh2::Tangent | nMesh2::Uv0, 
                  dstVertices, 
                  dstIndices, 
                  dstNumVertices, 
                  dstNumIndices);
    n_assert(dstNumVertices > srcNumVertices);
    n_assert(dstNumIndices > srcNumIndices);

    // transfer vertices
    int i;
    vector3 srcCoord, srcNorm, srcTangent;
    vector2 uv0;
    vector3 dstCoord, dstNorm, dstTangent;
    float w[4];
    int ji[4];
    for (i = 0; i < srcNumVertices; i++)
    {
        // read source data
        srcCoord.set(srcVertices[0], srcVertices[1], srcVertices[2]);
        srcNorm.set(srcVertices[3], srcVertices[4], srcVertices[5]);
        srcTangent.set(srcVertices[6], srcVertices[7], srcVertices[8]);
        uv0.set(srcVertices[9], srcVertices[10]);
        w[0] = srcVertices[11];
        w[1] = srcVertices[12];
        w[2] = srcVertices[13];
        w[3] = srcVertices[14];
        
        // FIXME: slow float/int conversion 
        ji[0] = int(srcVertices[15]);
        ji[1] = int(srcVertices[16]);
        ji[2] = int(srcVertices[17]);
        ji[3] = int(srcVertices[18]);
        srcVertices += srcVertexWidth;

        dstCoord   = (*jointMatrixArray44[ji[0]] * srcCoord * w[0]) +
                     (*jointMatrixArray44[ji[1]] * srcCoord * w[1]) +
                     (*jointMatrixArray44[ji[2]] * srcCoord * w[2]) +
                     (*jointMatrixArray44[ji[3]] * srcCoord * w[3]);
        dstNorm    = (*jointMatrixArray33[ji[0]] * srcNorm * w[0]) +
                     (*jointMatrixArray33[ji[1]] * srcNorm * w[1]) +
                     (*jointMatrixArray33[ji[2]] * srcNorm * w[2]) +
                     (*jointMatrixArray33[ji[3]] * srcNorm * w[3]);
        dstTangent = (*jointMatrixArray33[ji[0]] * srcTangent * w[0]) +
                     (*jointMatrixArray33[ji[1]] * srcTangent * w[1]) +
                     (*jointMatrixArray33[ji[2]] * srcTangent * w[2]) +
                     (*jointMatrixArray33[ji[3]] * srcTangent * w[3]);

        // write destination data
        *dstVertices++ = dstCoord.x;   *dstVertices++ = dstCoord.y;   *dstVertices++ = dstCoord.z;
        *dstVertices++ = dstNorm.x;    *dstVertices++ = dstNorm.y;    *dstVertices++ = dstNorm.z;
        *dstVertices++ = dstTangent.x; *dstVertices++ = dstTangent.y; *dstVertices++ = dstTangent.z;
        *dstVertices++ = uv0.x; *dstVertices++ = uv0.y;
    }

    // transfer indices
    for (i = 0; i < srcNumIndices; i++)
    {
        dstIndices[i] = srcIndices[i] - srcMinVertex;
    }
    
    // unlock source buffers
    this->mesh->UnlockVertices();
    this->mesh->UnlockIndices();

    // and render the dynamic mesh
    dynMesh.End(srcNumVertices, srcNumIndices);
}
*/


