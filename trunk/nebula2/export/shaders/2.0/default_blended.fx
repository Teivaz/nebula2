//------------------------------------------------------------------------------
//  ps2.0/default_blended.fx
//
//  DX9 shader for mesh blending.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;    // the model*view*projection matrix
shared float3   ModelEyePos;            // the eye position in model space
shared float3   ModelLightPos;          // the light position in model space

float4 LightDiffuse;                // light diffuse color        
float4 LightSpecular;               // light specular color
float4 LightAmbient;                // light ambient color
float4 MatDiffuse;                  // material diffuse color
float4 MatSpecular;                 // material specular color
float  MatSpecularPower;            // the material's specular power

texture DiffMap0;                   // 2d texture
texture BumpMap0;                   // 2d texture
texture ShadowMap;

int AlphaRef = 100;

int CullMode = 2;

int VertexStreams;
float4 VertexWeights1;
float4 VertexWeights2;

static bool EnableShadows = true;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position0 : POSITION0;
    float4 position1 : POSITION1;
    float4 position2 : POSITION2;
    float4 position3 : POSITION3;
    float4 position4 : POSITION4;
    float4 position5 : POSITION5;
    float4 position6 : POSITION6;
    float4 position7 : POSITION7;
    float3 normal0   : NORMAL0;
    float3 normal1   : NORMAL1;
    float3 normal2   : NORMAL2;
    float3 normal3   : NORMAL3;
    float3 normal4   : NORMAL4;
    float3 normal5   : NORMAL5;
    float3 normal6   : NORMAL6;
    float3 normal7   : NORMAL7;
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT; 
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
    float2 uv0          : TEXCOORD0;        // texture coordinate
    float3 primLightVec : TEXCOORD1;        // primary light vector
    float3 primHalfVec  : TEXCOORD2;        // primary half vector
    // float3 F_ex         : TEXCOORD3;        // light outscatter coefficient
    // float3 L_in         : TEXCOORD4;        // light inscatter coefficient
    // float4 shadowPos    : TEXCOORD5;        // shadow source position
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/bumpsampler.fx"

/*
sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};
*/

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 pos = 0;
    float3 normal = 0;
    if (VertexStreams > 0)
    {
        pos = vsIn.position0;
        normal = vsIn.normal0;
        if (VertexStreams > 1)
        {
            pos += (vsIn.position1-vsIn.position0) * VertexWeights1.y;
            normal += (vsIn.normal1-vsIn.normal0) * VertexWeights1.y;
            if (VertexStreams > 2)
            {
                pos += (vsIn.position2-vsIn.position0) * VertexWeights1.z;
                normal += (vsIn.normal2-vsIn.normal0) * VertexWeights1.z;
                if (VertexStreams > 3)
                {
                    pos += (vsIn.position3-vsIn.position0) * VertexWeights1.w;
                    normal += (vsIn.normal3-vsIn.normal0) * VertexWeights1.w;
                    if (VertexStreams > 4)
                    {
                        pos += (vsIn.position4-vsIn.position0) * VertexWeights2.x;
                        normal += (vsIn.normal4-vsIn.normal0) * VertexWeights2.x;
                        if (VertexStreams > 5)
                        {
                            pos += (vsIn.position5-vsIn.position0) * VertexWeights2.y;
                            normal += (vsIn.normal5-vsIn.normal0) * VertexWeights2.y;
                            if (VertexStreams > 6)
                            {
                                pos += (vsIn.position6-vsIn.position0) * VertexWeights2.z;
                                normal += (vsIn.normal6-vsIn.normal0) * VertexWeights2.z;
                            }
                        }
                    }
                }
            }
        }
    }
    vsOut.position = mul(pos, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsLighting(pos, normal, vsIn.tangent,
               ModelLightPos, ModelEyePos,
               vsOut.primLightVec, vsOut.primHalfVec);
               
    // vsAthmoFog(pos, ModelEyePos, ModelLightPos, vsOut.L_in, vsOut.F_ex);
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // compute lit color
    float4 diffuse  = MatDiffuse * LightDiffuse;
    float4 specular = MatSpecular * LightSpecular;
    float4 baseColor = psLighting(DiffSampler, BumpSampler,
                                  psIn.uv0, psIn.primLightVec, psIn.primHalfVec,
                                  diffuse, specular, LightAmbient, MatSpecularPower);
/*
    if (EnableShadows)
    {
        float4 shadowModulate = shadow(psIn.shadowPos, 0.0f, ShadowMapSampler);
        baseColor *= shadowModulate;
    }
*/    
    // return psAthmoFog(psIn.L_in, psIn.F_ex, baseColor);
    return baseColor;
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = true;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        CullMode         = <CullMode>;
        
        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        AlphaRef         = <AlphaRef>;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
