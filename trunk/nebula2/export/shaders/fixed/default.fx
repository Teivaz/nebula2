//------------------------------------------------------------------------------
//  fixed/default.fx
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

float3   LightPos;                  // the light's position in world space

//float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;

int AlphaRef = 100;

texture DiffMap0;

float4 LightAmbient;                // light's ambient component
float4 LightDiffuse;                // light's diffuse component
float4 LightSpecular;               // light's specular component

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0] = <Model>;
        ViewTransform = <View>;
    	ProjectionTransform = <Projection>;
    	TextureTransform[0] = <TextureTransform0>;

        ZWriteEnable = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        
        AlphaTestEnable  = True;
        AlphaFunc        = Greaterequal;
        AlphaRef         = <AlphaRef>;

        CullMode = <CullMode>;

        VertexShader = 0;
        PixelShader  = 0;

        MaterialDiffuse  = <MatDiffuse>;
        MaterialAmbient  = {1,1,1,1};//{0.5, 0.5, 0.5, 1.0};//<MatAmbient>;
        MaterialSpecular = <MatSpecular>;
        MaterialPower    = <MatSpecularPower>;

        Ambient = {0,0,0,0};//{0.2, 0.2, 0.2, 1.0};

        Lighting = True;
        LightEnable[0] = True;	
        // 0.75 is to avoid a red of more than 1.0

        LightAmbient[0]  = {0.5*0.75, 0.4500*0.75, 0.4000*0.75, 1};
        LightDiffuse[0]  = {0.8*0.75, 0.4527*0.75, 0.1157*0.75, 1};//<LightDiffuse>;
        LightSpecular[0] = {0.8*0.75, 0.4527*0.75, 0.1157*0.75, 1};//<LightSpecular>;
        LightPosition[0] = <LightPos>;
        LightRange[0]    = 500000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;
        // LightType must be the last light state that is set!
        LightType[0] = POINT;

        SpecularEnable	= false;

        FogEnable = true;
        FogColor = {0.2, 0.2, 0.3, 1.0};
        FogVertexMode = Exp2;
        FogDensity = 5.0e-4;

        FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0]    = Count2;
   
        Sampler[0] = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Diffuse;
        ColorArg2[0] = Texture;
        
        ColorOp[1]   = Disable;
    }
}

