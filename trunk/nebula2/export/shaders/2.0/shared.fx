//-------------------------------------------------------------------------------
//  shared.fx
//   
//  A pseudo-shader which lists all shared effect parameters.
//
//  (C) 2004 RadonLabs GmbH
//-------------------------------------------------------------------------------

shared float4x4 Model;                  // the model matrix
shared float4x4 View;                   // the view matrix
shared float4x4 Projection;             // the projection matrix
shared float4x4 ModelView;              // the model * view matrix
shared float4x4 ModelViewProjection;    // the model * view * projection matrix
shared float4x4 InvModel;               // the inverse model matrix
shared float4x4 InvView;                // the inverse view matrix
shared float4x4 InvModelView;           // the inverse model * view matrix
shared float3 ModelEyePos;              // the eye pos in model space
shared float3 ModelLightPos;            // the current light pos in model space
shared float4x4 TextureTransform0;      // texture transform for uv set 0
shared float4x4 TextureTransform1;      // texture transform for uv set 1
shared float4x4 TextureTransform2;      // texture transform for uv set 0
shared float4x4 TextureTransform3;      // texture transform for uv set 0

technique t0
{
    pass p0
    {
    
    }
}
