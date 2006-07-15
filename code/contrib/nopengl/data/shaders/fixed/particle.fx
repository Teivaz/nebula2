#line 1 "particle.fx"
//------------------------------------------------------------------------------
//  fixed/particle.fx
//
//  Particle system shader.
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 ModelViewProjection;       // the model*view*projection matrix
uniform float4x4 InvModelView;			    // the inverse model*view matrix

texture DiffMap0;

int AlphaSrcBlend = 5;  // SrcAlpha
int AlphaDstBlend = 6;  // InvSrcAlpha

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position  : POSITION;  // the particle position in world space
    float2 uv0       : TEXCOORD0; // the particle texture coordinates
    float2 extrude   : TEXCOORD1; // the particle corner offset
    float2 transform : TEXCOORD2; // the particle rotation and scale
    float4 color     : COLOR0;    // the particle color
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
// #include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{	
    float rotation = vsIn.transform[0];
    float size     = vsIn.transform[1];
	
    // build rotation matrix
    float sinAng, cosAng;
    sincos(rotation, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    float4 position =  vsIn.position;
    float3 extrude  =  float3(vsIn.extrude, 0.0f);
    extrude        *= size;
    extrude         =  mul(rot, extrude);    
    extrude         =  mul(extrude, (float3x3) InvModelView);
    position.xyz   += extrude.xyz;
            
    VsOutput vsOut;
    // transform to projection space
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0      = vsIn.uv0;
    vsOut.diffuse  = vsIn.color;

    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        TextureTransformFlags[0] = 0;
        CullMode  = None;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;        

        // Sampler[0] = <DiffSampler>;
        VertexShader = compile vs_2_0 vsMain();
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
           
        ColorOp[1] = Disable;        
        AlphaOp[1] = Disable;
    }
}
