#line 1 "default_nolight_nofog.fx"
//------------------------------------------------------------------------------
//  fixed/default_nolight_nofog.fx
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
int AlphaRef = 100;
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

        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialAmbient     = <MatAmbient>;

        //FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Count2;
   
        //Sampler[0] = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
    }
}

