#line 1 "terrain.fx"
//------------------------------------------------------------------------------
//  fixed/terrain.fx
//
//  The default shader for dx7 cards using fixed function pipeline,
//  running in 1 render pass.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 Model;

uniform float4x4 TextureTransform0;
uniform float4x4 TextureTransform1;

texture AmbientMap0;                // material weight texture
texture AmbientMap1;                // material detail texture

int CullMode = 2;       // CCW

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
/* sampler WeightSampler = sampler_state
{
    Texture = <AmbientMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler DetailSampler  = sampler_state
{
    Texture = <AmbientMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
}; */


//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0] = <Model>;

        CullMode = <CullMode>;                   
        // FVF = XYZ | TEX2;
       
        MaterialDiffuse = {1.0, 1.0, 1.0, 1.0};     
        MaterialAmbient = {1.0, 1.0, 1.0, 1.0};

        // Sampler[0]		= <WeightSampler>;
        // Sampler[1]		= <DetailSampler>;
   
        TexCoordIndex[0]         = CAMERASPACEPOSITION;
        TextureTransformFlags[0] = Count3;
        TextureTransform[0]      = <TextureTransform0>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;

        TexCoordIndex[1]         = CAMERASPACEPOSITION;
        TextureTransformFlags[1] = Count3;
        TextureTransform[1]      = <TextureTransform1>;

        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        
        ColorOp[2] = Disable;
        AlphaOp[0] = Disable; 
    }
}
