#line 1 "underwater.fx"
//------------------------------------------------------------------------------
//  underwater.fx
//
//  The shader for the underwater opacity layers, for DX8 and DX7 cards
//  
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------

shared float4x4 ModelViewProjection;
texture DiffMap0;
float4 MatTransparency;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler WaterSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------

struct VsInput
{
    float4 position : POSITION;
    float2 uv0 :      TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;         // position in projection space
    float2 uv0      : TEXCOORD0;        // tex coord
    float4 diffuse : COLOR;             // only controls the alpha, actually
};
//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------

VsOutput vsMain(const VsInput vsIn)
{
// the following is probably a really dumb way of accomplishing what I'm trying to accomplish,
// namely applying a constant alpha value (defined by MatTransparency) to the texture
// instead of using the texture's intrinsic alpha channel
    VsOutput vsOut;

    float4 pos = vsIn.position;
    
    // transform vertex position
    vsOut.position = mul(pos, ModelViewProjection);
                              
    vsOut.uv0 = vsIn.uv0;
    vsOut.diffuse = MatTransparency;
    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = False;
        AlphaTestEnable  = False;
        VertexShader = compile vs_1_1 vsMain();
        FogEnable = False;
        Sampler[0] = <WaterSampler>;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        AlphaOp[1] = Disable;
    }
}

