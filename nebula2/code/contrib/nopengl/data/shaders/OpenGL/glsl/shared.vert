//#line 1 "shared.fx"
//-------------------------------------------------------------------------------
//  shared.fx
//
//  A pseudo-shader which lists all uniform effect parameters.
//
//  (C) 2004 RadonLabs GmbH
//-------------------------------------------------------------------------------

uniform mat4  Model;                  // the model matrix
uniform mat4  View;                   // the view matrix
uniform mat4  Projection;             // the projection matrix
uniform mat4  ModelView;              // the model * view matrix
uniform mat4  ModelViewProjection;    // the model * view * projection matrix
uniform mat4  InvModel;               // the inverse model matrix
uniform mat4  InvView;                // the inverse view matrix
uniform mat4  InvModelView;           // the inverse model * view matrix
uniform mat4  ModelLightProjection;   // the model * light * projection matrix

uniform vec3  LightPos;               // the light position in world space
uniform vec4  LightDiffuse;           // light diffuse color
uniform vec4  LightSpecular;          // light specular color
uniform vec4  LightAmbient;           // light ambient color

uniform vec4  LightDiffuse1; //  = vec4(0.0, 0.0, 0.0, 0.0);
uniform vec4  LightSpecular1; // = vec4(0.0, 0.0, 0.0, 0.0);

uniform vec3  ModelEyePos;            // the eye pos in model space
uniform vec3  EyePos;                 // the eye pos in world space
uniform vec3  ModelLightPos;          // the current light pos in model space
uniform mat4  TextureTransform0;      // texture transform for uv set 0
uniform mat4  TextureTransform1;      // texture transform for uv set 1
uniform mat4  TextureTransform2;      // texture transform for uv set 0
uniform mat4  TextureTransform3;      // texture transform for uv set 0
uniform float Time;                   // the current global time
uniform vec4  DisplayResolution;      // the current display resolution

void main(void)
{
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_Position = ModelViewProjection * gl_Vertex;
    //gl_Position = ftransform();
}
