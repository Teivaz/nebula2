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
#include "shaders:../lib/lib.fx"

shared float4x4 InvModelView;               // inverse of model view
shared float4x4 ModelViewProjection;        // the model*view*projection matrix
shared float4x4 ModelLightProjection;       // the model*light*projection matrix
shared float3   ModelLightPos;              // the light's position in model space
shared float3   ModelEyePos;                // the eye position in model space

float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatTranslucency;

int AlphaRef = 100;

texture DiffMap0;

float4 LightAmbient;                // light's ambient component
float4 LightDiffuse;                // light's diffuse component
float4 LightSpecular;               // light's specular component

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

float Time;                         // the current time
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
#include "shaders:../lib/diffsampler.fx"

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
    
    // 0.75 is to avoid a red of more than 1.0
    vsOut.diffuse = 0.75 * LightDiffuse * MatDiffuse * intensity + 0.75 * LightAmbient;
    vsOut.uv0 = vsIn.uv0;

    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = true;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        CullMode         = <CullMode>;
        
        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        AlphaRef         = <AlphaRef>;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = 0;
        
        FogEnable = true;
        FogColor = {0.2, 0.2, 0.3, 1.0};
        FogVertexMode = None;
        FogTableMode = Exp2;
        FogDensity = 5.0e-4;

        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        
        AlphaOp[0] = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        
        AlphaOp[1] = Disable;
    }
}

