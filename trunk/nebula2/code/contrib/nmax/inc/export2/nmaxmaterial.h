//-----------------------------------------------------------------------------
//  nmaxmaterial.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXMATERIAL_H
#define N_MAXMATERIAL_H

//-----------------------------------------------------------------------------
/**
    @class nMaxMaterial
    @ingroup NebulaMaxExport2Contrib

    @brief Class for exporting 3DS Max materials.

    The followings are supported:
      -# Standard material.
      -# Muliti-sub material.
      -# Nebula2 custom mateiral which is shader censtric and more Nebula
         friendly.
    
*/
#include "mathlib/vector.h"
#include "gfx2/nshaderstate.h"

class nShapeNode;

//-----------------------------------------------------------------------------
//  Class_ID for Nebula2 custom material.
//  (This should be match the class_ID of the scripted plugin script.
//
#define N2MTL_CLASS_ID0 0x6b5ae640
#define N2MTL_CLASS_ID1 0x319f53b8

#define NEBULAMTL_CLASSID Class_ID(N2MTL_CLASS_ID0, N2MTL_CLASS_ID1)

//-----------------------------------------------------------------------------
class nMaxMaterial
{
public:
    enum Type 
    {
        Standard,
        MultiSub,
        Shell,
        NebulaCustom,
        Unknown,
    };

    nMaxMaterial();
    virtual ~nMaxMaterial();

    void Export(Mtl* mtl, nShapeNode* shapeNode, int matID);

    static Type GetType(Mtl* material);
    static Texmap* GetSubTexmap(Mtl* mtl, int subMapIdx);
    static bool HasMultiTexture(StdMat2* mat);

    static bool IsClassID(Mtl* mtl, ulong classID);

    int GetNumMaps(StdMat2 *mtl);

protected:
    void CreateDefaultMaterial(nShapeNode* shapeNode);

#if (MAX_RELEASE >= 6000)
    void GetDirectXMaterial(Mtl* mtl, IDxMaterial* dxMat, nShapeNode* shapeNode);
#endif
    void GetStandardMaterial(Mtl* mtl, nShapeNode* shapeNode);
    
    void GetNebulaMaterial(Mtl* mtl, nShapeNode* shapeNode);

    // @name Functions for vector4 type material animation
    // @{
    void ExportShaderAnimations(Mtl* mtl, nShapeNode* shapeNode);
    void CreateShaderAnimator(nShapeNode* shapeNode,
                              nShaderState::Param vectorParameter, 
                              Control* ctrlColor, Control* ctrlAlpha);
    // @}

    ///
    bool SetAlphaParam(StdMat2* stdMat, nShapeNode* shapeNode);
    ///
    void SetStandardNebulaShader(nShapeNode* shapeNode, bool hasOpacityMap = false);
    ///
    nShaderState::Param GetShaderParam(const char* param);

protected:

};
//-----------------------------------------------------------------------------
/**
    Check the given class ID is matched to the gien material's class ID.

    @param mtl pointer to the Mtl which to be checked.
    @param classID  class ID to check.
*/
inline
bool nMaxMaterial::IsClassID(Mtl* mtl, ulong classID)
{
    if (mtl && mtl->ClassID() == Class_ID(classID, 0))
        return true;

    return false;
}
//-----------------------------------------------------------------------------
#endif
