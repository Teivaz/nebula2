#line 1 "tree.fx"
//------------------------------------------------------------------------------
//  ps2.0/tree.fx
//
//  A tree trunk shader which can swing in the wind.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;       // the model*view*projection matrix
shared float4x4 Projection;                // the projection matrix
shared float4x4 ModelLightProjection;      // the model*light*projection matrix
shared float3   ModelLightPos;             // the light's position in model space
shared float3   ModelEyePos;               // the eye position in model space

//float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;

int AlphaRef = 100;

texture DiffMap0;

float4 LightAmbient;                // light's ambient component
float4 LightDiffuse;                // light's diffuse component
float4 LightSpecular;               // light's specular component

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

texture LightModMap;                // the light modulation map

float3x3 Swing;                     // the swing rotation matrix
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max

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
    float4 specular : COLOR1;           // LightSpecular * MatSpecular
};

struct PsOutput
{
    float4 color : COLOR;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/diffsampler.fx"

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
    // 0.75 is to avoid a red of more than 1.0
    vsOut.diffuse = 0.75 * LightAmbient + MatDiffuse * 0.75 * LightDiffuse * max(0, dot(N, L));
    vsOut.specular	= float4(0.0f, 0.0f, 0.0f, 1.0f);
    // vsOut.specular = MatSpecular * LightSpecular * pow(max(0, dot(R, V)), MatSpecularPower);
    
    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable     = true;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        
        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        AlphaRef         = <AlphaRef>;

        CullMode         = <CullMode>;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader = 0;
        
        FogEnable = true;
        FogColor = {0.2, 0.2, 0.3, 1.0};
        FogVertexMode = None;
        FogTableMode = Exp2;
        FogDensity = 5.0e-4;

        Sampler[0] = <DiffSampler>;
        
        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1] = Add;
        ColorArg1[1] = Current;
        ColorArg2[1] = Specular;
        
        ColorOp[2] = Disable;
    }
}
