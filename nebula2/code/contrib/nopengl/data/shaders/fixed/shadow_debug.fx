#line 1 "shadow_debug.fx"
//------------------------------------------------------------------------------
//  fixed/shadow_debug.fx
//
//  shadow debug shader DX7
//  
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 Model;
uniform float4x4 View;                   // the view matrix
uniform float4x4 Projection;             // the projection matrix

uniform float3 LightPos;             // the light's position in world space

float4 MatDiffuse;
float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        ViewTransform       = <View>;
    	ProjectionTransform = <Projection>;    
        
        ZEnable             = True;
        ZWriteEnable        = False;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        DepthBias           = -0.000001f;
        
        VertexShader        = 0;
        PixelShader         = 0;
        
        CullMode            = Cw;
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;  
        
        //NormalizeNormals    = True;

        Lighting            = True;
        LightEnable[0]      = False;
        SpecularEnable	    = False;
        //LightAmbient[0]  = {0.3, 0.3, 0.3, 1.0f};
        //LightDiffuse[0]  = {1.0, 1.0, 1.0, 1.0f};
        //LightSpecular[0] = {1.0, 1.0, 1.0, 1.0f};
        //LightPosition[0] = <LightPos>;
        //LightRange[0]    = 500000.0;
        //LightAttenuation0[0] = 1.0;
        //LightAttenuation1[0] = 0.0;
        //LightAttenuation2[0] = 0.0;
        
        // LightType must be the last light state that is set!
        //LightType[0] = POINT;

        //ColorVertex           = true;
        //DiffuseMaterialSource = COLOR1;

        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        //FogEnable        = False;

        //SrcBlend            = SrcAlpha;
        //DestBlend           = InvSrcAlpha;
        
        Ambient             = {1.0,  1.0,  1.0,  1.0f};
        MaterialDiffuse     = <MatDiffuse>;
        MaterialAmbient     = <MatDiffuse>;//{1.0f, 1.0f, 1.0f, 1.0f};

        // FVF = XYZ;
        
        Texture[0] = 0;
        TextureTransformFlags[0] = 0;
   
        //ColorOp[0]   = Modulate;
        //ColorArg1[0] = Current;
        //ColorArg2[0] = Diffuse;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        
        //FillMode         = Wireframe;        
    }
}
