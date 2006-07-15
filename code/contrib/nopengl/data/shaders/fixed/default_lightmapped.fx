#line 1 "default_lightmapped.fx"
//------------------------------------------------------------------------------
//  fixed/default_lightmapped.fx
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
uniform float4x4 TextureTransform1 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };

float4 MatAmbient;
float4 MatDiffuse;
int AlphaRef = 100;
int CullMode = 2;       // CCW

texture DiffMap0;
texture DiffMap1;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
// #include "../lib/diffsampler.fx"
// #include "../lib/lightmapsampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        TextureTransform[0] = <TextureTransform0>;
        TextureTransform[1] = <TextureTransform1>;
        
        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
 
        Lighting = false;
	    
        // FVF = XYZ | NORMAL | TEX2;
        
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Count2;

        TexCoordIndex[1] = 1;
        TextureTransformFlags[1] = Count2;
                
        // Sampler[0] = <DiffSampler>;
        // Sampler[1] = <LightmapSampler>;
        
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = 0;
        
        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;

        ColorOp[2]   = Disable;
    }
}

