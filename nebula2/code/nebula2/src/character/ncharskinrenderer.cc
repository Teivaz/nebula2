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
