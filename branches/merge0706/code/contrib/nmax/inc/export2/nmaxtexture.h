//-----------------------------------------------------------------------------
//  nmaxtexture.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXTEXTURE_H
#define N_MAXTEXTURE_H
//-----------------------------------------------------------------------------
/**
    @class nMaxTexture
    @ingroup NebulaMaxExport2Contrib

    @brief
*/
#include "gfx2/nshaderstate.h"

class nShapeNode;

//-----------------------------------------------------------------------------
class nMaxTexture
{
public:
    enum Type
    {
        Bitmap,      // bitmap texture
        Composite,   // composite map
        Mix,         // mix map
        RGBMultiply, // RGB Multiply map
        Unknown,
    };
    
    nMaxTexture();
    virtual ~nMaxTexture();

    void Export(Texmap* texmap, int subID, nShapeNode* shapeNode);
    void Export(Texmap* texmap, nShaderState::Param param, nShapeNode* shapeNode);

    nShaderState::Param GetShaderParamFromStdMapSlot(StdUVGen* uvGen, int subID);

    static bool IsClassID(Texmap* texmap, ulong classID);
    static Type GetType (Texmap* map);

protected:
    const char* SubMapIDToString(int subID);

    bool CopyTexture(const char* textureName);

    void ExportUVTransform(StdUVGen* uvGen, nShapeNode* shapeNode);
    void ExportUVAnimator(StdUVGen* uvGen, nShapeNode* shapeNode);

};
//-----------------------------------------------------------------------------
#endif
