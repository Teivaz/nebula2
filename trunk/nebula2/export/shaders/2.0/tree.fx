//------------------------------------------------------------------------------
//  ps2.0/tree.fx
//
//  A tree trunk shader which can swing in the wind.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 Model;                     // the model -> world matrix
float4x4 ModelViewProjection;       // the model*view*projection matrix
float4x4 ModelLightProjection;      // the model*light*projection matrix
float4   ModelLightPos;             // the light's position in model space
float4   ModelEyePos;               // the eye position in model space

float4 LightDiffuse;                // light diffuse color        
float4 LightSpecular;               // light specular color
float4 LightAmbient;                // light ambient color
float4 MatDiffuse;                  // material diffuse color
float4 MatSpecular;                 // material specular color
float4 MatAmbient;                  // material ambient color
float  MatSpecularPower;            // the material's specular power

texture DiffMap0;                   // 2d texture
texture BumpMap0;                   // 2d texture

int AlphaRef = 100;

float3x3 Swing;                     // the swing rotation matrix
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max

int CullMode = 2;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal :   NORMAL;  
    float3 tangent :  TANGENT; 
    float2 uv0 :      TEXCOORD0;
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
    float2 uv0          : TEXCOORD0;        // texture coordinate
    float3 primLightVec : TEXCOORD1;        // primary light vector
    float3 primHalfVec  : TEXCOORD2;        // primary half vector
    float3 F_ex         : TEXCOORD3;        // light outscatter coefficient
    float3 L_in         : TEXCOORD4;        // light inscatter coefficient
    float4 diffuse      : COLOR0;
    float4 specular     : COLOR1;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/bumpsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    // compute lerp factor based on height above min y
    float ipol = (vsIn.position.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);

    // compute rotated vertex position, normal and tangent in model space
    float4 rotPosition  = float4(mul(Swing, vsIn.position), 1.0f);
    float3 rotNormal    = mul(Swing, vsIn.normal);
    float3 rotTangent   = mul(Swing, vsIn.tangent);
    
    // lerp between original and rotated pos
    float4 lerpPosition = lerp(vsIn.position, rotPosition, ipol);
    float3 lerpNormal   = lerp(vsIn.normal, rotNormal, ipol);
    float3 lerpTangent  = lerp(vsIn.tangent, rotTangent, ipol);
    
    // transform vertex position
    vsOut.position = transformStatic(lerpPosition, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.diffuse  = MatDiffuse * LightDiffuse;
    vsOut.specular = MatSpecular * LightSpecular;                      
    
    // compute lighting
    vsLighting(lerpPosition, lerpNormal, lerpTangent,
               ModelLightPos, ModelEyePos,
               vsOut.primLightVec, vsOut.primHalfVec);

    vsAthmoFog(vsIn.position, ModelEyePos, ModelLightPos, vsOut.L_in, vsOut.F_ex);

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // compute lit color
    float4 baseColor = psLighting(DiffSampler, BumpSampler,
                                  psIn.uv0, psIn.primLightVec, psIn.primHalfVec,
                                  psIn.diffuse, psIn.specular, LightAmbient, MatSpecularPower);                                 

    return psAthmoFog(psIn.L_in, psIn.F_ex, baseColor);
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
