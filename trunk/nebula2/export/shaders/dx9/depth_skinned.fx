//------------------------------------------------------------------------------
//  depth_skinning.fx
//    
//  Render black pixels with depth values with skinning.
//------------------------------------------------------------------------------
#include "lib.fx"

float4x4 modelView;             // the modelview matrix
float4x4 modelViewProjection;   // the modelview*projection matrix
float4 modelEyePos;             // eye position in model space
float4 modelLightPos;           // light position in model space
float4 jointPalette[24 * 3];    // the joint palette for skinning

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES; 
};

struct VS_OUTPUT
{
    float4 position : POSITION;
};

struct PS_OUTPUT
{
    float4 color : COLOR0;
};

//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    float3 skinPos = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, jointPalette);
    vsOut.position = transformStatic(skinPos, modelViewProjection);
    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
PS_OUTPUT psMain(const VS_OUTPUT psIn)
{
    PS_OUTPUT psOut;
    const float4 black = float4(0.0f, 0.0f, 0.0f, 1.0f);
    psOut.color = black;
    return psOut;
}

//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
sampler NullSampler = sampler_state
{
    Texture = 0;
};
    
//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, PixelShader 1.1
//------------------------------------------------------------------------------
technique vs11_ps11
{
    pass p0
    {
        // render states
        ZWriteEnable     = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaTestEnable  = False;
        AlphaBlendEnable = False;

        Sampler[0] = <NullSampler>;
        
        VertexShader = compile vs_1_1 vsMain();
        PixelShader = compile ps_1_1 psMain();
    }
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, Fixed Function Pixel Pipeline
//------------------------------------------------------------------------------
technique vs11_ffpp
{
    pass p0
    {
        // render states
        ZWriteEnable     = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaTestEnable  = False;
        AlphaBlendEnable = False;
        
        Sampler[0] = (NullSampler);
        
        VertexShader = compile vs_1_1 vsMain();

        PixelShader = 0;
        TextureFactor = 0;
        
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = TFactor;
        
        AlphaOp[0] = Disable;
        ColorOp[1] = Disable;
    }
}
            
        
        
        
        