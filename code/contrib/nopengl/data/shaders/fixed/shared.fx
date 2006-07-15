#line 1 "shared.fx"
//-------------------------------------------------------------------------------
//  shared.fx
//   
//  A pseudo-shader which lists all uniform effect parameters.
//
//  (C) 2004 RadonLabs GmbH
//-------------------------------------------------------------------------------

uniform float4x4 Model;                  // the model matrix
uniform float4x4 View;                   // the view matrix
uniform float4x4 Projection;             // the projection matrix
uniform float4x4 ModelView;              // the model * view matrix
uniform float4x4 ModelViewProjection;    // the model * view * projection matrix
uniform float4x4 InvModel;               // the inverse model matrix
uniform float4x4 InvView;                // the inverse view matrix
uniform float4x4 InvModelView;           // the inverse model * view matrix
uniform float4x4 ModelLightProjection;   // the model * light * projection matrix

uniform float3   LightPos;               // the light position in world space
uniform float4   LightDiffuse;           // light diffuse color        
uniform float4   LightSpecular;          // light specular color
uniform float4   LightAmbient;           // light ambient color

uniform float4   LightDiffuse1 = { 0.0f, 0.0f, 0.0f, 0.0f };
uniform float4   LightSpecular1 = { 0.0f, 0.0f, 0.0f, 0.0f };

uniform float3   ModelEyePos;            // the eye pos in model space
uniform float3   EyePos;                 // the eye pos in world space
uniform float3   ModelLightPos;          // the current light pos in model space
uniform float4x4 TextureTransform0;      // texture transform for uv set 0
uniform float4x4 TextureTransform1;      // texture transform for uv set 1
uniform float4x4 TextureTransform2;      // texture transform for uv set 0
uniform float4x4 TextureTransform3;      // texture transform for uv set 0
uniform float Time;                      // the current global time
uniform float4 DisplayResolution;        // the current display resolution

technique t0
{
    pass p0
    {
    
    }
}
