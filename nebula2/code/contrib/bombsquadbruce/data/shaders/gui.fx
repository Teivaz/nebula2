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
        WorldTransform[0] = <Model>;
        ViewTransform = <View>;
    	ProjectionTransform = <Projection>;
    	
        ZEnable          = False;
        
        CullMode = None;

        StencilEnable = false;
        StencilZFail = KEEP;
        StencilPass =  KEEP;

        Sampler[0] = <DiffSampler>;

        LightEnable[0] = False;
        MaterialDiffuse = <MatDiffuse>;
        MaterialAmbient     = {1.0f, 1.0f, 1.0f, 1.0f};        
        Ambient = <MatDiffuse>;

        FVF = XYZ | TEX1;
        FogEnable        = False;
        
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

