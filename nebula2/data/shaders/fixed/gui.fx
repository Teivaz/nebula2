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
        WorldTransform[0] = <Model>;
        ViewTransform = <View>;
        ProjectionTransform = <Projection>;

        ZWriteEnable     = False;
        ZEnable          = False;
        // ZFunc            = Always;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = False;

        CullMode = None;

        StencilEnable = false;
        StencilZFail = KEEP;
        StencilPass =  KEEP;

        VertexShader = NULL;
        PixelShader  = NULL;

        Sampler[0] = <DiffSampler>;

        Lighting = True;
        LightEnable[0] = False;
        SpecularEnable = False;
        MaterialDiffuse = <MatDiffuse>;
        MaterialAmbient = { 1.0, 1.0, 1.0, 1.0 };
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

