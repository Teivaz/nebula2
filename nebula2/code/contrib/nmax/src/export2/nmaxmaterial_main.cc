 //-----------------------------------------------------------------------------
//  nmaxmaterial.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"

#include "export2/nmaxmaterial.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxtexture.h"

#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"
#include "export2/nmaxcontroller.h"
#include "export2/nmaxpoint3controller.h"
#include "export2/nmaxfloatcontroller.h"

#include "scene/nshapenode.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxMaterial::nMaxMaterial()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxMaterial::~nMaxMaterial()
{
}

//-----------------------------------------------------------------------------
/**
export material.
*/
void nMaxMaterial::Export(Mtl* mtl, nShapeNode* shapeNode, int matID)
{
    if (NULL == mtl)
    {
        // no material exist, so just create default material.
        CreateDefaultMaterial(shapeNode);
    }
    else
    {
        nMaxMaterial::Type materialType;
        materialType = nMaxMaterial::GetType(mtl);

        if (nMaxMaterial::Standard == materialType)
        {

        #if (MAX_RELEASE >= 6000)   // 3dsmax R6
            IDxMaterial* dxMaterial = (IDxMaterial*)mtl->GetInterface(IDXMATERIAL_INTERFACE);
            if (dxMaterial)
            {
                // we have directx material
                GetDirectXMaterial(mtl, dxMaterial, shapeNode);
            }
            else
        #endif
            {
                // export standard material
                GetStandardMaterial(mtl, shapeNode);
            }
        }
        else
        if (nMaxMaterial::MultiSub == materialType)
        {
            // we have multi-sub material.
            MultiMtl* mulMtl;
            Mtl* subMtl;

            mulMtl = (MultiMtl*)mtl;
            subMtl = mulMtl->GetSubMtl(matID);

            if (subMtl)
            {
                nMaxMaterial::Type subMtlType;
                subMtlType = nMaxMaterial::GetType(subMtl);

                if (nMaxMaterial::Standard == subMtlType)
                {
                 #if (MAX_RELEASE >= 6000)   // 3dsmax R6
                    IDxMaterial* dxMaterial = (IDxMaterial*)mtl->GetInterface(IDXMATERIAL_INTERFACE);
                    if (dxMaterial)
                    {
                        // we have directx material
                        GetDirectXMaterial(mtl, dxMaterial, shapeNode);
                    }
                    else
                 #endif
                    {
                        // export single material
                        GetStandardMaterial(subMtl, shapeNode);
                    }
                }
                else
                {
                    // call recursively until the material to be standard.
                    Export(subMtl, shapeNode, matID);
                }
            }
            else
                ; //FIXME: multi material but no sub-material?
        }
        else
        if (nMaxMaterial::NebulaCustom == materialType)
        {
            // nebula custom scripted plug-in material.

            GetNebulaMaterial(mtl, shapeNode);
        }
        else
        {
            // unknown material type. (nor standard, multi-sub or custom)
            n_maxlog(Error, "unknown material type");
        }
    }
}

//-----------------------------------------------------------------------------
/**
    The given shape node does not have any material, so we create default one.
*/
void nMaxMaterial::CreateDefaultMaterial(nShapeNode* shapeNode)
{
    n_maxlog(Low, "The shape node '%s' has no material, so create default one", shapeNode->GetFullName());

    //FIXME: do we have move this to nMaxMesh::GetMesh()?
    nClass* clazz = shapeNode->GetClass();
    if (nString(clazz->GetName()) == "nskinshapenode")
    {
        shapeNode->SetShader(nShapeNode::StringToFourCC("colr"), "shaders:default_skinned.fx");
    }
    else
    if (nString(clazz->GetName()) == "nshapenode")
    {
        shapeNode->SetShader(nShapeNode::StringToFourCC("colr"), "shaders:default.fx");
    }
    else
    {
        n_maxlog(Error, "Unknown shape node type [%s]", shapeNode->GetFullName());
        shapeNode->SetShader(nShapeNode::StringToFourCC("colr"), "shaders:default.fx");
    }

    shapeNode->SetTexture(nShaderState::DiffMap0, "textures:examples/brick.bmp");

    vector4 ambient(0.9f, 0.9f, 0.9f, 1.0f);
    vector4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    vector4 specular(0.5f,0.5f, 0.5f, 1.0f);
    //FIXME:
    float specularPower = 32.0f;

    shapeNode->SetVector(nShaderState::MatAmbient, ambient);
    shapeNode->SetVector(nShaderState::MatDiffuse, diffuse);
    shapeNode->SetVector(nShaderState::MatSpecular, specular);
    shapeNode->SetFloat(nShaderState::MatSpecularPower, specularPower);
}

