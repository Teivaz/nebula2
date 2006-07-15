#line 1 "leaf.fx"
//------------------------------------------------------------------------------
//  fixed/leaf.fx
//
//  A shader for leaf clusters. This shader is related to the tree.fx, as
//  it implements exactly the same swinging (but additionally renders
//  leaf clusters as sprites).
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

uniform float4x4 InvModelView;               // inverse of model view
uniform float4x4 ModelViewProjection;        // the model*view*projection matrix
uniform float4x4 ModelLightProjection;       // the model*light*projection matrix
uniform float3   ModelLightPos;              // the light's position in model space
uniform float3   ModelEyePos;                // the eye position in model space
uniform float    Time;                       // the current time

float4 MatAmbient;
float4 MatDiffuse;
float  MatTranslucency;

int AlphaRef = 100;

texture DiffMap0;

uniform float4 LightAmbient;         // light's ambient component
uniform float4 LightDiffuse;         // light's diffuse component

int CullMode = 2;       // CCW

float4 Wind;                        // wind direction, strength in .w
float3x3 Swing;                     // the swing rotation matrix
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max
float3 BoxCenter;                   // model space bounding box center

float SpriteSize;                   // the leaf size
float SpriteSwingTime;              // swing period
float SpriteSwingAngle;             // swing angle modulator
float InnerLightIntensity;          // light intensity at center
float OuterLightIntensity;          // light intensity at periphery

float4 GenAngle = { 3.54824948311, -11.6819286346, 10.4263944626, -2.29271507263 };

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 extrude  : NORMAL;           // sprite extrusion vector
    float2 uv0 :      TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

struct PsOutput
{
    float4 color : COLOR;
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
    
    // compute light vec and half vec for pixel shader lighting,
    // note: the surface coordinate system is identical with
    // the view coordinate system, so that's easy
    float3 lVec = normalize(ModelLightPos - position);
    float3 eVec = normalize(ModelEyePos - position);
    
    // compute a translucency/reflection factor (when eye looks towards sun)
    float3 eyeLightVector = normalize(ModelLightPos - ModelEyePos);
    float3 eyePosVector   = normalize(position - ModelEyePos);
    float translucency = abs(dot(eyeLightVector, eyePosVector) * MatTranslucency);
    
    // compute a fake lighting intensity (dot(N, L)
    float3 posCenter = vsIn.position - BoxCenter;
//    float diffIntensity = 1.0 + clamp(dot(normalize(posCenter), lVec), -1.0, 0.0); // max for angles < 90 degrees
    float diffIntensity = (dot(normalize(posCenter), lVec) + 1.0)*0.5;
  
    // compute a selfshadowing value
    float relDistToCenter = length(posCenter) / distance(BoxMaxPos, BoxCenter);
    relDistToCenter *= relDistToCenter;
    float selfShadow = lerp(InnerLightIntensity, OuterLightIntensity, relDistToCenter);
    
    float intensity = diffIntensity * selfShadow;
    
    vsOut.diffuse = LightDiffuse * MatDiffuse * intensity + LightAmbient;
    vsOut.uv0 = vsIn.uv0;

    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        TextureTransformFlags[0] = 0;

        CullMode = <CullMode>;
        AlphaRef = <AlphaRef>;

        VertexShader = compile vs_2_0 vsMain();
        
        // Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable;
    }
}
