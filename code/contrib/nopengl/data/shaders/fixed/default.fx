#line 1 "default.fx"
//------------------------------------------------------------------------------
//  fixed/default.fx
//
//  The default shader for DX7 cards
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 Model : World;
uniform float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f };

float4 MatAmbient : MaterialAmbient;
float4 MatDiffuse : MaterialDiffuse;
int AlphaRef = 100;
int CullMode = CCW;       // CCW

texture DiffMap0;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
    	TextureTransform[0] = <TextureTransform0>;

        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
        //MaterialDiffuse     = <MatDiffuse>;
        //MaterialAmbient     = <MatAmbient>;

        // FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Count2;
   
        Texture[0] = <DiffSampler>;
        //Target[0] = Texture2D;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
    }
}

