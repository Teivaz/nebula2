#line 1 "ground_lightmapped.fx"
//------------------------------------------------------------------------------
//  fixed/ground_lightmapped.fx
//
//  The default shader for DX7 cards
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 Model;
float4x4 View;
float4x4 Projection;
float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };
float4x4 TextureTransform1 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };

float3 LightPos;                  // the light's position in world space

float4 LightAmbient;                // light's ambient component
float4 LightDiffuse;                // light's diffuse component
float4 LightSpecular;               // light's specular component

float4 MatAmbient;
float4 MatDiffuse;

int AlphaRef = 10;

texture DiffMap0;
texture DiffMap1;

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/lightmapsampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;
        TextureTransform[0] = <TextureTransform0>;
        TextureTransform[1] = <TextureTransform1>;
   
        ZWriteEnable     = true;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;

        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        AlphaRef         = <AlphaRef>;

        CullMode         = <CullMode>;
        
        MaterialDiffuse  = <MatDiffuse>;
        MaterialAmbient  = {1.0f, 1.0f, 1.0f, 0.0f};

    	Ambient = {1.0f, 1.0f, 1.0f, 0.0f};
       
        Lighting = True;
        LightEnable[0]   = True;	
        LightAmbient[0]  =  {1.0f, 1.0f, 1.0f, 0.0f};
        LightDiffuse[0]  = <LightDiffuse>;
        LightSpecular[0] = <LightSpecular>;
        LightPosition[0] = <LightPos>;
        LightRange[0]    = 10000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 1.0;
        LightAttenuation2[0] = 1.0;

    	// LightType must be the last light state that is set!
        LightType[0] = POINT;

        SpecularEnable = false;

        FVF = XYZ | NORMAL | TEX2;
       
        VertexShader	= 0;
        PixelShader		= 0;
        
        Sampler[0]		= <DiffSampler>;
        Sampler[1]		= <LightmapSampler>;
    
        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
               
        ColorOp[1] = Modulate2X;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        
        ColorOp[2] = Disable;
    }
}

