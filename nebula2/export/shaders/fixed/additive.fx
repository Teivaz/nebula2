#line 1 "additive.fx"
//------------------------------------------------------------------------------
//  fixed/additive.fx
//
//  A simple unlit, additive transparency shader.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;

float4 MatDiffuse;                  // material diffuse color
texture DiffMap0;                   // 2d texture
int CullMode = 2;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0] = <Model>;
        ViewTransform = <View>;
    	ProjectionTransform = <Projection>;

        ZWriteEnable = false;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One;
        CullMode         = <CullMode>;
        
        AlphaTestEnable  = False;
        
        VertexShader = 0;
        PixelShader  = 0;

        FVF = XYZ | NORMAL | TEX1;
        
        MaterialAmbient = <MatDiffuse>;
        Lighting = True;
        LightEnable[0] = True;
        LightAmbient[0] = {1.0, 1.0, 1.0, 1.0};
        LightPosition[0] = {0,0,0};
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;
        LightRange[0] = 500000;
        LightType[0] = POINT;
        SpecularEnable	= false;

        Sampler[0] = <DiffSampler>;

        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1] = Disable;
        
        AlphaOp[0] = Disable;
    }
}
