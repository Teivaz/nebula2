#ifndef N_CHARSKINRENDERER_H
#define N_CHARSKINRENDERER_H
//------------------------------------------------------------------------------
/**
    @class nCharSkinRenderer
    @ingroup NebulaCharacterAnimationSystem

    @brief A smooth character skin renderer. Takes a pointer to an nMesh2
    object which contains the skin mesh, and a pointer to an 
    up to date nCharSkeleton object.

    (C) 2002 RadonLabs GmbH
*/

#include "kernel/ntypes.h"
#include "gfx2/ndynamicmesh.h"
#include "character/ncharskeleton.h"
#include "character/ncharjointpalette.h"
#include "variable/nvariable.h"

//-----------------------------------------------------------------------------
class nCharSkinRenderer
{
public:
    /// constructor
    nCharSkinRenderer();
    /// begin rendering the skin
    void Begin(nGfxServer2* gfxServer, nMesh2* mesh, const nCharSkeleton* skeleton);
    /// render a single skin fragment
    void Render(int meshGroupIndex, const nCharJointPalette& jointPalette);
    /// finish rendering
    void End();

private:
    nGfxServer2* gfxServer;
    nMesh2* mesh;
    nShader2* shader;
    const nCharSkeleton* charSkeleton;
    bool inBegin;
};

//------------------------------------------------------------------------------
#endif
