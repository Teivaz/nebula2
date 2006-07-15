#line 1 "default_blended.fx"
//------------------------------------------------------------------------------
//  fixed/default_blended.fx
//
//  DX7 shader for mesh blending with up to 7 parallel streams.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 ModelViewProjection;    // the model*view*projection matrix
uniform float3   ModelEyePos;            // the eye position in model space
uniform float3   ModelLightPos;          // the light position in model space

uniform float4 LightDiffuse;             // light diffuse color        
uniform float4 LightAmbient;             // light ambient color

float4 MatAmbient;                      // material diffuse color
float4 MatDiffuse;                      // material diffuse color

texture DiffMap0;                       // 2d texture
int AlphaRef = 100;
int CullMode = 2;       // CCW

int VertexStreams;
float4 VertexWeights1;
float4 VertexWeights2;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position0 : POSITION0;
    float4 position1 : POSITION1;
    float4 position2 : POSITION2;
    float4 position3 : POSITION3;
    float4 position4 : POSITION4;
    float4 position5 : POSITION5;
    float4 position6 : POSITION6;
    float3 normal0   : NORMAL0;
    float3 normal1   : NORMAL1;
    float3 normal2   : NORMAL2;
    float3 normal3   : NORMAL3;
    float3 normal4   : NORMAL4;
    float3 normal5   : NORMAL5;
    float3 normal6   : NORMAL6;
    float2 uv0      : TEXCOORD0;
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
// #include "shaders:../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 pos = 0;
    float3 normal = 0;
    if (VertexStreams > 0)
    {
        pos = vsIn.position0;
        normal = vsIn.normal0;
        if (VertexStreams > 1)
        {
            pos += (vsIn.position1-vsIn.position0) * VertexWeights1.y;
            normal += (vsIn.normal1-vsIn.normal0) * VertexWeights1.y;
            if (VertexStreams > 2)
            {
                pos += (vsIn.position2-vsIn.position0) * VertexWeights1.z;
                normal += (vsIn.normal2-vsIn.normal0) * VertexWeights1.z;                
                if (VertexStreams > 3)
                {
                    pos += (vsIn.position3-vsIn.position0) * VertexWeights1.w;
                    normal += (vsIn.normal3-vsIn.normal0) * VertexWeights1.w;
                    if (VertexStreams > 4)
                    {
                        pos += (vsIn.position4-vsIn.position0) * VertexWeights2.x;
                        normal += (vsIn.normal4-vsIn.normal0) * VertexWeights2.x;
                        if (VertexStreams > 5)
                        {
                            pos += (vsIn.position5-vsIn.position0) * VertexWeights2.y;
                            normal += (vsIn.normal5-vsIn.normal0) * VertexWeights2.y;
                            if (VertexStreams > 6)
                            {
                                pos += (vsIn.position6-vsIn.position0) * VertexWeights2.z;
                                normal += (vsIn.normal6-vsIn.normal0) * VertexWeights2.z;
                            }
                        }
                    }
                }                
            }
        }
    }

    float3 L = normalize(ModelLightPos - pos);
    float4 color = LightAmbient + MatDiffuse * LightDiffuse * max(0, dot(normal, L));

    vsOut.position = mul(pos, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.diffuse = color;

    return vsOut;
}

//------------------------------------------------------------------------------
//  The techniques.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode     = <CullMode>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile vs_2_0 vsMain();

        // Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
    }
}
