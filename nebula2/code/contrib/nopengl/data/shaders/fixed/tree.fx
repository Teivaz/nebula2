#line 1 "tree.fx"
//------------------------------------------------------------------------------
//  ps2.0/tree.fx
//
//  A tree trunk shader which can swing in the wind.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

uniform float4x4 ModelViewProjection;        // the model*view*projection matrix
uniform float3   ModelLightPos;              // the light's position in model space
uniform float3   ModelEyePos;                // the eye position in model space

uniform float4 LightDiffuse;                 // light diffuse color        
uniform float4 LightAmbient;                 // light ambient color
float4 MatDiffuse;                          // material diffuse color
float4 MatAmbient;                          // material ambient color

texture DiffMap0;                   // 2d texture

int AlphaRef = 100;

float3x3 Swing;                     // the swing rotation matrix
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max

int CullMode = 2;       // CCW

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float3 position : POSITION;
    float3 normal :   NORMAL;  
    float2 uv0 :      TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;         // position in projection space
    float2 uv0      : TEXCOORD0;        // texture coordinate
    float4 diffuse  : COLOR0;           // LightDiffuse * MatDiffuse
};

struct PsOutput
{
    float4 color : COLOR;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
// #include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    // compute lerp factor based on height above min y
    float ipol = (vsIn.position.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);

    // compute rotated vertex position, normal in model space
    float3 rotPosition  = mul(Swing, vsIn.position);
    float3 rotNormal    = mul(Swing, vsIn.normal);
    
    // lerp between original and rotated pos
    float3 lerpPosition = lerp(vsIn.position, rotPosition, ipol);
    float3 lerpNormal   = lerp(vsIn.normal, rotNormal, ipol);
    
    // transform vertex position
    vsOut.position = transformStatic(lerpPosition, ModelViewProjection);
    
    // compute lighting
    float3 N = lerpNormal;
    float3 P = lerpPosition;
    float3 L = normalize(ModelLightPos - P);
    float3 R = normalize(2 * dot(N, L) * N - L);        // reflection vector
    float3 V = normalize(ModelEyePos - P);
    
    vsOut.uv0 = vsIn.uv0;
    vsOut.diffuse = LightAmbient + MatDiffuse * LightDiffuse * max(0, dot(N, L));
    
    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        TextureTransformFlags[0] = 0;
        AlphaRef     = <AlphaRef>;
        CullMode     = <CullMode>;
        VertexShader = compile vs_2_0 vsMain();
        
        // Sampler[0]   = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[2] = Disable;
    }
}
