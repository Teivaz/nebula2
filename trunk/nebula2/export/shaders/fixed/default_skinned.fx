//------------------------------------------------------------------------------
//  fixed/default_skinned.fx
//
//  The default skinning shader for dx7 cards.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 ModelViewProjection;       // the model*view*projection matrix
float4x4 ModelLightProjection;      // the model*light*projection matrix
float4   ModelLightPos;             // the light's position in model space
float4   ModelEyePos;               // the eye position in model space
matrix<float,4,3> JointPalette[72];            // the joint palette for skinning

float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;

int AlphaRef = 10;

texture DiffMap0;

float4 LightAmbient;                // light's ambient component
float4 LightDiffuse;                // light's diffuse component
float4 LightSpecular;               // light's specular component

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal :   NORMAL;  
    float2 uv0 :      TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES; 
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
    float4 specular : COLOR1;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    // get skinned position, normal and tangent    
    float3 skinPos     = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    float3 skinNormal  = skinnedNormal(vsIn.normal, vsIn.weights, vsIn.indices, JointPalette);
    
    // transform vertex position
    vsOut.position = transformStatic(skinPos, ModelViewProjection);
    
    // compute lighting
    float3 N = skinNormal;
    float3 P = skinPos;
    float3 L = normalize(ModelLightPos - P);
    float3 R = normalize(2 * dot(N, L) * N - L);        // reflection vector
    float3 V = normalize(ModelEyePos - P);
    
    vsOut.uv0 = vsIn.uv0;
    // 0.75 is to avoid a red of more than 1.0
    vsOut.diffuse	= 0.75 * LightAmbient + MatDiffuse * 0.75 * LightDiffuse * max(0, dot(N, L));
    vsOut.specular	= float4(0.0f, 0.0f, 0.0f, 1.0f);
    // vsOut.specular	= MatSpecular * LightSpecular * pow(max(0, dot(R, V)), MatSpecularPower);

    return vsOut;
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

        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        AlphaRef         = <AlphaRef>;

        CullMode         = <CullMode>;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = 0;
        
        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Add;
        ColorArg1[1] = Current;
        ColorArg2[1] = Specular;
        
        ColorOp[2]   = Disable;
        
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        
        AlphaOp[1] = Disable;
    }
}