//-----------------------------------------------------------------------------
/**
    Export DirectX material of 3dsmax (only valid for more than version 6)
*/
#if (MAX_RELEASE >= 6000)
void nMaxMaterial::GetDirectXMaterial(Mtl* mtl, IDxMaterial* dxMat, nShapeNode* shapeNode)
{

}
#endif

//-----------------------------------------------------------------------------
/**
    see shader.xml for more details

        Zero         = 1:
        One          = 2:
        SrcColor     = 3:
        InvSrcColor  = 4:
        SrcAlpha     = 5:
        InvSrcAlpha  = 6:
        DestAlpha    = 7:
        InvDestAlpha = 8:
        DestColor    = 9:
        InvDestColor = 10

    @return true if there any subtractive or additive Transparency exist.
*/
bool nMaxMaterial::SetAlphaParam(StdMat2* stdMat, nShapeNode* shapeNode)
{
    bool hasAlpha = false;

    if (stdMat->GetTransparencyType() == TRANSP_SUBTRACTIVE)
    {
        shapeNode->SetInt(nShaderState::AlphaSrcBlend, 1); // Zero
        shapeNode->SetInt(nShaderState::AlphaDstBlend, 3); // SrcColor

        hasAlpha = true;
    }
    else 
    if (stdMat->GetTransparencyType() == TRANSP_ADDITIVE)
    {
        shapeNode->SetInt(nShaderState::AlphaSrcBlend, 5); // SrcAlpha
        shapeNode->SetInt(nShaderState::AlphaDstBlend, 2); // SrcColor

        hasAlpha = true;
    }

    return hasAlpha;
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxMaterial::SetStatndardNebulaShader(nShapeNode* shapeNode)
{
    if (0 == strcmp(shapeNode->GetClass()->GetName(), "nshapenode"))
    {
        shapeNode->SetShader(nShapeNode::StringToFourCC("colr"), "shaders:default.fx");
    }
    else
    if (0 == strcmp(shapeNode->GetClass()->GetName(), "nskinshapenode"))
    {
        shapeNode->SetShader(nShapeNode::StringToFourCC("colr"), "shaders:default_skinned.fx");
    }
    else
    if (0 == strcmp(shapeNode->GetClass()->GetName(), "nswingshapenode"))
    {
        //FIXME: we should have a method to determine what .fx file we will choose.
        shapeNode->SetShader(nShapeNode::StringToFourCC("colr"), "shaders:tree.fx");
        shapeNode->SetShader(nShapeNode::StringToFourCC("colr"), "shaders:leaf.fx");
    }
    else
    {
        // unknwon shapenode.
        n_maxlog(Error, "Could not specify the shader of %s", shapeNode->GetName());
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxMaterial::ExportShaderAnimations(Mtl* mtl)
{
    nMaxFloatController floatCtrl;

    StdMat2* stdMat = static_cast<StdMat2*>(mtl);

    IParamBlock2* pblock;
    pblock = stdMat->GetParamBlockByID(std2_extended);
    if (pblock)
    {
        // get alpha animation
        Control* control = pblock->GetController(std2_opacity);
        if (control)
        {
            floatCtrl.Export(control);
        }
    }

    Shader* shader;
    shader = (Shader*)stdMat->GetReference(2);
    if (NULL == shader)
    {
        return;
    }

    n_assert(SHADER_CLASS_ID == shader->SuperClassID());

    pblock = shader->GetParamBlockByID(shdr_params);
    if (pblock)
    {
        Control* control;

        // ambient
        control = pblock->GetController(shdr_ambient);
        if (control)
        {
            CreateShaderAnim(control, floatCtrl);
        }

        // diffuse
        control = pblock->GetController(shdr_diffuse);
        if (control)
        {
            CreateShaderAnim(control, floatCtrl);
        }

        // specular
        control = pblock->GetController(shdr_specular);
        if (control)
        {
            CreateShaderAnim(control, floatCtrl);
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxMaterial::CreateShaderAnim(Control* control, nMaxFloatController& alpha)
{
    // get color animation
    nMaxPoint3Controller point3Ctrl;
    point3Ctrl.Export(control);

    nArray<nMaxPoint3Controller::SampleKey> colorKeyArray;
    colorKeyArray = point3Ctrl.GetKeyArray();

    // check that if we have opacity animation
    int numKeys = point3Ctrl.GetNumKeys();
    if ( numKeys > 0)
    {
        //FIXME: assume that we have same time values for both of opacity and color.
        if (numKeys == alpha.GetNumKeys())
        {
            nArray<Sample> mergeSample;

            nArray<nMaxFloatController::SampleKey> alphaKeyArray;
            alphaKeyArray = alpha.GetKeyArray();

            // merge color and alpha values to new sample array.
            for (int i=0; i<numKeys; i++)
            {
                Sample sample;
                sample.time = colorKeyArray[i].time;
                
                vector3 color = colorKeyArray[i].key;
                float a = alphaKeyArray[i].key;

                sample.key  = vector4(color.x, color.y, color.z, a);

                mergeSample.Append(sample);
            }

            // create nvectoranimator based on mergedSample
            //...
        }
        else
        {
            for (int i=0; i<numKeys; i++)
            {
                Sample sample;
                sample.time = colorKeyArray[i].time;

                vector3 color = colorKeyArray[i].key;
                sample.key  = vector4(color.x, color.y, color.z, 1.0f);
             
                // create nvectoranimator.
                // ...
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
int nMaxMaterial::GetNumMaps(StdMat2 *mtl)
{
    int numMaps = 0;

    for (int mapID=0; mapID<mtl->NumSubTexmaps(); mapID++)
    {
        // get the texture map.
        Texmap* texmap = mtl->GetSubTexmap(mapID);

        if (texmap && mtl->MapEnabled(mapID))
            numMaps++;
    }

    return numMaps;
}

//-----------------------------------------------------------------------------
/**
*/
/*
bool nMaxMaterial::HasMultiTexture(StdMat2 *mat)
{
    Texmap* tex, *pSubTm;
    int i;

    //if (!mat->MapEnabled(ID_DI))
    //    return false ;
    //tex = (BitmapTex*) mat->GetSubTexmap(ID_DI);
    //if (!tex)
    //{
    //    return false;
    //}
    tex = GetSubTexmap(mat, ID_DI);
    if (NULL == tex)
        return false;

    // There is a bitmap in the self-illum slot. It should be used as a
    // dark map
    Texmap *pSITm = 0;

    //if (mat->MapEnabled(ID_SI))
    //{
    //    pSITm = (BitmapTex*) mat->GetSubTexmap(ID_SI);
    //    if (pSITm && pSITm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
    //        pSITm = NULL;
    //}
    sSITm = GetSubTexmap(mat, ID_SI);

    //if (pSITm && tex && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
    if (pSITm && IsClassID(tex, BMTEX_CLASS_ID))
        return true;

    // The other types of MultiTexturing
    //if (tex->ClassID() == Class_ID(MIX_CLASS_ID, 0))
    if (IsClassID(tex, MIX_CLASS_ID))
    {
        Mix *pMix = (Mix *) tex;

        // There is a "Mix" shader in the diffuse slot. It should be
        // used to set up a decal map

        if (pMix->NumSubTexmaps() != 2 && pMix->NumSubTexmaps() != 3)
            return false;

        for (i = 0; i < pMix->NumSubTexmaps(); i++)
        {
            pSubTm = pMix->GetSubTexmap(i);
            //if (pSubTm == NULL || pSubTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
            if (!IsClassID(pSubTm, BMTEX_CLASS_ID))
                return false;
        }

        return true;
    }
    else 
    //if (tex->ClassID() == Class_ID(COMPOSITE_CLASS_ID, 0))
    if (IsClassID(tex, COMPOSITE_CLASS_ID))
    {
        Composite *pComp = (Composite *) tex;

        // There is a "Composite" shader in the diffuse slot. It 
        // should be used to set up a glow map.

        if (pComp->NumSubTexmaps() != 2)
            return false;

        for (i = 0; i < pComp->NumSubTexmaps(); i++)
        {
            pSubTm = pComp->GetSubTexmap(i);
            //if (pSubTm == NULL || pSubTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
            if (!IsClassID(pSubTm, BMTEX_CLASS_ID))
                return false;
        }

        return(true);
    }
    else 
    //if (tex->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
    if (IsClassID(tex, RGBMULT_CLASS_ID))
    {
        RGBMult *pMult = (RGBMult *) tex;

        // There is an "RGB Mult" shader in the diffuse slot. It should
        // be used to set up a dark map.

        if (pMult->NumSubTexmaps() != 2)
            return false;

        for (i = 0; i < pMult->NumSubTexmaps(); i++)
        {
            pSubTm = pMult->GetSubTexmap(i);
            //if (pSubTm == NULL || pSubTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
            if (!IsClassID(pSubTm, BMTEX_CLASS_ID))
                return false;
        }

        return true;
    }

    return false;
}
*/
//-----------------------------------------------------------------------------
/*
CreateAnimator(Control* control, const char* paramName)
{
    Class_ID cId;
    cId = control->ClassID();

    IKeyControl* iKeyCtrl;
    iKeyCtrl = GetKeyControlInterface(control);
    if (iKeyCtrl)
    {
        if (iKeyCtrl->GetNumKeys() <= 0)
            return;

        //if (Class_ID(TCBINTERP_POINT3_CLASS_ID) == cId)
        //{
        //}
        //else
        //if ((Class_ID(HYBRIDINTERP_COLOR_CLASS_ID, 0)) == cId || 
        //    (Class_ID(HYBRIDINTERP_POINT3_CLASS_ID, 0)) == cId )
        //{
        //}
        //else
        //{
        //}
    }
    else
    {
        // IKeyControl does not exist.
    }

    return;
}
*/
/*
//
(Control* control, )
{
    TimeValue time;
    Point3 value;
    vector4 key;

    // create a new nVectorAnimator
    nKernelServer* ks = nKernelServer::Instance();
    nVectorAnimator* animator = (nVectorAnimator*)ks->New("nvectoranimator",);

    for (i=0, time=; i<numFrames; i++, time += GetTickPerFrame())
    {
        control->GetValue(t, &value, range);

        key.x = value.x;
        key.y = value.y;
        key.z = value.z;
        key.w = 0.0f;
        animator->AddKey(t, key);
    }
}
*/
//-----------------------------------------------------------------------------
/**
    Get material type from given max material
*/
nMaxMaterial::Type 
nMaxMaterial::GetType(Mtl* material)
{
    n_assert(material);
  
    nMaxMaterial::Type type;

    if (material->ClassID() == Class_ID(CMTL_CLASS_ID, 0) ||
        material->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        // top-bottom material or original standard material.
        type = Standard;
    }
    else
    if (material->ClassID() == Class_ID(MULTI_CLASS_ID, 0))
    {
        // multi material.
        type = MultiSub;
    }
    else
    if (material->ClassID() == NEBULAMTL_CLASSID)
    {
        // nebula2 custom material.
        type = NebulaCustom;
    }
    else
    {
        type = Unknown;
    }

    return type;
}

//-----------------------------------------------------------------------------
/**
    Returns a pointer to the 'i-th' sub-texmap managed by the material or texture.

    The followings are sub-texmap index for the 3dsmax standard material.

      - ID_AM - Ambient (value 0)
      - ID_DI - Diffuse (value 1)
      - ID_SP - Specular (value 2)
      - ID_SH - Shininess (value 3). In R3 and later this is called Glossiness.
      - ID_SS - Shininess strength (value 4). In R3 and later this is called Specular Level.
      - ID_SI - Self-illumination (value 5)
      - ID_OP - Opacity (value 6)
      - ID_FI - Filter color (value 7)
      - ID_BU - Bump (value 8)
      - ID_RL - Reflection (value 9)
      - ID_RR - Refraction (value 10)
      - ID_DP - Displacement (value 11)
    
*/
Texmap* nMaxMaterial::GetSubTexmap(Mtl* mtl, int subMapIdx)
{
    Texmap* map = 0;
    if (((StdMat2*)mtl)->MapEnabled(subMapIdx))
    {
        // only valid for bitmap texture map.
        map = ((BitmapTex*)mtl)->GetSubTexmap(subMapIdx);
        if (map && mtl->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
            map = 0;
    }
    return map;
}

