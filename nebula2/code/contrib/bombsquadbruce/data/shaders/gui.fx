#line 1 "gui.fx"
//------------------------------------------------------------------------------
//  gui.fx
//
//  A 2d rectangle shader for GUI rendering.
//------------------------------------------------------------------------------
shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;

texture DiffMap0;
float4 MatDiffuse;

//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZEnable          = False;

        CullMode = None;

        Sampler[0] = <DiffSampler>;

        LightEnable[0] = False;
        SpecularEnable = False;
        MaterialSpecular = { 0.0, 0.0, 0.0, 0.0 };
        Ambient = <MatDiffuse>;

        FVF = XYZ | TEX1;
        
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Disable;
   
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        
        AlphaOp[1] = Disable;
    }
}

