#line 1 "tree.fx"
//------------------------------------------------------------------------------
//  tree.fx
//
//  The DX8 shader for vegetation 
//  
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------

shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;
texture DiffMap0;
float MatTranslucency; // the SQUARE ROOT of this is the distance at which objects become completely transparent
float4 Wind;
float Time;
float4 MatDiffuse;
shared float4 LightAmbient;   
shared float4 LightDiffuse;    
shared float3 LightPos; 

float WindIntensityFactor = 0.0007;
float SwaySpeedFactor = 0.5;
//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "diffsampler.fx"
#include "lib.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------

StdVsOutput vsMain(const StdVsInput vsIn)
{
    StdVsOutput vsOut;

    float4 pos = vsIn.position;
    float flexiness = pow(pos.x,2) + pow(pos.z,2);
    pos.x += sin(Time*SwaySpeedFactor) * flexiness * WindIntensityFactor / 2;
    pos = mul( pos, Model );
    pos.xz += Wind.xz * Wind.w * flexiness * WindIntensityFactor;
    vsOut.position = mul( mul( pos, View ), Projection );

    // texturing
    vsOut.uv0 = vsIn.uv0;
    // lighting
    float3 worldNormal = mul( vsIn.normal, Model );
    vsOut.diffuse = MatDiffuse * saturate( LightAmbient + LightDiffuse * saturate( dot( -worldNormal, LightPos ) ) );
    // fog
    float posSquared = (vsOut.position.z * vsOut.position.z ) / MatTranslucency;
    vsOut.diffuse.a -= posSquared;
    vsOut.fog = 1.0 - posSquared;
    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = True; // this makes the alpha prettier -- does it cause other problems?

        AlphaTestEnable  = True;
        AlphaRef         = 50;

        VertexShader = compile vs_1_1 vsMain();

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

