//-----------------------------------------------------------------------------
//  nmaxmaterial_cstm.cc
//
//  Handles Nebula2 custom material of 3dsmax.
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
//#include "export2/nmaxpoint3controller.h"
//#include "export2/nmaxfloatcontroller.h"

#include "scene/nshapenode.h"

// the followings are 3dmax predifined custom attribute.
#define NUM_PREDEFINED_CA  3
const char* predefinedCA[NUM_PREDEFINED_CA] = {
    "Viewport Manager",
    "DirectX Manager",
    "mental ray: material custom attribute"
};

//-----------------------------------------------------------------------------
/**
    Check the given name of the custom attribute is predefined standard 
    custom attribute of 3dsmax or not.
    
    @param caName custom attribute name.
*/
static
bool IsPredefinedCA(TCHAR* caName)
{
    for (uint i=0; i<NUM_PREDEFINED_CA; i++)
    {
        if (n_stricmp (caName, predefinedCA[i]) == 0)
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Retrieves material values from Mtl's parameter block's parameters then
    specifies it to the given shape node.

    @param mtl pointer to the Mtl which contains material of the node.
    @param shapeNode pointer to the nebula shape node to be specified its material.
*/
void nMaxMaterial::GetNebulaMaterial(Mtl* mtl, nShapeNode* shapeNode)
{
    if (NULL == mtl || NULL == shapeNode)
        return;

    ICustAttribContainer* caContainer = mtl->GetCustAttribContainer();
    if (caContainer)
    {
        // we assume that there's only one custom attribute.
        int numAttribs = caContainer->GetNumCustAttribs();

        CustAttrib* custAttr = 0;

        for (int ca=0; ca<numAttribs; ca++)
        {
            custAttr = (CustAttrib*)caContainer->GetCustAttrib(ca);

            if (custAttr)
            {
                // we only deal with nebula2 custom material's CA. skip others.
                TCHAR* caName = custAttr->GetName();
                if (IsPredefinedCA(caName))
                    continue;

                int numParamBlocks = custAttr->NumParamBlocks();

                for (int i=0; i<numParamBlocks; i++)
                {
                    // NOTE: the only valid parameter block which can be exported is 
                    //       the custom attribute's one but we process it with general way.
                    IParamBlock2* pblock2 = custAttr->GetParamBlock(i);

                    // get the number of the params.
                    int numParams = pblock2->NumParams();

                    nShaderState::Param shaderParam;
                    int paramType;

                    for (int j=0; j<numParams; j++)
                    {
                        ParamBlockDesc2* pblockDesc = pblock2->GetDesc();
                        if (NULL == pblockDesc)
                            return;

                        ParamID paramID = pblockDesc->IndextoID(j);
                        ParamDef& paramDef = pblockDesc->GetParamDef(paramID);

                        TCHAR* name = paramDef.int_name;
                        
                        // find nShaderState::Param which match to paramter name.
                        shaderParam = this->GetShaderParam(name);

                        // filter it if the given parameter name does not match
                        // to nebula's shader state parameter.
                        //if (shaderParam == nShaderState::InvalidParameter)
                        //    continue;

                        paramType = pblock2->GetParameterType(j);

                        switch(paramType)
                        {
                        case TYPE_STRING:
                            {
                                BOOL result;
                                // .fx effect file name. (e.g. "shaders:default.fx")
                                TCHAR* value; 
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                if (result)
                                {
                                    // is this parameter for 'shader'?
                                    if (strcmp(name, "Shader") == 0)
                                    {
                                        shapeNode->SetShader(nShapeNode::StringToFourCC("colr"), value);
                                    }
                                }
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;

                        case TYPE_FLOAT:
                            {
                                BOOL result;
                                float value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                if (result)
                                    shapeNode->SetFloat(shaderParam, value);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;

                        case TYPE_BOOL:
                            {
                                BOOL result;
                                int value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                if (result)
                                    shapeNode->SetBool(shaderParam, ((value > 0) ? true : false));
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;

                        case TYPE_INT:
                            {
                                BOOL result;
                                int value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                if (result)
                                    shapeNode->SetInt(shaderParam, value);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;

                        case TYPE_RGBA:
                            {
                                BOOL result0, result1;
                                Color value;
                                Interval interval;
                                result0 = pblock2->GetValue(j, 0, value, interval);

                                // Get alpha value of the Color.
                                // The next parameter of the Color type always should be alpha which type is float.
                                float alpha;
                                int alphaIndex = j + 1; // next param index of the color.
                                if (alphaIndex < numParams)
                                {
                                    int type = pblock2->GetParameterType(alphaIndex);
                                    if (type == TYPE_FLOAT)
                                    {
                                        result1 = pblock2->GetValue(alphaIndex, 0, alpha, interval);
                                    }
                                }
                                
                                vector4 color;
                                color.set(value.r, value.g, value.b, alpha);
                                if (result0 && result1)
                                    shapeNode->SetVector(shaderParam, color);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;

                    // The following types are Available in 3dsmax6 or higher. 
                    // See '$3dsmax/maxsdk/include/paramtype.h' for more details.
                    #if MAX_RELEASE >= 6000
                        case TYPE_FRGBA:
                            {
                                BOOL result;
                                AColor value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                vector4 color;
                                color.set(value.r, value.g, value.b, value.a);
                                if (result)
                                    shapeNode->SetVector(shaderParam, color);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;

                        case TYPE_POINT4:
                            {
                                BOOL result;
                                Point4 pt4;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, pt4, interval);

                                vector4 value;
                                value.set(pt4.x, pt4.y, pt4.z, pt4.w);
                                if (result)
                                    shapeNode->SetVector(shaderParam, value);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;
                    #endif

                        case TYPE_TEXMAP:
                            {
                                BOOL result;
                                Texmap* value;
                                Interval interval;
                                
                                result = pblock2->GetValue(j, 0, value, interval);
                                if (result)
                                {
                                    nMaxTexture texture;
                                    texture.Export(value, shaderParam, shapeNode);
                                }
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;

                        default:
                            n_maxlog(Error, "Unsupported type of the parameter %s.", name);
                            break;
                        }// end of switch.
                    }//end of for each number of parameters.
                }// end of for each param blocks
            }
        }
    }

    // export shader animations.
    if (IsClassID(mtl, DMTL_CLASS_ID))
    {
        // we only export shader animation for original standard material type.
        ExportShaderAnimations(mtl);
    }
}

//-----------------------------------------------------------------------------
/**
    Retrieves matched shader param from the given script parameter name.

    @note
    parameter's internal name should be one of the member of type nShaderState::Param.
    (it's case senstive)

    @return return 'nShaderState::InvalidParameter' if there is no matched param.
*/
nShaderState::Param 
nMaxMaterial::GetShaderParam(const char* param)
{
    if (strcmp("Model", param) == 0)
        return nShaderState::Model;     // matrix: the model matrix (aka World)
    else
    if (strcmp("InvModel", param) == 0)
        return nShaderState::InvModel;  // matrix: the inverse model matrix
    else
    if (strcmp("View", param) == 0)
        return nShaderState::View;      // matrix: the view matrix
    else
    if (strcmp("InvView", param) == 0)
        return nShaderState::InvView;   // matrix: the inverse view matrix
    else
    if (strcmp("Projection", param) == 0)
        return nShaderState::Projection; // matrix: the projection matrix
    else
    if (strcmp("ModelView", param) == 0)
        return nShaderState::ModelView; // matrix: the model*view matrix
    else
    if (strcmp("InvModelView", param) == 0)
        return nShaderState::InvModelView; // matrix: the inverse of the model view matrix
    else
    if (strcmp("ModelViewProjection", param) == 0)
        return nShaderState::ModelViewProjection; // matrix: the model*view*projection matrix
    else
    if (strcmp("ModelLightProjection", param) == 0)
        return nShaderState::ModelLightProjection; // matrix: the model*light*projection matrix
    else
    if (strcmp("ModelShadowProjection", param) == 0)
        return nShaderState::ModelShadowProjection; // matrix: the model*shadow*projection matrix (shadow == shadow source)
    else
    if (strcmp("EyePos", param) == 0)
        return nShaderState::EyePos; // vector: the eye position in world space
    else
    if (strcmp("ModelEyePos", param) == 0)
        return nShaderState::ModelEyePos; // vector: the eye position in model space
    else
    if (strcmp("ModelLightPos", param) == 0)
        return nShaderState::ModelLightPos; // vector[]: the light position in model space
    else
    if (strcmp("LightPos", param) == 0)
        return nShaderState::LightPos; // vector3:  the light position in world space (must be vec3 for the FF pipeline)
    else
    if (strcmp("LightPos4", param) == 0)
        return nShaderState::LightPos4; // vector4:  the light position in world space (must be vec4 for shadow extrude)
    else
    if (strcmp("MatAmbient", param) == 0)
        return nShaderState::MatAmbient; // color: material ambient component
    else
    if (strcmp("MatDiffuse", param) == 0)
        return nShaderState::MatDiffuse; // color: material diffuse component
    else
    if (strcmp("MatSpecular", param) == 0)
        return nShaderState::MatSpecular; // color: material specular component
    else
    if (strcmp("MatSpecularPower", param) == 0)
        return nShaderState::MatSpecularPower; // float: material specular power
    else
    if (strcmp("MatTransparency", param) == 0)
        return nShaderState::MatTransparency;  // float: material transparency
    else
    if (strcmp("MatFresnel", param) == 0)
        return nShaderState::MatFresnel; // float: material fresnel term
    else
    if (strcmp("Scale", param) == 0)
        return nShaderState::Scale; // float: material scale (for waves, etc...)
    else
    if (strcmp("Noise", param) == 0)
        return nShaderState::Noise; // float: material noise scale (for wavec, etc...)
    else
    if (strcmp("MatTranslucency", param) == 0)
        return nShaderState::MatTranslucency; // float: material translucency
    else
    if (strcmp("AlphaRef", param) == 0)
        return nShaderState::AlphaRef; // int: alpha ref (0 - 255)
    else
    if (strcmp("CullMode", param) == 0)
        return nShaderState::CullMode; // int: cull mode (1 = No culling, 2 = CW, 3 = CCW)
    else
    if (strcmp("DirAmbient", param) == 0)
        return nShaderState::DirAmbient; // color array[6]: directional ambient colors
    else
    if (strcmp("FogDistances", param) == 0)
        return nShaderState::FogDistances; // vector: x=near, y=far, z=bottom, w=top
    else
    if (strcmp("FogNearBottomColor", param) == 0)
        return nShaderState::FogNearBottomColor; // color: rgb, a=lerp
    else
    if (strcmp("FogNearTopColor", param) == 0)
        return nShaderState::FogNearTopColor; // color: rgb, a=lerp
    else
    if (strcmp("FogFarBottomColor", param) == 0)
        return nShaderState::FogFarBottomColor; // color: rgb, a=lerp
    else
    if (strcmp("FogFarTopColor", param) == 0)
        return nShaderState::FogFarTopColor; // color: rgb, a=lerp
    else
    if (strcmp("LightAmbient", param) == 0)
        return nShaderState::LightAmbient; // color[]: light ambient component
    else
    if (strcmp("LightDiffuse", param) == 0)
        return nShaderState::LightDiffuse; // color[]: light diffuse component
    else
    if (strcmp("LightSpecular", param) == 0)
        return nShaderState::LightSpecular; // color[]: light specular component
    else
    if (strcmp("LightAmbient1", param) == 0)
        return nShaderState::LightAmbient1;  // color[]: additional light ambient component
    else
    if (strcmp("LightDiffuse1", param) == 0)
        return nShaderState::LightDiffuse1; // color[]: additional light diffuse component
    else
    if (strcmp("LightSpecular1", param) == 0)
        return nShaderState::LightSpecular1; // color[]: additional light specular component
    else
    if (strcmp("DiffMap0", param) == 0)
        return nShaderState::DiffMap0; // texture: diffuse map layer 0
    else
    if (strcmp("DiffMap1", param) == 0)
        return nShaderState::DiffMap1; // texture: diffuse map layer 1
    else
    if (strcmp("DiffMap2", param) == 0)
        return nShaderState::DiffMap2; // texture: diffuse map layer 2
    else
    if (strcmp("DiffMap3", param) == 0)
        return nShaderState::DiffMap3; // texture: diffuse map layer 3
    else
    if (strcmp("SpecMap0", param) == 0)
        return nShaderState::SpecMap0; // texture: specular map layer 0
    else
    if (strcmp("SpecMap1", param) == 0)
        return nShaderState::SpecMap1; // texture: specular map layer 1
    else
    if (strcmp("SpecMap2", param) == 0)
        return nShaderState::SpecMap2;  // texture: specular map layer 2
    else
    if (strcmp("SpecMap3", param) == 0)
        return nShaderState::SpecMap3; // texture: specular map layer 3
    else
    if (strcmp("AmbientMap0", param) == 0)
        return nShaderState::AmbientMap0; // texture: ambient map layer 1
    else
    if (strcmp("AmbientMap1", param) == 0)
        return nShaderState::AmbientMap1; // texture: ambient map layer 2
    else
    if (strcmp("AmbientMap2", param) == 0)
        return nShaderState::AmbientMap2; // texture: ambient map layer 3
    else
    if (strcmp("AmbientMap3", param) == 0)
        return nShaderState::AmbientMap3; // texture: ambient map layer 4
    else
    if (strcmp("BumpMap0", param) == 0)
        return nShaderState::BumpMap0; // texture: bump map layer 0
    else
    if (strcmp("BumpMap1", param) == 0)
        return nShaderState::BumpMap1; // texture: bump map layer 1
    else
    if (strcmp("BumpMap2", param) == 0)
        return nShaderState::BumpMap2; // texture: bump map layer 2
    else
    if (strcmp("BumpMap3", param) == 0)
        return nShaderState::BumpMap3; // texture: bump map layer 3
    else
    if (strcmp("CubeMap0", param) == 0)
        return nShaderState::CubeMap0; // texture: cube map layer 0
    else
    if (strcmp("CubeMap1", param) == 0)
        return nShaderState::CubeMap1; // texture: cube map layer 1
    else
    if (strcmp("CubeMap2", param) == 0)
        return nShaderState::CubeMap2; // texture: cube map layer 2
    else
    if (strcmp("CubeMap3", param) == 0)
        return nShaderState::CubeMap3; // texture: cube map layer 3
    else
    if (strcmp("NoiseMap0", param) == 0)
        return nShaderState::NoiseMap0; // texture: noise map layer 0
    else
    if (strcmp("NoiseMap1", param) == 0)
        return nShaderState::NoiseMap1; // texture: noise map layer 1
    else
    if (strcmp("NoiseMap2", param) == 0)
        return nShaderState::NoiseMap2; // texture: noise map layer 2
    else
    if (strcmp("NoiseMap3", param) == 0)
        return nShaderState::NoiseMap3; // texture: noise map layer 3
    else
    if (strcmp("LightModMap", param) == 0)
        return nShaderState::LightModMap; // texture: light modulation map
    else
    if (strcmp("ShadowMap", param) == 0)
        return nShaderState::ShadowMap; // texture: shadow map
    else
    if (strcmp("SpecularMap", param) == 0)
        return nShaderState::SpecularMap; // texture: x^y lookup map for specular highlight
    else
    if (strcmp("ShadowModMap", param) == 0)
        return nShaderState::ShadowModMap; // texture: shadow modulation map
    else
    if (strcmp("JointPalette", param) == 0)
        return nShaderState::JointPalette; // matrix array: joint palette for skinning
    else
    if (strcmp("Time", param) == 0)
        return nShaderState::Time; // float: current time in seconds
    else
    if (strcmp("Wind", param) == 0)
        return nShaderState::Wind; // vector: the direction and strength
    else
    if (strcmp("Swing", param) == 0)
        return nShaderState::Swing; // matrix: the swing rotation matrix
    else
    if (strcmp("InnerLightIntensity", param) == 0)
        return nShaderState::InnerLightIntensity; // float: light intensity at center
    else
    if (strcmp("OuterLightIntensity", param) == 0)
        return nShaderState::OuterLightIntensity; // float: light intensity at periphery
    else
    if (strcmp("BoxMinPos", param) == 0)
        return nShaderState::BoxMinPos; // vector: bounding box min pos in model space
    else
    if (strcmp("BoxMaxPos", param) == 0)
        return nShaderState::BoxMaxPos; // vector: bounding box max pos in model space
    else
    if (strcmp("BoxCenter", param) == 0)
        return nShaderState::BoxCenter; // vector: bounding box center in model space
    else
    if (strcmp("MinDist", param) == 0)
        return nShaderState::MinDist; // float: a minimum distance
    else
    if (strcmp("MaxDist", param) == 0)
        return nShaderState::MaxDist; // float: a maximum distance
    else
    if (strcmp("SpriteSize", param) == 0)
        return nShaderState::SpriteSize; // float: size of sprites
    else
    if (strcmp("MinSpriteSize", param) == 0)
        return nShaderState::MinSpriteSize; // float: minimum sprite size
    else
    if (strcmp("MaxSpriteSize", param) == 0)
        return nShaderState::MaxSpriteSize; // float: maximum sprite size
    else
    if (strcmp("SpriteSwingAngle", param) == 0)
        return nShaderState::SpriteSwingAngle; // float: swing angle for sprites (rad)
    else
    if (strcmp("SpriteSwingTime", param) == 0)
        return nShaderState::SpriteSwingTime; // float: swing time for sprites
    else
    if (strcmp("SpriteSwingTranslate", param) == 0)
        return nShaderState::SpriteSwingTranslate; // float3: sprite swing translation
    else
    if (strcmp("DisplayResolution", param) == 0)
        return nShaderState::DisplayResolution; // float2: current display width in pixels
    else
    if (strcmp("TexGenS", param) == 0)
        return nShaderState::TexGenS; // float4: texgen parameter
    else
    if (strcmp("TexGenT", param) == 0)
        return nShaderState::TexGenT; // float4: texgen parameter
    else
    if (strcmp("TexGenR", param) == 0)
        return nShaderState::TexGenR; // float4: texgen parameter
    else
    if (strcmp("TexGenQ", param) == 0)
        return nShaderState::TexGenQ; // float4: texgen parameter
    else
    if (strcmp("TextureTransform0", param) == 0)
        return nShaderState::TextureTransform0; // matrix: the texture matrix for layer 0
    else
    if (strcmp("TextureTransform1", param) == 0)
        return nShaderState::TextureTransform1; // matrix: the texture matrix for layer 1
    else
    if (strcmp("TextureTransform2", param) == 0)
        return nShaderState::TextureTransform2; // matrix: the texture matrix for layer 2
    else
    if (strcmp("TextureTransform3", param) == 0)
        return nShaderState::TextureTransform3; // matrix: the texture matrix for layer 3
    else
    if (strcmp("SampleOffsets", param) == 0)
        return nShaderState::SampleOffsets; // float4[]: filter kernel sample offsets
    else
    if (strcmp("SampleWeights", param) == 0)
        return nShaderState::SampleWeights; // float4[]: filter kernel sample weights
    else
    if (strcmp("VertexStreams", param) == 0)
        return nShaderState::VertexStreams; // int: number of parallel vertex streams
    else
    if (strcmp("VertexWeights1", param) == 0)
        return nShaderState::VertexWeights1; // float4: weights of streams 1-4
    else
    if (strcmp("VertexWeights2", param) == 0)
        return nShaderState::VertexWeights2; // float4: weights of streams 5-8
    else
    if (strcmp("AlphaSrcBlend", param) == 0)
        return nShaderState::AlphaSrcBlend; // int: Alpha Source Blend Factor
    else
    if (strcmp("AlphaDstBlend", param) == 0)
        return nShaderState::AlphaDstBlend; // int: Alpha Dest Blend Factor
    else
    if (strcmp("BumpScale", param) == 0)
        return nShaderState::BumpScale; // float
    else
    if (strcmp("FresnelBias", param) == 0)
        return nShaderState::FresnelBias; // float
    else
    if (strcmp("FresnelPower", param) == 0)
        return nShaderState::FresnelPower; // float
    else
    if (strcmp("Intensity0", param) == 0)
        return nShaderState::Intensity0; // float
    else
    if (strcmp("Intensity1", param) == 0)
        return nShaderState::Intensity1; // float
    else
    if (strcmp("Intensity2", param) == 0)
        return nShaderState::Intensity2; // float
    else
    if (strcmp("Intensity3", param) == 0)
        return nShaderState::Intensity3; // float
    else
    if (strcmp("Amplitude", param) == 0)
        return nShaderState::Amplitude; // float
    else
    if (strcmp("Frequency", param) == 0)
        return nShaderState::Frequency; // float
    else
    if (strcmp("Velocity", param) == 0)
        return nShaderState::Velocity; // float3
    else
    if (strcmp("StencilFrontZFailOp", param) == 0)
        return nShaderState::StencilFrontZFailOp; // int: front faces stencil depth fail operation
    else
    if (strcmp("StencilFrontPassOp", param) == 0)
        return nShaderState::StencilFrontPassOp; // int: front faces stencil pass operation
    else
    if (strcmp("StencilBackZFailOp", param) == 0)
        return nShaderState::StencilBackZFailOp; // int: front faces stencil depth fail operation
    else
    if (strcmp("StencilBackPassOp", param) == 0)
        return nShaderState::StencilBackPassOp; // int: front faces stencil depth fail operation
    else
        return nShaderState::InvalidParameter;
}