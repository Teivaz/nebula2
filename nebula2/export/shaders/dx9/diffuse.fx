//------------------------------------------------------------------------------
//  diffuse.fx
//  Render the diffuse component for one light/surface interaction.
//------------------------------------------------------------------------------
#include "lib.fx"

float4x4 modelView;             // the modelview matrix
float4x4 modelViewProjection;   // the modelview*projection matrix
float4 modelEyePos;             // eye position in model space
float4 modelLightPos;           // light position in model space
float4 matDiffuse;              // the material's diffuse reflection value
float4 matAmbient;              // the material's ambient reflection value
float4 lightDiffuse;            // light diffuse color
float4 lightSpecular;           // light specular color
float4 lightAmbient;            // light ambient color

texture bumpMap;                // a normal bump map
texture normMap;                // a normalization cube map

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv0      : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position  : POSITION;
    float2 bumpCoord : TEXCOORD0;
    float3 lightVec  : TEXCOORD1;
    float4 diffuse   : COLOR0;
    float4 ambient   : COLOR1;
};

struct PS_OUTPUT
{
    float4 color : COLOR0;
};

//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
sampler BumpMap = sampler_state
{
    Texture = <bumpMap>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler NormMap = sampler_state
{
    Texture = <normMap>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    vsOut.position = transformStatic(vsIn.position, modelViewProjection);
    vsOut.lightVec = tangentSpaceLightVector(vsIn.position, modelLightPos, vsIn.normal, vsIn.tangent);
    vsOut.bumpCoord = vsIn.uv0;
    vsOut.diffuse = lightDiffuse * matDiffuse;
    vsOut.ambient = lightAmbient * matAmbient;    
    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
PS_OUTPUT psMain(const VS_OUTPUT psIn)
{
    PS_OUTPUT psOut;
    
    // sample surface normal
    float3 surfaceNormal = 2.0f * (tex2D(BumpMap, psIn.bumpCoord) - 0.5f);
    
    // sample normalized tangent space light vector
    float3 lightVector = 2.0f * (texCUBE(NormMap, psIn.lightVec) - 0.5f);
    
    // compute diffuse intensity
    float diffuseIntensity = saturate(dot(surfaceNormal, lightVector));
    
    // write resulting color
    psOut.color = (diffuseIntensity * psIn.diffuse) + psIn.ambient;
    return psOut;
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, PixelShader 1.1
//------------------------------------------------------------------------------
technique vs11_ps11
{
    pass p0
    {
        ZWriteEnable     = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One;                
        AlphaTestEnable  = False;
        
        Sampler[0] = <BumpMap>;
        Sampler[1] = <NormMap>;
        
        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = compile ps_1_1 psMain();
    }
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, Fixed Function Pixel Pipeline
//------------------------------------------------------------------------------
technique vs11_ffpp
{
    pass p0
    {
        ZWriteEnable     = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One;                
        AlphaTestEnable  = False;
        
        Sampler[0] = <BumpMap>;
        Sampler[1] = <NormMap>;
        
        VertexShader = compile vs_1_1 vsMain();
        
        PixelShader = 0;
        
        ColorOp[0]   = SelectArg1;      // sample bump map
        ColorArg1[0] = Texture;
        
        ColorOp[1]   = DotProduct3;     // (bump) dot (normalization cube map)
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        
        ColorOp[2]   = MultiplyAdd;     // multiply with diffuse and add ambient
        ColorArg0[2] = Diffuse;
        ColorArg1[2] = Current;
        ColorArg2[2] = Specular;
        
        ColorOp[3]   = Disable;
        AlphaOp[0]   = Disable;
    }
}
    
    
    
    
    
