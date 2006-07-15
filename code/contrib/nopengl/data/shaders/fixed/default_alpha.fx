#line 1 "default_alpha.fx"
//------------------------------------------------------------------------------
//  fixed/default_alpha.fx
//
//  The default shader for DX7 cards
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 Model;
uniform float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f };

float4 MatAmbient;
float4 MatDiffuse;
int AlphaSrcBlend = 5;  // SrcAlpha
int AlphaDstBlend = 6;  // InvSrcAlpha
int CullMode = 2;       // CCW

texture DiffMap0;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
// #include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
    	TextureTransform[0] = <TextureTransform0>;

        CullMode            = <CullMode>;
        SrcBlend            = <AlphaSrcBlend>;
        DestBlend           = <AlphaDstBlend>;        
        MaterialDiffuse     = <MatDiffuse>;
        MaterialAmbient     = <MatAmbient>;

        // FVF = XYZ | NORMAL | TEX1;

        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Count2;
   
        // Sampler[0] = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable;
    }
}

