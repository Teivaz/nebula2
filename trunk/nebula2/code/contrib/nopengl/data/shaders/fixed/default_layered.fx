#line 1 "default_layered.fx"
//------------------------------------------------------------------------------
//  fixed/default_layered.fx
//
//  The default shader for DX7 cards
//  
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 Model;
const float4x4 TextureTransform0 = {20.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 20.0f, 0.0f, 0.0f, 
                               0.0f, 0.0f, 20.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f };
const float4x4 TextureTransform1 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };

float4 MatAmbient;
float4 MatDiffuse;

texture DiffMap0;
texture DiffMap1;
texture DiffMap2;
texture DiffMap3;
texture NoiseMap0;
texture NoiseMap1;
texture NoiseMap2;

int CullMode = 2;       // CCW

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
/* sampler LayerSampler0 = sampler_state
{
    Texture = <NoiseMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler DiffSampler0  = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler LayerSampler1 = sampler_state
{
    Texture = <NoiseMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler DiffSampler1  = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler LayerSampler2 = sampler_state
{
    Texture = <NoiseMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler DiffSampler2  = sampler_state
{
    Texture = <DiffMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler DiffSampler3  = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
}; */
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {        
        WorldTransform[0] = <Model>;
        CullMode          = <CullMode>;
        AlphaTestEnable   = False;
        MaterialDiffuse   = <MatDiffuse>;
        MaterialAmbient   = <MatAmbient>;

        // FVF = XYZ | NORMAL | TEX2;

        TexCoordIndex[0]         = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransform[0]      = <TextureTransform0>;
        // Sampler[0]		         = <DiffSampler0>;
               
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;

        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable; 
    }
    pass p1
    {
        AlphaBlendEnable = True;
        SrcBlend		 = SrcAlpha;
        DestBlend		 = InvSrcAlpha;
        AlphaTestEnable  = True;
        AlphaRef         = 1;

        // Sampler[0]       = <DiffSampler1>;
        // Sampler[1]       = <LayerSampler0>;
        
        TexCoordIndex[1]         = 0;
        TextureTransformFlags[1] = Count2;
        TextureTransform[1]      = <TextureTransform1>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;
       
        ColorOp[1]   = SelectArg1;
        ColorArg1[1] = Current;
        ColorArg2[1] = Current;
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;

        ColorOp[2] = Disable;
        AlphaOp[2] = Disable; 
    }
    pass p2
    {
        // Sampler[0]   = <DiffSampler2>;
        // Sampler[1]   = <LayerSampler1>;
 
    }
    pass p3
    {
        // Sampler[0]   = <DiffSampler3>;
        // Sampler[1]   = <LayerSampler2>; 
    }
}

