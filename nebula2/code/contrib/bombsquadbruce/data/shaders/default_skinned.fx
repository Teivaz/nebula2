#line 1 "default_skinned.fx"
//------------------------------------------------------------------------------
//  fixed/default_skinned.fx
//
//  The default skinning shader for dx7 cards.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "lib.fx"

shared float4x4 ModelViewProjection;       // the model*view*projection matrix
shared float4x4 Model;
matrix<float,4,3> JointPalette[72];            // the joint palette for skinning

int AlphaRef = 10;

texture DiffMap0;

shared float4 LightAmbient;                // light's ambient component
shared float4 LightDiffuse;                // light's diffuse component
shared float3 LightPos;                    // we interpret this as the direction of a directional light 

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
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "diffsampler.fx"

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
    
    vsOut.uv0 = vsIn.uv0;
	vsOut.diffuse = lightDiffuse( skinNormal, Model, LightAmbient, LightDiffuse, LightPos );
	
    return vsOut;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        AlphaRef         = <AlphaRef>;

        VertexShader = compile vs_1_1 vsMain();
        FogEnable = False;
        
        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
               
        ColorOp[1]   = Disable;
        
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        
        AlphaOp[1] = Disable;
    }
}
