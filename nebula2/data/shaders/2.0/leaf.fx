//------------------------------------------------------------------------------
//  2.0/leaf.fx
//
//  A shader for leaf clusters. This shader is related to the tree.fx, as
//  it implements exactly the same swinging (but additionally renders
//  leaf clusters as sprites).
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 InvModelView;           // inverse of model view
shared float4x4 ModelViewProjection;    // the model*view*projection matrix
shared float3   ModelLightPos;          // the light's position in model space
shared float3   ModelEyePos;            // the eye position in model space
shared float    Time;                   // the current time

shared float4 LightDiffuse;             // light diffuse color        
shared float4 LightSpecular;            // light specular color
shared float4 LightAmbient;             // light ambient color

float4 MatDiffuse;                  // material diffuse color
float4 MatSpecular;                 // material specular color
float4 MatAmbient;                  // material ambient color
float  MatSpecularPower;            // the material's specular power

texture DiffMap0;                   // 2d texture
texture BumpMap0;                   // 2d texture

int AlphaRef = 100;

float4 Wind;                        // wind direction, strength in .w
float3x3 Swing;                     // the swing rotation matrix
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max
float3 BoxCenter;                   // model space bounding box center

float SpriteSwingTime;              // swing period
float SpriteSwingAngle;             // swing angle modulator
float InnerLightIntensity;          // light intensity at center
float OuterLightIntensity;          // light intensity at periphery

float4 GenAngle = { 3.54824948311, -11.6819286346, 10.4263944626, -2.29271507263 };

int CullMode = 2;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 extrude  : NORMAL;           // sprite extrusion vector
    float2 uv0 : TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;         // position in projection space
    float2 uv0      : TEXCOORD0;        // texture coordinate
    float4 diffuse  : TEXCOORD1;
    float4 ambient  : TEXCOORD2;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/bumpsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    // compute swayed position in model space (see tree.fx for details)
    float ipol = (vsIn.position.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);
    float4 rotPosition  = float4(mul(Swing, vsIn.position), 1.0f);
    float4 position = lerp(vsIn.position, rotPosition, ipol);
        
    // compute rotation angle
    float t = frac((Time / SpriteSwingTime) + vsIn.position.x);
    float t2 = t * t;
    float t3 = t2 * t;
    float ang = SpriteSwingAngle * t * (GenAngle.x * t3 + GenAngle.y * t2 + GenAngle.z * t + GenAngle.w);

    // build rotation matrix
    float sinAng, cosAng;
    sincos(ang, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    // rotate extrude vector and transform to model space
    float3 extrudeVec = mul(rot, vsIn.extrude);
    float3 modelExtrudeVec = mul(extrudeVec, (float3x3)InvModelView);
        
    // extrude to corner position
    position.xyz += modelExtrudeVec.xyz;
    
    // transform to projection space
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    
    // compute light vec
    float3 lVec = normalize(ModelLightPos - position);
    
    // compute a model space normal and tangent vector
    float3 posCenter = vsIn.position - BoxCenter;
    float3 normal = normalize(posCenter);
    float3 tangent = cross(normal, float3(0.0f, 1.0f, 0.0f));

    // compute a selfshadowing value
    float relDistToCenter = dot(posCenter, posCenter) / dot(BoxCenter-BoxMaxPos*0.8f, BoxCenter-BoxMaxPos*0.8f);
    float selfShadow = lerp(InnerLightIntensity, OuterLightIntensity, relDistToCenter);
        
    // compute lighting
//    float diffIntensity = 1.0 + clamp(dot(normal, lVec), -1.0, 0.0);    // max for angles < 90 degrees
    float diffIntensity = (dot(normal, lVec) + 1.0)*0.5;
    vsOut.diffuse = diffIntensity * LightDiffuse * MatDiffuse * selfShadow;
    vsOut.ambient = LightAmbient * MatAmbient;

    // vsAthmoFog(vsIn.position, ModelEyePos, ModelLightPos, vsOut.L_in, vsOut.F_ex);

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // compute lit color
    float4 baseColor = psLightingLeaf(DiffSampler, psIn.uv0, psIn.diffuse, psIn.ambient);
    clip(baseColor.a - (AlphaRef / 255.0));    

    // return psAthmoFog(psIn.L_in, psIn.F_ex, baseColor);
    return baseColor;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode = None;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
