#line 1 "cubeskybox.fx"
//------------------------------------------------------------------------------
//  2.0/cubeskybox.fx
//
//  A skybox textured by a cube map.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 ModelViewProjection;
shared float4x4 Model;

texture CubeMap0;
float4 MatDiffuse;

//------------------------------------------------------------------------------
struct VsInput
{
    float4 position  : POSITION;  // the particle position in world space
};

struct VsOutput
{
    float4 position : POSITION;
    float3 uv0 : TEXCOORD0;
};

//------------------------------------------------------------------------------
sampler EnvMapSampler = sampler_state
{
    Texture = <CubeMap0>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0      = mul(vsIn.position, (float3x3)Model);
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    return texCUBE(EnvMapSampler, psIn.uv0) * MatDiffuse;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode = CCW;
        AlphaRef = 0;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}

    

    


