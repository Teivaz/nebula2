#ifndef N_CHARSKINRENDERER_H
#define N_CHARSKINRENDERER_H
//------------------------------------------------------------------------------
/**
    A smooth character skin renderer. Takes a pointer to an nMesh2
    object which contains the skin mesh, and a pointer to an 
    uptodate nCharSkeleton object.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_DYNAMICMESH_H
#include "gfx2/ndynamicmesh.h"
#endif

#ifndef N_CHARSKELETON_H
#include "character/ncharskeleton.h"
#endif

#ifndef N_CHARJOINTPALETTE_H
#include "character/ncharjointpalette.h"
#endif

#ifndef N_VARIABLE_H
#include "variable/nvariable.h"
#endif

//-----------------------------------------------------------------------------
class nCharSkinRenderer
{
public:
    /// constructor
    nCharSkinRenderer();
    /// begin rendering the skin
    void Begin(nGfxServer2* gfxServer, nMesh2* mesh, nVariable::Handle jointPaletteVarHandle, const nCharSkeleton* skeleton);
    /// render a single skin fragment
    void Render(int meshGroupIndex, const nCharJointPalette& jointPalette);
    /// finish rendering
    void End();

private:
    nGfxServer2* gfxServer;
    nMesh2* mesh;
    nShader2* shader;
    nVariable::Handle jointPaletteVarHandle;
    const nCharSkeleton* charSkeleton;
    bool inBegin;
};

//------------------------------------------------------------------------------
#endif
