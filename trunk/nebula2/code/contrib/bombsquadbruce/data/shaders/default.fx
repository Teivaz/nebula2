#line 1 "default.fx"
//------------------------------------------------------------------------------
//  default.fx
//
//  The default shader for DX8 cards
//  
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "lib.fx"

shared float4x4 Model;
shared float4x4 ModelViewProjection;
texture DiffMap0;
float MatTranslucency; // the SQUARE ROOT of this is the distance at which objects become completely transparent
shared float4 LightAmbient;   
shared float4 LightDiffuse;    
shared float3 LightPos; 
//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------

StdVsOutput vsMain(const StdVsInput vsIn)
{
    StdVsOutput vsOut;

    // transform vertex position
    vsOut.diffuse = lightDiffuse( vsIn.normal, Model, LightAmbient, LightDiffuse, LightPos );
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.fog = fog( vsOut.diffuse.a, vsOut.position, MatTranslucency );
    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        AlphaTestEnable  = True;
        AlphaRef         = 100;

        CullMode = 2; // default value (CW); must not be 0 for software vp

        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = 0;

        SpecularEnable	= False;
        
        FogEnable = True;
        FogColor = {0.6, 0.8, 1.0, 0.0};
                
        Sampler[0] = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Diffuse;
        ColorArg2[0] = Texture;
        
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Diffuse;
        AlphaArg2[0] = Texture;        
              
        ColorOp[1] = Disable;
    }
}

