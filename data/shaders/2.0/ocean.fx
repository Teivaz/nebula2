//------------------------------------------------------------------------------
//  ps2.0/ocean.fx
//
//  Ocean shader, ported from the nVidia FXComposer sample of the same name.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

shared float4x4 Model;
shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
shared float4x4 InvView;
shared float    Time;

float3   ModelLightPos;
float3   LightPos;
int      LightIndex;
int      LightType;
float    LightRange;
float4   LightDiffuse;
float4   LightSpecular;
float4   LightAmbient;
float    FallOffStart;
float4   ShadowIndex;

bool AlphaBlendEnable;
float BumpScale;
float4 TexGenS;                 // texture scale
float FresnelBias;
float FresnelPower;
float Intensity0;               // hdr multiplier
float4 MatDiffuse;              // deep water color
float4 MatSpecular;             // shallow water color
float4 MatAmbient;              // reflection color
float MatSpecularPower;
float Intensity1;               // reflection amount
float Intensity2;               // water color amount
float Amplitude;                // wave amplitude
float Frequency;                // wave frequency
float4 Velocity;                // wave speed
int CullMode = 2; // CW

texture BumpMap0;
texture CubeMap0;

#include "shaders:../lib/lib.fx"

//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT;
};

struct VsOutput
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 row0         : TEXCOORD1; // first row of the 3x3 transform from tangent to cube space
    float3 row1         : TEXCOORD2; // second row of the 3x3 transform from tangent to cube space
    float3 row2         : TEXCOORD3; // third row of the 3x3 transform from tangent to cube space
    float2 bumpCoord0   : TEXCOORD4;
    float2 bumpCoord1   : TEXCOORD5;
    float2 bumpCoord2   : TEXCOORD6;
    float3 eyeVector    : TEXCOORD7;
};

struct Wave
{
    float freq;     // 2*PI / wavelength
    float amp;      // amplitude
    float phase;    // speed * 2*PI / wavelength
    float2 dir;
};

#define NumWaves 2
Wave wave[NumWaves] =
{
	{ 1.0, 1.0, 0.5, float2(-1, 0) },
	{ 2.0, 0.5, 1.3, float2(-0.7, 0.7) }
};

//------------------------------------------------------------------------------
sampler NormalMapSampler = sampler_state
{
    Texture = <BumpMap0>;
	MagFilter = Linear;
	MinFilter = Linear;
	MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
    MipMapLodBias = -0.75;
};

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
float evaluateWave(Wave w, float2 pos, float t)
{
  return w.amp * sin(dot(w.dir, pos) * w.freq + t * w.phase);
}

float evaluateWaveDeriv(Wave w, float2 pos, float t)
{
  return w.freq * w.amp * cos(dot(w.dir, pos) * w.freq + t * w.phase);
}

float evaluateWaveSharp(Wave w, float2 pos, float t, float k)
{
  return w.amp * pow(sin(dot(w.dir, pos) * w.freq + t * w.phase)* 0.5 + 0.5, k);
}

float evaluateWaveDerivSharp(Wave w, float2 pos, float t, float k)
{
  return k * w.freq * w.amp * pow(sin(dot(w.dir, pos) * w.freq + t * w.phase)* 0.5 + 0.5, k - 1) * cos(dot(w.dir, pos) * w.freq + t * w.phase);
}

//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    wave[0].freq = Frequency;
    wave[0].amp  = Amplitude;
    wave[1].freq = Frequency * 2.0;
    wave[1].amp  = Amplitude * 0.5;

    float4 pos = vsIn.position;

    // sum wave
    pos.y = 0.0;
    float ddx = 0.0;
    float ddy = 0.0;
    for (int i = 0; i < NumWaves; i++)
    {
	    pos.y += evaluateWave(wave[i], pos.xz, Time);
	    float deriv = evaluateWaveDeriv(wave[i], pos.xz, Time);
	    ddx += deriv * wave[i].dir.x;
	    ddy += deriv * wave[i].dir.y;
	}

    // compute tangent basis
    float3 B = float3(1, ddx, 0);
    float3 T = float3(0, ddy, 1);
    float3 N = float3(-ddx, 1, -ddy);

	// compute output vertex position
	vsOut.position = mul(pos, ModelViewProjection);

	// pass texture coordinates for fetching the normal map
	vsOut.uv0.xy = vsIn.uv0 * TexGenS.xy;
	float modTime = fmod(Time, 100.0f);
	vsOut.bumpCoord0.xy = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy;
	vsOut.bumpCoord1.xy = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy * 4.0f;
	vsOut.bumpCoord2.xy = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy * 8.0f;

    // compute the 3x3 tranform from tangent space to object space
    // first rows are the tangent and binormal scaled by the bump scale
    float3x3 objToTangentSpace;
    objToTangentSpace[0] = BumpScale * normalize(T);
    objToTangentSpace[1] = BumpScale * normalize(B);
    objToTangentSpace[2] = normalize(N);

    vsOut.row0.xyz = mul(objToTangentSpace, Model[0].xyz);
    vsOut.row1.xyz = mul(objToTangentSpace, Model[1].xyz);
    vsOut.row2.xyz = mul(objToTangentSpace, Model[2].xyz);

    // compute the eye vector (going from shaded point to eye) in cube space
    float4 worldPos = mul(pos, Model);
    vsOut.eyeVector = InvView[3] - worldPos; // view inv. transpose contains eye position in world space in last row

    return vsOut;
}

//------------------------------------------------------------------------------
float4 psMain(VsOutput psIn, uniform bool hdr) : COLOR
{
    // sum normal maps
    half4 t0 = tex2D(NormalMapSampler, psIn.bumpCoord0.xy) * 2.0 - 1.0;
    half4 t1 = tex2D(NormalMapSampler, psIn.bumpCoord1.xy) * 2.0 - 1.0;
    half4 t2 = tex2D(NormalMapSampler, psIn.bumpCoord2.xy) * 2.0 - 1.0;
    half3 N = t0.xyz + t1.xyz + t2.xyz;

    half3x3 m; // tangent to world matrix
    m[0] = psIn.row0;
    m[1] = psIn.row1;
    m[2] = psIn.row2;
    half3 Nw = mul(m, N.xyz);
    Nw = normalize(Nw);

    // reflection
    float3 E = normalize(psIn.eyeVector);
    half3 R = reflect(-E, Nw);

    half4 reflection = texCUBE(EnvMapSampler, R);
    // hdr effect (multiplier in alpha channel)
    reflection.rgb *= (1.0 + reflection.a*Intensity0);

	// fresnel - could use 1D tex lookup for this
    half facing = 1.0 - max(dot(E, Nw), 0);
    half fresnel = FresnelBias + (1.0 - FresnelBias) * pow(facing, FresnelPower);

    half4 waterColor = lerp(MatDiffuse, MatSpecular, facing);
    float4 color = waterColor * Intensity2 + reflection * MatAmbient * Intensity1 * LightDiffuse * 0.5 * fresnel;

    color.a = fresnel * 0.5 + 0.5;
    if (hdr)
    {
        return color * float4(0.25, 0.25, 0.25, 1.0);
    }
    else
    {
        return color;
    }
}

//------------------------------------------------------------------------------
technique tDepth
{
    pass p0
    {
        CullMode = None;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain(false);
    }
}

technique tColor
{
    pass p0
    {
        CullMode = None;
        SrcBlend  = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain(false);
    }
}

technique tColorHDR
{
    pass p0
    {
        CullMode = None;
        SrcBlend  = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain(true);
    }
}


