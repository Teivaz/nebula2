#line 1 "default_alpha2.fx"
//------------------------------------------------------------------------------
//  fixed/default_alpha2.fx
//
//  A 2 layer alpha shader. Note: the alpha channel of the first 
//  texture indicates the transparency of the 2nd texture!
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 Model;

uniform float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f };
uniform float4x4 TextureTransform1 = {1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f };

float4 MatAmbient;
float4 MatDiffuse;
int AlphaSrcBlend = 5;  // SrcAlpha
int AlphaDstBlend = 6;  // InvSrcAlpha
int CullMode = 2;       // CCW

texture DiffMap0;
texture DiffMap1;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
// #include "../lib/diffsampler.fx"
// #include "../lib/diff1sampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
    	TextureTransform[0] = <TextureTransform0>;
    	TextureTransform[1] = <TextureTransform1>;

        CullMode            = <CullMode>;
        SrcBlend            = <AlphaSrcBlend>;
        DestBlend           = <AlphaDstBlend>;
        MaterialDiffuse  = <MatDiffuse>;
        MaterialAmbient  = <MatAmbient>;

        // FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransformFlags[1] = Count2;
   
        // Sampler[0] = <DiffSampler>;
        // Sampler[1] = <Diff1Sampler>;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;

        ColorOp[1]   = BlendCurrentAlpha;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = BlendCurrentAlpha;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;
                
        ColorOp[2]   = Disable;
        AlphaOp[2]   = Disable;
    }
}

