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
                         const nCharSkeleton* charSkeleton)
{
    n_assert(gfxServer && mesh && charSkeleton);

    this->gfxServer = gfxServer;
    this->mesh = mesh;
    this->charSkeleton = charSkeleton;
    this->inBegin = true;

    // set the current mesh in the gfx server
    gfxServer->SetMesh(mesh);

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
    static const int maxJointPaletteSize = 72;
    static matrix44 jointArray[maxJointPaletteSize];

    // extract the current joint palette from the skeleton in the
    // right format for the skinning shader
    int paletteSize = jointPalette.GetNumJoints();
    n_assert(paletteSize <= maxJointPaletteSize);
    int paletteIndex;
    for (paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
    {
        const nCharJoint& joint = this->charSkeleton->GetJointAt(jointPalette.GetJointIndexAt(paletteIndex));
        jointArray[paletteIndex] = joint.GetSkinMatrix44();
    }

    // transfer the joint palette to the current shader
    if (this->shader->IsParameterUsed(nShaderState::JointPalette))
    {
        this->shader->SetMatrixArray(nShaderState::JointPalette, jointArray, paletteSize);
    }

    // set current vertex and index range and draw mesh
    const nMeshGroup& meshGroup = this->mesh->GetGroup(meshGroupIndex);
    this->gfxServer->SetVertexRange(meshGroup.GetFirstVertex(), meshGroup.GetNumVertices());
    this->gfxServer->SetIndexRange(meshGroup.GetFirstIndex(), meshGroup.GetNumIndices());
    this->gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
}
